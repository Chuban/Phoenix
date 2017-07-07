#ifndef CNSFVTHERMALRESISTIVEBCUSEROBJECT_H
#define CNSFVTHERMALRESISTIVEBCUSEROBJECT_H

#include "BCUserObject.h"
#include "SinglePhaseFluidProperties.h"
#include "CNSFVThermalBCUserObject.h"

#include "Coupleable.h"
#include "MooseMesh.h"
#include "MooseVariable.h"

class CNSFVThermalResistiveBCUserObject;

template <> InputParameters validParams<CNSFVThermalResistiveBCUserObject>();

class CNSFVThermalResistiveBCUserObject : public CNSFVThermalBCUserObject
{
public:
  CNSFVThermalResistiveBCUserObject(const InputParameters & parameters);

  virtual std::vector<Real> getGhostCellValue(unsigned int iside,
                                              dof_id_type ielem,
                                              const std::vector<Real> & uvec1,
                                              const RealVectorValue & dwave) const;

protected:
  const Real _resistivity;
  const Real _thickness;
};

#endif // CNSFVTHERMALRESISTIVEBCUSEROBJECT_H
