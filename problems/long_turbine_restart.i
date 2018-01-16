[GlobalParams]
  family = LAGRANGE
  order = FIRST
  dynamic_viscosity = 1.846e-5 # true = 1.846e-5
  mu = 1.846e-5 # true = 1.846e-5
[]

[Mesh]
  type = FileMesh
  file = /home/ENP/staff/acahill/Projects/phoenix/long_turbine_init_Checkpoint_cp/0695_mesh.cpr
  parallel_type = REPLICATED
[]

[Problem]
	restart_file_base = /home/ENP/staff/acahill/Projects/phoenix/long_turbine_init_Checkpoint_cp/LATEST
[]

[Variables]
  [./solid_temperature]
    block = solid
    initial_from_file_var = solid_temperature
  [../]
  [./rho]
    block = fluid
    scaling = 1.
    family = LAGRANGE
    order = FIRST
    initial_from_file_var = rho
  [../]
  [./rhou]
    block = fluid
    scaling = 1.
    family = LAGRANGE
    order = FIRST
    initial_from_file_var = rhou
  [../]
  [./rhov]
    block = fluid
    scaling = 1.
    family = LAGRANGE
    order = FIRST
    initial_from_file_var = rhov
  [../]
  [./rhoE]
    block = fluid
    scaling = 9.869232667160121e-6
    family = LAGRANGE
    order = FIRST
    initial_from_file_var = rhoE
  [../]
[]

[AuxVariables]
  [./enthalpy]
    block = fluid
    initial_from_file_var = enthalpy
  [../]
  [./temperature]
    block = fluid
    initial_from_file_var = temperature
  [../]
  [./vel_x]
    block = fluid
    initial_from_file_var = vel_x
  [../]
  [./vel_y]
    block = fluid
    initial_from_file_var = vel_y
  [../]
  [./pressure]
    block = fluid
    initial_from_file_var = pressure
  [../]
  [./internal_energy]
    block = fluid
    initial_from_file_var = internal_energy
  [../]
  [./specific_volume]
    block = fluid
    initial_from_file_var = specific_volume
  [../]
  [./Mach]
    block = fluid
    initial_from_file_var = Mach
  [../]
  [./global_temperature]
    initial_from_file_var = global_temperature
  [../]
[]

[Functions]
  [./zero_function]
    type = ParsedVectorFunction
    value_x = 0
    value_y = 0
    value_z = 0
  [../]
[]

[Kernels]
  # The viscous flux kernels are not added by the NS module actions.  Why?
  # The thermal flux kernel is not added by the NS module actions.  Why?
  [./thermal_time]
    type = SpecificHeatConductionTimeDerivative
    variable = solid_temperature
  [../]
  [./thermal_space]
    type = HeatConductionDMI
    variable = solid_temperature
  [../]
  [./decay_heat_source]
		type = HeatSource
		variable = solid_temperature
		value = 19.04e6 # W / m^3
	[../]
  [./rhou_viscous]
    type = NSMomentumViscousFlux
    variable = rhou
    component = 0
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
  [../]
  [./rhov_viscous]
    type = NSMomentumViscousFlux
    variable = rhov
    component = 1
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
  [../]
  [./rhoE_viscous]
    type = NSEnergyViscousFlux
    variable = rhoE
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
  [../]
  [./rhoE_thermal]
    type = NSEnergyThermalFlux
    variable = rhoE
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
    temperature = temperature
  [../]
[]

[InterfaceKernels]
  [./interface_flux]
    type = NSThermalFluxInterface
    variable = rhoE
    neighbor_var = solid_temperature
    boundary = interface
    rho = rho
    fluid_properties = ideal_gas
    var_heat_flux_func = zero_function
    neighbor_heat_flux_func = zero_function
    radiation_temp = radiation_T
  [../]
[]


