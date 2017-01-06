#ifndef STEATITE_H
#define STEATITE_H

#include "ThermalMaterial.h"

// Forward Declarations
class Steatite;

template <> InputParameters validParams<Steatite>();

class Steatite : public ThermalMaterial {
public:
  Steatite(const InputParameters &parameters);

protected:
  virtual void computeQpProperties() override;
};

#endif // STEATITE_H
