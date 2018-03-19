#include "../bart_builder.h"

#include "gtest/gtest.h"

class BARTBuilderTest : public ::testing::Test {
 protected:
  void SetUp () {
    prm.declare_entry ("do nda", "true", dealii::Patterns::Bool(), "");
    prm.declare_entry ("finite element polynomial degree", "1",
                       dealii::Patterns::Integer(), "");
    prm.declare_entry ("ho spatial discretization", "",
                       dealii::Patterns::Anything(), "");
    prm.declare_entry ("nda spatial discretization", "",
                       dealii::Patterns::Anything(), "");
  }

  template <int dim>
  void FEBuilderTest ();

  dealii::ParameterHandler prm;
};

template <int dim>
void BARTBuilderTest::FEBuilderTest () {
  BARTBuilder<dim> builders (prm);

  // set values to parameters
  prm.set ("ho spatial discretization", "cfem");
  prm.set ("nda spatial discretization", "cfem");
  builders.SetParams (prm);
  std::vector<dealii::FiniteElement<dim, dim>*> fe_ptrs;
  builders.BuildFESpaces (fe_ptrs);

  // testing for FE names
  EXPECT_EQ (fe_ptrs.front()->get_name(),
      "FE_Q<"+dealii::Utilities::int_to_string(dim)+">(1)");
  EXPECT_EQ (fe_ptrs.back()->get_name(),
      "FE_Q<"+dealii::Utilities::int_to_string(dim)+">(1)");

  // changing FE types
  prm.set ("ho spatial discretization", "dfem");
  prm.set ("nda spatial discretization", "dfem");
  builders.SetParams (prm);
  fe_ptrs.clear ();
  builders.BuildFESpaces (fe_ptrs);
  EXPECT_EQ (fe_ptrs.front()->get_name(),
      "FE_DGQ<"+dealii::Utilities::int_to_string(dim)+">(1)");
  EXPECT_EQ (fe_ptrs.back()->get_name(),
      "FE_DGQ<"+dealii::Utilities::int_to_string(dim)+">(1)");

  // changing NDA FE type
  prm.set ("nda spatial discretization", "cmfd");
  builders.SetParams (prm);
  fe_ptrs.clear ();
  builders.BuildFESpaces (fe_ptrs);
  EXPECT_EQ (fe_ptrs.back()->get_name(),
      "FE_DGQ<"+dealii::Utilities::int_to_string(dim)+">(0)");

  // changing NDA FE type
  prm.set ("nda spatial discretization", "rtk");
  builders.SetParams (prm);
  fe_ptrs.clear ();
  builders.BuildFESpaces (fe_ptrs);
  EXPECT_EQ (fe_ptrs.back()->get_name(),
      "FE_RaviartThomas<"+dealii::Utilities::int_to_string(dim)+">(1)");
}

TEST_F(BARTBuilderTest, FEBuilder_Test) {
  FEBuilderTest<2>();
  FEBuilderTest<3>();
}