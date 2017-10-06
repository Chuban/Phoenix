#include "SolutionTimeAndPostProcessorAdaptiveDT.h"

template <>
InputParameters
validParams<SolutionTimeAndPostProcessorAdaptiveDT>()
{
  InputParameters params = validParams<SolutionTimeAdaptiveDT>();

  params.addParam<PostprocessorName>("postprocessor_mon", "If specified, the postprocessor value is monitored for relative changes.");
  params.addParam<Real>("threshold", "The threshold for relative change in the postprocessor output.");

  return params;
}

SolutionTimeAndPostProcessorAdaptiveDT::SolutionTimeAndPostProcessorAdaptiveDT(const InputParameters & parameters)
  : SolutionTimeAdaptiveDT(parameters),
    PostprocessorInterface(this),
    _pps_value(isParamValid("postprocessor_mon") ? &getPostprocessorValue("postprocessor_mon") : NULL),
    _threshold(getParam<Real>("threshold")),
    _old_pp_value(0)
{
}

SolutionTimeAndPostProcessorAdaptiveDT::~SolutionTimeAndPostProcessorAdaptiveDT()
{
  SolutionTimeAdaptiveDT::~SolutionTimeAdaptiveDT();
}

void SolutionTimeAndPostProcessorAdaptiveDT::preSolve()
{
  SolutionTimeAdaptiveDT::preSolve();

  if (_pps_value)
    _old_pp_value = *_pps_value;
}

void SolutionTimeAndPostProcessorAdaptiveDT::step()
{
  SolutionTimeAdaptiveDT::step();

  if (std::abs((*_pps_value - _old_pp_value) / _old_pp_value) > _threshold)
    rejectStep();
}

Real SolutionTimeAndPostProcessorAdaptiveDT::computeInitialDT()
{
  return SolutionTimeAdaptiveDT::computeInitialDT();
}

Real SolutionTimeAndPostProcessorAdaptiveDT::computeDT()
{
  return SolutionTimeAdaptiveDT::computeDT();
}

void SolutionTimeAndPostProcessorAdaptiveDT::rejectStep()
{
  SolutionTimeAdaptiveDT::rejectStep();
}
