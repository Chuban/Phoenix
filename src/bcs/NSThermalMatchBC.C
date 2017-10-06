#include "NS.h"
#include "NSThermalMatchBC.h"

#include "MooseMesh.h"
#include "MooseVariable.h"

template<>
InputParameters validParams<NSThermalMatchBC>()
{
  InputParameters params = validParams<MatchedValueBC>();
  params.addRequiredCoupledVar(NS::density, "density");
  params.addRequiredCoupledVar(NS::momentum_x, "x-momentum");
  params.addCoupledVar(NS::momentum_y, "y-momentum");
  params.addCoupledVar(NS::momentum_z, "z-momentum");
  params.addRequiredParam<UserObjectName>("fluid_properties", "The name of the user object for fluid properties");
  return params;
}

NSThermalMatchBC::NSThermalMatchBC(const InputParameters & parameters) :
    MatchedValueBC(parameters),
    _rho_var(coupled(NS::density)),
    _rho(coupledValue(NS::density)),
    _rhou_var(coupled(NS::momentum_x)),
    _rhou(coupledValue(NS::momentum_x)),
    _rhov_var(_mesh.dimension() >= 2 ? coupled(NS::momentum_y) : libMesh::invalid_uint),
    _rhov(_mesh.dimension() >= 2 ? coupledValue(NS::momentum_y) : _zero),
    _rhow_var(_mesh.dimension() >= 3 ? coupled(NS::momentum_z) : libMesh::invalid_uint),
    _rhow(_mesh.dimension() >= 3 ? coupledValue(NS::momentum_z) : _zero),
    _fp(getUserObject<IdealGasFluidProperties>("fluid_properties"))
{
}

Real
NSThermalMatchBC::computeQpResidual()
{
  // For the total energy, the essential BC is:
  // rho*E = rho*(c_v*T + |u|^2/2)
  //
  // or, in residual form, (In general, this BC is coupled to the velocity/momentum variables.)
  // rho*E - rho*(c_v*T + |u|^2/2) = 0
  // rho*E - rho*c_v*T - 0.5 * |mom|^2 / rho = 0
  //
  // ***at a no-slip wall*** this further reduces to (no coupling to velocity/momentum variables):
  // rho*E - rho*cv*T = 0

  return _u[_qp] - _rho[_qp] * _fp.cv() * _v[_qp] - 0.5 * (_rhou[_qp] * _rhou[_qp] + _rhov[_qp] * _rhov[_qp] + _rhow[_qp] * _rhow[_qp])  / _rho[_qp];
}

Real
NSThermalMatchBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  if (jvar == _rhou_var)
    return -1. * _rhou[_qp] / _rho[_qp];
  if (jvar == _rhov_var)
    return -1. * _rhov[_qp] / _rho[_qp];
  if (jvar == _rhow_var)
    return -1. * _rhow[_qp] / _rho[_qp];

  return _rho[_qp] * _fp.cv() * MatchedValueBC::computeQpOffDiagJacobian(jvar);
}
