/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   https://www.lammps.org/, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include "../testing/core.h"
#include "info.h"
#include "input.h"
#include "lammps.h"
#include "library.h"
#include "utils.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <cstdio>
#include <mpi.h>

// whether to print verbose output (i.e. not capturing LAMMPS screen output).
bool verbose = false;

using LAMMPS_NS::utils::split_words;

namespace LAMMPS_NS {

#define STRINGIFY(val) XSTR(val)
#define XSTR(val) #val

class ComputeGlobalTest : public LAMMPSTest {
protected:
    void SetUp() override
    {
        testbinary = "ComputeGlobalTest";
        LAMMPSTest::SetUp();
        if (info->has_style("atom", "full")) {
            BEGIN_HIDE_OUTPUT();
            command("variable input_dir index \"" STRINGIFY(TEST_INPUT_FOLDER) "\"");
            command("include \"${input_dir}/in.fourmol\"");
            command("group allwater molecule 3:6");
            command("region half block 0.0 INF INF INF INF INF");
            END_HIDE_OUTPUT();
        }
    }

    double get_scalar(const char *id)
    {
        return *(double *)lammps_extract_compute(lmp, id, LMP_STYLE_GLOBAL, LMP_TYPE_SCALAR);
    }

    double *get_vector(const char *id)
    {
        return (double *)lammps_extract_compute(lmp, id, LMP_STYLE_GLOBAL, LMP_TYPE_VECTOR);
    }

