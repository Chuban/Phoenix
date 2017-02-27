#include "MooseMesh.h"
#include "NS.h"
#include "NSInterfaceTemperature.h"

// FluidProperties includes
#include "IdealGasFluidProperties.h"

template <> InputParameters validParams<NSInterfaceTemperature>() {
  InputParameters params = validParams<InterfaceKernel>();
  params.addClassDescription("Enforce continuity between a variable and the "
                             "fluid temperature across a subdomain boundary.");
  params.addRequiredCoupledVar(NS::density, "density");
  params.addRequiredCoupledVar(NS::momentum_x, "x-momentum");
  params.addCoupledVar(NS::momentum_y,
                       "y-momentum"); // only required in 2D and 3D
  params.addCoupledVar(NS::momentum_z, "z-momentum"); // only required in 3D
  params.addRequiredCoupledVar(NS::total_energy, "total energy");
  params.addRequiredParam<UserObjectName>(
      "fluid_properties", "The name of the user object for fluid properties");
  return params;
}

NSInterfaceTemperature::NSInterfaceTemperature(
    const InputParameters &parameters)
    : InterfaceKernel(parameters),

      // Coupled variables
      _rho(coupledValue(NS::density)), _rho_u(coupledValue(NS::momentum_x)),
      _rho_v(_mesh.dimension() >= 2 ? coupledValue(NS::momentum_y) : _zero),
      _rho_w(_mesh.dimension() == 3 ? coupledValue(NS::momentum_z) : _zero),
      _rho_E(coupledValue(NS::total_energy)),

      // Coupled gradients
      _grad_rho(coupledGradient(NS::density)),
      _grad_rho_u(coupledGradient(NS::momentum_x)),
      _grad_rho_v(_mesh.dimension() >= 2 ? coupledGradient(NS::momentum_y)
                                         : _grad_zero),
      _grad_rho_w(_mesh.dimension() == 3 ? coupledGradient(NS::momentum_z)
                                         : _grad_zero),
      _grad_rho_E(coupledGradient(NS::total_energy)),

      // FluidProperties UserObject
      _fp(getUserObject<IdealGasFluidProperties>("fluid_properties")) {}

Real NSInterfaceTemperature::computeQpResidual(Moose::DGResidualType type) {

  Real res = _u[_qp] - _u_neighbor[_qp];

  switch (type) {
  case Moose::Element:
    return res * _test[_i][_qp];

  case Moose::Neighbor:
    return res * _test_neighbor[_i][_qp];
  }

  mooseError("Internal error.");
}

Real NSInterfaceTemperature::computeQpJacobian(Moose::DGJacobianType type) {
  return 0;
  switch (type) {
  case Moose::ElementElement:
    return _grad_phi[_j][_qp] * _normals[_qp] * _test[_i][_qp];

  case Moose::NeighborNeighbor:
    return _grad_phi_neighbor[_j][_qp] * _normals[_qp] *
           _test_neighbor[_i][_qp];

  case Moose::ElementNeighbor:
    return _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test[_i][_qp];

  case Moose::NeighborElement:
    return _grad_phi[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
  }
  mooseError("Internal error.");
}
