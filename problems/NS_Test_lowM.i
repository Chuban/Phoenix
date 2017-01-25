[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  xmax = 0.1
  ymin = 0.
  ymax = 0.025
  uniform_refine = 1
  elem_type = QUAD9
[]

[MeshModifiers]
[]

[Functions]
  [./rhou_inlet_func]
    type = ParsedFunction
    value = '(-4 * (40 * y - 0.5)^2 + 1) * 1.161 * 34.7'
  [../]
  [./dt_limiter]
    type = PiecewiseLinear
    y = '0 1e-2 1e-2'
    x = '0 1e-2 1e4'
  [../]
[]

[BCs]
  active = 'cold_wall rhou_left_boundary wall_vel_y'
  [./rho_boundary]
    type = DirichletBC
    variable = rho
    boundary = top
    value = 1.161
  [../]
  [./wall_vel_y]
    type = DirichletBC
    variable = rhov
    boundary = 'top bottom left'
    value = 0
  [../]
  [./cold_wall]
    type = NSThermalBC
    variable = rhoE
    boundary = left
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 1
    final = 300
  [../]
  [./hot_wall_const]
    type = NSThermalBC
    variable = rhoE
    boundary = top_to_0
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 0.001
    final = 350
  [../]
  [./hot_wall_flux]
    type = NeumannBC
    variable = rhoE
    boundary = top_to_0
    value = 1e4
  [../]
  [./rhou_left_boundary]
    type = FunctionDirichletBC
    variable = rhou
    boundary = 'left top bottom'
    function = rhou_inlet_func
  [../]
  [./rhou_top_bottom_boundary]
    type = DirichletBC
    variable = rhou
    boundary = 'top bottom'
    value = 0
  [../]
[]

[Modules]
  [./FluidProperties]
    [./ideal_gas]
      type = IdealGasFluidProperties
      gamma = 1.4
      R = 287
      k = 1
      mu = 1.846e-5
    [../]
  [../]
  [./NavierStokes]
    [./Variables]
      # 'rho rhou rhov   rhoE'
      scaling = '1.  1.    1.    9.869232667160121e-6'
    [../]
    [./Kernels]
      fluid_properties = ideal_gas
    [../]
    [./BCs]
      [./inlet]
        type = NSWeakStagnationInletBC
        boundary = left
        stagnation_pressure = 102036 # Pa, Mach=0.1 at 1 atm
        stagnation_temperature = 300.6 # K, Mach=0.1 at 1 atm
        sx = 1.
        sy = 0.
        fluid_properties = ideal_gas
      [../]
      [./solid_walls]
        type = NSNoPenetrationBC
        boundary = 'top bottom'
        fluid_properties = ideal_gas
      [../]
      [./outlet]
        type = NSStaticPressureOutletBC
        boundary = right
        specified_pressure = 99842.826 # Pa
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
    type = Air
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    temperature = temperature
    enthalpy = enthalpy
    dynamic_viscosity = 1.846e-5
    fluid_properties = ideal_gas
    vel_z = 0
    rhow = 0
  [../]
[]

[Postprocessors]
  [./entropy_error]
    type = NSEntropyError
    execute_on = 'initial timestep_end'
    rho_infty = 1.225
    p_infty = 101325
    rho = rho
    pressure = pressure
    fluid_properties = ideal_gas
  [../]
  [./dt]
    type = NSExplicitTimestepSelector
    vel_y = vel_y
    vel_x = vel_x
    vel_z = 0
    mu = 1
    beta = 5e2
    rho = rho
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
  type = Transient
  num_steps = 100
  dt = 1e-5
  dtmin = 1.e-12
  start_time = 0.0
  nl_rel_tol = 1e-9
  nl_max_its = 6
  l_tol = 1e-4
  l_max_its = 100
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-5
    cutback_factor = 0.8
    growth_factor = 1.25
  [../]
  [./Quadrature]
    type = TRAP
    order = FIRST
  [../]
  [./TimeIntegrator]
    type = CrankNicolson
  [../]
[]

[Outputs]
  interval = 1
  exodus = true
[]

[ICs]
  [./rho_IC]
    variable = rho
    type = ConstantIC
    value = 1.161
  [../]
  [./rhou_IC]
    function = rhou_inlet_func
    variable = rhou
    type = FunctionIC
  [../]
  [./rhov_IC]
    variable = rhov
    type = ConstantIC
    value = 0
  [../]
  [./rhoE_IC]
    variable = rhoE
    type = ConstantIC
    value = 2.5e5
  [../]
[]

