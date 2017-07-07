#ifndef CNSFVTHERMALBCUSEROBJECT_H
#define CNSFVTHERMALBCUSEROBJECT_H

#include "BCUserObject.h"
#include "PhoenixFluidProperties.h"

#include "Coupleable.h"
#include "MooseMesh.h"
#include "MooseVariable.h"

class CNSFVThermalBCUserObject;

template <> InputParameters validParams<CNSFVThermalBCUserObject>();

class CNSFVThermalBCUserObject : public BCUserObject, public Coupleable
{
public:
  CNSFVThermalBCUserObject(const InputParameters & parameters);

  virtual std::vector<Real> getGhostCellValue(unsigned int iside,
                                              dof_id_type ielem,
                                              const std::vector<Real> & uvec1,
                                              const RealVectorValue & dwave) const;

protected:
  MooseMesh & _mesh;
  MooseVariable * _temperature;
  const PhoenixFluidProperties & _fp;
  MooseEnum _condition;
};

#endif // CNSFVTHERMALBCUSEROBJECT_H
