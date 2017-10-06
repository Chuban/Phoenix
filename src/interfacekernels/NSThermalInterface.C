// MOOSE includes
#include "Assembly.h"
#include "MooseVariable.h"

// libMesh includes
#include "libmesh/quadrature.h"

#include "NS.h"
#include "NSThermalInterface.h"

template<>
InputParameters validParams<NSThermalInterface>()
{
  InputParameters params = validParams<InterfaceKernel>();
  params.addRequiredCoupledVar(NS::density, "The density of the neighboring fluid");
  params.addRequiredParam<UserObjectName>("fluid_properties", "The name of the user object for fluid properties");
  params.addParam<MaterialPropertyName>("thermal_conductivity", "thermal_conductivity", "The thermal conductivity property name");
  params.addParam<MaterialPropertyName>("density", "density", "The density property name for the non-fluid material");
  params.addParam<MaterialPropertyName>("specific_heat", "specific_heat", "The specific heat property name for the non-fluid material");
  return params;
}

NSThermalInterface::NSThermalInterface(const InputParameters & parameters) :
    InterfaceKernel(parameters),

    // The NS fluid is assumed to be in the primary domain.
    _rho(coupledValue(NS::density)),
    _grad_rho(coupledGradient(NS::density)),
    _fp(getUserObject<IdealGasFluidProperties>("fluid_properties")),
    _kappa(getMaterialProperty<Real>("thermal_conductivity")),

    //  We need the material properties for the neighbor domain.
    _kappa_neighbor(getNeighborMaterialProperty<Real>("thermal_conductivity")),
    _rho_neighbor(getNeighborMaterialProperty<Real>("density")),
    _specific_heat_neighbor(getNeighborMaterialProperty<Real>("specific_heat"))

{
  if (!parameters.isParamValid("boundary"))
  {
    mooseError("In order to use the InterfaceDiffusion dgkernel, you must specify a boundary where it will live.");
  }
}

Real NSThermalInterface::computeQpResidual(Moose::DGResidualType type)
{
	if (!_var.activeOnSubdomain(_current_elem->subdomain_id()))
    return 0.;

  // We assume that NS::total_enrgy (rhoE) is setup as the primary variable.

  Real T = _u[_qp] / (_rho[_qp] * _fp.cv());
  Real r = 0.5 * (T + _neighbor_value[_qp]);

  switch (type)
  {
  case Moose::Element:
    return T - r;

  case Moose::Neighbor:
    return _neighbor_value[_qp] - r;
  }

  return 0.;
}

Real NSThermalInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0;
  return jac;
  Real dTdElement = _phi[_j][_qp] / (_rho[_qp] * _fp.cv());

  switch (type)
  {
    case Moose::ElementElement:
      jac -= 0.5 * dTdElement * _test[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac += 0.5 * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborElement:
      jac += 0.5 * dTdElement * _test_neighbor[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      jac -= 0.5 * _phi_neighbor[_j][_qp] * _test[_i][_qp];
      break;
  }

  return jac;
}
