// clang-format off
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

/* ----------------------------------------------------------------------
   Contributing author: W. Michael Brown (Intel)
------------------------------------------------------------------------- */

#ifdef FIX_CLASS
// clang-format off
FixStyle(nve/asphere/intel,FixNVEAsphereIntel);
// clang-format on
#else

#ifndef LMP_FIX_NVE_ASPHERE_INTEL_H
#define LMP_FIX_NVE_ASPHERE_INTEL_H

#include "fix_nve.h"

namespace LAMMPS_NS {

class FixNVEAsphereIntel : public FixNVE {
 public:
  FixNVEAsphereIntel(class LAMMPS *, int, char **);
  void init() override;
  void setup(int vflag) override;
  void initial_integrate(int) override;
  void final_integrate() override;
  void reset_dt() override;
  double memory_usage() override;

 private:
  double *_dtfm, *_inertia0, *_inertia1, *_inertia2;
  int _nlocal3, _nlocal_max;
  double dtq;
  class AtomVecEllipsoid *avec;
};

}    // namespace LAMMPS_NS
#endif
#endif
