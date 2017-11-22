#include "RadiationBC.h"

template<> InputParameters validParams<RadiationBC>()
{
  InputParameters params = validParams<IntegratedBC>();

  params.addParam<MaterialPropertyName>("epsilon", "epsilon", "Emissivity of the boundary");
  params.addParam<Real>("ambient_temp", 300., "Temperature of the ambient radiation field (K)");

  return params;
}

RadiationBC::RadiationBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _epsilon(getMaterialProperty<Real>("epsilon")),
    _stefan_boltzmann(5.670367e-8),
    _env_T(getParam<Real>("ambient_temp"))
{
}

Real RadiationBC::computeQpResidual()
{
  return _test[_i][_qp] * _epsilon[_qp] * _stefan_boltzmann * ( std::pow(_u[_qp], 4) - std::pow(_env_T, 4) );
}

Real RadiationBC::computeQpJacobian()
{
  return _test[_i][_qp] * 4.0 * _epsilon[_qp] * _stefan_boltzmann * std::pow(_u[_qp], 3) * _phi[_j][_qp];
}
