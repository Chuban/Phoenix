#include "PhoenixFluidProperties.h"

template <> InputParameters validParams<PhoenixFluidProperties>()
{
  InputParameters params = validParams<FluidProperties>();

  return params;
}

PhoenixFluidProperties::PhoenixFluidProperties(const InputParameters & parameters)
  : SinglePhaseFluidProperties(parameters)
{
}

PhoenixFluidProperties::~PhoenixFluidProperties() {}
