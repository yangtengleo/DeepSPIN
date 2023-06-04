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

#ifdef COMPUTE_CLASS
// clang-format off
ComputeStyle(msd/chunk,ComputeMSDChunk);
// clang-format on
#else

#ifndef LMP_COMPUTE_MSD_CHUNK_H
#define LMP_COMPUTE_MSD_CHUNK_H

#include "compute.h"

namespace LAMMPS_NS {

class ComputeMSDChunk : public Compute {
 public:
  ComputeMSDChunk(class LAMMPS *, int, char **);
  ~ComputeMSDChunk() override;
  void init() override;
  void setup() override;
  void compute_array() override;

  void lock_enable() override;
  void lock_disable() override;
  int lock_length() override;
  void lock(class Fix *, bigint, bigint) override;
  void unlock(class Fix *) override;

  double memory_usage() override;

 private:
  int nchunk;
  char *idchunk;
  class ComputeChunkAtom *cchunk;
  char *id_fix;
  class FixStore *fix;
  int firstflag;

  double *massproc, *masstotal;
  double **com, **comall;
  double **msd;

  void allocate();
};

}    // namespace LAMMPS_NS

#endif
#endif
