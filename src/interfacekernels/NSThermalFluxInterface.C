// MOOSE includes
#include "Assembly.h"
#include "MooseVariable.h"

// libMesh includes
#include "libmesh/quadrature.h"

#include "NS.h"
#include "NSThermalFluxInterface.h"

template<>
InputParameters validParams<NSThermalFluxInterface>()
{
  InputParameters params = validParams<InterfaceKernel>();
  params.addRequiredCoupledVar(NS::density, "The density of the neighboring fluid");
  params.addRequiredParam<UserObjectName>("fluid_properties", "The name of the user object for fluid properties");
  params.addParam<MaterialPropertyName>("thermal_conductivity", "thermal_conductivity", "The thermal conductivity property name");
  params.addParam<MaterialPropertyName>("density", "density", "The density property name for the non-fluid material");
  params.addParam<MaterialPropertyName>("specific_heat", "specific_heat", "The specific heat property name for the non-fluid material");
  params.addParam<FunctionName>("var_heat_flux_func", "The vector fuction for the heat flux into the variable (W/m^2)");
  params.addParam<FunctionName>("neighbor_heat_flux_func", "The vector function for the heat flux into the neighbor (W/m^2)");
  params.addParam<PostprocessorName>("radiation_temp", 300., "The temperature of the radiation field (K)");
  return params;
}

NSThermalFluxInterface::NSThermalFluxInterface(const InputParameters & parameters) :
  InterfaceKernel(parameters),
  PostprocessorInterface(this),

  // The NS fluid is assumed to be in the primary domain.
  _rho(coupledValue(NS::density)),
  _grad_rho(coupledGradient(NS::density)),
  _fp(getUserObject<IdealGasFluidProperties>("fluid_properties")),
  _kappa(getMaterialProperty<Real>("thermal_conductivity")),
  _epsilon(getMaterialProperty<Real>("epsilon")),

  //  We need the material properties for the neighbor domain.
  _kappa_neighbor(getNeighborMaterialProperty<Real>("thermal_conductivity")),
  _epsilon_neighbor(getNeighborMaterialProperty<Real>("epsilon")),

  _var_flux_func(getFunction("var_heat_flux_func")),
  _neighbor_flux_func(getFunction("neighbor_heat_flux_func")),

  _stefan_boltzmann(5.670367e-8),
  _rad_T(hasPostprocessorByName(getParam<PostprocessorName>("radiation_temp")) ? getPostprocessorValueByName(getParam<PostprocessorName>("radiation_temp"))
                                                                               : getDefaultPostprocessorValue("radiation_temp"))

{
  if (!parameters.isParamValid("boundary"))
  {
    mooseError("In order to use the InterfaceDiffusion dgkernel, you must specify a boundary where it will live.");
  }
}