    double **get_array(const char *id)
    {
        return (double **)lammps_extract_compute(lmp, id, LMP_STYLE_GLOBAL, LMP_TYPE_ARRAY);
    }
};

TEST_F(ComputeGlobalTest, Energy)
{
    if (lammps_get_natoms(lmp) == 0.0) GTEST_SKIP();
    int has_tally = lammps_config_has_package("TALLY");

    BEGIN_HIDE_OUTPUT();
    command("pair_style lj/cut/coul/cut 10.0");
    command("pair_coeff * * 0.01 3.0");
    command("bond_style harmonic");
    command("bond_coeff * 100.0 1.5");

    command("compute ke1 all ke");
    command("compute ke2 allwater ke");
    command("compute pe1 all pe");
    command("compute pe2 all pe bond");
    command("compute pe3 all pe angle dihedral");
    command("compute pr1 all pressure thermo_temp");
    command("compute pr2 all pressure NULL virial");
    command("compute pr3 all pressure NULL angle dihedral");
    std::string thermo_style = "c_ke1 c_ke2 c_pe1 c_pe2 c_pe3 c_pr1 c_pr2 c_pr3";

    if (has_tally) {
        command("compute pe4 all pe/tally allwater");
        command("compute pe5 all pe/mol/tally all");
        command("compute pe6 all pe pair");
        thermo_style += " c_pe4 c_pe5[*]";
    }

    command("thermo_style custom " + thermo_style);
    command("run 0 post no");
    END_HIDE_OUTPUT();

    EXPECT_DOUBLE_EQ(get_scalar("ke1"), 2.3405256449146168);
    EXPECT_DOUBLE_EQ(get_scalar("ke2"), 1.192924237073665);
    EXPECT_DOUBLE_EQ(get_scalar("pe1"), 24155.155261642241);
    EXPECT_DOUBLE_EQ(get_scalar("pe2"), 361.37528652881286);
    EXPECT_DOUBLE_EQ(get_scalar("pe3"), 0.0);
    EXPECT_NEAR(get_scalar("pr1"), 1956948.4735454607, 0.0000000005);
    EXPECT_NEAR(get_scalar("pr2"), 1956916.7725807722, 0.0000000005);
    EXPECT_DOUBLE_EQ(get_scalar("pr3"), 0.0);
    auto pr1 = get_vector("pr1");
    auto pr2 = get_vector("pr2");
    auto pr3 = get_vector("pr3");
    EXPECT_NEAR(pr1[0], 2150600.9207200543, 0.0000000005);
    EXPECT_NEAR(pr1[1], 1466949.7512112649, 0.0000000005);
    EXPECT_NEAR(pr1[2], 2253294.7487050635, 0.0000000005);
    EXPECT_NEAR(pr1[3], 856643.16926486336, 0.0000000005);
    EXPECT_NEAR(pr1[4], 692710.86929464422, 0.0000000005);
    EXPECT_NEAR(pr1[5], -44403.909298603547, 0.0000000005);
    EXPECT_NEAR(pr2[0], 2150575.6989334146, 0.0000000005);
    EXPECT_NEAR(pr2[1], 1466911.3911461537, 0.0000000005);
    EXPECT_NEAR(pr2[2], 2253263.2276627473, 0.0000000005);
    EXPECT_NEAR(pr2[3], 856632.34707690508, 0.0000000005);
    EXPECT_NEAR(pr2[4], 692712.89222328411, 0.0000000005);
    EXPECT_NEAR(pr2[5], -44399.277068014424, 0.0000000005);
    EXPECT_DOUBLE_EQ(pr3[0], 0.0);
    EXPECT_DOUBLE_EQ(pr3[1], 0.0);
    EXPECT_DOUBLE_EQ(pr3[2], 0.0);
    EXPECT_DOUBLE_EQ(pr3[3], 0.0);
    EXPECT_DOUBLE_EQ(pr3[4], 0.0);
    EXPECT_DOUBLE_EQ(pr3[5], 0.0);

    if (has_tally) {
        EXPECT_DOUBLE_EQ(get_scalar("pe4"), 15425.840923850392);
        auto pe5 = get_vector("pe5");
        EXPECT_DOUBLE_EQ(pe5[0], 23803.966677151559);
        EXPECT_DOUBLE_EQ(pe5[1], -94.210004432380643);
        EXPECT_DOUBLE_EQ(pe5[2], 115.58040355478101);
        EXPECT_DOUBLE_EQ(pe5[3], -31.557101160514257);
    }

    TEST_FAILURE(".*ERROR: Compute pressure must use group all.*",
                 command("compute pr5 allwater pressure thermo_temp"););
    TEST_FAILURE(".*ERROR: Compute pressure requires temperature ID to include kinetic energy.*",
                 command("compute pr5 all pressure NULL"););
    TEST_FAILURE(".*ERROR: Could not find compute pressure temperature ID",
                 command("compute pr5 all pressure xxx"););

    TEST_FAILURE(".*ERROR: Reuse of compute ID 'pe2'.*", command("compute pe2 all pe"););
    TEST_FAILURE(".*ERROR: Compute pe must use group all.*", command("compute pe allwater pe"););
    TEST_FAILURE(".*ERROR: Illegal compute command.*", command("compute pe potential"););
}

TEST_F(ComputeGlobalTest, Geometry)
{
    if (lammps_get_natoms(lmp) == 0.0) GTEST_SKIP();
    int has_extra = lammps_config_has_package("EXTRA-COMPUTE");

    BEGIN_HIDE_OUTPUT();
    command("pair_style lj/cut 10.0");
    command("pair_coeff * * 0.01 3.0");
    command("bond_style harmonic");
    command("bond_coeff * 100.0 1.5");

    command("compute com1 all com");
    command("compute com2 allwater com");
    command("compute mu1 all dipole");
    command("compute mu2 allwater dipole geometry ");
    command("compute rg1 all gyration");
    command("compute rg2 allwater gyration");
    std::string thermo_style = "c_com1[*] c_com2[*] c_rg1[*] c_rg2[*]";

    if (has_extra) {
        command("compute mom1 all momentum");
        command("compute mom2 allwater momentum");
        command("compute mop1 all stress/mop x 0.0 total");
        command("compute mop2 all stress/mop/profile z lower 0.5 kin conf");
        thermo_style += " c_mu1 c_mu2 c_mop1[*] c_mop2[1][1]";
    }

    command("thermo_style custom " + thermo_style);
    command("run 0 post no");
    END_HIDE_OUTPUT();

    auto com1 = get_vector("com1");
    auto com2 = get_vector("com2");
    auto mu1  = get_vector("mu1");
    auto mu2  = get_vector("mu2");
    auto rg1  = get_vector("rg1");
    auto rg2  = get_vector("rg2");

    EXPECT_DOUBLE_EQ(com1[0], 1.4300952724948282);
    EXPECT_DOUBLE_EQ(com1[1], -0.29759806705328351);
    EXPECT_DOUBLE_EQ(com1[2], -0.7245120195899285);
    EXPECT_DOUBLE_EQ(com2[0], 1.7850913321989679);
    EXPECT_DOUBLE_EQ(com2[1], -0.45168408952146238);
    EXPECT_DOUBLE_EQ(com2[2], -0.60215022088294912);

    EXPECT_DOUBLE_EQ(get_scalar("mu1"), 1.8335537504770163);
    EXPECT_DOUBLE_EQ(get_scalar("mu2"), 1.7849382239204072);
    EXPECT_DOUBLE_EQ(mu1[0], 0.41613191281297729);
    EXPECT_DOUBLE_EQ(mu1[1], 1.0056523085627747);
    EXPECT_DOUBLE_EQ(mu1[2], -1.4756073398127658);
    EXPECT_DOUBLE_EQ(mu2[0], -0.029474795088977768);
    EXPECT_DOUBLE_EQ(mu2[1], 1.153516133030746);
    EXPECT_DOUBLE_EQ(mu2[2], -1.3618135814069394);

    EXPECT_DOUBLE_EQ(get_scalar("rg1"), 3.8495643473797196);
    EXPECT_DOUBLE_EQ(get_scalar("rg2"), 5.4558163385611342);
    EXPECT_DOUBLE_EQ(rg1[0], 3.6747807397432752);
    EXPECT_DOUBLE_EQ(rg1[1], 6.5440303159316278);
    EXPECT_DOUBLE_EQ(rg1[2], 4.6003346089421457);
    EXPECT_DOUBLE_EQ(rg1[3], -0.4639249501367636);
    EXPECT_DOUBLE_EQ(rg1[4], -1.8859032304357459);
    EXPECT_DOUBLE_EQ(rg1[5], 0.2339161878440186);
    EXPECT_DOUBLE_EQ(rg2[0], 6.2582260148310143);
    EXPECT_DOUBLE_EQ(rg2[1], 13.353763805454184);
    EXPECT_DOUBLE_EQ(rg2[2], 10.153942099825425);
    EXPECT_DOUBLE_EQ(rg2[3], 1.2965604701522486);
    EXPECT_DOUBLE_EQ(rg2[4], -5.0315240817290841);
    EXPECT_DOUBLE_EQ(rg2[5], 1.1103378503822141);
    if (has_extra) {
        auto mom1 = get_vector("mom1");
        auto mom2 = get_vector("mom2");
        auto mop1 = get_vector("mop1");
        auto mop2 = get_array("mop2");
        EXPECT_DOUBLE_EQ(mom1[0], 0.0054219056685341164);
        EXPECT_DOUBLE_EQ(mom1[1], -0.054897225112275558);
        EXPECT_DOUBLE_EQ(mom1[2], 0.059097392692385661);
        EXPECT_DOUBLE_EQ(mom2[0], -0.022332069630161717);
        EXPECT_DOUBLE_EQ(mom2[1], -0.056896553865696115);
        EXPECT_DOUBLE_EQ(mom2[2], 0.069179891052881484);
        EXPECT_DOUBLE_EQ(mop1[0], 3522311.3572200728);
        EXPECT_DOUBLE_EQ(mop1[1], 2871104.9055934539);
        EXPECT_DOUBLE_EQ(mop1[2], -4136077.5224247416);
        EXPECT_DOUBLE_EQ(mop2[0][0], -8.0869239999999998);
        EXPECT_DOUBLE_EQ(mop2[0][1], 0.0);
        EXPECT_DOUBLE_EQ(mop2[0][2], 0.0);
        EXPECT_DOUBLE_EQ(mop2[1][0], -7.5869239999999998);
        EXPECT_DOUBLE_EQ(mop2[1][1], 0.0);
        EXPECT_DOUBLE_EQ(mop2[1][2], 0.0);
    }
}

TEST_F(ComputeGlobalTest, Reduction)
{
    if (lammps_get_natoms(lmp) == 0.0) GTEST_SKIP();

    BEGIN_HIDE_OUTPUT();
    command("pair_style lj/cut 10.0");
    command("pair_coeff * * 0.01 3.0");
    command("bond_style harmonic");
    command("bond_coeff * 100.0 1.5");

    command("variable v atom sqrt(vx*vx+vy*vy+vz*vz)");
    command("variable id atom id");
    command("fix chg all store/state 0 q");
    command("compute ke all ke/atom");
    command("compute min allwater reduce min x fx v_v");
    command("compute chg all reduce max f_chg");
    command("compute max all reduce max y fy v_v");
    command("compute ave all reduce/region half ave z fz v_v");
    command("compute sum allwater reduce/region half sum vx vy vz");
    command("compute rep all reduce max v_id v_v v_id y replace 1 2 replace 3 4");
    std::string thermo_style = "c_min[*] c_chg c_max[*] c_sum[*] c_ave[*] c_rep[*]";

    command("thermo_style custom " + thermo_style);
    command("run 0 post no");
    END_HIDE_OUTPUT();

    auto min = get_vector("min");
    auto max = get_vector("max");
    auto sum = get_vector("sum");
    auto ave = get_vector("ave");
    auto rep = get_vector("rep");

    EXPECT_DOUBLE_EQ(get_scalar("chg"), 0.51000000000000001);

    EXPECT_DOUBLE_EQ(min[0], -2.7406520384725965);
    EXPECT_DOUBLE_EQ(min[1], -20385.448391361348);
    EXPECT_DOUBLE_EQ(min[2], 0.00071995632406981081);

    EXPECT_DOUBLE_EQ(max[0], 4.0120175892854135);
    EXPECT_DOUBLE_EQ(max[1], 21193.39005673242);
    EXPECT_DOUBLE_EQ(max[2], 0.0072167889062371513);

    EXPECT_DOUBLE_EQ(sum[0], 0.0021436162503408024);
    EXPECT_DOUBLE_EQ(sum[1], -0.013760203913131267);
    EXPECT_DOUBLE_EQ(sum[2], 0.017517003988402391);

    EXPECT_DOUBLE_EQ(ave[0], -1.3013763067943667);
    EXPECT_DOUBLE_EQ(ave[1], -619.60864441905312);
    EXPECT_DOUBLE_EQ(ave[2], 0.0035263629500884397);

    // index of max v_v
    EXPECT_DOUBLE_EQ(rep[0], 20);
    EXPECT_DOUBLE_EQ(rep[1], max[2]);
    // index of max y
    EXPECT_DOUBLE_EQ(rep[2], 26);
    EXPECT_DOUBLE_EQ(rep[3], max[0]);
}

} // namespace LAMMPS_NS

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    ::testing::InitGoogleMock(&argc, argv);

    if (platform::mpi_vendor() == "Open MPI" && !LAMMPS_NS::Info::has_exceptions())
        std::cout << "Warning: using OpenMPI without exceptions. Death tests will be skipped\n";

    // handle arguments passed via environment variable
    if (const char *var = getenv("TEST_ARGS")) {
        std::vector<std::string> env = split_words(var);
        for (auto arg : env) {
            if (arg == "-v") {
                verbose = true;
            }
        }
    }

    if ((argc > 1) && (strcmp(argv[1], "-v") == 0)) verbose = true;

    int rv = RUN_ALL_TESTS();
    MPI_Finalize();
    return rv;
}
