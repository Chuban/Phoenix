[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  xmin = -0.1
  xmax = 0.1
  ymin = 0.
  ymax = 0.125
  uniform_refine = 1
  elem_type = QUAD9
[]

[MeshModifiers]
  [./wind_tunnel_box]
    type = SubdomainBoundingBox
    top_right = '0.1 0.1 0'
    bottom_left = '-0.1 0 0'
    block_id = 1
  [../]
  [./dividing_wall]
    type = SideSetsBetweenSubdomains
    master_block = 0
    new_boundary = center
    paired_block = 1
    depends_on = wind_tunnel_box
  [../]
  [./wall_split]
    type = BreakBoundaryOnSubdomain
    boundaries = 'left right'
    depends_on = wind_tunnel_box
  [../]
[]

[Variables]
  [./solid_temperature]
    scaling = 0.001
    block = 0
    initial_condition = 300
  [../]
[]

[Functions]
  [./rhou_inlet_func]
    type = ParsedFunction
    value = '(-4 * (10 * y - 0.5)^2 + 1) * 1.161 * 34.7'
  [../]
[]

[Kernels]
  [./thermal_space]
    type = HeatConductionDMI
    variable = solid_temperature
  [../]
  [./thermal_time]
    type = SpecificHeatConductionTimeDerivative
    variable = solid_temperature
  [../]
[]

[BCs]
  [./rhoE_thermal_inlet]
    type = NSThermalBC
    variable = rhoE
    boundary = 'left_to_1 bottom'
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 1
    final = 300
  [../]
  [./rhou_dirichlet_block_1]
    type = FunctionDirichletBC
    variable = rhou
    boundary = 'bottom left_to_1 center'
    function = rhou_inlet_func
  [../]
  [./rhou_dirichlet_block_0]
    type = DirichletBC
    variable = rhou
    boundary = 'left_to_0 top right_to_0'
    value = 0
  [../]
  [./rhov_dirichlet]
    type = DirichletBC
    variable = rhov
    boundary = 'top left center bottom right_to_0'
    value = 0
  [../]
  [./rho_neumann]
    type = NeumannBC
    variable = rho
    boundary = 'bottom center top'
  [../]
  [./rhou_neumann]
    type = NeumannBC
    variable = rhou
    boundary = 'bottom center top'
  [../]
  [./rhov_neumann]
    type = NeumannBC
    variable = rhov
    boundary = 'bottom center top'
  [../]
  [./solid_temperature_top]
    type = DirichletBC
    variable = solid_temperature
    boundary = 'top left_to_0'
    value = 300
  [../]
  [./rhoE_match_solid_temperature]
    type = NSThermalMatchBC
    variable = rhoE
    boundary = center
    fluid_properties = ideal_gas
    rho = rho
    v = solid_temperature
  [../]
  [./rhoE_neumann]
    type = NeumannBC
    variable = rhoE
    boundary = bottom
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
        boundary = left_to_1
        stagnation_pressure = 102036 # Pa, Mach=0.1 at 1 atm
        stagnation_temperature = 300.6 # K, Mach=0.1 at 1 atm
        sx = 1.
        sy = 0.
        fluid_properties = ideal_gas
      [../]
      [./solid_walls]
        type = NSNoPenetrationBC
        boundary = 'center bottom'
        fluid_properties = ideal_gas
      [../]
      [./outlet]
        type = NSStaticPressureOutletBC
        boundary = right_to_1
        specified_pressure = 99842.826 # Pa
        fluid_properties = ideal_gas
      [../]
    [../]
  [../]
[]

[Materials]
  [./Al2024]
    type = Aluminum2024
    block = 0
    temperature = solid_temperature
  [../]
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
  petsc_options_value = '300                                 bjacobi    ilu                      4'
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
    block = 1
  [../]
  [./rhou_IC_block_0]
    variable = rhou
    value = 0
    type = ConstantIC
    block = 0
  [../]
[]

[InterfaceKernels]
  [./thermal_flux_interface]
    neighbor_var = rhoE
    fluid_properties = ideal_gas
    rho = rho
    variable = solid_temperature
    boundary = center
    type = NSThermalFluxInterface
  [../]
[]

