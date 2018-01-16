#ifndef AIRFLUIDPROPERTIES_H
#define AIRFLUIDPROPERTIES_H

#include "PhoenixFluidProperties.h"

class AirFluidProperties;

template <> InputParameters validParams<AirFluidProperties>();

/**
 * Air fluid properties
 */
class AirFluidProperties : public PhoenixFluidProperties
{
public:
  AirFluidProperties(const InputParameters & parameters);
  virtual ~AirFluidProperties();

  virtual Real pressure(Real v, Real u) const override;
  virtual Real temperature(Real v, Real u) const override;
  virtual Real c(Real v, Real u) const override;
  virtual Real beta(Real p, Real T) const override;

  virtual Real cp(Real v, Real u) const override;

  virtual Real cv(Real v, Real u) const override;

  virtual Real gamma(Real v, Real u) const override;

  virtual Real mu(Real v, Real u) const override;

  virtual Real k(Real v, Real u) const override;

  // These are not implemented.
  virtual Real s(Real v, Real u) const override;
  virtual Real g(Real v, Real u) const override;
  virtual void dp_duv(Real v, Real u, Real & dp_dv, Real & dp_du, Real & dT_dv, Real & dT_du) const override;

  /// Compute internal energy and density from specific entropy and pressure
  virtual void rho_e_ps(Real pressure, Real entropy, Real & rho, Real & e) const override;
  virtual void rho_e_dps(Real pressure,
                         Real entropy,
                         Real & rho,
                         Real & drho_dp,
                         Real & drho_ds,
                         Real & e,
                         Real & de_dp,
                         Real & de_ds) const override;

  virtual void rho_e(Real pressure, Real temperature, Real & rho, Real & e) const override;
  virtual Real rho(Real pressure, Real temperature) const override;
  virtual void rho_dpT(Real pressure, Real temperature, Real & rho, Real & drho_dp, Real & drho_dT) const override;
  virtual void e_dpT(Real pressure, Real temperature, Real & e, Real & de_dp, Real & de_dT) const;

  virtual Real e(Real pressure, Real rho) const override;
  virtual void e_dprho(Real pressure, Real rho, Real & e, Real & de_dp, Real & de_drho) const override;

  virtual Real h(Real pressure, Real temperature) const override;
  virtual void h_dpT(Real pressure, Real temperature, Real & h, Real & dh_dp, Real & dh_dT) const override;

  virtual Real p_from_h_s(Real h, Real s) const override;
  virtual Real dpdh_from_h_s(Real h, Real s) const override;
  virtual Real dpds_from_h_s(Real h, Real s) const override;

  virtual void s_from_h_p(Real h, Real p, Real & s, Real & ds_dh, Real & ds_dp) const override;

  virtual Real P(Real rho, Real temperature) const override;

protected:
  Real _R;

  Real _beta;

private:
  Real _interpHelper(Real value, unsigned int i1, unsigned int i2) const;

  // data source: http://www.engineeringtoolbox.com/dry-air-properties-d_973.html accessed 7/6/2017
  // u = cv * T
  // _matData = {U, T, Cp, Cv, gamma, mu, k, Cv*T*(gamma-1)}
  const std::vector<std::vector<Real>> _matData = {{  125160,   143080,   161010,   179000, 197092.5,   215340,   233740,   252385, 271312.5,   290560,   330075,   371200,   413985,   458400,   504270,   551530,   599925,   649280,   699550,   750420,   801990,   854000,   906255,   958980,  1011885,  1065000,  1118250,  1171690,  1225125,  1278760,  1386150,  1493760,  1601740,  1709820,  1818110},  // U
                                                   {     175,      200,      225,      250,      275,      300,      325,      350,      375,      400,      450,      500,      550,      600,      650,      700,      750,      800,      850,      900,      950,     1000,     1050,     1100,     1150,     1200,     1250,     1300,     1350,     1400,     1500,     1600,     1700,     1800,     1900},  // T
                                                   {  1002.3,   1002.5,   1002.7,   1003.1,   1003.8,   1004.9,   1006.3,   1008.2,   1010.6,   1013.5,   1020.6,   1029.5,   1039.8,   1051.1,   1062.9,     1075,     1087,   1098.7,   1110.1,   1120.9,   1131.3,   1141.1,   1150.2,   1158.9,     1167,   1174.6,   1181.7,   1188.4,   1194.6,   1200.5,   1211.2,   1220.7,   1229.3,     1237,     1244},  // Cp
                                                   {   715.2,    715.4,    715.6,      716,    716.7,    717.8,    719.2,    721.1,    723.5,    726.4,    733.5,    742.4,    752.7,      764,    775.8,    787.9,    799.9,    811.6,      823,    833.8,    844.2,      854,    863.1,    871.8,    879.9,    887.5,    894.6,    901.3,    907.5,    913.4,    924.1,    933.6,    942.2,    949.9,    956.9},  // Cv
                                                   {   1.401,    1.401,    1.401,    1.401,    1.401,      1.4,      1.4,    1.398,    1.397,    1.395,    1.391,    1.387,    1.381,    1.376,     1.37,    1.364,    1.359,    1.354,    1.349,    1.344,     1.34,    1.336,    1.333,    1.329,    1.326,    1.323,    1.321,    1.319,    1.316,    1.314,    1.311,    1.308,    1.305,    1.302,      1.3},  // gamma
                                                   {1.182E-5, 1.329E-5, 1.467E-5, 1.599E-5, 1.725E-5, 1.846E-5, 1.962E-5, 2.075E-5, 2.181E-5, 2.286E-5, 2.485E-5, 2.670E-5, 2.849E-5, 3.017E-5, 3.178E-5, 3.332E-5, 3.482E-5, 3.624E-5, 3.763E-5, 3.897E-5, 4.026E-5, 4.153E-5, 4.276E-5, 4.396E-5, 4.511E-5, 4.626E-5, 4.736E-5, 4.846E-5, 4.952E-5, 5.057E-5, 5.264E-5, 5.457E-5, 5.646E-5, 5.829E-5, 6.008E-5},  // mu
                                                   { 0.01593,  0.01809,   0.0202,  0.02227,  0.02428,  0.02624,  0.02816,  0.03003,  0.03186,  0.03365,   0.0371,  0.04041,  0.04357,  0.04661,  0.04954,  0.05236,  0.05509,  0.05774,   0.0603,  0.06276,   0.0652,  0.06754,  0.06985,  0.07209,  0.07427,   0.0764,  0.07849,  0.08054,  0.08253,   0.0845,  0.08831,  0.09199,  0.09554,  0.09899,  0.10233},  // k
                                                   {50189.16, 57375.08, 64565.01, 71779, 79034.0925, 86136, 93496, 100449.23, 107711.0625, 114771.2, 129059.325, 143654.4, 157728.285, 172358.4, 186579.9, 200756.92, 215373.075, 229845.12, 244142.95, 258144.48, 272676.6, 286944, 301782.915, 315504.42, 329874.51, 343995, 358958.25, 373769.11, 387139.5, 401530.64, 431092.65, 460078.08, 488530.7, 516365.64, 545433}}; // Cv*T*(gamma-1)
};

#endif /* AIRFLUIDPROPERTIES_H */
