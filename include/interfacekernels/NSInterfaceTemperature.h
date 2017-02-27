#ifndef NSINTERFACETEMPERATURE_H
#define NSINTERFACETEMPERATURE_H

#include "IdealGasFluidProperties.h"
#include "InterfaceKernel.h"

class NSInterfaceTemperature;

template <> InputParameters validParams<NSInterfaceTemperature>();

/*
 * Enforce continuity between a variable and the fluid temperature across a
 * subdomain boundary.
 */

class NSInterfaceTemperature : public InterfaceKernel {
public:
  NSInterfaceTemperature(const InputParameters &parameters);

protected:
  // Coupled variables
  const VariableValue &_rho;
  const VariableValue &_rho_u;
  const VariableValue &_rho_v;
  const VariableValue &_rho_w;
  const VariableValue &_rho_E;

  // Gradients
  const VariableGradient &_grad_rho;
  const VariableGradient &_grad_rho_u;
  const VariableGradient &_grad_rho_v;
  const VariableGradient &_grad_rho_w;
  const VariableGradient &_grad_rho_E;

  // Fluid properties
  const IdealGasFluidProperties &_fp;

  virtual Real computeQpResidual(Moose::DGResidualType type) override;
  virtual Real computeQpJacobian(Moose::DGJacobianType type) override;
};

#endif // NSINTERFACETEMPERATURE_H
