#include "../../src/aqdata/aq_base.h"

#include <fstream>

#include <deal.II/base/logstream.h>

template <int dim>
class AQDerivedMock : public AQBase<dim>
{
public:
  AQDerivedMock (dealii::ParameterHandler &prm);
  ~AQDerivedMock ();

  void produce_angular_quad ();
  void initialize_component_index ();
};

template <int dim>
AQDerivedMock<dim>::AQDerivedMock (dealii::ParameterHandler &prm)
    : AQBase<dim> (prm)
{}

template <int dim>
AQDerivedMock<dim>::~AQDerivedMock ()
{}

template <int dim>
void AQDerivedMock<dim>::produce_angular_quad ()
{
  dealii::deallog << "Mocking producing angular quadrature" << std::endl;

  this->wi_ = (dim==2 ? std::vector<double> (4, this->k_pi) :
               std::vector<double> (8, this->k_pi));
  this->omega_i_.resize (this->wi_.size());
  for (int i=0; i<this->wi_.size(); ++i)
    for (int j=0; j<dim; ++j)
      this->omega_i_[i][j] = i;
  this->n_dir_ = this->omega_i_.size();
  this->n_total_ho_vars_ = this->n_dir_ * this->n_group_;
}

template <int dim>
void AQDerivedMock<dim>::initialize_component_index ()
{
  dealii::deallog << "Mocking initializing component index" << std::endl;
}

template <int dim>
void test()
{
  dealii::ParameterHandler prm;

  // mock relevant input strings
  std::string ref_bc = "true";
  std::string transport_model = "mock";
  std::string num_grp = "2";
  std::string n_azi_angle = "2";
  std::string aq_name = "mock";

  prm.declare_entry ("have reflective BC", "true", dealii::Patterns::Bool (), "");
  prm.declare_entry ("transport model", "mock", dealii::Patterns::Anything (), "");
  prm.declare_entry ("angular quadrature order", "2", dealii::Patterns::Integer (), "");
  prm.declare_entry ("angular quadrature name", "mock", dealii::Patterns::Anything (), "");
  prm.declare_entry ("number of groups", "1", dealii::Patterns::Integer (), "");

  // set parameter handler entries with mocking values
  prm.set ("have reflective BC", ref_bc);
  prm.set ("transport model", transport_model);
  prm.set ("angular quadrature order", n_azi_angle);
  prm.set ("number of groups", num_grp);
  prm.set ("angular quadrature name", aq_name);

  std::unique_ptr<AQBase<dim>> aq_mock =
      std::unique_ptr<AQBase<dim>>(new AQDerivedMock<dim>(prm));
  aq_mock->make_aq ();
  auto omega_i = aq_mock->get_all_directions ();
  auto wi = aq_mock->get_angular_weights ();
  dealii::deallog << "SN order: " << aq_mock->get_sn_order () << "; ";
  dealii::deallog << "Total components: " << aq_mock->get_n_total_ho_vars() << std::endl;
  // the first part is get_xxx functionality
  for (int i=0; i<wi.size(); ++i)
  {
    dealii::deallog << "Weight: " << wi[i] << "; Omega: ";
    for (int j=0; j<dim; ++j)
      dealii::deallog << omega_i[i][j] << " ";
    dealii::deallog << std::endl;
  }
}

int main ()
{
  const std::string logname = "output";
  std::ofstream logfile (logname.c_str());
  dealii::deallog.attach (logfile, false);

  // put the test code here
  dealii::deallog.push ("2D");
  test<2> ();
  dealii::deallog.pop ();
  dealii::deallog << std::endl
                  << "++++++++++++++++++++++++++++++++++++++++++"
                  << std::endl << std::endl;
  dealii::deallog.push ("3D");
  test<3> ();
  dealii::deallog.pop ();

  return 0;
}