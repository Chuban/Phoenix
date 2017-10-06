#ifndef NSTHERMALFLUXINTERFACE_H
#define NSTHERMALFLUXINTERFACE_H

#include "InterfaceKernel.h"
#include "IdealGasFluidProperties.h"
#include "DerivativeMaterialInterface.h"
#include "MooseParsedVectorFunction.h"

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

  Function & _var_flux_func;
  Function & _neighbor_flux_func;
};

#endif // NSTHERMALFLUXINTERFACE_H
