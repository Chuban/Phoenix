#include "MooseMesh.h"
#include "NS.h"
#include "NSExplicitTimestepSelector.h"

// libmesh includes
#include "libmesh/quadrature.h"

template <> InputParameters validParams<NSExplicitTimestepSelector>() {
  InputParameters params = validParams<ElementPostprocessor>();

  // Coupled variables
  params.addRequiredCoupledVar(NS::velocity_x, "velocity in the x- direction");
  params.addCoupledVar(NS::velocity_y, "velocity in the y- direction");
  params.addCoupledVar(NS::velocity_z, "velocity in the z- direction");
  params.addRequiredCoupledVar(NS::density, "fluid density");

  // Material properties
  params.addRequiredParam<Real>("mu", "dynamic viscosity");

  // Other parameters
  params.addRequiredParam<Real>(
      "beta",
      "0 < beta < 1, choose some fraction of the limiting timestep size");

  return params;
}

NSExplicitTimestepSelector::NSExplicitTimestepSelector(
    const InputParameters &parameters)
    : ElementPostprocessor(parameters), _u_vel(coupledValue(NS::velocity_x)),
      _v_vel(_mesh.dimension() >= 2 ? coupledValue(NS::velocity_y) : _zero),
      _w_vel(_mesh.dimension() == 3 ? coupledValue(NS::velocity_z) : _zero),
      _rho(coupledValue(NS::density)),

      // Material properties
      _mu(getParam<Real>("mu")),

      // Other parameters
      _beta(getParam<Real>("beta")) {}

NSExplicitTimestepSelector::~NSExplicitTimestepSelector() {}

void NSExplicitTimestepSelector::initialize() {
  _value = std::numeric_limits<Real>::max();
}

void NSExplicitTimestepSelector::execute() {
  Real h_min = _current_elem->hmin();

  // The space dimension plays a role in the diffusive dt limit.  The more
  // space dimensions there are, the smaller this limit is.
  Real dim = static_cast<Real>(_current_elem->dim());

  for (unsigned qp = 0; qp < _qrule->n_points(); ++qp) {

    // Don't divide by zero...
    RealVectorValue vel(_u_vel[qp], _v_vel[qp], _w_vel[qp]);
    Real vel_mag = std::max(vel.norm(), std::numeric_limits<Real>::epsilon());

    // For explicit Euler, we always have to satisfy the Courant condition for
    // stability.
    Real courant_limit_dt = h_min / vel_mag;

    // But we also have to obey the diffusive time restriction,
    // dt <= 1/(2*nu)*(1/h1^2 + 1/h2^2 + 1/h3^2)^(-1) <=
    //    <= h_min^2 / n_dim / (2*nu)
    Real diffusive_limit_dt = 0.5 * h_min * h_min / (_mu / _rho[qp]) / dim;

    // And the "combined" condition, dt <= 2*nu/|u|^2
    Real combined_limit_dt = 2. * (_mu / _rho[qp]) / vel_mag / vel_mag;

    // // Debugging:
    // Moose::out << "courant_limit_dt   = " << courant_limit_dt   << "\n"
    //           << "diffusive_limit_dt = " << diffusive_limit_dt << "\n"
    //           << "combined_limit_dt  = " << combined_limit_dt
    //           << std::endl;

    _value = std::min(
        _value, _beta * std::min(std::min(courant_limit_dt, diffusive_limit_dt),
                                 combined_limit_dt));
  }
}

Real NSExplicitTimestepSelector::getValue() {
  _communicator.min(_value);
  return _value;
}

void NSExplicitTimestepSelector::threadJoin(const UserObject &uo) {
  const NSExplicitTimestepSelector &pps =
      dynamic_cast<const NSExplicitTimestepSelector &>(uo);
  _value = std::min(_value, pps._value);
}
