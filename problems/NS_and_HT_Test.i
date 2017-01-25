[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
  xmax = 0.1
  ymin = 0.
  ymax = 0.05
  elem_type = QUAD9
[]

[MeshModifiers]
  [./box_1]
    type = SubdomainBoundingBox
    top_right = '.1 0.025 0'
    bottom_left = '.05 0 0'
    block_id = 1
  [../]
  [./box_2]
    type = SubdomainBoundingBox
    top_right = '.04 0.025 0'
    bottom_left = '0.0 0 0'
    block_id = 2
  [../]
  [./box_3]
    type = SubdomainBoundingBox
    top_right = '0.1 0.05 0'
    bottom_left = '0 0.025 0'
    block_id = 3
  [../]
  [./break_boundaries]
    type = BreakBoundaryOnSubdomain
    boundaries = 'left right'
  [../]
  [./inner_wall]
    type = SideSetsBetweenSubdomains
    master_block = 3
    new_boundary = 8
    paired_block = '0 1 2'
  [../]
[]

[Variables]
  [./test]
    block = 3
  [../]
[]

[Functions]
  [./rhou_inlet_func]
    type = ParsedFunction
    value = '(-4 * (40 * y - 0.5)^2 + 1) * 1.161 * 34.7'
  [../]
[]

[Kernels]
  [./test_kernel]
    type = Diffusion
    variable = test
    block = 3
  [../]
[]

[BCs]
  active = 'cold_wall test_left wall_vel_y test_right rhou_boundary rho_boundary'
  [./rho_boundary]
    type = DirichletBC
    variable = rho
    boundary = 'left_to_2 bottom 8'
    value = 1.161
  [../]
  [./wall_vel_y]
    type = DirichletBC
    variable = rhov
    boundary = 'left_to_2 bottom 8'
    value = 0
  [../]
  [./cold_wall]
    type = NSThermalBC
    variable = rhoE
    boundary = left_to_2
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 1
    final = 300
  [../]
  [./rhou_boundary]
    type = FunctionDirichletBC
    variable = rhou
    boundary = 'left_to_2 bottom 8'
    function = rhou_inlet_func
  [../]
  [./hot_wall_const]
    type = NSThermalBC
    variable = rhoE
    boundary = top_to_0
    initial = 300
    fluid_properties = ideal_gas
    rho = rho
    duration = 0.001
    final = 500
  [../]
  [./hot_wall_flux]
    type = NeumannBC
    variable = rhoE
    boundary = top_to_0
    value = 1e4
  [../]
  [./test_right]
    type = DirichletBC
    variable = test
    boundary = right_to_3
    value = 0
  [../]
  [./test_left]
    type = DirichletBC
    variable = test
    boundary = left_to_3
    value = 1
  [../]
[]

[Modules]
  [./FluidProperties]
    [./ideal_gas]
      type = IdealGasFluidProperties
      gamma = 1.4
      R = 287
      k = 1
      mu = 1
    [../]
  [../]
  [./NavierStokes]
    [./Variables]
      # 'rho rhou rhov   rhoE'
      scaling = '1.  1.    1.    9.869232667160121e-6'
      block = '0 1 2'
    [../]
    [./Kernels]
      fluid_properties = ideal_gas
      block = '0 1 2'
    [../]
    [./BCs]
      [./inlet]
        type = NSWeakStagnationInletBC
        boundary = left_to_2
        stagnation_pressure = 102036 # Pa, Mach=0.1 at 1 atm
        stagnation_temperature = 300.6 # K, Mach=0.1 at 1 atm
        sx = 1.
        sy = 0.
        fluid_properties = ideal_gas
      [../]
      [./solid_walls]
        type = NSNoPenetrationBC
        boundary = '8 bottom'
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
  [./fluid]
    # This value is not used in the Euler equations, but it *is* used
    # by the stabilization parameter computation, which it decreases
    # the amount of artificial viscosity added, so it's best to use a
    # realistic value.
    type = Air
    block = '0 1 2 3'
    rho = rho
    rhou = rhou
    rhov = rhov
    rhoE = rhoE
    vel_x = vel_x
    vel_y = vel_y
    temperature = temperature
    enthalpy = enthalpy
    dynamic_viscosity = 1
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
  dt = 1e-5
  dtmin = 1.e-12
  start_time = 0.0
  num_steps = 5
  nl_rel_tol = 1e-9
  nl_max_its = 6
  l_tol = 1e-4
  l_max_its = 100
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
  [./exodus_oversample]
     type = Exodus
     refinements = 10
     file_base = oversample
  [../]
[]

[ICs]
  [./rho_IC]
    variable = rho
    type = ConstantIC
    value = 1.161
    block = '0 1 2'
  [../]
  [./rhou_IC]
    function = rhou_inlet_func
    variable = rhou
    type = FunctionIC
    block = '0 1 2'
  [../]
  [./rhov_IC]
    variable = rhov
    type = ConstantIC
    value = 0
    block = '0 1 2'
  [../]
  [./rhoE_IC]
    variable = rhoE
    type = ConstantIC
    value = 2.5e5
    block = '0 1 2'
  [../]
[]
