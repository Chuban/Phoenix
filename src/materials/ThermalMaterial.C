#include "ThermalMaterial.h"
#include <iostream>

template <> InputParameters validParams<ThermalMaterial>() {
  InputParameters params = validParams<Material>();

  params.addRequiredCoupledVar("temperature", "Coupled Temperature");

  return params;
}

ThermalMaterial::ThermalMaterial(const InputParameters &parameters)
    : DerivativeMaterialInterface<Material>(parameters),
      _temperature(coupledValue("temperature")),
      _thermal_conductivity(declareProperty<Real>("thermal_conductivity")),
      _d_thermal_conductivity_dT(declarePropertyDerivative<Real>(
          "thermal_conductivity", getVar("temperature", 0)->name())),
      _specific_heat(declareProperty<Real>("specific_heat")),
      _d_specific_heat_dT(declarePropertyDerivative<Real>(
          "specific_heat", getVar("temperature", 0)->name())),
      _density(declareProperty<Real>("density")),
      _d_density_dT(declarePropertyDerivative<Real>(
          "density", getVar("temperature", 0)->name())),
      _epsilon(declareProperty<Real>("epsilon")),
      _d_epsilon_dT(declarePropertyDerivative<Real>(
          "epsilon", getVar("temperature", 0)->name())) {}

void ThermalMaterial::checkQpTemperature() {
  if (_temperature[_qp] < 0) {
    std::stringstream msg;
    msg << "WARNING:  Negative temperature!\n"
        << "\t_qp: " << _qp << "\n"
        << "\ttemp: " << _temperature[_qp] << "\n"
        << "\telem: " << _current_elem->id() << "\n"
        << "\tproc: " << processor_id() << "\n";
    mooseWarning(msg.str());
  }
}

void ThermalMaterial::computeQpProperties() {
  mooseWarning("You forgot to override computeQpProperties().");
}

void ThermalMaterial::defaultQpProperties() {
  _thermal_conductivity[_qp] = 10.;
  _d_thermal_conductivity_dT[_qp] = 0.;
  _specific_heat[_qp] = 1000.;
  _d_specific_heat_dT[_qp] = 0.;
  _density[_qp] = 1000.;
  _d_density_dT[_qp] = 0.;
  _epsilon[_qp] = 0.5;
  _d_epsilon_dT[_qp] = 0.;
}
