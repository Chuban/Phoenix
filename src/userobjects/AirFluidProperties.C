#include "AirFluidProperties.h"

template <> InputParameters validParams<AirFluidProperties>()
{
  InputParameters params = validParams<PhoenixFluidProperties>();
  params.addParam<Real>("R", "Gas constant");
  params.addParam<Real>("beta", 0, "Coefficient of thermal expansion");
  params.addClassDescription("Fluid properties for air");
  return params;
}

AirFluidProperties::AirFluidProperties(const InputParameters & parameters)
  : PhoenixFluidProperties(parameters),
    _R(isParamValid("R") ? getParam<Real>("R") : 287.058), // default value in J/(kg*K) for dry air
    _beta(getParam<Real>("beta"))
{
  mooseWarning("Using pressure independent tabulated values for T, Cp, Cv, gamma, mu, and k as functions of internal energy 'u'.", "\n",
               "Pressure, sound speed, density, and energy follow the ideal gas law.");
}

AirFluidProperties::~AirFluidProperties() {}

Real AirFluidProperties::pressure(Real v, Real u) const
{
  if (v == 0.0)
    mooseError(name(), ": Invalid value of specific volume detected (v = ", v, ").");

  // The std::max function serves as a hard limiter, which will guarantee non-negative pressure
  // when resolving strongly nonlinear waves
  return std::max(1e-8, (gamma(v, u) - 1.) * u / v);
}

Real AirFluidProperties::temperature(Real /*v*/, Real u) const { return _interpHelper(u, 0, 1); }

Real AirFluidProperties::c(Real v, Real u) const
{
  Real temp = temperature(v, u);
  // The std::max function serves as a hard limiter, which will guarantee non-negative speed of
  // sound
  // when resolving strongly nonlinear waves
  return std::sqrt(std::max(1e-8, gamma(v, u) * _R * temp));
}

Real AirFluidProperties::beta(Real, Real) const { return _beta; }

Real AirFluidProperties::cp(Real /*v*/, Real u) const { return _interpHelper(u, 0, 2); }

Real AirFluidProperties::cv(Real /*v*/, Real u) const { return _interpHelper(u, 0, 3); }

Real AirFluidProperties::gamma(Real /*v*/, Real u) const { return _interpHelper(u, 0, 4); }

Real AirFluidProperties::k(Real /*v*/, Real u) const { return _interpHelper(u, 0, 6); }

Real AirFluidProperties::mu(Real /*v*/, Real u) const { return _interpHelper(u, 0, 5); }

Real AirFluidProperties::s(Real, Real) const
{
  mooseError(name(), ": s() not implemented.");
  return 0;
}

Real AirFluidProperties::g(Real, Real) const
{
  mooseError(name(), ": g() not implemented.");
  return 0;
}

void AirFluidProperties::dp_duv(Real, Real, Real &, Real &, Real &, Real &) const
{
  mooseError(name(), ": dp_duv() not implemented.");
}

void AirFluidProperties::rho_e_ps(Real, Real, Real &, Real &) const
{
  mooseError(name(), ": rho_e_ps() not implemented.");
}

void AirFluidProperties::rho_e_dps(Real, Real, Real &, Real &, Real &, Real &, Real &, Real &) const
{
  mooseError(name(), ": rho_e_dps() not implemented.");
}

void AirFluidProperties::rho_e(Real, Real, Real &, Real &) const
{
  mooseError(name(), ": rho_e() not implemented.");
}

Real AirFluidProperties::rho(Real pressure, Real temperature) const
{
  Real Ga = _interpHelper(temperature, 1, 4);
  Real Cv = _interpHelper(temperature, 1, 3);

  if ((Ga - 1) * pressure == 0.)
    mooseError(name(),
               ": Invalid gamma or pressure detected in rho(pressure = ",
               pressure,
               ", gamma = ",
               Ga,
               ")");

  return pressure / (Ga - 1.0) / Cv / temperature;
}

void AirFluidProperties::rho_dpT(Real, Real, Real &, Real &, Real &) const
{
  mooseError(name(), ": rho_dpT() not implemented.");
}

void AirFluidProperties::e_dpT(Real, Real , Real &, Real &, Real &) const
{
  mooseError(name(), ": e_dpT() not implemented.");
}

Real AirFluidProperties::e(Real pressure, Real rho) const
{
  // (P / rho) = Cv * T * (gamma - 1)
  Real Ga = _interpHelper(pressure / rho, 7, 4);

  return pressure / (Ga - 1) / rho;
}

void AirFluidProperties::e_dprho(Real, Real, Real &, Real &, Real &) const
{
  mooseError(name(), ": e_dprho() not implemented.");
}

Real AirFluidProperties::h(Real, Real) const
{
  mooseError(name(), ": h() not implemented.");
  return 0.;
}

void AirFluidProperties::h_dpT(Real, Real, Real &, Real &, Real &) const
{
  mooseError(name(), ": h_dpT() not implemented.");
}

Real AirFluidProperties::p_from_h_s(Real, Real) const
{
  mooseError(name(), ": p_from_h_s() not implemented.");
  return 0.;
}

Real AirFluidProperties::dpdh_from_h_s(Real, Real) const
{
  mooseError(name(), ": dpdh_from_h_s() not implemented.");
  return 0.;
}

Real AirFluidProperties::P(Real rho, Real temperature) const
{
  return rho * _interpHelper(temperature, 1, 7);
}

Real AirFluidProperties::_interpHelper(Real value, unsigned int i1, unsigned int i2) const
{
  // Interpolate _matData[i2] using 'value' found in _matData[i1].
  auto uIter = std::lower_bound(_matData[i1].begin(), _matData[i1].end(), value);
  Real uIndex = uIter - _matData[i1].begin();
  if (uIter == _matData[i1].begin())
  {
    mooseError("Exceeded lower bound of tabulated EOS data.\n",
               "value: ", value, ", i1: ", i1, ", i2: ", i2);
    return _matData[i2].front();
  }
  else if (uIter == _matData[i1].end())
  {
    mooseError("Exceeded upper bound of tabulated EOS data.\n",
               "value: ", value, ", i1: ", i1, ", i2: ", i2);
    return _matData[i2].back();
  }
  else
    return _matData[i2][uIndex - 1] + (_matData[i2][uIndex] - _matData[i2][uIndex - 1]) * (value - _matData[i1][uIndex - 1]) / (_matData[i1][uIndex] - _matData[i1][uIndex - 1]);
}
