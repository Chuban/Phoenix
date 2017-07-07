#include "ResistiveThermalInterface.h"

template<> InputParameters validParams<ResistiveThermalInterface>()
{
  InputParameters params = validParams<InterfaceKernel>();
  params.addParam<MaterialPropertyName>("thermal_conductivity", "thermal_conductivity", "The thermal conductivity property name");
  return params;
}

ResistiveThermalInterface::ResistiveThermalInterface(const InputParameters & parameters) :
    InterfaceKernel(parameters),
    _k(getMaterialProperty<Real>("thermal_conductivity")),
    _k_neighbor(getNeighborMaterialProperty<Real>("thermal_conductivity"))
{
}

Real ResistiveThermalInterface::computeQpResidual(Moose::DGResidualType type)
{
  Real r = (_k[_qp] * _grad_u[_qp] - _k_neighbor[_qp] * _grad_u_neighbor[_qp]) * _normals[_qp];
  r = (_u[_qp] - _u_neighbor[_qp] - 25.);

  switch (type)
  {
  case Moose::Element:
    r *= 0.5 * _test[_i][_qp];
    break;

  case Moose::Neighbor:
    r *= -0.5 * _test_neighbor[_i][_qp];
    break;
  }
  mooseWarning(_current_elem->id(), " ", r);

  return r;
}

Real ResistiveThermalInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0.;  

  switch (type)
  {
    case Moose::ElementElement:
      //jac += _k[_qp] * _grad_phi[_j][_qp] * _normals[_qp];
      jac += _phi[_j][_qp];
      jac *= 0.5 * _test[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      //jac -= _k_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp];
      jac -= _phi_neighbor[_j][_qp];
      jac *= -0.5 * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborElement:
      //jac -= _k[_qp] * _grad_phi[_j][_qp] * _normals[_qp];
      jac -= _phi_neighbor[_j][_qp];
      jac *= -0.5 * _test_neighbor[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      //jac += _k_neighbor[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp];
      jac += _phi[_j][_qp];
      jac *= 0.5 * _test[_i][_qp];
      break;
  }

  return jac;
}
