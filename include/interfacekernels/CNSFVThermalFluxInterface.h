#ifndef CNSFVTHERMALFLUXINTERFACE_H
#define CNSFVTHERMALFLUXINTERFACE_H

#include "InterfaceKernel.h"
#include "CNSFVThermalBCUserObject.h"
#include "InternalSideFluxBase.h"
#include "SlopeLimitingBase.h"
#include "SinglePhaseFluidProperties.h"

//Forward Declarations
class CNSFVThermalFluxInterface;

template<>
InputParameters validParams<CNSFVThermalFluxInterface>();

class CNSFVThermalFluxInterface : public InterfaceKernel
{
public:
  CNSFVThermalFluxInterface(const InputParameters & parameters);

protected:
  virtual void computeElemNeighResidual(Moose::DGResidualType type);
  virtual Real computeQpResidual(Moose::DGResidualType type);
  virtual Real computeQpJacobian(Moose::DGJacobianType type);
  virtual Real computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar);

  /// choose an equation
  MooseEnum _component;

  // "1" denotes the "left" state
  // "2" denotes the "right" state

  // piecewise constant variable values in cells
  const VariableValue & _rhoc1;
  const VariableValue & _rhouc1;
  const VariableValue & _rhovc1;
  const VariableValue & _rhowc1;
  const VariableValue & _rhoec1;

  // extrapolated variable values at side center
  const MaterialProperty<Real> & _rho1;
  const MaterialProperty<Real> & _rhou1;
  const MaterialProperty<Real> & _rhov1;
  const MaterialProperty<Real> & _rhow1;
  const MaterialProperty<Real> & _rhoe1;

  // material properties for gradient matching
  const MaterialProperty<Real> & _k_neighbor;

  const CNSFVThermalBCUserObject & _bc_uo;
  const InternalSideFluxBase & _flux;
  const SlopeLimitingBase & _slope;

  unsigned int _rho_var;
  unsigned int _rhou_var;
  unsigned int _rhov_var;
  unsigned int _rhow_var;
  unsigned int _rhoe_var;

  const SinglePhaseFluidProperties & _fp;

  std::map<unsigned int, unsigned int> _jmap;
};

#endif // CNSFVTHERMALFLUXINTERFACE_H
