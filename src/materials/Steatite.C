#include "Steatite.h"

template <> InputParameters validParams<Steatite>() {
  InputParameters params = validParams<ThermalMaterial>();

  return params;
}

Steatite::Steatite(const InputParameters &parameters)
    : ThermalMaterial(parameters) {}

void Steatite::computeQpProperties() {
  // ThermalMaterial::checkQpTemperature();
  ThermalMaterial::defaultQpProperties();
  _thermal_conductivity[_qp] = 3.;
  _specific_heat[_qp] = 750.;
  _density[_qp] = 2830.;
  _epsilon[_qp] = 0.95;
}
