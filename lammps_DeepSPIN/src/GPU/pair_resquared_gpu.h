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

#ifdef PAIR_CLASS
// clang-format off
PairStyle(resquared/gpu,PairRESquaredGPU);
// clang-format on
#else

#ifndef LMP_PAIR_RESQUARED_GPU_H
#define LMP_PAIR_RESQUARED_GPU_H

#include "pair_resquared.h"

namespace LAMMPS_NS {

class PairRESquaredGPU : public PairRESquared {
 public:
  PairRESquaredGPU(LAMMPS *lmp);
  ~PairRESquaredGPU() override;
  void cpu_compute(int, int, int, int, int *, int *, int **);
  void compute(int, int) override;
  void init_style() override;
  double memory_usage() override;

  enum { GPU_FORCE, GPU_NEIGH, GPU_HYB_NEIGH };

 private:
  int gpu_mode;
  double cpu_time;
  int quat_nmax;
  double **quat;
};

}    // namespace LAMMPS_NS
#endif
#endif
