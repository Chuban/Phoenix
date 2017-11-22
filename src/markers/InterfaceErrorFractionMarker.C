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
	// The element should REFINE if:
  // 1) the error is greater than the refine cutoff.
	//		OR
  // 2) any neighbor has an adjacent child that wants and is able to REFINE.
  //     The current element needs to REFINE to keep the level difference <= 1.
	//		OR
  // 3) the interface neighbor will REFINE due to an error greater than the refine cutoff.
	//		OR
  // 4) the interface neighbor will REFINE for reason (2).
  //     The current element needs to REFINE to keep the interface level difference == 0.

	// The element may COARSEN if:
	// 1) all of the terminal descendents of the top parent with a level greater than or 
	//		 equal to the level of the current element may COARSEN.
	//		AND
	// 2) all of the terminal descendents of the top parent interface with a level greater than or 
	//		 equal to the level of the current element may COARSEN.
	//	 	AND
	// 3) the level of this element is the highest level in either parent
	//		OR
	// 4) the parent is not on the interface and the cell may COARSEN normally

  Real error;

  // Refinement route (1):
  error = _error_vector[_current_elem->id()];
  if ( error > _refine_cutoff )
    return REFINE;

	// Examine each side of the element.
  for ( unsigned int s = 0; s < _current_elem->n_sides(); s++ )
  {
		// Refinement route (2):
    const Elem * neighbor = _current_elem->neighbor_ptr(s);
		if ( neighbor != NULL && neighbor->has_children() )
    {
			for ( unsigned int c = 0; c < neighbor->n_children(); c++)
			{
				const Elem * child = neighbor->child_ptr(c);

				error = _error_vector[child->id()];
		    if ( error > _refine_cutoff && child->level() < _adaptivity.getMaxHLevel() && _current_elem->contains_edge_of(child) )
					return REFINE;
			}
		}

    // Is this side on the boundary of interest?
    if ( _mesh.getMesh().get_boundary_info().has_boundary_id(_current_elem, s, _bnd_id) )
    {
      if ( neighbor == NULL )
        mooseError("No neighbor was found.  Make sure you specified the proper boundary in InterfaceErrorFractionMarker.");

			// Refinement route (3):
      error = _error_vector[neighbor->id()];
      if ( error > _refine_cutoff )
				return REFINE;

			// Refinement route (4):
			for ( unsigned int ns = 0; ns < neighbor->n_sides(); ns++ )
			{
				const Elem * nNeighbor = neighbor->neighbor_ptr(ns);
				if ( nNeighbor != NULL && nNeighbor->has_children() )
				{
					for (unsigned int c = 0; c < nNeighbor->n_children(); c++ )
					{
						const Elem * child = nNeighbor->child_ptr(c);

						error = _error_vector[child->id()];
						if ( error > _refine_cutoff && child->level() < _adaptivity.getMaxHLevel() && neighbor->contains_edge_of(child) )
						{
							return REFINE;
						}
					}
				}
			}
    }
  }

  // COARSEN criterion (1), (2), & (3):
	const Elem * topParent = _current_elem->top_parent();

	for ( unsigned int s = 0; s < topParent->n_sides(); s++ )
	{
		if ( _mesh.getMesh().get_boundary_info().has_boundary_id(topParent, s, _bnd_id) )
		{
			// The top parent is on the interface.
			MarkerValue coarsenParentMark;
			MarkerValue coarsenNeighborMark;
			Real maxHLevel = 0;
			const Elem * topParentNeighbor = topParent->neighbor_ptr(s);

			coarsenParentMark = getMaxChildMarker(topParent, _current_elem->level());
			coarsenNeighborMark = getMaxChildMarker(topParentNeighbor, _current_elem->level());
			maxHLevel = std::max(getMaxChildLevel(topParent), getMaxChildLevel(topParentNeighbor));

			if ( coarsenParentMark == COARSEN &&			// COARSEN criterion (1)
					 coarsenNeighborMark == COARSEN &&		// COARSEN criterion (2)
					 _current_elem->level() == maxHLevel)	// COARSEN criterion (3)
				return COARSEN;
			else
				return DO_NOTHING;
		}
	}

	// COARSEN criterion (4):
	error = _error_vector[_current_elem->id()];
	if ( error < _coarsen_cutoff )
		return COARSEN;
	else
		return DO_NOTHING;
	
}

Marker::MarkerValue InterfaceErrorFractionMarker::getMaxChildMarker(const Elem * elem, unsigned int h_min)
{
	if ( elem->has_children() )
	{
		MarkerValue workingMark = COARSEN;
		for ( unsigned int c = 0; c < elem->n_children(); c++ )
		{
			const Elem * child = elem->child_ptr(c);
			workingMark = std::max(getMaxChildMarker(child, h_min), workingMark);
		}
		return workingMark;
	}
	else
	{
		Real error = _error_vector[elem->id()];
		if ( elem->level() >= h_min )
		{
			if ( error > _refine_cutoff )
				return REFINE;
			else if ( error < _coarsen_cutoff )
				return COARSEN;
			else
				return DO_NOTHING;
		}
		else
			// If the level of the element is less than h_min, we want to ignore this element.
			return DONT_MARK;
	}
}

unsigned int InterfaceErrorFractionMarker::getMaxChildLevel(const Elem * elem)
{
	if ( elem->has_children() )
	{
		unsigned int level = 0;
		for ( unsigned int c = 0; c < elem->n_children(); c++ )
		{
			const Elem * child = elem->child_ptr(c);
			level = std::max(level, getMaxChildLevel(child));
		}
		return level;
	}
	else
	{
		return elem->level();
	}
}
