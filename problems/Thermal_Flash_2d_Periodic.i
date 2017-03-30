[Mesh]
  type = FileMesh
  file = ../meshes/WAXTS_Channel.e
  dim = 2
[]

[Variables]
  active = ''
  [./solid_temperature]
    scaling = 0.001
    block = '0 2'
    initial_condition = 300
  [../]
[]

[Functions]
  [./rhou_inlet_func]
    type = ParsedFunction
    value = '(-4 * (10 * y - 0.5)^2 + 1) * 1.161 * 34.7'
  [../]
[]

[BCs]
  active = 'Periodic rhoE_thermal rhou_dirichlet rhov_dirichlet rhoE_neumann'
  [./rhov_dirichlet]
    type = DirichletBC
    variable = rhov
    boundary = 'top bottom'
    value = 0
  [../]
  [./rho_neumann]
    type = NeumannBC
    variable = rho
    boundary = 'bottom top'
  [../]
  [./rhou_neumann]
    type = NeumannBC
    variable = rhou
    boundary = 'top bottom'
  [../]
  [./rhov_neumann]
    type = NeumannBC
    variable = rhov
    boundary = 'bottom top'
  [../]
  [./rhoE_neumann]
    type = NeumannBC
    variable = rhoE
    boundary = 'top bottom left right'
  [../]
  [./Periodic]
    [./rhou_periodic]
      variable = rhou
      translation = '-0.2 0 0'
      secondary = left
      primary = right
    [../]
    [./rhov_periodic]
      variable = rhov
      translation = '-0.2 0 0'
      secondary = left
      primary = right
    [../]
    [./rhoE_periodic]
      variable = rhoE
      translation = '-0.2 0 0'
      secondary = left
      primary = right
    [../]
    [./rho_periodic]
      variable = rho
      translation = '-0.2 0 0'
      secondary = left
      primary = right
    [../]
  [../]
  [./rhoE_thermal]
    type = NSThermalBC
    variable = rhoE
    boundary = 'top bottom'
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 1
    final = 300
  [../]
  [./rhou_dirichlet]
    type = FunctionDirichletBC
    variable = rhou
    boundary = 'top bottom'
    function = rhou_inlet_func
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
      # 'rho rhou rhov rhoE'
      scaling = '1.  1.    1.    9.869232667160121e-6'
      block = 1
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
  [./NS_fluid]
    # This value is not used in the Euler equations, but it *is* used
    # by the stabilization parameter computation, which it decreases
    # the amount of artificial viscosity added, so it's best to use a
    # realistic value.
    type = Air
    block = 1
    vel_y = vel_y
    vel_x = vel_x
    vel_z = 0
    rhow = 0
    rhov = rhov
    rhou = rhou
    enthalpy = enthalpy
    fluid_properties = ideal_gas
    rhoE = rhoE
    rho = rho
    dynamic_viscosity = 1.846e-5
    temperature = temperature
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
    block = 1
  [../]
  [./delta_t]
    type = TimestepSize
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
  dt = 1e-6
  dtmin = 1.e-12
  start_time = 0.0
  nl_rel_tol = 1e-9
  nl_max_its = 6
  l_tol = 1e-4
  l_max_its = 100
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
  petsc_options_value = '300                bjacobi  ilu          4'
  end_time = 1
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-5
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
  exodus = true
  [./exodux_output]
    type = Exodus
  [../]
[]

[ICs]
  [./rho_IC]
    variable = rho
    type = ConstantIC
    value = 1.161
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
  [./rhou_IC_block_1]
    function = rhou_inlet_func
    variable = rhou
    type = FunctionIC
  [../]
[]

