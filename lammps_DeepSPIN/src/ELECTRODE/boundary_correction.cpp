/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   https://www.lammps.org/, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

/* ----------------------------------------------------------------------
   Contributing authors: Ludwig Ahrens-Iwers (TUHH), Shern Tee (UQ), Robert Meißner (TUHH)
------------------------------------------------------------------------- */

#include "boundary_correction.h"

#include "atom.h"
#include "comm.h"
#include "force.h"

using namespace LAMMPS_NS;

// use EW3DC slab correction
//
BoundaryCorrection::BoundaryCorrection(LAMMPS *lmp) : Pointers(lmp) {}

void BoundaryCorrection::setup(double x, double y, double z)
{
  xprd_wire = x;
  yprd_wire = y;
  zprd_slab = z;
  volume = x * y * z;
  area = x * y;
  qqrd2e = force->qqrd2e;
  scale = 1.0;
}
void BoundaryCorrection::setup(double x, double y, double z, double g)
{
  xprd_wire = x;
  yprd_wire = y;
  zprd_slab = z;
  volume = x * y * z;
  area = x * y;
  qqrd2e = force->qqrd2e;
  scale = 1.0;
  g_ewald = g;
}

std::vector<int> BoundaryCorrection::gather_recvcounts(int n)
{
  int const nprocs = comm->nprocs;
  std::vector<int> recvcounts = std::vector<int>(nprocs);
  MPI_Allgather(&n, 1, MPI_INT, &recvcounts.front(), 1, MPI_INT, world);
  return recvcounts;
}

std::vector<int> BoundaryCorrection::gather_displs(const std::vector<int> &recvcounts)
{
  int const nprocs = comm->nprocs;
  std::vector<int> displs = std::vector<int>(nprocs);
  displs[0] = 0;
  for (int i = 1; i < nprocs; i++) displs[i] = displs[i - 1] + recvcounts[i - 1];
  return displs;
}

std::vector<bigint> BoundaryCorrection::gather_jmat(bigint *imat)
{
  int nlocal = atom->nlocal;
  bigint ngroup = 0;
  int ngrouplocal = 0;
  for (int i = 0; i < nlocal; i++)
    if (imat[i] > -1) ngrouplocal++;
  MPI_Allreduce(&ngrouplocal, &ngroup, 1, MPI_INT, MPI_SUM, world);

  std::vector<bigint> jmat_local = std::vector<bigint>(ngrouplocal);
  for (int i = 0, n = 0; i < nlocal; i++) {
    if (imat[i] < 0) continue;
    jmat_local[n++] = imat[i];
  }

  // gather global matrix indexing
  std::vector<bigint> jmat = std::vector<bigint>(ngroup);
  std::vector<int> recvcounts = gather_recvcounts(ngrouplocal);
  std::vector<int> displs = gather_displs(recvcounts);
  MPI_Allgatherv(&jmat_local.front(), ngrouplocal, MPI_LMP_BIGINT, &jmat.front(),
                 &recvcounts.front(), &displs.front(), MPI_LMP_BIGINT, world);
  return jmat;
}
