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
  return params;
}

NSThermalFluxInterface::NSThermalFluxInterface(const InputParameters & parameters) :
    InterfaceKernel(parameters),

    // The NS fluid is assumed to be in the neighboring domain.
    _rho_neighbor_var(*getVar(NS::density, 0)),
    _rho_neighbor(_rho_neighbor_var.slnNeighbor()),
    _grad_rho_neighbor(_rho_neighbor_var.gradSlnNeighbor()),
    _fp(getUserObject<IdealGasFluidProperties>("fluid_properties")),
    _kappa_neighbor(getNeighborMaterialProperty<Real>("thermal_conductivity")),

    //  We need the material properties for the local domain.
    _kappa(getMaterialProperty<Real>("thermal_conductivity")),
    _rho(getMaterialProperty<Real>("density")),
    _specific_heat(getMaterialProperty<Real>("specific_heat"))

{
  if (!parameters.isParamValid("boundary"))
  {
    mooseError2("In order to use the InterfaceDiffusion dgkernel, you must specify a boundary where it will live.");
  }
}

Real
NSThermalFluxInterface::computeQpResidual(Moose::DGResidualType type)
{
  // We'll compute the thermal diffusivities from material properties.
  Real D = _kappa[_qp] / (_rho[_qp] * _specific_heat[_qp]);
  Real D_neighbor = _kappa_neighbor[_qp] / (_rho_neighbor[_qp] * _fp.cv());

  // We assume that NS::total_enrgy (rhoE) is setup as the neighboring variable.
  // We want to match the gradient of temperature, not total energy.
  // At a no slip wall (u = 0):
  //   rhoE = rho * cv * T
  //   \grad(rhoE) = rho * cv * \grad(T) + cv * T * \grad(rho)
  //   \grad(T) = (\grad(rhoE) - cv * T * \grad(rho)) / (rho * cv)
  //   \grad(T) = (\grad(rhoE) - rhoE * \grad(rho) / rho) / (rho * cv)
  RealVectorValue gradT = (_grad_neighbor_value[_qp] - _neighbor_value[_qp] * _grad_rho_neighbor[_qp] / _rho_neighbor[_qp]);
  gradT /= _rho_neighbor[_qp] * _fp.cv();
  Real r = 0.5 * (-D * _grad_u[_qp] * _normals[_qp] + -D_neighbor * gradT * _normals[_qp]);

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
  Real D = _kappa[_qp] / (_rho[_qp] * _specific_heat[_qp]);
  Real D_neighbor = _kappa_neighbor[_qp] / (_rho_neighbor[_qp] * _fp.cv());
  RealVectorValue dGradTdNeighbor = (_grad_phi_neighbor[_j][_qp] - _phi_neighbor[_j][_qp] * _grad_rho_neighbor[_qp] / _rho_neighbor[_qp]);
  dGradTdNeighbor /= _rho_neighbor[_qp] * _fp.cv();

  switch (type)
  {
    case Moose::ElementElement:
      jac -= 0.5 * D * _grad_phi[_j][_qp] * _normals[_qp] * _test[_i][_qp];
      break;

    case Moose::NeighborNeighbor:
      jac += 0.5 * D_neighbor * dGradTdNeighbor * _normals[_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::NeighborElement:
      jac += 0.5 * D * _grad_phi[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp];
      break;

    case Moose::ElementNeighbor:
      jac -= 0.5 * D_neighbor * dGradTdNeighbor * _normals[_qp] * _test[_i][_qp];
      break;
  }

  return jac;
}
