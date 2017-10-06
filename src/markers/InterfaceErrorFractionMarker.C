#include "InterfaceErrorFractionMarker.h"

// libMesh includes
#include "libmesh/error_vector.h"

template <> InputParameters validParams<InterfaceErrorFractionMarker>()
{
  InputParameters params = validParams<ErrorFractionMarker>();

  params.addRequiredParam<BoundaryName>("boundary", "The boundary ID name");

  params.addClassDescription("Error fraction marker with interface consistency");
  return params;
}

InterfaceErrorFractionMarker::InterfaceErrorFractionMarker(const InputParameters & parameters)
  : ErrorFractionMarker(parameters),
    _bnd_id(_mesh.getBoundaryID(getParam<BoundaryName>("boundary")))
{
}

Marker::MarkerValue InterfaceErrorFractionMarker::computeElementMarker()
{
  Real error;
  MarkerValue elementMark = DO_NOTHING;

  error = _error_vector[_current_elem->id()];
  if (error > _refine_cutoff)
    elementMark = REFINE;
  else if (error < _coarsen_cutoff)
    elementMark = COARSEN;

	// Examine each side of the element.
  for ( unsigned int s = 0; s < _current_elem->n_sides(); s++ )
  {
    // Is this side on the boundary of interest?
    if ( _mesh.getMesh().get_boundary_info().has_boundary_id(_current_elem, s, _bnd_id) )
    {
      const Elem * neighborElem = _current_elem->neighbor_ptr(s);
      if ( neighborElem == NULL )
        mooseError("No neighbor was found.  Make sure you specified the proper boundary in InterfaceErrorFractionMarker.");

      MarkerValue neighborMark = DO_NOTHING;

      error = _error_vector[neighborElem->id()];
      if (error > _refine_cutoff)
				neighborMark = REFINE;
			else if (error < _coarsen_cutoff)
				neighborMark = COARSEN;

      // The goal is to maintain a consistent h_level across the interface.
      //  If one element lacks a parent, both lack a parent and h_level = 0.
      //  If h_level = 0, coarsening cannot happen and only DO_NOTHING and REFINE are options.
      if ( _current_elem->parent() != NULL )
      {
        // Run checks to see if we can COARSEN consistently.
        //  1) All siblings (parent's children) must be active, i.e. no additional refinement.
        //  2) The error value for all siblings indicates COARSEN.
        bool allSiblingsActive = true;
        MarkerValue siblingMarker = COARSEN;
        const Elem * currentParent = _current_elem->parent();
        const Elem * neighborParent = neighborElem->parent();

        for ( unsigned int c = 0; c < currentParent->n_children(); c++ )
        {
          const Elem * child = currentParent->child_ptr(c);

          if ( child->active() )
          {
		        error = _error_vector[child->id()];
						if (error > _refine_cutoff)
							siblingMarker = static_cast<MarkerValue>(siblingMarker + REFINE);
						else if (error < _coarsen_cutoff)
							siblingMarker = static_cast<MarkerValue>(siblingMarker + COARSEN);
		        else
		          siblingMarker = static_cast<MarkerValue>(siblingMarker + DO_NOTHING);
          }
          else
          {
            allSiblingsActive = false;
            break;
          }
        } // currentParent FOR loop

        for ( unsigned int c = 0; c < neighborParent->n_children(); c++ )
        {
          const Elem * child = neighborParent->child_ptr(c);

          if ( child->active() )
          {
		        error = _error_vector[child->id()];
						if (error > _refine_cutoff)
							siblingMarker = static_cast<MarkerValue>(siblingMarker + REFINE);
						else if (error < _coarsen_cutoff)
							siblingMarker = static_cast<MarkerValue>(siblingMarker + COARSEN);
		        else
		          siblingMarker = static_cast<MarkerValue>(siblingMarker + DO_NOTHING);
          }
          else
          {
            allSiblingsActive = false;
            break;
          }
        } // neighborParent FOR loop

        if ( allSiblingsActive && siblingMarker == COARSEN )
          return COARSEN;

      } // if ( _current_elem->parent() != NULL )

      if ( std::max(elementMark, neighborMark) == REFINE )
				return REFINE;
			else
				return DO_NOTHING;
    }
  }
  
  return elementMark;
}
