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
  return params;
}

NSThermalFluxInterface::NSThermalFluxInterface(const InputParameters & parameters) :
  InterfaceKernel(parameters),

  // The NS fluid is assumed to be in the primary domain.
  _rho(coupledValue(NS::density)),
  _grad_rho(coupledGradient(NS::density)),
  _fp(getUserObject<IdealGasFluidProperties>("fluid_properties")),
  _kappa(getMaterialProperty<Real>("thermal_conductivity")),

  //  We need the material properties for the neighbor domain.
  _kappa_neighbor(getNeighborMaterialProperty<Real>("thermal_conductivity")),
  _rho_neighbor(getNeighborMaterialProperty<Real>("density")),
  _specific_heat_neighbor(getNeighborMaterialProperty<Real>("specific_heat")),

  _var_flux_func(getFunction("var_heat_flux_func")),
  _neighbor_flux_func(getFunction("neighbor_heat_flux_func"))

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

  RealVectorValue gradT = _grad_u[_qp] - _u[_qp] * _grad_rho[_qp] / _rho[_qp];
  gradT /= _rho[_qp] * _fp.cv();

  // Find the average heat flux between the elements.
  // Don't forget to remove any external heat sources.
  Real qVar = -1. * _kappa[_qp] * gradT * _normals[_qp];																		// Flux into the current element.
  Real qNeighbor = -1. * _kappa_neighbor[_qp] * _grad_neighbor_value[_qp] * _normals[_qp];  // Flux out of the neighbor element.

  // External heat fluxes.
  Real elementHeatFlux;
  Real neighborHeatFlux;
  if(_var.activeOnSubdomain(_current_elem->subdomain_id()))
  {
    elementHeatFlux = _var_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp];
    neighborHeatFlux = _neighbor_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp];
  }
  else
  {
    elementHeatFlux = _neighbor_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp];
    neighborHeatFlux = _var_flux_func.vectorValue(_t, _q_point[_qp]) * _normals[_qp];
  }

  Real r = 0.5 * (qVar - elementHeatFlux + qNeighbor - neighborHeatFlux);

  switch (type)
  {
		case Moose::Element:
		  r -= elementHeatFlux;
		  r *= _test[_i][_qp];
		  break;

		case Moose::Neighbor:
		  r += neighborHeatFlux;
		  r *= -_test_neighbor[_i][_qp];
		  break;
  }

  return r;
}

Real
NSThermalFluxInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0;
  RealVectorValue dGradTdElement = (_grad_phi[_j][_qp] - _phi[_j][_qp] * _grad_rho[_qp] / _rho[_qp]);
  dGradTdElement /= _rho[_qp] * _fp.cv();

  switch (type)
  {
    case Moose::ElementElement:
      jac += 0.5 * _kappa[_qp] * dGradTdElement * _normals[_qp] * _test[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac -= 0.5 * _kappa_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborElement:
      jac -= 0.5 * _kappa[_qp] * dGradTdElement * _normals[_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      jac += 0.5 * _kappa_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test[_i][_qp];
      break;
  }

  return jac;
}
