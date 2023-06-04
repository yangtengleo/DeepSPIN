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
FixStyle(nve/bpm/sphere,FixNVEBPMSphere);
// clang-format on
#else

#ifndef LMP_FIX_NVE_BPM_SPHERE_H
#define LMP_FIX_NVE_BPM_SPHERE_H

#include "fix_nve.h"

namespace LAMMPS_NS {

class FixNVEBPMSphere : public FixNVE {
 public:
  FixNVEBPMSphere(class LAMMPS *, int, char **);

  void init() override;
  void initial_integrate(int) override;
  void final_integrate() override;

 protected:
  double inertia, inv_inertia;
  int extra;
  int dlm;
};

}    // namespace LAMMPS_NS

#endif
#endif
