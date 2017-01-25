#include "Aluminum2024.h"
#include <iostream>

template <> InputParameters validParams<Aluminum2024>() {
  InputParameters params = validParams<ThermalMaterial>();

  return params;
}

Aluminum2024::Aluminum2024(const InputParameters &parameters)
    : ThermalMaterial(parameters) {}

void Aluminum2024::computeQpProperties() {

  // Precompute some powers of temperature for speed.
  const Real T2 = std::pow(_temperature[_qp], 2);
  const Real T3 = std::pow(_temperature[_qp], 3);
  const Real T4 = std::pow(_temperature[_qp], 4);

  // Make sure that the temperature is above zero and set defaults.
  ThermalMaterial::checkQpTemperature();
  ThermalMaterial::defaultQpProperties();

  if (_temperature[_qp] <= 4) {
    _thermal_conductivity[_qp] = 3.03119;
    _d_thermal_conductivity_dT[_qp] = 0.;
  } else if (4 < _temperature[_qp] && _temperature[_qp] <= 50) {
    _thermal_conductivity[_qp] =
        -0.6765738 + 0.938525 * _temperature[_qp] + -0.002896119 * T2;
    _d_thermal_conductivity_dT[_qp] =
        0.938525 - 0.005792238 * _temperature[_qp];
  } else if (50 < _temperature[_qp] && _temperature[_qp] <= 120) {
    _thermal_conductivity[_qp] =
        10.25948 + 0.6163078 * _temperature[_qp] - 8.031264e-4 * T2;
    _d_thermal_conductivity_dT[_qp] =
        0.6163078 - 0.0016062528 * _temperature[_qp];
  } else if (120 < _temperature[_qp] && _temperature[_qp] <= 700) {
    _thermal_conductivity[_qp] = -12.17384 + 1.076003 * _temperature[_qp] +
                                 -0.003922898 * T2 + 7.72158e-6 * T3 +
                                 -5.396842e-9 * T4;
    _d_thermal_conductivity_dT[_qp] = 1.076003 -
                                      0.007845796 * _temperature[_qp] +
                                      2.316474e-5 * T2 - 2.1587368e-8 * T3;
  } else {
    _thermal_conductivity[_qp] = 171.528;
    _d_thermal_conductivity_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 116) {
    _specific_heat[_qp] = 564.859;
    _d_specific_heat_dT[_qp] = 0.;
  } else if (116 < _temperature[_qp] && _temperature[_qp] <= 700) {
    _specific_heat[_qp] = 198.8192 + 3.941858 * _temperature[_qp] +
                          -0.007384158 * T2 + 5.218285e-6 * T3;
    _d_specific_heat_dT[_qp] =
        3.941858 - 0.014768316 * _temperature[_qp] + 1.565485e-5 * T2;
  } else {
    _specific_heat[_qp] = 1129.75;
    _d_specific_heat_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 755) {
    _density[_qp] = 2813.898 + 0.02810992 * _temperature[_qp] +
                    -7.443022e-4 * T2 + 1.039896e-6 * T3 + -5.689519e-10 * T4;
    _d_density_dT[_qp] = 0.02810992 - 0.0014886044 * _temperature[_qp] +
                         3.11968e-6 * T2 - 2.2758076e-9 * T3;
  } else {
    _density[_qp] = 2673.52;
    _d_density_dT[_qp] = 0.;
  }

  _epsilon[_qp] = 0.35;
  _d_epsilon_dT[_qp] = 0.;
}
