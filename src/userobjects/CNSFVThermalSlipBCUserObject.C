#include "CNSFVThermalSlipBCUserObject.h"

template <> InputParameters validParams<CNSFVThermalSlipBCUserObject>()
{
  InputParameters params = validParams<BCUserObject>();

  params.addClassDescription("A user object that computes the ghost cell values based on the slip boundary condition.");

  params.addRequiredCoupledVar("temperature", "The temperature of the interface.");
  params.addRequiredParam<UserObjectName>("fluid_properties", "Name for fluid properties user object");

  return params;
}

CNSFVThermalSlipBCUserObject::CNSFVThermalSlipBCUserObject(const InputParameters & parameters)
  : BCUserObject(parameters),
    Coupleable(this, true),
    _mesh(_subproblem.mesh()),
    _temperature(getVar("temperature", 0)),
    _fp(getUserObject<SinglePhaseFluidProperties>("fluid_properties"))
{
  if (!_temperature->isNodal())
    mooseError("CNSFVThermalSlipBCUserObject need to be coupled to a nodal temperature variable.");
}

std::vector<Real>
CNSFVThermalSlipBCUserObject::getGhostCellValue(unsigned int iside,
                                                dof_id_type ielem,
                                                const std::vector<Real> & uvec1,
                                                const RealVectorValue & dwave) const
{
  // First, we need the average temperature on the interface.
  Node * node;
  unsigned int numNodes = _mesh.elemPtr(ielem)->side(iside)->n_nodes();
  Real avgTemp = 0;
  for (unsigned int n = 0; n < numNodes; n++)
  {
    node = _mesh.elemPtr(ielem)->side(iside)->node_ptr(n);
    avgTemp += _temperature->getNodalValue(const_cast<Node &>(*node));
  }
  avgTemp /= numNodes;

  /// pass the inputs to local

  Real rho1 = uvec1[0];
  Real rhoInv = 1. / rho1;
  Real rhou1 = uvec1[1];
  Real rhov1 = uvec1[2];
  Real rhow1 = uvec1[3];
  Real rhoe1 = uvec1[4];

  Real nx = dwave(0);
  Real ny = dwave(1);
  Real nz = dwave(2);

  std::vector<Real> urigh(5, 0.);

  Real kinEng = 0.5 * rhoInv * (rhou1 * rhou1 + rhov1 * rhov1 * rhow1 * rhow1);
  Real intEng = (rhoe1 - kinEng) * rhoInv;
  Real     T1 = _fp.temperature(rhoInv, intEng);
  Real rhoScl = std::pow(T1 / avgTemp, _fp.gamma(rhoInv, intEng) - 1);

  Real mdotn = rhou1 * nx + rhov1 * ny + rhow1 * nz;

  urigh[0] = rhoScl * (rho1);
  rhoInv = 1. / urigh[0];
  urigh[1] = rhoScl * (rhou1 - 2. * mdotn * nx);
  urigh[2] = rhoScl * (rhov1 - 2. * mdotn * ny);
  urigh[3] = rhoScl * (rhow1 - 2. * mdotn * nz);
  urigh[4] = 0.5 * (urigh[1] * urigh[1] +
                    urigh[2] * urigh[2] +
                    urigh[3] * urigh[3]) * rhoInv + urigh[0] * _fp.cv(0., 0.) * avgTemp;

  return urigh;
}
