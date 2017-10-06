#ifndef SOLUTIONTIMEANDPOSTPROCESSORADAPTIVEDT_H
#define SOLUTIONTIMEANDPOSTPROCESSORADAPTIVEDT_H

#include "SolutionTimeAdaptiveDT.h"
#include "PostprocessorInterface.h"

class SolutionTimeAndPostProcessorAdaptiveDT;

template <>
InputParameters validParams<SolutionTimeAndPostProcessorAdaptiveDT>();

/**
 *
 */
class SolutionTimeAndPostProcessorAdaptiveDT : public SolutionTimeAdaptiveDT, public PostprocessorInterface
{
public:
  SolutionTimeAndPostProcessorAdaptiveDT(const InputParameters & parameters);
  virtual ~SolutionTimeAndPostProcessorAdaptiveDT();

  virtual void preSolve() override;
  virtual void step() override;
  virtual void rejectStep() override;

protected:
  virtual Real computeInitialDT() override;
  virtual Real computeDT() override;

  const PostprocessorValue * _pps_value;
  const Real _threshold;
  Real _old_pp_value;
};

#endif /* SOLUTIONTIMEANDPOSTPROCESSORADAPTIVEDT_H */
