#ifndef HEATCONDUCTIONDMIKERNEL_H
#define HEATCONDUCTIONDMIKERNEL_H

#include "DerivativeMaterialInterface.h"
#include "Diffusion.h"

// Forward Declarations
class HeatConductionKernelDMI;

template <> InputParameters validParams<HeatConductionKernelDMI>();

class HeatConductionKernelDMI : public DerivativeMaterialInterface<Diffusion> {
public:
  HeatConductionKernelDMI(const InputParameters &parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

private:
  const MaterialProperty<Real> &_diffusion_coefficient;
  const MaterialProperty<Real> &_d_diffusion_coefficient_dT;
};

#endif // HEATCONDUCTIONDMIKERNEL_H
