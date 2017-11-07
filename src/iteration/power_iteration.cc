#include "power_iteration.h"

template <int dim>
PowerIteration<dim>::PowerIteration (const ParameterHandler &prm)
:
EigenBase<dim> (prm)
{
}

template <int dim>
PowerIteration<dim>::~PowerIteration ()
{
}

template <int dim>
void PowerIteration<dim>::eigen_iterations
(std::vector<Vector<double> > &sflxes_proc,
 std::vector<std_cxx11::shared_ptr<EquationBase<dim> > > &equ_ptrs,
 std_cxx11::shared_ptr<IGBase<dim> > ig_ptr,
 std_cxx11::shared_ptr<MGBase<dim> > mg_ptr)
{
  double err_k = 1.0;
  double err_phi = 1.0;
  int iter = 0;
  while (err_k>this->err_k_tol || err_phi>this->err_phi_tol)
  {
    // update sflxes, fission source and keff from previous fission with current sflxes
    this->update_prev_sflxes_fiss_src_keff (sflxes_proc);
    // scale chi_nu_sigf by a factor of keff
    equ_ptrs.back()->scale_fiss_transfer_matrices (this->keff);
    // assemble fission source as a "fixed source"
    equ_ptrs.back()->assemble_fixed_linear_form (sflxes_proc);
    // perform multigroup iterations
    mg_ptr->mg_iterations (sflxes_proc, equ_ptrs, ig_ptr);
    // calculate fission source and keff thereafter
    this->calculate_fiss_src_keff (sflxes_proc, equ_ptrs.front());
    // calculate errors of QoI for convergence check
    err_phi = this->estimate_phi_diff (sflxes_proc, this->sflxes_proc_prev_eigen);
    err_k = this->estimate_k_diff ();
    // print on screen about the errors
    this->pcout << std::endl << std::endl << "PI iter: " << iter++
    << ", err_k: " << err_k << ", err_phi: " << err_phi << std::endl << std::endl;
  }
}

template class PowerIteration<2>;
template class PowerIteration<3>;
