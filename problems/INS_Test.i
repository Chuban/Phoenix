# This input file tests Dirichlet pressure in/outflow boundary conditions for the incompressible NS equations.

[GlobalParams]
  gravity = '0 0 0'
  rho = 1
  mu = 1
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  xmax = 3.0
  ymax = 1.0
  nx = 30
  ny = 10
  elem_type = QUAD9
[]

[MeshModifiers]
  [./downstream]
    type = SubdomainBoundingBox
    top_right = '3 1 0'
    bottom_left = '0.5 0 0'
    block_id = 1
  [../]
  [./split_top]
    type = BreakBoundaryOnSubdomain
    boundaries = top
  [../]
[]

[Variables]
  [./vel_x]
    order = SECOND
    family = LAGRANGE
  [../]
  [./vel_y]
    order = SECOND
    family = LAGRANGE
  [../]
  [./p]
    order = FIRST
    family = LAGRANGE
  [../]
  [./temperature]
    initial_condition = 300
  [../]
[]

[Kernels]
  [./mass]
    type = INSMass
    variable = p
    u = vel_x
    v = vel_y
    p = p
  [../]
  [./x_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_x
    u = vel_x
    v = vel_y
    p = p
    component = 0
    integrate_p_by_parts = false
  [../]
  [./y_momentum_space]
    type = INSMomentumLaplaceForm
    variable = vel_y
    u = vel_x
    v = vel_y
    p = p
    component = 1
    integrate_p_by_parts = false
  [../]
  [./temperature_space]
    type = INSTemperature
    variable = temperature
    k = 1
    u = vel_x
    v = vel_y
    cp = 1
  [../]
  [./x_momentum_time]
    type = INSMomentumTimeDerivative
    variable = vel_x
  [../]
  [./y_momentum_time]
    type = INSMomentumTimeDerivative
    variable = vel_y
  [../]
  [./temperature_time]
    type = INSTemperatureTimeDerivative
    variable = temperature
    cp = 1
  [../]
[]

[BCs]
  [./x_no_slip]
    type = DirichletBC
    variable = vel_x
    boundary = 'top bottom'
    value = 0.0
  [../]
  [./y_no_slip]
    type = DirichletBC
    variable = vel_y
    boundary = 'left top bottom'
    value = 0.0
  [../]
  [./inlet_p]
    type = DirichletBC
    variable = p
    boundary = left
    value = 1000
  [../]
  [./outlet_p]
    type = DirichletBC
    variable = p
    boundary = right
    value = 0.0
  [../]
  [./inlet_temperature]
    type = DirichletBC
    variable = temperature
    boundary = left
    value = 300
  [../]
  [./hot_wall]
    type = DirichletBC
    variable = temperature
    boundary = top_to_1
    value = 1000
  [../]
  [./outlet_temperature]
    type = NeumannBC
    variable = temperature
    boundary = right
  [../]
  [./insulating_wall]
    type = NeumannBC
    variable = temperature
    boundary = 'bottom top_to_0'
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
  type = Transient
  dt = 1e-3
  petsc_options_iname = '-ksp_gmres_restart -pc_type -sub_pc_type -sub_pc_factor_levels'
  petsc_options_value = '300                bjacobi  ilu          4'
  line_search = none
  nl_rel_tol = 1e-12
  nl_max_its = 6
  l_tol = 1e-6
  l_max_its = 300
  end_time = 30
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-3
  [../]
  [./TimeIntegrator]
    type = CrankNicolson
  [../]
[]

[Adaptivity]
  marker = adaptivity_marker
  max_h_level = 4
  [./Indicators]
    [./adaptivity_indicator]
      type = GradientJumpIndicator
      variable = temperature
      scale_by_flux_faces = true
    [../]
  [../]
  [./Markers]
    [./adaptivity_marker]
      type = ErrorFractionMarker
      coarsen = 0.1
      indicator = adaptivity_indicator
      refine = 0.8
    [../]
  [../]
[]

[Outputs]
  exodus = true
[]