[AuxKernels]
  [./add_solid_to_global_T]
    type = ParsedAux
    function = solid_temperature
    args = solid_temperature
    variable = global_temperature
    block = solid
  [../]
  [./add_fluid_to_global_T]
    type = ParsedAux
    function = temperature
    args = temperature
    variable = global_temperature
    block = fluid
  [../]
[]

[BCs]
  [./rhou_viscous_interface]
    type = NSMomentumViscousBC
    variable = rhou
    component = 0
    boundary = interface
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
  [../]
  [./rhov_viscous_interface]
    type = NSMomentumViscousBC
    variable = rhov
    component = 1
    boundary = interface
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
  [../]
  [./rhoE_thermal_interface]
    type = NSThermalMatchBC
    variable = rhoE
    v = solid_temperature
    boundary = interface
    rho = rho
    rhou = rhou
    rhov = rhov
    fluid_properties = ideal_gas
  [../]
  [./rhoE_viscous_interface]
    type = NSEnergyViscousBC
    variable = rhoE
    boundary = interface
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    fluid_properties = ideal_gas
    temperature = temperature
  [../]
  [./rhou_interface_velocity]
    type = NSImposedVelocityBC
    variable = rhou
    rho = rho
    desired_velocity = 0.
    boundary = 'interface'
  [../]
  [./rhov_wall_velocity]
    type = NSImposedVelocityBC
    variable = rhov
    rho = rho
    desired_velocity = 0.
    boundary = 'interface wall inlet'
  [../]
[]

[Modules]
  [./FluidProperties]
    [./ideal_gas]
      # mu loaded from GlobalParams
      type = IdealGasFluidProperties
      gamma = 1.4
      R = 287
      k = 2.57e-2 # real value = 2.57e-2
    [../]
  [../]
  [./NavierStokes]
#    [./Variables]
#      # 'rho rhou rhov   rhoE'
#      scaling = '1.  1.    1.    9.869232667160121e-6'
#      family = LAGRANGE
#      order = FIRST
#      block = fluid
#    [../]
#    [./ICs]
#      initial_velocity = '42 0 0' # M = v / sqrt(gamma*R*T) = 0.047
#      initial_pressure = 101325.
#      initial_temperature = 1750.
#      fluid_properties = ideal_gas
#    [../]
    [./Kernels]
      fluid_properties = ideal_gas
    [../]
    [./BCs]
      [./inlet]
        type = NSWeakStagnationInletBC
        boundary = inlet
        stagnation_pressure = 101482. # Pa, M = 0.047 at 101325 Pa
        stagnation_temperature = 1501. # K, M = 0.047 at 101325 Pa
        sx = 1.
        sy = 0.
        fluid_properties = ideal_gas
      [../]
      [./solid_walls]
        type = NSNoPenetrationBC
        boundary = 'wall interface'
        fluid_properties = ideal_gas
      [../]
      [./outlet]
        type = NSStaticPressureOutletBC
        boundary = outlet
        specified_pressure = 101325 # Pa
        fluid_properties = ideal_gas
      [../]
    [../]
  [../]
[]

[Materials]
  [./fluid]
    # This value is not used in the Euler equations, but it *is* used
    # by the stabilization parameter computation, which it decreases
    # the amount of artificial viscosity added, so it's best to use a
    # realistic value.
    # dynamic_viscosity loaded from GlobalParams
    type = Air
    block = fluid
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    temperature = temperature
    enthalpy = enthalpy
    fluid_properties = ideal_gas
  [../]
	[./solid]
		type = GenericConstantMaterial
		block = solid
		prop_names = 'thermal_conductivity density specific_heat epsilon'
		prop_values = '1.5475 8880 163. 1.0'
	[../]
[]

[Postprocessors]
  [./entropy_error]
    type = NSEntropyError
    execute_on = 'initial timestep_end'
    block = fluid
    rho_infty = 0.245 # kg / m^3, air density at 1500 K
    p_infty = 101325. # Pa
    rho = rho
    pressure = pressure
    fluid_properties = ideal_gas
  [../]
	[./radiation_T]
    type = SideAverageValue
    execute_on = 'initial timestep_end'
    boundary = interface
    variable = global_temperature
  [../]
