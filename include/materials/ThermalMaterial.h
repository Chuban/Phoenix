#ifndef THERMALMATERIAL_H
#define THERMALMATERIAL_H

#include "DerivativeMaterialInterface.h"
#include "Material.h"

// Forward Declarations
class ThermalMaterial;

template <> InputParameters validParams<ThermalMaterial>();

class ThermalMaterial : public DerivativeMaterialInterface<Material> {
public:
  ThermalMaterial(const InputParameters &parameters);

protected:
  virtual void computeQpProperties();
  virtual void defaultQpProperties();
  virtual void checkQpTemperature();

  const VariableValue &_temperature;

  MaterialProperty<Real> &_thermal_conductivity;
  MaterialProperty<Real> &_d_thermal_conductivity_dT;

  MaterialProperty<Real> &_specific_heat;
  MaterialProperty<Real> &_d_specific_heat_dT;

  MaterialProperty<Real> &_density;
  MaterialProperty<Real> &_d_density_dT;

  MaterialProperty<Real> &_epsilon;
  MaterialProperty<Real> &_d_epsilon_dT;
};

#endif // THERMALMATERIAL_H
