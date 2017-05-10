#ifndef CNSFVTEMPAUX_H
#define CNSFVTEMPAUX_H

#include "AuxKernel.h"
#include "SinglePhaseFluidProperties.h"

class CNSFVTempAux;

template <> InputParameters validParams<CNSFVTempAux>();

/**
 * An aux kernel for calculating Mach number
 */
class CNSFVTempAux : public AuxKernel
{
public:
  CNSFVTempAux(const InputParameters & parameters);
  virtual ~CNSFVTempAux() {}

protected:
  virtual Real computeValue();

  const MaterialProperty<Real> & _temp;
};

#endif
