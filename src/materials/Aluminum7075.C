#include "Aluminum7075.h"

template <> InputParameters validParams<Aluminum7075>() {
  InputParameters params = validParams<ThermalMaterial>();

  return params;
}

Aluminum7075::Aluminum7075(const InputParameters &parameters)
    : ThermalMaterial(parameters) {}

void Aluminum7075::computeQpProperties() {

  // Precompute some powers of temperature for speed.
  const Real T2 = std::pow(_temperature[_qp], 2);
  const Real T3 = std::pow(_temperature[_qp], 3);
  const Real T4 = std::pow(_temperature[_qp], 4);
  const Real T5 = std::pow(_temperature[_qp], 5);

  // Make sure that the temperature is above zero and set defaults.
  ThermalMaterial::checkQpTemperature();
  ThermalMaterial::defaultQpProperties();

  if (_temperature[_qp] <= 116) {
    _thermal_conductivity[_qp] = 77.5554;
    _d_thermal_conductivity_dT[_qp] = 0.;
  } else if (116 < _temperature[_qp] && _temperature[_qp] < 477) {
    _thermal_conductivity[_qp] = 7.820747 + 0.819439 * _temperature[_qp] +
                                 -0.00216484 * T2 + 2.440757e-6 * T3;
    _d_thermal_conductivity_dT[_qp] =
        0.819439 - 0.00432968 * _temperature[_qp] + 7.32227e-6 * T2;
  } else if (477 < _temperature[_qp] && _temperature[_qp] < 700) {
    _thermal_conductivity[_qp] =
        -8.842465 + 0.644486 * _temperature[_qp] + -5.607477e-4 * T2;
    _d_thermal_conductivity_dT[_qp] =
        0.644486 - 0.0011214954 * _temperature[_qp];
  } else {
    _thermal_conductivity[_qp] = 167.531;
    _d_thermal_conductivity_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 116) {
    _specific_heat[_qp] = 572.12;
    _d_specific_heat_dT[_qp] = 0.;
  } else if (116 < _temperature[_qp] && _temperature[_qp] <= 700) {
    _specific_heat[_qp] = 153.4967 + 4.888757 * _temperature[_qp] +
                          -0.0128256 * T2 + 1.626604e-5 * T3 +
                          -7.073173e-9 * T4;
    _d_specific_heat_dT[_qp] = 4.888757 - 0.0256512 * _temperature[_qp] +
                               0.00004879812 * T2 - 2.8292692e-8 * T3;
  } else {
    _specific_heat[_qp] = 1172.07;
    _d_specific_heat_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 20) {
    _density[_qp] = 2754.296 - 0.003592712 * _temperature[_qp];
    _d_density_dT[_qp] = 0.;
  } else if (20 < _temperature[_qp] && _temperature[_qp] <= 700) {
    _density[_qp] = 2753.524 + 0.05647875 * _temperature[_qp] +
                    -0.001127433 * T2 + 2.657999e-6 * T3 + -3.148685e-9 * T4 +
                    1.417919e-12 * T5;
    _d_density_dT[_qp] = 0.05647875 - 0.002254866 * _temperature[_qp] +
                         7.97399e-6 * T2 - 1.259474e-8 * T3 + 7.089595e-12 * T4;
  } else {
    _density[_qp] = 2634.62;
    _d_density_dT[_qp] = 0.;
  }

  _epsilon[_qp] = 0.35;
  _d_epsilon_dT[_qp] = 0.;
}
