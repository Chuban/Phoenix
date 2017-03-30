# ### 2D transient simulation of
# ### Mach 3 wind tunnel with a forward step

[GlobalParams]
  order = CONSTANT
  family = MONOMIAL
  rho = rho
  rhou = momx
  rhov = momy
  rhoe = rhoe
  fluid_properties = fp
  slope_reconstruction = rslope
  slope_limiting = lslope
  boundary_list = '1 2 3 4'
  boundary_condition_user_object_list = 'step_bcuo upper_wall_bcuo inflow_bcuo outflow_bcuo'
  infinity_density = 1.
  infinity_x_velocity = 0.1
  infinity_pressure = 0.71428571428571428571
  implicit = false
[]

[Mesh]
  type = FileMesh
  file = /home/moose-user/Projects/phoenix/meshes/2d_mach3step.e
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
[]

[Modules]
  [./FluidProperties]
    [./fp]
      type = IdealGasFluidProperties
      gamma = 1.4
      R = 0.71428571428571428571
      mu = 1e-4
    [../]
  [../]
[]

[UserObjects]
  [./inflow_bcuo]
    type = CNSFVRiemannInvariantBCUserObject
    execute_on = linear
  [../]
  [./outflow_bcuo]
    type = CNSFVRiemannInvariantBCUserObject
    execute_on = linear
  [../]
  [./rslope]
    type = CNSFVLeastSquaresSlopeReconstruction
    execute_on = linear
  [../]
  [./lslope]
    type = CNSFVMinmaxSlopeLimiting
    execute_on = linear
  [../]
  [./riemann]
    type = CNSFVHLLCViscousInternalSideFlux
    execute_on = linear
  [../]
  [./inflow_bc]
    type = CNSFVRiemannInvariantBoundaryFlux
    bc_uo = inflow_bcuo
    execute_on = linear
  [../]
  [./outflow_bc]
    type = CNSFVRiemannInvariantBoundaryFlux
    bc_uo = outflow_bcuo
    execute_on = linear
  [../]
  [./step_bcuo]
    type = CNSFVNoSlipBCUserObject
    execute_on = linear
  [../]
  [./step_bc]
    type = CNSFVHLLCViscousBoundaryFlux
    execute_on = linear
    bc_uo = step_bcuo
  [../]
  [./upper_wall_bcuo]
    type = CNSFVNoSlipBCUserObject
    execute_on = linear
  [../]
  [./upper_wall_bc]
    type = CNSFVHLLCViscousBoundaryFlux
    execute_on = linear
    bc_uo = upper_wall_bcuo
  [../]
[]

[Variables]
  [./rho]
  [../]
  [./momx]
  [../]
  [./momy]
  [../]
  [./rhoe]
  [../]
[]

[AuxVariables]
  [./mach]
  [../]
  [./pres]
  [../]
[]

[ICs]
  [./rho_ic]
    variable = rho
    type = ConstantIC
    value = 1.
  [../]
  [./rhou_ic]
    variable = momx
    type = ConstantIC
    value = 0.1
  [../]
  [./rhov_ic]
    variable = momy
    type = ConstantIC
    value = 0.
  [../]
  [./rhoe_ic]
    variable = rhoe
    type = ConstantIC
    value = 1.79071428571
  [../]
  [./mach_ic]
    type = CNSFVMachIC
    variable = mach
  [../]
  [./pres_ic]
    type = CNSFVPressureIC
    variable = pres
  [../]
[]

[Kernels]
  # ### Time derivative of mass
  # ### Time derivative of momentum in x-direction
  # ### Time derivative of momentum in y-direction
  # ### Time derivative of total energy
  [./time_rho]
    implicit = true
    type = TimeDerivative
    variable = rho
  [../]
  [./time_momx]
    implicit = true
    type = TimeDerivative
    variable = momx
  [../]
  [./time_momy]
    implicit = true
    type = TimeDerivative
    variable = momy
  [../]
  [./time_rhoe]
    implicit = true
    type = TimeDerivative
    variable = rhoe
  [../]
[]

