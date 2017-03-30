#ifndef CNSFVHLLCVISCOUSWALLBOUNDARYFLUX_H
#define CNSFVHLLCVISCOUSWALLBOUNDARYFLUX_H

#include "BoundaryFluxBase.h"
#include "BCUserObject.h"
#include "SinglePhaseFluidProperties.h"

// Forward Declarations
class CNSFVHLLCViscousBoundaryFlux;

template<>
InputParameters validParams<CNSFVHLLCViscousBoundaryFlux>();

/**
 * A user object that computes the slip boundary flux using the HLLC approximate Riemann solver
 */
class CNSFVHLLCViscousBoundaryFlux : public BoundaryFluxBase
{
public:

  CNSFVHLLCViscousBoundaryFlux(const InputParameters & parameters);
  virtual ~CNSFVHLLCViscousBoundaryFlux();

  virtual void calcFlux(unsigned int iside,
                        unsigned int ielem,
                        const std::vector<Real> & uvec1,
                        const RealVectorValue & dwave,
                        std::vector<Real> & flux) const;

  virtual void calcJacobian(unsigned int iside,
                            unsigned int ielem,
                            const std::vector<Real> & uvec1,
                            const RealVectorValue & dwave,
                            DenseMatrix<Real> & jac1) const;

protected:

  const BCUserObject & _bc_uo;
  const SinglePhaseFluidProperties & _fp;
};

#endif // CNSFVHLLCVISCOUSWALLBOUNDARYFLUX_H
