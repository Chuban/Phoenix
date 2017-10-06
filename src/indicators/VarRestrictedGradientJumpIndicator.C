#include "MooseVariable.h"
#include "VarRestrictedGradientJumpIndicator.h"

template <> InputParameters validParams<VarRestrictedGradientJumpIndicator>()
{
  InputParameters params = validParams<InternalSideIndicator>();
  return params;
}

VarRestrictedGradientJumpIndicator::VarRestrictedGradientJumpIndicator(const InputParameters & parameters)
  : InternalSideIndicator(parameters)
{
}

Real VarRestrictedGradientJumpIndicator::computeQpIntegral()
{
  Real jump = 0.;

  if ( _var.activeOnSubdomain(_current_elem->subdomain_id()) &&
       _var.activeOnSubdomain(_neighbor_elem->subdomain_id()) )
  	jump = (_grad_u[_qp] - _grad_u_neighbor[_qp]) * _normals[_qp];

  return jump * jump;
}