[DGKernels]
  # ### Mass conservation eqn
  # ### Momentum balance eqn in x-direction
  # ### Momentum balance eqn in y-direction
  # ### Total energy conservation eqn
  [./mass]
    type = CNSFVKernel
    variable = rho
    component = mass
    flux = riemann
  [../]
  [./momx]
    type = CNSFVKernel
    variable = momx
    component = x-momentum
    flux = riemann
  [../]
  [./momy]
    type = CNSFVKernel
    variable = momy
    component = y-momentum
    flux = riemann
  [../]
  [./etot]
    type = CNSFVKernel
    variable = rhoe
    component = total-energy
    flux = riemann
  [../]
[]

[AuxKernels]
  [./mach]
    type = CNSFVMachAux
    variable = mach
  [../]
  [./pres]
    type = CNSFVPressureAux
    variable = pres
  [../]
[]

[BCs]
  # # wall
  # # inflow
  # # outflow
  [./wall_mass]
    type = CNSFVBC
    boundary = 1
    variable = rho
    component = mass
    flux = step_bc
  [../]
  [./wall_momx]
    type = CNSFVBC
    boundary = 1
    variable = momx
    component = x-momentum
    flux = step_bc
  [../]
  [./wall_momy]
    type = CNSFVBC
    boundary = 1
    variable = momy
    component = y-momentum
    flux = step_bc
  [../]
  [./wall_etot]
    type = CNSFVBC
    boundary = 1
    variable = rhoe
    component = total-energy
    flux = step_bc
  [../]
  [./inflow_mass]
    type = CNSFVBC
    boundary = 3
    variable = rho
    component = mass
    flux = inflow_bc
  [../]
  [./inflow_momx]
    type = CNSFVBC
    boundary = 3
    variable = momx
    component = x-momentum
    flux = inflow_bc
  [../]
  [./inflow_momy]
    type = CNSFVBC
    boundary = 3
    variable = momy
    component = y-momentum
    flux = inflow_bc
  [../]
  [./inflow_etot]
    type = CNSFVBC
    boundary = 3
    variable = rhoe
    component = total-energy
    flux = inflow_bc
  [../]
  [./outflow_mass]
    type = CNSFVBC
    boundary = 4
    variable = rho
    component = mass
    flux = outflow_bc
  [../]
  [./outflow_momx]
    type = CNSFVBC
    boundary = 4
    variable = momx
    component = x-momentum
    flux = outflow_bc
  [../]
  [./outflow_momy]
    type = CNSFVBC
    boundary = 4
    variable = momy
    component = y-momentum
    flux = outflow_bc
  [../]
  [./outflow_etot]
    type = CNSFVBC
    boundary = 4
    variable = rhoe
    component = total-energy
    flux = outflow_bc
  [../]
  [./upper_wall_momx]
    type = CNSFVBC
    variable = momx
    boundary = 2
    component = x-momentum
    flux = upper_wall_bc
  [../]
  [./upper_wall_momy]
    type = CNSFVBC
    variable = momy
    boundary = 2
    component = y-momentum
    flux = upper_wall_bc
  [../]
  [./upper_wall_mass]
    type = CNSFVBC
    variable = rho
    boundary = 2
    component = mass
    flux = upper_wall_bc
  [../]
  [./upper_wall_etot]
    type = CNSFVBC
    variable = rhoe
    boundary = 2
    component = total-energy
    flux = upper_wall_bc
  [../]
[]

[Materials]
  [./cnsfv]
    type = CNSFVMaterial
    block = 1
  [../]
[]

[Postprocessors]
  [./dt]
    type = CNSFVTimeStepLimit
    execute_on = 'initial timestep_end'
    cfl = 0.1
  [../]
[]

#[Preconditioning]
#  [./SMP]
#    type = SMP
#    solve_type = PJFNK
#  [../]
#[]

[Executioner]
  type = Transient
  solve_type = LINEAR
  l_tol = 1e-4
  trans_ss_check = true
  ss_check_tol = 1e-12
  start_time = 0.
  end_time = 10
  [./TimeIntegrator]
    type = ExplicitTVDRK2
  [../]
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
  [../]
[]

[Outputs]
  print_perf_log = true
  [./Exodus]
    type = Exodus
    execute_on = 'initial timestep_end final'
    file_base = 2d_mach3step_out
    elemental_as_nodal = true
  [../]
  [./CONSOLE]
    type = Console
    output_linear = true
    output_nonlinear = true
    execute_postprocessors_on = none
    interval = 1
  [../]
[]
