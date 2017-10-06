#include "GlobalTemperatureAux.h"

template <> InputParameters validParams<GlobalTemperatureAux>()
{
  InputParameters params = validParams<NSTemperatureAux>();

  params.addRequiredCoupledVar("solid_temperature", "The temperature of the solid materials");

  return params;
}

GlobalTemperatureAux::GlobalTemperatureAux(const InputParameters & parameters)
  : NSTemperatureAux(parameters),
    _solid_temperature(coupledValue("solid_temperature"))
{
}

Real GlobalTemperatureAux::computeValue()
{
  return NSTemperatureAux::computeValue() + _solid_temperature[_qp];
}
