#ifndef CNSFVNOSLIPWALLBCUSEROBJECT_H
#define CNSFVNOSLIPWALLBCUSEROBJECT_H

#include "BCUserObject.h"
#include "SinglePhaseFluidProperties.h"

class CNSFVNoSlipBCUserObject;

template<>
InputParameters validParams<CNSFVNoSlipBCUserObject>();

/**
 * A user object that computes the ghost cell values based on the no slip wall boundary condition
 */
class CNSFVNoSlipBCUserObject : public BCUserObject
{
public:

  CNSFVNoSlipBCUserObject(const InputParameters & parameters);

  virtual std::vector<Real> getGhostCellValue(unsigned int iside,
                                              unsigned int ielem,
                                              const std::vector<Real> & uvec1,
                                              const RealVectorValue & dwave) const;

protected:

  const SinglePhaseFluidProperties & _fp;
};

#endif // CNSFVNOSLIPWALLBCUSEROBJECT_H
