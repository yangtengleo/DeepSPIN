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
PairStyle(dpd/ext/tstat/kk,PairDPDExtTstatKokkos<LMPDeviceType>);
PairStyle(dpd/ext/tstat/kk/device,PairDPDExtTstatKokkos<LMPDeviceType>);
PairStyle(dpd/ext/tstat/kk/host,PairDPDExtTstatKokkos<LMPHostType>);
// clang-format off
#else

#ifndef LMP_PAIR_DPD_EXT_TSTAT_KOKKOS_H
#define LMP_PAIR_DPD_EXT_TSTAT_KOKKOS_H

#include "pair_dpd_ext_tstat.h"
#include "pair_kokkos.h"
#include "kokkos_type.h"

#if !defined(DPD_USE_RAN_MARS) && !defined(DPD_USE_Random_XorShift64) && !defined(Random_XorShift1024)
#define DPD_USE_Random_XorShift64
#endif

#ifdef DPD_USE_RAN_MARS
#include "rand_pool_wrap_kokkos.h"
#else
#include "Kokkos_Random.hpp"
#endif

namespace LAMMPS_NS {

template<class DeviceType>
class PairDPDExtTstatKokkos : public PairDPDExtTstat {
 public:
  typedef DeviceType device_type;
  typedef ArrayTypes<DeviceType> AT;
  typedef EV_FLOAT value_type;

  PairDPDExtTstatKokkos(class LAMMPS*);
  ~PairDPDExtTstatKokkos() override;

  void allocate() override;

  void init_style() override;
  double init_one(int i, int j) override;
  void compute(int, int) override;

  struct params_dpd {
    KOKKOS_INLINE_FUNCTION
    params_dpd() {cut=ws=wsT=gamma=sigma=gammaT=sigmaT=0;}
    KOKKOS_INLINE_FUNCTION
    params_dpd(int /*i*/) {cut=ws=wsT=gamma=sigma=gammaT=sigmaT=0;}
    F_FLOAT cut,ws,wsT,gamma,sigma,gammaT,sigmaT;
  };

  template<int NEIGHFLAG, int VFLAG>
  struct TagDPDExtTstatKokkos{};

  template<int NEIGHFLAG, int VFLAG>
  KOKKOS_INLINE_FUNCTION
  void operator () (TagDPDExtTstatKokkos<NEIGHFLAG,VFLAG>, const int &i) const;

  template<int NEIGHFLAG, int VFLAG>
  KOKKOS_INLINE_FUNCTION
  void operator () (TagDPDExtTstatKokkos<NEIGHFLAG,VFLAG>, const int &i, EV_FLOAT&) const;

  template<int NEIGHFLAG>
  KOKKOS_INLINE_FUNCTION
  void v_tally_xyz(EV_FLOAT &ev, const int &i, const int &j,
      const F_FLOAT &fx,const F_FLOAT &fy, const F_FLOAT &fz,
      const F_FLOAT &delx,const F_FLOAT &dely, const F_FLOAT &delz) const;
 private:
  double special_lj[4];
  int eflag,vflag;
  int neighflag,nlocal;
  double dtinvsqrt;

  int need_dup;

  using KKDeviceType = typename KKDevice<DeviceType>::value;

  template<typename DataType, typename Layout>
  using DupScatterView = KKScatterView<DataType, Layout, KKDeviceType, KKScatterSum, KKScatterDuplicated>;

  template<typename DataType, typename Layout>
  using NonDupScatterView = KKScatterView<DataType, Layout, KKDeviceType, KKScatterSum, KKScatterNonDuplicated>;

  DupScatterView<F_FLOAT*[3], typename DAT::t_f_array::array_layout> dup_f;
  DupScatterView<F_FLOAT*[6], typename DAT::t_virial_array::array_layout> dup_vatom;
  NonDupScatterView<F_FLOAT*[3], typename DAT::t_f_array::array_layout> ndup_f;
  NonDupScatterView<F_FLOAT*[6], typename DAT::t_virial_array::array_layout> ndup_vatom;

#ifdef DPD_USE_RAN_MARS
  RandPoolWrap rand_pool;
  typedef RandWrap rand_type;
#elif defined(DPD_USE_Random_XorShift64)
  Kokkos::Random_XorShift64_Pool<DeviceType> rand_pool;
  typedef typename Kokkos::Random_XorShift64_Pool<DeviceType>::generator_type rand_type;
#elif defined(DPD_USE_Random_XorShift1024)
  Kokkos::Random_XorShift1024_Pool<DeviceType> rand_pool;
  typedef typename Kokkos::Random_XorShift1024_Pool<DeviceType>::generator_type rand_type;
#endif
  typename AT::t_x_array_randomread x;
  typename AT::t_x_array_randomread v;
  typename AT::t_f_array f;
  typename AT::t_int_1d_randomread type;

  typename AT::t_neighbors_2d d_neighbors;
  typename AT::t_int_1d_randomread d_ilist;
  typename AT::t_int_1d_randomread d_numneigh;

  typename AT::tdual_ffloat_2d k_cutsq;
  typename AT::t_ffloat_2d d_cutsq;

  Kokkos::DualView<params_dpd**,Kokkos::LayoutRight,DeviceType> k_params;
  typename Kokkos::DualView<params_dpd**,
    Kokkos::LayoutRight,DeviceType>::t_dev_const_um params;

  DAT::tdual_virial_array k_vatom;
  typename AT::t_virial_array d_vatom;

  KOKKOS_INLINE_FUNCTION
  int sbmask(const int& j) const;
  friend void pair_virial_fdotr_compute<PairDPDExtTstatKokkos>(PairDPDExtTstatKokkos*);

};
}
#endif
#endif
