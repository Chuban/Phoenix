#include "NS.h"
#include "MooseVariable.h"
#include "CNSFVThermalFluxInterface.h"

#include "libmesh/quadrature.h"

template<>
InputParameters validParams<CNSFVThermalFluxInterface>()
{
  MooseEnum component("mass x-momentum y-momentum z-momentum total-energy");

  InputParameters params = validParams<InterfaceKernel>();
  params.addClassDescription("A DGKernel for the CNS equations.");
  params.addParam<MooseEnum>("component", component, "Choose one of the conservation equations");

  params.addRequiredCoupledVar("rho", "Conserved variable: rho");
  params.addRequiredCoupledVar("rhou", "Conserved variable: rhou");
  params.addCoupledVar("rhov", "Conserved variable: rhov");
  params.addCoupledVar("rhow", "Conserved variable: rhow");
  params.addRequiredCoupledVar("rhoe", "Conserved variable: rhoe");

  params.addRequiredParam<UserObjectName>("bc_uo", "The boundary user object to use");
  params.addRequiredParam<UserObjectName>("flux", "The name of internal side flux object to use");
  params.addRequiredParam<UserObjectName>("slope_limiting", "The name of the slope limiter to use");

  params.addRequiredParam<UserObjectName>("fluid_properties", "Name for fluid properties user object");

  return params;
}

CNSFVThermalFluxInterface::CNSFVThermalFluxInterface(const InputParameters & parameters) :
    InterfaceKernel(parameters),
    _component(getParam<MooseEnum>("component")),
    _rhoc1(coupledValue("rho")),
    _rhouc1(coupledValue("rhou")),
    _rhovc1(isCoupled("rhov") ? coupledValue("rhov") : _zero),
    _rhowc1(isCoupled("rhow") ? coupledValue("rhow") : _zero),
    _rhoec1(coupledValue("rhoe")),
    _rho1(getMaterialProperty<Real>("rho")),
    _rhou1(getMaterialProperty<Real>("rhou")),
    _rhov1(getMaterialProperty<Real>("rhov")),
    _rhow1(getMaterialProperty<Real>("rhow")),
    _rhoe1(getMaterialProperty<Real>("rhoe")),
    _k_neighbor(getNeighborMaterialProperty<Real>("thermal_conductivity")),
    _bc_uo(getUserObject<CNSFVThermalBCUserObject>("bc_uo")),
    _flux(getUserObject<InternalSideFluxBase>("flux")),
    _slope(getUserObject<SlopeLimitingBase>("slope_limiting")),
    _rho_var(coupled("rho")),
    _rhou_var(coupled("rhou")),
    _rhov_var(isCoupled("rhov") ? coupled("rhov") : zero),
    _rhow_var(isCoupled("rhow") ? coupled("rhow") : zero),
    _rhoe_var(coupled("rhoe")),
    _fp(getUserObject<SinglePhaseFluidProperties>("fluid_properties"))
{
  _jmap.insert(std::pair<unsigned int, unsigned int>(_rho_var, 0));
  _jmap.insert(std::pair<unsigned int, unsigned int>(_rhou_var, 1));
  _jmap.insert(std::pair<unsigned int, unsigned int>(_rhov_var, 2));
  _jmap.insert(std::pair<unsigned int, unsigned int>(_rhow_var, 3));
  _jmap.insert(std::pair<unsigned int, unsigned int>(_rhoe_var, 4));

  if (!parameters.isParamValid("boundary"))
  {
    mooseError("In order to use the CNSFVThermalFluxInterface kernel, you must specify a boundary where it will live.");
  }
  if (!parameters.isParamValid("block"))
  {
    mooseError("In order to use the CNSFVThermalFluxInterface kernel, you must specify the block in which the variable lives.");
  }
}

void CNSFVThermalFluxInterface::computeElemNeighResidual(Moose::DGResidualType type)
{
  // This is necessary as FVM variables are not defined on the interface but at the center of the elements.
  if (this->blockRestricted() && !this->hasBlocks(_current_elem->subdomain_id()))
    // Our variable is defined on the other side of the interface.
    return;

  bool is_elem;
  if (type == Moose::Element)
    is_elem = true;
  else
    is_elem = false;

  const VariableTestValue & test_space = is_elem ? _test : _test_neighbor;
  DenseVector<Number> & re = is_elem ? _assembly.residualBlock(_var.number())
                                     : _assembly.residualBlockNeighbor(_neighbor_var.number());

  for (_qp = 0; _qp < _qrule->n_points(); _qp++)
    for (_i = 0; _i < test_space.size(); _i++)
      re(_i) += _JxW[_qp] * _coord[_qp] * computeQpResidual(type);
}

