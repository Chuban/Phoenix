#include "Atmosphere.h"

template <> InputParameters validParams<Atmosphere>() {
  InputParameters params = validParams<ThermalMaterial>();

  return params;
}

Atmosphere::Atmosphere(const InputParameters &parameters)
    : ThermalMaterial(parameters),
      _mu(declareProperty<Real>("mu")),
      _d_mu_dT(declarePropertyDerivative<Real>("mu", getVar("temperature", 0)->name()))
{
}

void Atmosphere::computeQpProperties() {

  // Precompute some powers of temperature for speed.
  const Real T2 = std::pow(_temperature[_qp], 2);
  const Real T3 = std::pow(_temperature[_qp], 3);
  const Real T4 = std::pow(_temperature[_qp], 4);
  const Real T5 = std::pow(_temperature[_qp], 5);

  // Make sure that the temperature is above zero and set defaults.
  ThermalMaterial::checkQpTemperature();
  ThermalMaterial::defaultQpProperties();

  if (_temperature[_qp] <= 70) {
    _thermal_conductivity[_qp] = 6.50957e-3;
    _d_thermal_conductivity_dT[_qp] = 0.;
  } else if (70 < _temperature[_qp] && _temperature[_qp] <= 1000) {
    _thermal_conductivity[_qp] =
        -8.404165e-4 + 1.107418e-4 * _temperature[_qp] + -8.635537e-8 * T2 +
        6.31411e-11 * T3 + -1.88168e-14 * T4;
    _d_thermal_conductivity_dT[_qp] = 1.107418e-4 +
                                      -1.7271074e-7 * _temperature[_qp] +
                                      1.894233e-10 * T2 + -7.52672e-14 * T3;
  } else {
    _thermal_conductivity[_qp] = 6.78703e-2;
    _d_thermal_conductivity_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 100) {
    _specific_heat[_qp] = 1013.09;
    _d_specific_heat_dT[_qp] = 0.;
  } else if (100 < _temperature[_qp] && _temperature[_qp] <= 375) {
    _specific_heat[_qp] = 1010.97 + 0.0439479 * _temperature[_qp] +
                          -2.922398e-4 * T2 + 6.503467e-7 * T3;
    _d_specific_heat_dT[_qp] =
        0.0439479 - 5.844796e-4 * _temperature[_qp] + 1.95104e-6 * T2;
  } else if (375 < _temperature[_qp] && _temperature[_qp] <= 1300) {
    _specific_heat[_qp] = 1093.29 + -0.6355521 * _temperature[_qp] +
                          0.001633992 * T2 + -1.412935e-6 * T3 +
                          5.59492e-10 * T4 + 8.663072e-14 * T5;
    _d_specific_heat_dT[_qp] = -0.6355521 + 0.003267984 * _temperature[_qp] -
                               4.2388e-6 * T2 + 2.237968e-9 * T3 +
                               4.331536e-13 * T4;
  } else if (1300 < _temperature[_qp] && _temperature[_qp] <= 3000) {
    _specific_heat[_qp] = 701.0807 + 0.8493867 * _temperature[_qp] +
                          -5.846487e-4 * T2 + 2.302436e-7 * T3 +
                          -4.846758e-11 * T4 + 4.23502e-15 * T5;
    _d_specific_heat_dT[_qp] = 0.8493867 - 0.0011692974 * _temperature[_qp] +
                               6.907308e-7 * T2 - 1.9387032e-10 * T3 +
                               2.11751e-14 * T4;
  } else {
    _specific_heat[_qp] = 1307.22;
    _d_specific_heat_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 80) {
    _density[_qp] = 4.40895;
    _d_density_dT[_qp] = 0.;
  } else if (80 < _temperature[_qp] && _temperature[_qp] <= 3000) {
    _density[_qp] = 352.716 / _temperature[_qp];
    _d_density_dT[_qp] = -352.716 / T2;
  } else {
    _density[_qp] = 0.117572;
    _d_density_dT[_qp] = 0.;
  }

  if (_temperature[_qp] <= 120) {
    _mu[_qp] = 8.4681e-6;
    _d_mu_dT[_qp] = 0.;
  } else if (120 < _temperature[_qp] && _temperature[_qp] <= 600) {
    _mu[_qp] = -1.132275e-7 + 7.94333e-8 * _temperature[_qp] +
               -7.197989e-11 * T2 + 5.158693e-14 * T3 + -1.592472e-17 * T4;
    _d_mu_dT[_qp] = 7.94333e-8 - 1.4395978e-10 * _temperature[_qp] +
                    1.5476079e-13 * T2 - 6.369888e-17 * T3;
  } else if (600 < _temperature[_qp] && _temperature[_qp] <= 2150) {
    _mu[_qp] = 3.892629e-6 + 5.75387e-8 * _temperature[_qp] +
               -2.675811e-11 * T2 + 9.709691e-15 * T3 + -1.355541e-18 * T4;
    _d_mu_dT[_qp] = 5.75387e-8 - 5.351622e-11 * _temperature[_qp] +
                    2.9129073e-14 * T2 - 5.422164e-18 * T3;
  } else {
    _mu[_qp] = 7.14455e-5;
    _d_mu_dT[_qp] = 0.;
  }
}
