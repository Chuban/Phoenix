#ifndef ALUMINUM2024_H
#define ALUMINUM2024_H

#include "ThermalMaterial.h"

// Forward Declarations
class Aluminum2024;

template <> InputParameters validParams<Aluminum2024>();

class Aluminum2024 : public ThermalMaterial {
public:
  Aluminum2024(const InputParameters &parameters);

protected:
  virtual void computeQpProperties() override;
};

#endif // ALUMINUM2024_H
