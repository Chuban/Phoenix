#ifndef CNSFVHLLCVISCOUSINTERNALSIDEFLUX_H
#define CNSFVHLLCVISCOUSINTERNALSIDEFLUX_H

#include "InternalSideFluxBase.h"
#include "SinglePhaseFluidProperties.h"

// Forward Declarations
class CNSFVHLLCViscousInternalSideFlux;

template<>
InputParameters validParams<CNSFVHLLCViscousInternalSideFlux>();

/**
 * A user object that computes internal side flux using the HLLC approximate Riemann solver
 *
 * Reference article
 *
 * Batten, P., Leschziner, M. A., & Goldberg, U. C. (1997).
 * Average-state Jacobians and implicit methods for compressible viscous and turbulent flows.
 * Journal of computational physics, 137(1), 38-78.
 */
class CNSFVHLLCViscousInternalSideFlux : public InternalSideFluxBase
{
public:

  CNSFVHLLCViscousInternalSideFlux(const InputParameters & parameters);
  virtual ~CNSFVHLLCViscousInternalSideFlux();

  virtual void calcFlux(unsigned int iside,
                        unsigned int ielem,
                        unsigned int ineig,
                        const std::vector<Real> & uvec1,
                        const std::vector<Real> & uvec2,
                        const RealVectorValue & dwave,
                        std::vector<Real> & flux) const;

  virtual void calcJacobian(unsigned int iside,
                            unsigned int ielem,
                            unsigned int ineig,
                            const std::vector<Real> & uvec1,
                            const std::vector<Real> & uvec2,
                            const RealVectorValue & dwave,
                            DenseMatrix<Real> & jac1,
                            DenseMatrix<Real> & jac2) const;

protected:

  const SinglePhaseFluidProperties & _fp;
};

#endif //CNSFVHLLCVISCOUSINTERNALSIDEFLUX_H