[]

[Preconditioning]
  active = 'FSP'
  [./SMP]
    type = SMP
    full = true
  [../]
  [./FSP]
    type = FSP
    solve_type = PJFNK
    full = true
    topsplit = 'T-NS'
    [./T-NS]
      splitting = 'temperature NS'
      splitting_type = additive
      petsc_options = ''
      petsc_options_iname = ''
      petsc_options_value = ''
    [../]
    [./temperature]
      vars = 'solid_temperature'
      petsc_options = ''
      petsc_options_iname = '-pc_type -pc_hypre_type'
      petsc_options_value = 'hypre boomeramg'
    [../]
    [./NS]
      vars = 'rho rhou rhov rhoE'
      petsc_options = ''
      petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
      petsc_options_value = 'lu mumps'
    [../]
  [../]
[]

[Executioner]
  # We use trapezoidal quadrature.  This improves stability by
  # mimicking the "group variable" discretization approach.
  # ss_tmin = 0.001
  # num_steps = 100
  type = Transient
  dt = 1e-6
  dtmin = 1.e-12
  dtmax = 1.e-5
  start_time = 0.0
  end_time = 2e-3
  nl_rel_tol = 1e-2 # 1e-5
  nl_abs_tol = 1e-3 # We need this as we approach steady state.
  nl_max_its = 10
  l_tol = 1e-2 # 1e-4
  l_max_its = 25
  trans_ss_check = false
  ss_check_tol = 1e-9
  [./TimeStepper]
    type = SolutionTimeAdaptiveDT
    dt = 1e-7
  [../]
  [./Quadrature]
    type = TRAP
    order = FIRST
  [../]
[]

[Adaptivity]
  marker = dont_mark
  max_h_level = 1
  [./Indicators]
    [./rho_grad_jump]
      type = VarRestrictedGradientJumpIndicator
      variable = rho
    [../]
    [./rhou_grad_jump]
      type = VarRestrictedGradientJumpIndicator
      variable = rhou
    [../]
    [./rhov_grad_jump]
      type = VarRestrictedGradientJumpIndicator
      variable = rhov
    [../]
    [./solid_temperature_grad_jump]
      type = VarRestrictedGradientJumpIndicator
      variable = solid_temperature
    [../]
  [../]
  [./Markers]
    [./rho_iefm]
      type = InterfaceErrorFractionMarker
      indicator = rho_grad_jump
      boundary = interface
      refine = 0.10
      coarsen = 0.10
    [../]
    [./rhou_iefm]
      type = InterfaceErrorFractionMarker
      indicator = rhou_grad_jump
      boundary = interface
      refine = 0.10
      coarsen = 0.10
    [../]
    [./rhov_iefm]
      type = InterfaceErrorFractionMarker
      indicator = rhov_grad_jump
      boundary = interface
      refine = 0.10
      coarsen = 0.10
    [../]
		[./solid_temperature_iefm]
      type = InterfaceErrorFractionMarker
      indicator = solid_temperature_grad_jump
      boundary = interface
      refine = 0.10
      coarsen = 0.10
    [../]
    [./final_marker]
      type = ComboMarker
      markers = 'rho_iefm rhou_iefm rhov_iefm'
    [../]
    [./dont_mark]
			type = UniformMarker
			mark = DO_NOTHING
		[../]
  [../]
[]

[Outputs]
  print_perf_log = true
  [./Exodus]
    type = Exodus
    file_base = long_turbine_restart
    execute_on = 'initial timestep_end final'
    output_material_properties = false
    interval = 50
  [../]
  [./CONSOLE]
    type = Console
    output_linear = true
    output_nonlinear = true
    interval = 1
  [../]
  [./Checkpoint]
    type = Checkpoint
    num_files = 2
    interval = 1
  [../]
[]

