#ifndef NSTHERMALINTERFACE_H
#define NSTHERMALINTERFACE_H

#include "InterfaceKernel.h"
#include "IdealGasFluidProperties.h"
#include "DerivativeMaterialInterface.h"

//Forward Declarations
class NSThermalInterface;

template<>
InputParameters validParams<NSThermalInterface>();

/**
 * DG kernel for interfacing diffusion between two variables on adjacent blocks
 */
class NSThermalInterface : public InterfaceKernel
{
public:
  NSThermalInterface(const InputParameters & parameters);

protected:
  // virtual void computeElemNeighResidual(Moose::DGResidualType type);
  virtual Real computeQpResidual(Moose::DGResidualType type);
  virtual Real computeQpJacobian(Moose::DGJacobianType type);

private:
  const VariableValue & _rho;
  const VariableGradient & _grad_rho;
  const IdealGasFluidProperties & _fp;
  const MaterialProperty<Real> & _kappa;

  const MaterialProperty<Real> & _kappa_neighbor; // thermal conductivity
  const MaterialProperty<Real> & _rho_neighbor;
  const MaterialProperty<Real> & _specific_heat_neighbor;
};

#endif // NSTHERMALINTERFACE_H
