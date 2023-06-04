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

#ifdef COMMAND_CLASS
// clang-format off
CommandStyle(set,Set);
// clang-format on
#else

#ifndef LMP_SET_H
#define LMP_SET_H

#include "command.h"

namespace LAMMPS_NS {

class Set : public Command {
 public:
  Set(class LAMMPS *lmp) : Command(lmp){};
  void command(int, char **) override;

 private:
  char *id;
  int *select;
  int style, ivalue, newtype, count, index_custom, icol_custom;
  int ximage, yimage, zimage, ximageflag, yimageflag, zimageflag;
  int cc_index;
  bigint nsubset;
  double dvalue, xvalue, yvalue, zvalue, wvalue, fraction;

  int varflag, varflag1, varflag2, varflag3, varflag4;
  int ivar1, ivar2, ivar3, ivar4;
  double *vec1, *vec2, *vec3, *vec4;

  int discflag;

  void selection(int);
  void set(int);
  void setrandom(int);
  void topology(int);
  void varparse(const char *, int);
};

}    // namespace LAMMPS_NS

#endif
#endif
