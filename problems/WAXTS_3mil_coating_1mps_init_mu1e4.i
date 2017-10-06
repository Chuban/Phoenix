[GlobalParams]
  family = LAGRANGE
  order = FIRST
  dynamic_viscosity = 1.846e-5 # true = 1.846e-5
  mu = 1.846e-5 # true = 1.846e-5
[]

[Mesh]
  type = FileMesh
  file = /home/ENP/staff/acahill/Projects/phoenix/meshes/WAXTS_3mil_coating.e
  uniform_refine = 0
[]

[Variables]
  [./solid_temperature]
    block = 'solid_wall heat_flux_plate backing holder coupon coating'
    initial_condition = 300.
  [../]
[]

[Functions]
  [./Xe_profile]
    type = ParsedVectorFunction
    value_x = 0
    value_y = 17.4e4*exp(-1*(x-mu)^2/(2*sigma^2))
    value_z = 0
    vars = 'mu sigma'
    vals = '0 0.0058'
  [../]
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
  [../]
[]

[BCs]
  [./rhou_viscous_interface]
    type = NSMomentumViscousBC
    variable = rhou
    component = 0
    boundary = 'interface slip_wall'
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
    boundary = 'interface slip_wall'
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
    boundary = 'interface slip_wall'
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
    boundary = interface
  [../]
  [./rhov_wall_velocity]
    type = NSImposedVelocityBC
    variable = rhov
    rho = rho
    desired_velocity = 0.
    boundary = 'slip_wall interface'
  [../]
  [./ambient_temperature]
    type = DirichletBC
    variable = solid_temperature
    boundary = exterior
    value = 300.
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
    [./Variables]
      # 'rho rhou rhov   rhoE'
      scaling = '1.  1.    1.    9.869232667160121e-6'
      family = LAGRANGE
      order = FIRST
      block = wind_tunnel
    [../]
    [./ICs]
      initial_pressure = 101325.
      initial_temperature = 300.
      # initial_velocity = '1.04157 0 0' # Mach 0.003: = 0.003*sqrt(gamma*R*T)
      initial_velocity = '0.1 0 0'
      fluid_properties = ideal_gas
    [../]
    [./Kernels]
      fluid_properties = ideal_gas
    [../]
    [./BCs]
      [./inlet]
        type = NSWeakStagnationInletBC
        boundary = inlet # 1 inlet
        stagnation_pressure = 101325.63835 # Pa, Mach=0.003 at 1 atm
        stagnation_temperature = 300.0066151 # K, Mach=0.003 at 1 atm
        sx = 1.
        sy = 0.
        fluid_properties = ideal_gas
      [../]
      [./solid_walls]
        type = NSNoPenetrationBC
        boundary = 'slip_wall interface'
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
    block = wind_tunnel
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
  [./tunnel_body]
    type = Aluminum2024
    temperature = solid_temperature
    block = solid_wall
  [../]
  [./holder_backing]
    type = Aluminum2024
    temperature = solid_temperature
    block = backing
  [../]
  [./sample_holder]
    type = Steatite
    temperature = solid_temperature
    block = holder
  [../]
  [./sample]
    type = Aluminum2024
    temperature = solid_temperature
    block = coupon
  [../]
  [./coating]
    type = Aluminum2024
    temperature = solid_temperature
    block = coating
  [../]
  [./hfp_body]
    type = Steatite
    temperature = solid_temperature
    block = heat_flux_plate
  [../]
[]

[Postprocessors]
  [./entropy_error]
    type = NSEntropyError
    execute_on = 'initial timestep_end'
    block = wind_tunnel
    rho_infty = 1.1768292682926829
    p_infty = 101325
    rho = rho
    pressure = pressure
    fluid_properties = ideal_gas
  [../]
[]

[Preconditioning]
  [./SMP_PJFNK]
    type = SMP
    full = true
    solve_type = PJFNK
  [../]
[]

[Executioner]
  # We use trapezoidal quadrature.  This improves stability by
  # mimicking the "group variable" discretization approach.
  # num_steps = 10
  type = Transient
  dt = 1e-6
  dtmin = 1.e-12
  dtmax = 1.e-3
  start_time = 0.0
  end_time = 0.001
  nl_rel_tol = 1e-3
  nl_abs_tol = 1e-4 # We need this as we approach steady state.
  nl_max_its = 10
  l_tol = 1e-4
  l_max_its = 25
  trans_ss_check = true
  ss_check_tol = 1e-9
  ss_tmin = 0.001
  [./TimeStepper]
    type = SolutionTimeAdaptiveDT
    dt = 1e-9
  [../]
  [./Quadrature]
    type = TRAP
    order = FIRST
  [../]
[]

[Adaptivity]
  marker = final_marker
  max_h_level = 2
  [./Indicators]
    [./rho_grad_jump]
      type = VarRestrictedGradientJumpIndicator
      variable = rho
    [../]
    [./rhou_grad_jump]
      type = VarRestrictedGradientJumpIndicator
      variable = rhou
    [../]
  [../]
  [./Markers]
    [./rho_iefm]
      type = InterfaceErrorFractionMarker
      indicator = rho_grad_jump
      boundary = interface
      refine = 0.25
      coarsen = 0.5
    [../]
    [./rhou_iefm]
      type = InterfaceErrorFractionMarker
      indicator = rhou_grad_jump
      boundary = interface
      refine = 0.25
      coarsen = 0.5
    [../]
    [./final_marker]
      type = ComboMarker
      markers = 'rho_iefm rhou_iefm'
    [../]
  [../]
[]

[Outputs]
  print_perf_log = true
  [./Exodus]
    type = Exodus
    file_base = WAXTS_3mil_coating_1mps_init_mu1e4_output
    execute_on = 'initial timestep_end final'
    output_material_properties = false
    interval = 1
  [../]
  [./CONSOLE]
    type = Console
    output_linear = true
    output_nonlinear = true
    interval = 1
  [../]
[]

