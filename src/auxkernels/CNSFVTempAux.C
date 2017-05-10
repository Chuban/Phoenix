#include "CNSFVTempAux.h"

template <> InputParameters validParams<CNSFVTempAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription("An aux kernel for calculating fluid temperature.");
  return params;
}

CNSFVTempAux::CNSFVTempAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _temp(getMaterialProperty<Real>("temperature"))
{
}

Real
CNSFVTempAux::computeValue()
{
  return _temp[_qp];
}
