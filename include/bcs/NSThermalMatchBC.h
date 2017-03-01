#ifndef NSTHERMALMATCHBC_H
#define NSTHERMALMATCHBC_H

#include "MatchedValueBC.h"
#include "IdealGasFluidProperties.h"

class NSThermalMatchBC;

template<>
InputParameters validParams<NSThermalMatchBC>();

class NSThermalMatchBC : public MatchedValueBC
{
public:
  NSThermalMatchBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  unsigned int _rho_var;
  const VariableValue & _rho;

  const IdealGasFluidProperties & _fp;
};

#endif //NSTHERMALMATCHBC_H
