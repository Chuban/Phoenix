#include "InterfaceErrorFractionMarker.h"

// libMesh includes
#include "libmesh/error_vector.h"

template <> InputParameters validParams<InterfaceErrorFractionMarker>()
{
  InputParameters params = validParams<ErrorFractionMarker>();

  params.addRequiredParam<BoundaryName>("boundary", "The boundary ID name");

  params.addClassDescription("Error fraction marker that avoids an interface.");
  return params;
}

InterfaceErrorFractionMarker::InterfaceErrorFractionMarker(const InputParameters & parameters)
  : ErrorFractionMarker(parameters),
    _bnd_id(_mesh.getBoundaryID(getParam<BoundaryName>("boundary")))
{
}

Marker::MarkerValue InterfaceErrorFractionMarker::computeElementMarker()
{
	// Examine each side of the element.
  for ( unsigned int s = 0; s < _current_elem->n_sides(); s++ )
  {
    // Is this side on the boundary of interest?
    if ( _mesh.getMesh().get_boundary_info().has_boundary_id(_current_elem, s, _bnd_id) )
			return DO_NOTHING;
	}

	Real error = _error_vector[_current_elem->id()];
  
	if (error > _refine_cutoff)
    return REFINE;
  else if (error < _coarsen_cutoff)
    return COARSEN;

  return DO_NOTHING;
}
