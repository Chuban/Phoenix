#include "CNSFVThermalResistiveBCUserObject.h"

template <> InputParameters validParams<CNSFVThermalResistiveBCUserObject>()
{
  MooseEnum condition("slip no-slip");

  InputParameters params = validParams<CNSFVThermalBCUserObject>();

  params.addClassDescription("A user object that computes the ghost cell values based on a thermally resistive boundary condition.");
  params.addParam<Real>("resistivity", 0.0, "The resistivity of the interface.");
  params.addParam<Real>("thickness", 0.0, "The thickness of the interface.");

  return params;
}

CNSFVThermalResistiveBCUserObject::CNSFVThermalResistiveBCUserObject(const InputParameters & parameters)
  : CNSFVThermalBCUserObject(parameters),
  _resistivity(getParam<Real>("resistivity")),
  _thickness(getParam<Real>("thickness"))
{
}

std::vector<Real> CNSFVThermalResistiveBCUserObject::getGhostCellValue(unsigned int iside,
                                                                       dof_id_type ielem,
                                                                       const std::vector<Real> & uvec1,
                                                                       const RealVectorValue & dwave) const
{
  /// pass the inputs to local

  Real rho1 = uvec1[0];
  Real rhoInv = 1. / rho1;  // specific volume
  Real rhou1 = uvec1[1];
  Real rhov1 = uvec1[2];
  Real rhow1 = uvec1[3];
  Real rhoe1 = uvec1[4];

  Real nx = dwave(0);
  Real ny = dwave(1);
  Real nz = dwave(2);

  Real kinEng = 0.5 * rhoInv * (rhou1 * rhou1 + rhov1 * rhov1 + rhow1 * rhow1); // kinetic energy density (J/m^3)
  Real intEng = (rhoe1 - kinEng) * rhoInv;  // specific internal energy (J/kg)
  Real fluidT = _fp.temperature(rhoInv, intEng);

  // We need the average temperature on the interface.
	Elem * element = _mesh.elemPtr(ielem);
  Node * node;
  unsigned int numNodes = element->side(iside)->n_nodes();
  Real bndTemp = 0;
  for (unsigned int n = 0; n < numNodes; n++)
  {
    node = element->side(iside)->node_ptr(n);
    bndTemp += _temperature->getNodalValue(const_cast<Node &>(*node));
  }
  bndTemp /= numNodes;

	// We need the temperature gradient in the neighboring element.
	Elem * neighbor = _mesh.elemPtr(ielem)->neighbor(iside);
	auto gradPhi = _temperature->gradPhi();
	Real heatFlux = _temperature->getGradient(neighbor, gradPhi.stdVector()) * dwave;

  // Compute the temperature jump across the interface.
  Real deltaT = heatFlux * _resistivity * _thickness;

  // We don't actually want to use the boundary temperature.
  // We want to project the cell temperature to the ghost cell through the bounary temperature.
  Real ghostTemp = 2 * (bndTemp - deltaT) - fluidT;

  Real mdotn = rhou1 * nx + rhov1 * ny + rhow1 * nz;

  std::vector<Real> urigh(5, 0.);
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
