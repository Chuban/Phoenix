#ifndef THERMALBOUNDARYFLUXBASE_H
#define THERMALBOUNDARYFLUXBASE_H

#include "GeneralUserObject.h"

// Forward Declarations
class ThermalBoundaryFluxBase;

template <> InputParameters validParams<ThermalBoundaryFluxBase>();

class ThermalBoundaryFluxBase : public GeneralUserObject
{
public:
  ThermalBoundaryFluxBase(const InputParameters & parameters);

  virtual void execute();
  virtual void initialize();
  virtual void finalize();

  /**
   * Get the boundary flux vector
   * @param[in]   iside     local  index of current side
   * @param[in]   ielem     global index of the current element
   * @param[in]   uvec1     vector of variables on the host side
   * @param[in]   temp      boundary temperature
   * @param[in]   dwave     vector of unit normal
   */
  virtual const std::vector<Real> & getFlux(unsigned int iside,
                                            dof_id_type ielem,
                                            const std::vector<Real> & uvec1,
                                            const Real & temp,
                                            const RealVectorValue & dwave,
                                            THREAD_ID tid) const;

  /**
   * Solve the Riemann problem on the boundary face
   * @param[in]   iside     local  index of current side
   * @param[in]   ielem     global index of the current element
   * @param[in]   uvec1     vector of variables on the host side
   * @param[in]   temp      boundary temperature
   * @param[in]   dwave     vector of unit normal
   * @param[out]  flux      flux vector for conservation equations
   */
  virtual void calcFlux(unsigned int iside,
                        dof_id_type ielem,
                        const std::vector<Real> & uvec1,
                        const Real & temp,
                        const RealVectorValue & dwave,
                        std::vector<Real> & flux) const = 0;

  /**
   * Get the boundary Jacobian matrix
   * @param[in]   iside     local  index of current side
   * @param[in]   ielem     global index of the current element
   * @param[in]   uvec1     vector of variables on the host side
   * @param[in]   temp      boundary temperature
   * @param[in]   dwave     vector of unit normal
   */
  virtual const DenseMatrix<Real> & getJacobian(unsigned int iside,
                                                dof_id_type ielem,
                                                const std::vector<Real> & uvec1,
                                                const Real & temp,
                                                const RealVectorValue & dwave,
                                                THREAD_ID tid) const;

  /**
   * Compute the Jacobian matrix on the boundary face
   * @param[in]   iside     local  index of current side
   * @param[in]   ielem     global index of the current element
   * @param[in]   uvec1     vector of variables on the host side
   * @param[in]   temp      boundary temperature
   * @param[in]   dwave     vector of unit normal
   * @param[out]  jac1      Jacobian matrix contribution
   */
  virtual void calcJacobian(unsigned int iside,
                            dof_id_type ielem,
                            const std::vector<Real> & uvec1,
                            const Real & temp,
                            const RealVectorValue & dwave,
                            DenseMatrix<Real> & jac1) const = 0;

protected:
  mutable unsigned int _cached_side_id;
  mutable dof_id_type _cached_elem_id;

  /// Threaded storage for fluxes
  mutable std::vector<std::vector<Real>> _flux;

  /// Threaded storage for jacobians
  mutable std::vector<DenseMatrix<Real>> _jac1;

private:
  static Threads::spin_mutex _mutex;
};

#endif // THERMALBOUNDARYFLUXBASE_H
