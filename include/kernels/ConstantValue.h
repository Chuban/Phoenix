#ifndef CONSTANTVALUE_H
#define CONSTANTVALUE_H

#include "Kernel.h"

class ConstantValue;

template <> InputParameters validParams<ConstantValue>();

class ConstantValue : public Kernel {
public:
  ConstantValue(const InputParameters &parameters);

protected:
  Real _value;

  virtual Real computeQpResidual() override;

  virtual Real computeQpJacobian() override;
};

#endif /* CONSTANTVALUE_H */
