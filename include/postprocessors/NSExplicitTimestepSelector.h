#ifndef NSEXPLICITTIMESTEPSELECTOR_H
#define NSEXPLICITTIMESTEPSELECTOR_H

#include "ElementPostprocessor.h"

class NSExplicitTimestepSelector;

template <> InputParameters validParams<NSExplicitTimestepSelector>();

/**
 * Postprocessor that computes the minimum value of h_min/|u|,
 * where |u| is coupled in as an aux variable.
 */
class NSExplicitTimestepSelector : public ElementPostprocessor {
public:
  NSExplicitTimestepSelector(const InputParameters &parameters);
  virtual ~NSExplicitTimestepSelector();

  virtual void initialize();
  virtual void execute();
  virtual Real getValue();
  virtual void threadJoin(const UserObject &uo);

protected:
  /// The value of dt (NOTE: _dt member variable is already defined)
  Real _value;

  /// Coupled variables:
  const VariableValue &_u_vel;
  const VariableValue &_v_vel;
  const VariableValue &_w_vel;
  const VariableValue &_rho;

  /// Material properties:  the explicit time scheme limit for the viscous
  /// problem also depends on the kinematic viscosity.
  Real _mu;

  /// We can compute maximum stable timesteps based on the linearized
  /// theory, but even those timesteps are sometimes still too large
  /// for explicit timestepping in a "real" problem.  Therefore, we
  /// provide an additional "fudge" factor, 0 < beta < 1, that can be
  /// used to reduce the selected timestep even further.
  Real _beta;
};

#endif /* NSEXPLICITTIMESTEPSELECTOR_H */
