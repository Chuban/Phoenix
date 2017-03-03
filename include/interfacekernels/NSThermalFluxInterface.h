#ifndef NSTHERMALFLUXINTERFACE_H
#define NSTHERMALFLUXINTERFACE_H

#include "InterfaceKernel.h"
#include "IdealGasFluidProperties.h"
#include "DerivativeMaterialInterface.h"

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

private:
  MooseVariable & _rho_neighbor_var;
  const VariableValue & _rho_neighbor;
  const VariableGradient & _grad_rho_neighbor;
  const IdealGasFluidProperties & _fp;
  const MaterialProperty<Real> & _kappa_neighbor;

  const MaterialProperty<Real> & _kappa; // thermal conductivity
  const MaterialProperty<Real> & _rho;
  const MaterialProperty<Real> & _specific_heat;
};

#endif // NSTHERMALFLUXINTERFACE_H
