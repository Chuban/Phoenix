#ifndef ALUMINUM7075_H
#define ALUMINUM7075_H

#include "ThermalMaterial.h"

// Forward Declarations
class Aluminum7075;

template <> InputParameters validParams<Aluminum7075>();

class Aluminum7075 : public ThermalMaterial {
public:
  Aluminum7075(const InputParameters &parameters);

protected:
  virtual void computeQpProperties() override;
};

#endif // ALUMINUM7075_H
