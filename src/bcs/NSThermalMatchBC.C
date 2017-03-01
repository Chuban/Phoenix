#include "NS.h"
#include "NSThermalMatchBC.h"

template<>
InputParameters validParams<NSThermalMatchBC>()
{
  InputParameters params = validParams<MatchedValueBC>();
  params.addRequiredCoupledVar(NS::density, "density");
  params.addRequiredParam<UserObjectName>("fluid_properties", "The name of the user object for fluid properties");
  return params;
}

NSThermalMatchBC::NSThermalMatchBC(const InputParameters & parameters) :
    MatchedValueBC(parameters),
    _rho_var(coupled(NS::density)),
    _rho(coupledValue(NS::density)),
    _fp(getUserObject<IdealGasFluidProperties>("fluid_properties"))
{
}

Real
NSThermalMatchBC::computeQpResidual()
{
  // For the total energy, the essential BC is:
  // rho*E = rho*(c_v*T + |u|^2/2)
  //
  // or, in residual form, (In general, this BC is coupled to the velocity variables.)
  // rho*E - rho*(c_v*T + |u|^2/2) = 0
  //
  // ***at a no-slip wall*** this further reduces to (no coupling to velocity variables):
  // rho*E - rho*cv*T = 0

  return _u[_qp] - (_rho[_qp] * _fp.cv() * _v[_qp]);
}

Real
NSThermalMatchBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  return _rho[_qp] * _fp.cv() * MatchedValueBC::computeQpOffDiagJacobian(jvar);
}
