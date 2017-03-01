[Variables]
  [./test]
    block = 0
  [../]
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  xmax = 0.1
  ymin = 0.
  ymax = 0.05
  uniform_refine = 1
  elem_type = QUAD9
[]

[MeshModifiers]
  [./wind_tunnel_box]
    type = SubdomainBoundingBox
    top_right = '0.1 0.025 0'
    bottom_left = '0 0 0'
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

[Functions]
  [./rhou_inlet_func]
    type = ParsedFunction
    value = '(-4 * (40 * y - 0.5)^2 + 1) * 1.161 * 34.7'
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = test
  [../]
[]

[BCs]
  active = 'cold_wall rhou_neumann_center rhou_block_1 rhou_block_0 test_top rho_neumann rhov rhov_neumann_center'
  [./cold_wall]
    type = NSThermalBC
    variable = rhoE
    boundary = left_to_1
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 1
    final = 300
  [../]
  [./rhou_block_1]
    type = FunctionDirichletBC
    variable = rhou
    boundary = 'bottom left_to_1 center'
    function = rhou_inlet_func
  [../]
  [./rhou_block_0]
    type = DirichletBC
    variable = rhou
    boundary = 'left_to_0 top right_to_0'
    value = 0
  [../]
  [./rhov]
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
  [./rhou_neumann_center]
    type = NeumannBC
    variable = rhou
    boundary = 'bottom center top'
  [../]
  [./rhov_neumann_center]
    type = NeumannBC
    variable = rhov
    boundary = 'bottom center top'
  [../]
  [./rhoE_neumann_center]
    type = NeumannBC
    variable = rhoE
    boundary = 'bottom center top'
  [../]
  [./test_top]
    type = DirichletBC
    variable = test
    boundary = top
    value = 2.52e5
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
        boundary = 'center bottom top'
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
  active = 'fluid'
  [./fluid]
    # This value is not used in the Euler equations, but it *is* used
    # by the stabilization parameter computation, which it decreases
    # the amount of artificial viscosity added, so it's best to use a
    # realistic value.
    type = Air
    block = '0 1'
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
  [./Al2024]
    type = Aluminum2024
    block = 0
    temperature = HT_temperature
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
  num_steps = 5
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
  [./exodus_oversample]
    type = Exodus
    file_base = oversample
    refinements = 1
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
  [./rhou_block_1]
    function = rhou_inlet_func
    variable = rhou
    type = FunctionIC
    block = 1
  [../]
  [./rhou_block_0]
    variable = rhou
    value = 0
    type = ConstantIC
    block = 0
  [../]
[]

[InterfaceKernels]
  [./test_interface_rhoE]
    neighbor_var = rhoE
    variable = test
    boundary = center
    type = NSThermalInterface
    rhow = 0
    rhov = rhov
    rhou = rhou
    fluid_properties = ideal_gas
    rho = rho
    rhoE = rhoE
  [../]
[]

