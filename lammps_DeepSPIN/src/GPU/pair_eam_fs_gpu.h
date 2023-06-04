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
PairStyle(eam/fs/gpu,PairEAMFSGPU);
// clang-format on
#else

#ifndef LMP_PAIR_EAM_FS_GPU_H
#define LMP_PAIR_EAM_FS_GPU_H

#include "pair_eam.h"

namespace LAMMPS_NS {

class PairEAMFSGPU : public PairEAM {
 public:
  PairEAMFSGPU(class LAMMPS *);
  ~PairEAMFSGPU() override;
  void coeff(int, char **) override;
  void compute(int, int) override;
  void init_style() override;
  double single(int, int, int, int, double, double, double, double &) override;
  double memory_usage() override;
  void *extract(const char *, int &) override { return nullptr; }

  int pack_forward_comm(int, int *, double *, int, int *) override;
  void unpack_forward_comm(int, int, double *) override;

  enum { GPU_FORCE, GPU_NEIGH, GPU_HYB_NEIGH };

 protected:
  void read_file(char *) override;
  void file2array() override;

  int gpu_mode;
  double cpu_time;
  void *fp_pinned;
  bool fp_single;
};

}    // namespace LAMMPS_NS

#endif
#endif
