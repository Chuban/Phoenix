#ifndef NSTHERMALFLUXINTERFACE_H
#define NSTHERMALFLUXINTERFACE_H

#include "InterfaceKernel.h"
#include "IdealGasFluidProperties.h"
#include "DerivativeMaterialInterface.h"
#include "MooseParsedVectorFunction.h"
#include "PostprocessorInterface.h"

//Forward Declarations
class NSThermalFluxInterface;

template<> InputParameters validParams<NSThermalFluxInterface>();

class NSThermalFluxInterface : public InterfaceKernel,
                               public PostprocessorInterface
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
  const MaterialProperty<Real> & _epsilon;

  const MaterialProperty<Real> & _kappa_neighbor; // thermal conductivity
  const MaterialProperty<Real> & _epsilon_neighbor;

  Function & _var_flux_func;
  Function & _neighbor_flux_func;

  const Real _stefan_boltzmann;
  const PostprocessorValue & _rad_T;

};

#endif // NSTHERMALFLUXINTERFACE_H
