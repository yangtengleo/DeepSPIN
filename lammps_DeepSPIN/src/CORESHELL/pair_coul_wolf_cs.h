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
PairStyle(coul/wolf/cs,PairCoulWolfCS);
// clang-format on
#else

#ifndef LMP_PAIR_COUL_WOLF_CS_H_
#define LMP_PAIR_COUL_WOLF_CS_H_

#include "pair_coul_wolf.h"

namespace LAMMPS_NS {

class PairCoulWolfCS : public PairCoulWolf {
 public:
  PairCoulWolfCS(class LAMMPS *);
  void compute(int, int) override;
};

}    // namespace LAMMPS_NS

#endif
#endif /* LMP_PAIR_COUL_WOLF_CS_H_ */
