#include "ThermalBoundaryFluxBase.h"

// Static mutex definition
Threads::spin_mutex ThermalBoundaryFluxBase::_mutex;

template <> InputParameters validParams<ThermalBoundaryFluxBase>()
{
  InputParameters params = validParams<GeneralUserObject>();
  return params;
}

ThermalBoundaryFluxBase::ThermalBoundaryFluxBase(const InputParameters & parameters)
  : GeneralUserObject(parameters)
{
  _flux.resize(libMesh::n_threads());
  _jac1.resize(libMesh::n_threads());
}

void
ThermalBoundaryFluxBase::initialize()
{
  _cached_elem_id = 0;
  _cached_side_id = libMesh::invalid_uint;
}

void
ThermalBoundaryFluxBase::execute()
{
}

void
ThermalBoundaryFluxBase::finalize()
{
}

const std::vector<Real> &
ThermalBoundaryFluxBase::getFlux(unsigned int iside,
                                dof_id_type ielem,
                                const std::vector<Real> & uvec1,
                                const Real & temp,
                                const RealVectorValue & dwave,
                                THREAD_ID tid) const
{
  Threads::spin_mutex::scoped_lock lock(_mutex);
  if (_cached_elem_id != ielem || _cached_side_id != iside)
  {
    _cached_elem_id = ielem;
    _cached_side_id = iside;

    calcFlux(iside, ielem, uvec1, temp, dwave, _flux[tid]);
  }
  return _flux[tid];
}

const DenseMatrix<Real> &
ThermalBoundaryFluxBase::getJacobian(unsigned int iside,
                                     dof_id_type ielem,
                                     const std::vector<Real> & uvec1,
                                     const Real & temp,
                                     const RealVectorValue & dwave,
                                     THREAD_ID tid) const
{
  Threads::spin_mutex::scoped_lock lock(_mutex);
  if (_cached_elem_id != ielem || _cached_side_id != iside)
  {
    _cached_elem_id = ielem;
    _cached_side_id = iside;

    calcJacobian(iside, ielem, uvec1, temp, dwave, _jac1[tid]);
  }
  return _jac1[tid];
}
