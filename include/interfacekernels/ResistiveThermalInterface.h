#ifndef RESISTIVETHERMALINTERFACE_H
#define RESISTIVETHERMALINTERFACE_H

#include "InterfaceKernel.h"

//Forward Declarations
class ResistiveThermalInterface;

template<> InputParameters validParams<ResistiveThermalInterface>();

class ResistiveThermalInterface : public InterfaceKernel
{
public:
  ResistiveThermalInterface(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(Moose::DGResidualType type);
  virtual Real computeQpJacobian(Moose::DGJacobianType type);

  const MaterialProperty<Real> & _k;
  const MaterialProperty<Real> & _k_neighbor;
};

#endif  // RESISTIVETHERMALINTERFACE_H
