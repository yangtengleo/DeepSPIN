/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   https://www.lammps.org/, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifdef DIHEDRAL_CLASS
// clang-format off
DihedralStyle(charmm,DihedralCharmm);
// clang-format on
#else

#ifndef LMP_DIHEDRAL_CHARMM_H
#define LMP_DIHEDRAL_CHARMM_H

#include "dihedral.h"

namespace LAMMPS_NS {

class DihedralCharmm : public Dihedral {
 public:
  DihedralCharmm(class LAMMPS *);
  ~DihedralCharmm() override;
  void compute(int, int) override;
  void coeff(int, char **) override;
  void init_style() override;
  void write_restart(FILE *) override;
  void read_restart(FILE *) override;
  void write_data(FILE *) override;

 protected:
  double *k, *weight, *cos_shift, *sin_shift;
  int *multiplicity, *shift;
  double **lj14_1, **lj14_2, **lj14_3, **lj14_4;
  int implicit, weightflag;

  virtual void allocate();
};

}    // namespace LAMMPS_NS

#endif
#endif
