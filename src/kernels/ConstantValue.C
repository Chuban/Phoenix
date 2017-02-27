#include "ConstantValue.h"

template <> InputParameters validParams<ConstantValue>() {
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Set a variable to a constant value.");
  params.addParam<Real>(
      "value", "This is the value to which the variable should be set.");
  return params;
}

ConstantValue::ConstantValue(const InputParameters &parameters)
    : Kernel(parameters), _value(getParam<Real>("value")) {}

Real ConstantValue::computeQpResidual() {
  return (_u[_qp] - _value) * _test[_i][_qp];
}

Real ConstantValue::computeQpJacobian() {
  return _phi[_j][_qp] * _test[_i][_qp];
}
