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
ComputeStyle(temp/chunk,ComputeTempChunk);
// clang-format on
#else

#ifndef LMP_COMPUTE_TEMP_CHUNK_H
#define LMP_COMPUTE_TEMP_CHUNK_H

#include "compute.h"

namespace LAMMPS_NS {

class ComputeTempChunk : public Compute {
 public:
  ComputeTempChunk(class LAMMPS *, int, char **);
  ~ComputeTempChunk() override;
  void init() override;
  double compute_scalar() override;
  void compute_vector() override;
  void compute_array() override;

  void remove_bias(int, double *) override;
  void remove_bias_all() override;
  void restore_bias(int, double *) override;
  void restore_bias_all() override;

  void lock_enable() override;
  void lock_disable() override;
  int lock_length() override;
  void lock(class Fix *, bigint, bigint) override;
  void unlock(class Fix *) override;

  double memory_usage() override;

 private:
  int nchunk, maxchunk, comflag, biasflag;
  int nvalues;
  int *which;
  char *idchunk;
  class ComputeChunkAtom *cchunk;
  double adof, cdof;
  char *id_bias;
  class Compute *tbias;    // ptr to additional bias compute
  bigint comstep;

  double *sum, *sumall;
  int *count, *countall;
  double *massproc, *masstotal;
  double **vcm, **vcmall;

  void vcm_compute();
  void temperature(int);
  void kecom(int);
  void internal(int);
  void allocate();
};

}    // namespace LAMMPS_NS

#endif
#endif
