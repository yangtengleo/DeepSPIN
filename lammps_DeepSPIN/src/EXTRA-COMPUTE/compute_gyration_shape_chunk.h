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
ComputeStyle(gyration/shape/chunk,ComputeGyrationShapeChunk);
// clang-format on
#else

#ifndef LMP_COMPUTE_GYRATION_SHAPE_CHUNK_H
#define LMP_COMPUTE_GYRATION_SHAPE_CHUNK_H

#include "compute.h"

namespace LAMMPS_NS {

class ComputeGyrationShapeChunk : public Compute {
 public:
  char *id_gyration_chunk;    // fields accessed by other classes

  ComputeGyrationShapeChunk(class LAMMPS *, int, char **);
  ~ComputeGyrationShapeChunk() override;
  void init() override;
  void setup() override;
  void compute_array() override;

  int lock_length() override;

  double memory_usage() override;

 private:
  int current_nchunks, former_nchunks;
  int firstflag;
  double **shape_parameters;
  class Compute *c_gyration_chunk;

  void allocate();
};

}    // namespace LAMMPS_NS

#endif
#endif
