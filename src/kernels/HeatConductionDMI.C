/************************************************************/
/* Modified from HeatConduction.C in the heat_conduction    */
/*  module to make use of the derivative material interface.*/
/*  Note that conductivity is assumed to vary with          */
/*  temperature only.                                       */
/************************************************************/

#include "HeatConductionDMI.h"
#include "MooseMesh.h"

template <> InputParameters validParams<HeatConductionKernelDMI>() {
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription(
      "Compute thermal conductivity using the derivative material interface");
  params.addParam<MaterialPropertyName>(
      "thermal_conductivity", "thermal_conductivity",
      "Property name of the diffusivity (Default: thermal_conductivity)");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

HeatConductionKernelDMI::HeatConductionKernelDMI(
    const InputParameters &parameters)
    : DerivativeMaterialInterface<Diffusion>(parameters),
      _diffusion_coefficient(getMaterialProperty<Real>("thermal_conductivity")),
      _d_diffusion_coefficient_dT(getMaterialPropertyDerivative<Real>(
          "thermal_conductivity", _var.name())) {}

Real HeatConductionKernelDMI::computeQpResidual() {
  return _diffusion_coefficient[_qp] * Diffusion::computeQpResidual();
}

Real HeatConductionKernelDMI::computeQpJacobian() {
  Real jac = _diffusion_coefficient[_qp] * Diffusion::computeQpJacobian();
  jac += _d_diffusion_coefficient_dT[_qp] * _phi[_j][_qp] *
         Diffusion::computeQpResidual();
  return jac;
}
