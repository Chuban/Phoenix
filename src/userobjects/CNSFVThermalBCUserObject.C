#include "CNSFVThermalBCUserObject.h"

template <> InputParameters validParams<CNSFVThermalBCUserObject>()
{
  MooseEnum condition("slip no-slip");

  InputParameters params = validParams<BCUserObject>();

  params.addClassDescription("A user object that computes the ghost cell values based on the slip boundary condition.");

  params.addParam<MooseEnum>("condition", condition, "What type of interface is this?");
  params.addRequiredCoupledVar("temperature", "The temperature of the interface.");
  params.addRequiredParam<UserObjectName>("fluid_properties", "Name for fluid properties user object");

  return params;
}

CNSFVThermalBCUserObject::CNSFVThermalBCUserObject(const InputParameters & parameters)
  : BCUserObject(parameters),
    Coupleable(this, true),
    _mesh(_subproblem.mesh()),
    _temperature(getVar("temperature", 0)),
    _fp(getUserObject<PhoenixFluidProperties>("fluid_properties")),
    _condition(getParam<MooseEnum>("condition"))
{
  if (!_temperature->isNodal())
    mooseError("CNSFVThermalBCUserObject need to be coupled to a nodal temperature variable.");
}

std::vector<Real> CNSFVThermalBCUserObject::getGhostCellValue(unsigned int iside,
                                                              dof_id_type ielem,
                                                              const std::vector<Real> & uvec1,
                                                              const RealVectorValue & dwave) const
{
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

  Real kinEng = 0.5 * rhoInv * (rhou1 * rhou1 + rhov1 * rhov1 + rhow1 * rhow1);
  Real intEng = (rhoe1 - kinEng) * rhoInv;
  Real fluidT = _fp.temperature(rhoInv, intEng);

  // We need the average temperature on the interface.
  Node * node;
  unsigned int numNodes = _mesh.elemPtr(ielem)->side(iside)->n_nodes();
  Real bndTemp = 0;
  for (unsigned int n = 0; n < numNodes; n++)
  {
    node = _mesh.elemPtr(ielem)->side(iside)->node_ptr(n);
    bndTemp += _temperature->getNodalValue(const_cast<Node &>(*node));
  }
  bndTemp /= numNodes;

	// Since there's no resistive layer, dT is zero.
	Real deltaT = 0.;

  // We don't actually want to use the boundary temperature.
  // We want to project the cell temperature to the ghost cell through the bounary temperature.
  Real ghostTemp = 2 * (bndTemp - deltaT) - fluidT;

  Real mdotn = rhou1 * nx + rhov1 * ny + rhow1 * nz;

  urigh[0] = rho1;  // There can be no mass flux across this boundary -> constant density
  Real ghostPres = _fp.P(urigh[0], ghostTemp);
  if (_condition == "slip")
  {
    urigh[1] = rhou1 - 2. * mdotn * nx;
    urigh[2] = rhov1 - 2. * mdotn * ny;
    urigh[3] = rhow1 - 2. * mdotn * nz;
  }
  else
  {
    urigh[1] = -1. * rhou1;
    urigh[2] = -1. * rhov1;
    urigh[3] = -1. * rhow1;
  }
  urigh[4] = 0.5 * rhoInv * (urigh[1] * urigh[1] +
                             urigh[2] * urigh[2] +
                             urigh[3] * urigh[3]) + urigh[0] * _fp.e(ghostPres, urigh[0]);

  return urigh;
}