Real
NSThermalFluxInterface::computeQpResidual(Moose::DGResidualType type)
{
  // We assume that NS::total_enrgy (rhoE) is setup as the primary variable.
  // We want to match the gradient of temperature, not total energy.
  // At a no slip wall (u = 0):
  //   rhoE = rho * cv * T
  //   \grad(rhoE) = rho * cv * \grad(T) + cv * T * \grad(rho)
  //   \grad(T) = (\grad(rhoE) - cv * T * \grad(rho)) / (rho * cv)
  //   \grad(T) = (\grad(rhoE) - rhoE * \grad(rho) / rho) / (rho * cv)

  // This kernel touches each element on the boundary twice: once as an Element and once as a Neighbor.
  // The VariableValue references do NOT seem to update.
  //  - "_u" and "_grad_u" ALWAYS point to the fluid energy density which does not exist in the solid.
  //  - "_neighbor_value" and "_grad_neighbor_value" ALWAYS point to the solid temperature which does not exist in the fluid.
  // Material properties DO seem to update.
  //  - "_kappa" ALWAYS points to the thermal conductivity of the cell called Element.
  //  - "_kappa_neighbor" ALWAYS points to the thermal conductivity of the cell called Neighbor.
  // The residual values should be:
  //  - For Moose::Element  -> +1.0 * average conductive heat flux * _test          + 0.5 * external input heat flux * _test
  //  - For Moose::Neighbor -> -1.0 * average conductive heat flux * _test_neighbor + 0.5 * external input heat flux * _test_neighbor
  // Remember that the normal vector switches.
  // Heat fluxes are computed out of the current element and into of the neighbor.

  // Since VariableValue references to NOT update, this is always the gradient of the fluid temperature.
  RealVectorValue gradFluidT = _grad_u[_qp] - _u[_qp] * _grad_rho[_qp] / _rho[_qp];
  gradFluidT /= _rho[_qp] * _fp.cv();

  // These are the heat fluxes.
  Real fluidHeatFlux;
  Real fluidExternalHeatFlux;
  Real solidHeatFlux;
  Real solidExternalHeatFlux;
  Real elementExternalHeatFlux;
  Real neighborExternalHeatFlux;
  if( _var.activeOnSubdomain(_current_elem->subdomain_id()) )
  {
    fluidHeatFlux = -1.0 * _kappa[_qp] * gradFluidT * _normals[_qp];
    solidHeatFlux = -1.0 * _kappa_neighbor[_qp] * _grad_neighbor_value[_qp] * _normals[_qp];

    fluidExternalHeatFlux = -1.0 * _var_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp];
    solidExternalHeatFlux = -1.0 * _neighbor_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp] +
                            _epsilon_neighbor[_qp] * _stefan_boltzmann * ( std::pow(_neighbor_value[_qp], 4) - std::pow(_rad_T, 4) );
    
    elementExternalHeatFlux  = fluidExternalHeatFlux;
    neighborExternalHeatFlux = solidExternalHeatFlux;
  }
  else
  {
    fluidHeatFlux = -1.0 * _kappa_neighbor[_qp] * gradFluidT * _normals[_qp];
    solidHeatFlux = -1.0 * _kappa[_qp] * _grad_neighbor_value[_qp] * _normals[_qp];

    fluidExternalHeatFlux = -1.0 * _var_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp];
    solidExternalHeatFlux = -1.0 * _neighbor_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp] +
                            _epsilon[_qp] * _stefan_boltzmann * ( std::pow(_neighbor_value[_qp], 4) - std::pow(_rad_T, 4) );
    
    elementExternalHeatFlux =  solidExternalHeatFlux;
    neighborExternalHeatFlux = fluidExternalHeatFlux;
  }

  // This is the average conductive heat flux.
  Real r = 0.5 * (fluidHeatFlux - fluidExternalHeatFlux + solidHeatFlux - solidExternalHeatFlux);

  switch (type)
  {
		case Moose::Element:
		  r += 0.5 * elementExternalHeatFlux;
		  r *= _test[_i][_qp];
		  break;

		case Moose::Neighbor:
		  r -= 0.5 * neighborExternalHeatFlux;
		  r *= -_test_neighbor[_i][_qp];
		  break;
  }

  return r;
}

Real
NSThermalFluxInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0.;

  RealVectorValue dGradFluidTdrhoE = (_grad_phi[_j][_qp] - _phi[_j][_qp] * _grad_rho[_qp] / _rho[_qp]);
  dGradFluidTdrhoE /= _rho[_qp] * _fp.cv();

	if( _var.activeOnSubdomain(_current_elem->subdomain_id()) )
  {
		switch (type)
		{
			case Moose::ElementElement:
			  jac -= 0.5 * _kappa[_qp] * dGradFluidTdrhoE * _normals[_qp] * _test[_i][_qp];
			  break;

			case Moose::NeighborNeighbor:
			  jac += 0.5 * _kappa_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
        jac += 4.0 * _epsilon_neighbor[_qp] * _stefan_boltzmann * std::pow(_neighbor_value[_qp], 3) * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];
			  break;

			case Moose::NeighborElement:
			  jac += 0.5 * _kappa[_qp] * dGradFluidTdrhoE * _normals[_qp] * _test_neighbor[_i][_qp];
			  break;

			case Moose::ElementNeighbor:
			  jac -= 0.5 * _kappa_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test[_i][_qp];
        jac -= 2.0 * _epsilon_neighbor[_qp] * _stefan_boltzmann * std::pow(_neighbor_value[_qp], 3) * _phi_neighbor[_j][_qp] * _test[_i][_qp]; 
			  break;
		}
  }
  else
  {
    switch (type)
		{
			case Moose::ElementElement:
			  jac -= 0.5 * _kappa[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test[_i][_qp];
			  break;

			case Moose::NeighborNeighbor:
			  jac += 0.5 * _kappa_neighbor[_qp] * dGradFluidTdrhoE * _normals[_qp] * _test_neighbor[_i][_qp];
			  break;

			case Moose::NeighborElement:
			  jac += 0.5 * _kappa[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
        jac += 2.0 * _epsilon[_qp] * _stefan_boltzmann * std::pow(_neighbor_value[_qp], 3) * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];
			  break;

			case Moose::ElementNeighbor:
			  jac -= 0.5 * _kappa_neighbor[_qp] * dGradFluidTdrhoE * _normals[_qp] * _test[_i][_qp];
			  break;
		}
  }

  return jac;
}
