#ifndef GLOBALTEMPERATUREAUX_H
#define GLOBALTEMPERATUREAUX_H

#include "NSTemperatureAux.h"

template <> InputParameters validParams<NSTemperatureAux>();

class GlobalTemperatureAux : public NSTemperatureAux
{
public:
  GlobalTemperatureAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  const VariableValue & _solid_temperature;
};

#endif // GLOBALTEMPERATUREAUX_H
