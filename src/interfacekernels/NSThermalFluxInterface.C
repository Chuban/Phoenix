#include "NS.h"
#include "NSThermalFluxInterface.h"

template<>
InputParameters validParams<NSThermalFluxInterface>()
{
  InputParameters params = validParams<InterfaceKernel>();
  params.addParam<Real>("D",1.,"The diffusion coefficient.");
  params.addParam<Real>("D_neighbor",1.,"The neighboring diffusion coefficient.");
  params.addRequiredCoupledVar(NS::density, "density");
  params.addRequiredParam<UserObjectName>("fluid_properties", "The name of the user object for fluid properties");
  return params;
}

NSThermalFluxInterface::NSThermalFluxInterface(const InputParameters & parameters) :
    InterfaceKernel(parameters),
    _D(getParam<Real>("D")),
    _D_neighbor(getParam<Real>("D_neighbor")),
    // _rho is assumed to be in the neighbor domain.
    _rho_var(*getVar(NS::density, 0)),
    _rho(_rho_var.slnNeighbor()),
    _grad_rho(_rho_var.gradSlnNeighbor()),
    _fp(getUserObject<IdealGasFluidProperties>("fluid_properties"))
{
  if (!parameters.isParamValid("boundary"))
  {
    mooseError2("In order to use the InterfaceDiffusion dgkernel, you must specify a boundary where it will live.");
  }
}

Real
NSThermalFluxInterface::computeQpResidual(Moose::DGResidualType type)
{
  // We assume that NS::total_enrgy (rhoE) is setup as the neighboring variable.
  // We want to match the gradient of temperature, not total energy.
  // At a no slip wall (u = 0):
  //   rhoE = rho * cv * T
  //   \grad(rhoE) = rho * cv * \grad(T) + cv * T * \grad(rho)
  //   \grad(T) = (\grad(rhoE) - cv * T * \grad(rho)) / (rho * cv)
  //   \grad(T) = (\grad(rhoE) - rhoE * \grad(rho) / rho) / (rho * cv)
  RealVectorValue gradT = (_grad_neighbor_value[_qp] - _neighbor_value[_qp] * _grad_rho[_qp] / _rho[_qp]);
  gradT /= _rho[_qp] * _fp.cv();
  Real r = 0.5 * (-_D * _grad_u[_qp] * _normals[_qp] + -_D_neighbor * gradT * _normals[_qp]);

  switch (type)
  {
  case Moose::Element:
    r *= _test[_i][_qp];
    break;

  case Moose::Neighbor:
    r *= -_test_neighbor[_i][_qp];
    break;
  }

  return r;
}

Real
NSThermalFluxInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0;
  RealVectorValue dGradTdNeighbor = (_grad_phi_neighbor[_j][_qp] - _phi_neighbor[_j][_qp] * _grad_rho[_qp] / _rho[_qp]);
  dGradTdNeighbor /= _rho[_qp] * _fp.cv();

  switch (type)
  {
    case Moose::ElementElement:
      jac -= 0.5 * _D * _grad_phi[_j][_qp] * _normals[_qp] * _test[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac += 0.5 * _D_neighbor * dGradTdNeighbor * _normals[_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborElement:
      jac += 0.5 * _D * _grad_phi[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      jac -= 0.5 * _D_neighbor * dGradTdNeighbor * _normals[_qp] * _test[_i][_qp];
      break;
  }

  return jac;
}
