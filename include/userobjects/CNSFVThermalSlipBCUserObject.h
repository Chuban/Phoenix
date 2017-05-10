#ifndef CNSFVTHERMALSLIPWALLBCUSEROBJECT_H
#define CNSFVTHERMALSLIPWALLBCUSEROBJECT_H

#include "BCUserObject.h"
#include "SinglePhaseFluidProperties.h"

#include "Coupleable.h"
#include "MooseMesh.h"
#include "MooseVariable.h"

class CNSFVThermalSlipBCUserObject;

template <> InputParameters validParams<CNSFVThermalSlipBCUserObject>();

class CNSFVThermalSlipBCUserObject : public BCUserObject,
                                     public Coupleable
{
public:
  CNSFVThermalSlipBCUserObject(const InputParameters & parameters);

  virtual std::vector<Real> getGhostCellValue(unsigned int iside,
                                              dof_id_type ielem,
                                              const std::vector<Real> & uvec1,
                                              const RealVectorValue & dwave) const;

protected:
  MooseMesh & _mesh;
  MooseVariable * _temperature;
  const SinglePhaseFluidProperties & _fp;
};

#endif // CNSFVTHERMALSLIPWALLBCUSEROBJECT_H
