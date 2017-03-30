#include "CNSFVNoSlipBCUserObject.h"

template<>
InputParameters validParams<CNSFVNoSlipBCUserObject>()
{
  InputParameters params = validParams<BCUserObject>();

  params.addClassDescription("A user object that computes the ghost cell values based on the slip wall boundary condition.");

  params.addRequiredParam<UserObjectName>("fluid_properties",
  "Name for fluid properties user object");

  return params;
}

CNSFVNoSlipBCUserObject::CNSFVNoSlipBCUserObject(const InputParameters & parameters)
  : BCUserObject(parameters),
    _fp(getUserObject<SinglePhaseFluidProperties>("fluid_properties"))
{
}

std::vector<Real>
CNSFVNoSlipBCUserObject::getGhostCellValue(unsigned int /*iside*/,
                                           unsigned int /*ielem*/,
                                           const std::vector<Real> & uvec1,
                                           const RealVectorValue & dwave) const
{
  /// pass the inputs to local

  Real rho1  = uvec1[0];
  Real rhou1 = uvec1[1];
  Real rhov1 = uvec1[2];
  Real rhow1 = uvec1[3];
  Real rhoe1 = uvec1[4];

  Real nx = dwave(0);
  Real ny = dwave(1);
  Real nz = dwave(2);

  std::vector<Real> urigh(5, 0.);

  urigh[0] = rho1;
  urigh[1] = -1. * rhou1;
  urigh[2] = -1. * rhov1;
  urigh[3] = -1. * rhow1;
  urigh[4] = rhoe1;

  return urigh;
}
