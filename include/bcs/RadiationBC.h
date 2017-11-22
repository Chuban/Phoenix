#ifndef RADIATIONBC_H
#define RADIATIONBC_H

#include "IntegratedBC.h"

class RadiationBC;

template<> InputParameters validParams<RadiationBC>();

class RadiationBC : public IntegratedBC
{
public:
  RadiationBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<Real> & _epsilon;
  const Real _stefan_boltzmann;
  const Real & _env_T;
};

#endif // RADIATIONBC_H