Real CNSFVThermalFluxInterface::computeQpResidual(Moose::DGResidualType type)
{
  /// the size of flux vector is five
  /// 0 for mass
  /// 1 for x-momentum
  /// 2 for y-momentum
  /// 3 for z-momentum
  /// 4 for total-energy

  std::vector<Real> uvec1 = {_rho1[_qp], _rhou1[_qp], _rhov1[_qp], _rhow1[_qp], _rhoe1[_qp]};

  std::vector<Real> uvec2 = _bc_uo.getGhostCellValue(_current_side,
                                                     _current_elem->id(),
                                                     uvec1,
                                                     _normals[_qp]);
  

  // We assume that temperature has been set as the neighboring variable.
  Real re = 0.;
  const std::vector<Real> & flux = _flux.getFlux(_current_side,
                                                 _current_elem->id(),
                                                 _neighbor_elem->id(),
                                                 uvec1,
                                                 uvec2,
                                                 _normals[_qp],
                                                 _tid);

  switch (type)
  {
    case Moose::Element:
      re = flux[_component] * _test[_i][_qp];
      break;

    case Moose::Neighbor:
      // We set the thermal gradient in the neighboring material to match the heat flux into the fluid.
      if (_component == "total-energy")
      {
        // The slope limiter computes the gradients of the primitive variables (p,u,v,w,T).
        Real rhoInv = 1. / uvec1[0];
        Real kinEng = 0.5 * rhoInv * (uvec1[1] * uvec1[1] + uvec1[2] * uvec1[2] + uvec1[3] * uvec1[3]);
        Real intEng = (uvec1[4] - kinEng) * rhoInv;
        re = _fp.k(rhoInv, intEng) * _slope.limitElementSlope()[4] * _normals[_qp] * _test_neighbor[_i][_qp] / _k_neighbor[_qp];
      }
      else
        re = 0.;
      break;
  }

  return re;
}

Real CNSFVThermalFluxInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  /// input variables are the original constant monomial from the "left" and "right" cells

  std::vector<Real> uvec1 = {_rhoc1[_qp], _rhouc1[_qp], _rhovc1[_qp], _rhowc1[_qp], _rhoec1[_qp]};
  std::vector<Real> uvec2 = _bc_uo.getGhostCellValue(_current_side,
                                                     _current_elem->id(),
                                                     uvec1,
                                                     _normals[_qp]);

  const DenseMatrix<Real> & fjac1 = _flux.getJacobian(Moose::Element,
                                                      _current_side,
                                                      _current_elem->id(),
                                                      _neighbor_elem->id(),
                                                      uvec1,
                                                      uvec2,
                                                      _normals[_qp],
                                                      _tid);

  // const DenseMatrix<Real> & fjac2 = _flux.getJacobian(Moose::Neighbor,
  //                                                     _current_side,
  //                                                     _current_elem->id(),
  //                                                     _neighbor_elem->id(),
  //                                                     uvec1,
  //                                                     uvec2,
  //                                                     _normals[_qp],
  //                                                     _tid);

  Real re = 0.;
  return re;
  switch (type)
  {
    case Moose::ElementElement:
      re = fjac1(_component, _component) * _phi[_j][_qp] * _test[_i][_qp];
      break;
    case Moose::ElementNeighbor:
      // re = fjac2(_component, _component) * _phi_neighbor[_j][_qp] * _test[_i][_qp];
      re = 0.;
      break;
    case Moose::NeighborElement:
      re = -fjac1(_component, _component) * _phi[_j][_qp] * _test_neighbor[_i][_qp];
      break;
    case Moose::NeighborNeighbor:
      // re = -fjac2(_component, _component) * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];
      re = 0.;
      break;
  }

  return re;
}

Real CNSFVThermalFluxInterface::computeQpOffDiagJacobian(Moose::DGJacobianType type, unsigned int jvar)
{
  /// input variables are the original constant monomial from the "left" and "right" cells

  std::vector<Real> uvec1 = {_rhoc1[_qp], _rhouc1[_qp], _rhovc1[_qp], _rhowc1[_qp], _rhoec1[_qp]};
  std::vector<Real> uvec2 = _bc_uo.getGhostCellValue(_current_side,
                                                     _current_elem->id(),
                                                     uvec1,
                                                     _normals[_qp]);

  const DenseMatrix<Real> & fjac1 = _flux.getJacobian(Moose::Element,
                                                      _current_side,
                                                      _current_elem->id(),
                                                      _neighbor_elem->id(),
                                                      uvec1,
                                                      uvec2,
                                                      _normals[_qp],
                                                      _tid);

  // const DenseMatrix<Real> & fjac2 = _flux.getJacobian(Moose::Neighbor,
  //                                                     _current_side,
  //                                                     _current_elem->id(),
  //                                                     _neighbor_elem->id(),
  //                                                     uvec1,
  //                                                     uvec2,
  //                                                     _normals[_qp],
  //                                                     _tid);

  Real re = 0.;
  return re;
  switch (type)
  {
    case Moose::ElementElement:
      re = fjac1(_component, _jmap[jvar]) * _phi[_j][_qp] * _test[_i][_qp];
      break;
    case Moose::ElementNeighbor:
      // re = fjac2(_component, _jmap[jvar]) * _phi_neighbor[_j][_qp] * _test[_i][_qp];
      re = 0.;
      break;
    case Moose::NeighborElement:
      re = -fjac1(_component, _jmap[jvar]) * _phi[_j][_qp] * _test_neighbor[_i][_qp];
      break;
    case Moose::NeighborNeighbor:
      // re = -fjac2(_component, _jmap[jvar]) * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp];
      re = 0.;
      break;
  }

  return re;
}
