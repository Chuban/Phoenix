#ifndef INTERFACEERRORFRACTIONMARKER_H
#define INTERFACEERRORFRACTIONMARKER_H

#include "MooseMesh.h"
#include "ErrorFractionMarker.h"

class InterfaceErrorFractionMarker;

template <> InputParameters validParams<InterfaceErrorFractionMarker>();

class InterfaceErrorFractionMarker : public ErrorFractionMarker
{
public:
  InterfaceErrorFractionMarker(const InputParameters & parameters);

protected:
  virtual MarkerValue computeElementMarker() override;

  const BoundaryID _bnd_id;

private:
	MarkerValue getMaxChildMarker(const Elem *, unsigned int);
  unsigned int getMaxChildLevel(const Elem *);

};

#endif /* INTERFACEERRORFRACTIONMARKER_H */
