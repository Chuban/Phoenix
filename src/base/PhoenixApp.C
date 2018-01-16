#include "AppFactory.h"
#include "ModulesApp.h"
#include "Moose.h"
#include "MooseSyntax.h"
#include "PhoenixApp.h"

#include "Aluminum2024.h"
#include "Aluminum7075.h"
#include "Steatite.h"

#include "InterfaceDiffusion.h"
#include "NSThermalMatchBC.h"
#include "NSThermalInterface.h"
#include "NSThermalFluxInterface.h"
#include "HeatConductionDMI.h"
#include "RadiationBC.h"

#include "CNSFVNoSlipBCUserObject.h"
#include "CNSFVHLLCViscousBoundaryFlux.h"
#include "CNSFVHLLCViscousInternalSideFlux.h"
#include "CNSFVThermalBCUserObject.h"
#include "CNSFVThermalResistiveBCUserObject.h"
#include "CNSFVTempAux.h"
#include "AirFluidProperties.h"

#include "SolutionTimeAndPostProcessorAdaptiveDT.h"

#include "GlobalTemperatureAux.h"

#include "VarRestrictedGradientJumpIndicator.h"
#include "InterfaceErrorFractionMarker.h"

template <> InputParameters validParams<PhoenixApp>() {
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

PhoenixApp::PhoenixApp(InputParameters parameters) : MooseApp(parameters) {
  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  PhoenixApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  PhoenixApp::associateSyntax(_syntax, _action_factory);
}

PhoenixApp::~PhoenixApp() {}

// External entry point for dynamic application loading
extern "C" void PhoenixApp__registerApps() { PhoenixApp::registerApps(); }
void PhoenixApp::registerApps() { registerApp(PhoenixApp); }

// External entry point for dynamic object registration
extern "C" void PhoenixApp__registerObjects(Factory &factory) {
  PhoenixApp::registerObjects(factory);
}
void PhoenixApp::registerObjects(Factory &factory) {
  
  // Materials
  registerMaterial(Aluminum2024);
  registerMaterial(Aluminum7075);
  registerMaterial(Steatite);

  // Kernels
  registerNamedKernel(HeatConductionKernelDMI, "HeatConductionDMI");

  // Boundary Conditions
  registerBoundaryCondition(NSThermalMatchBC);
  registerBoundaryCondition(RadiationBC);

  // Auxkernels
  registerAuxKernel(CNSFVTempAux);
  registerAuxKernel(GlobalTemperatureAux);

  // Postprocessors

  // User Objects
  registerUserObject(CNSFVNoSlipBCUserObject);
  registerUserObject(CNSFVHLLCViscousBoundaryFlux);
  registerUserObject(CNSFVHLLCViscousInternalSideFlux);
  registerUserObject(CNSFVThermalBCUserObject);
  registerUserObject(CNSFVThermalResistiveBCUserObject);
  registerUserObject(AirFluidProperties);

  // Interface Kernels
  registerInterfaceKernel(InterfaceDiffusion);
  registerInterfaceKernel(NSThermalInterface);
  registerInterfaceKernel(NSThermalFluxInterface);

  // Time Steppers
  registerTimeStepper(SolutionTimeAndPostProcessorAdaptiveDT);

  // Indicators
  registerIndicator(VarRestrictedGradientJumpIndicator);

  // Markers
  registerMarker(InterfaceErrorFractionMarker);
}

// External entry point for dynamic syntax association
extern "C" void PhoenixApp__associateSyntax(Syntax &syntax,
                                            ActionFactory &action_factory) {
  PhoenixApp::associateSyntax(syntax, action_factory);
}
void PhoenixApp::associateSyntax(Syntax & /*syntax*/,
                                 ActionFactory & /*action_factory*/) {}
