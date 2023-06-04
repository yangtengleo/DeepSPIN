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
PairStyle(morse,PairMorse);
// clang-format on
#else

#ifndef LMP_PAIR_MORSE_H
#define LMP_PAIR_MORSE_H

#include "pair.h"

namespace LAMMPS_NS {

class PairMorse : public Pair {
 public:
  PairMorse(class LAMMPS *);
  ~PairMorse() override;
  void compute(int, int) override;

  void settings(int, char **) override;
  void coeff(int, char **) override;
  double init_one(int, int) override;
  void write_restart(FILE *) override;
  void read_restart(FILE *) override;
  void write_restart_settings(FILE *) override;
  void read_restart_settings(FILE *) override;
  void write_data(FILE *) override;
  void write_data_all(FILE *) override;
  double single(int, int, int, int, double, double, double, double &) override;
  void *extract(const char *, int &) override;

 protected:
  double cut_global;
  double **cut;
  double **d0, **alpha, **r0;
  double **morse1;
  double **offset;

  virtual void allocate();
};

}    // namespace LAMMPS_NS

#endif
#endif
