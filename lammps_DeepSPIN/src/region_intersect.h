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

#ifdef REGION_CLASS
// clang-format off
RegionStyle(intersect,RegIntersect);
// clang-format on
#else

#ifndef LMP_REGION_INTERSECT_H
#define LMP_REGION_INTERSECT_H

#include "region.h"

namespace LAMMPS_NS {

class RegIntersect : public Region {
 public:
  RegIntersect(class LAMMPS *, int, char **);
  ~RegIntersect() override;
  void init() override;
  int inside(double, double, double) override;
  int surface_interior(double *, double) override;
  int surface_exterior(double *, double) override;
  void shape_update() override;
  void pretransform() override;
  void set_velocity() override;
  void length_restart_string(int &) override;
  void write_restart(FILE *) override;
  int restart(char *, int &) override;
  void reset_vel() override;

 private:
  char **idsub;
};

}    // namespace LAMMPS_NS

#endif
#endif
