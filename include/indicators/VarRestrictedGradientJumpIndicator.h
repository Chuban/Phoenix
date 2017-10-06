#ifndef VARRESTRICTEDGRADIENTJUMPINDICATOR_H
#define VARRESTRICTEDGRADIENTJUMPINDICATOR_H

#include "InternalSideIndicator.h"

class VarRestrictedGradientJumpIndicator;

template <> InputParameters validParams<VarRestrictedGradientJumpIndicator>();

class VarRestrictedGradientJumpIndicator : public InternalSideIndicator
{
public:
  VarRestrictedGradientJumpIndicator(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;
};

#endif /* VARRESTRICTEDGRADIENTJUMPINDICATOR_H */
