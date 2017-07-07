#ifndef PHOENIXFLUIDPROPERTIES_H
#define PHOENIXFLUIDPROPERTIES_H

#include "SinglePhaseFluidProperties.h"

class PhoenixFluidProperties;

template <> InputParameters validParams<PhoenixFluidProperties>();

class PhoenixFluidProperties : public SinglePhaseFluidProperties
{
public:
  PhoenixFluidProperties(const InputParameters & parameters);
  virtual ~PhoenixFluidProperties();

  virtual Real P(Real rho, Real temperature) const = 0;
};

#endif /* PHOENIXFLUIDPROPERTIES_H */
