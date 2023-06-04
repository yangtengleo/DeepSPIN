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

#ifdef FIX_CLASS
// clang-format off
FixStyle(STORE,FixStore);
// clang-format on
#else

#ifndef LMP_FIX_STORE_H
#define LMP_FIX_STORE_H

#include "fix.h"

namespace LAMMPS_NS {

class FixStore : public Fix {
 public:
  int nrow, ncol;     // size of global data array
  int nvalues;        // number of per-atom values
  double *vstore;     // vector storage for GLOBAL or PERATOM
  double **astore;    // array storage for GLOBAL or PERATOM
  int disable;        // 1 if operations (except grow) are currently disabled

  FixStore(class LAMMPS *, int, char **);
  ~FixStore() override;
  int setmask() override;
  void reset_global(int, int);

  void write_restart(FILE *) override;
  void restart(char *) override;

  void grow_arrays(int) override;
  void copy_arrays(int, int, int) override;
  int pack_exchange(int, double *) override;
  int unpack_exchange(int, double *) override;
  int pack_restart(int, double *) override;
  void unpack_restart(int, int) override;
  int size_restart(int) override;
  int maxsize_restart() override;

  double memory_usage() override;

 private:
  int flavor;     // GLOBAL or PERATOM
  int vecflag;    // 1 if ncol=1 or nvalues=1

  double *rbuf;    // restart buffer for GLOBAL vec/array
};

}    // namespace LAMMPS_NS

#endif
#endif
