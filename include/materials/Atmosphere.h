#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#include "ThermalMaterial.h"

// Forward Declarations
class Atmosphere;

template <> InputParameters validParams<Atmosphere>();

class Atmosphere : public ThermalMaterial {
public:
  Atmosphere(const InputParameters &parameters);

protected:
  virtual void computeQpProperties() override;

  MaterialProperty<Real> &_mu;
  MaterialProperty<Real> &_d_mu_dT;

  MaterialProperty<Real> &_gamma;
  MaterialProperty<Real> &_d_gamma_dT;
};

#endif // ATMOSPHERE_H
