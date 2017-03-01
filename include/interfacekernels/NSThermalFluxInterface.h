#ifndef NSTHERMALFLUXINTERFACE_H
#define NSTHERMALFLUXINTERFACE_H

#include "InterfaceKernel.h"
#include "IdealGasFluidProperties.h"

//Forward Declarations
class NSThermalFluxInterface;

template<>
InputParameters validParams<NSThermalFluxInterface>();

/**
 * DG kernel for interfacing diffusion between two variables on adjacent blocks
 */
class NSThermalFluxInterface : public InterfaceKernel
{
public:
  NSThermalFluxInterface(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(Moose::DGResidualType type);
  virtual Real computeQpJacobian(Moose::DGJacobianType type);

  Real _D;
  Real _D_neighbor;

  MooseVariable & _rho_var;
  const VariableValue & _rho;
  const VariableGradient & _grad_rho;

  const IdealGasFluidProperties & _fp;
};

#endif // NSTHERMALFLUXINTERFACE_H
