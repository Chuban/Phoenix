[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 12
  ny = 12
  xmax = 0.1
  ymax = 0.1
  uniform_refine = 1
[]

[MeshModifiers]
  [./block_1_box]
    type = SubdomainBoundingBox
    top_right = '0.1 0.050 0'
    bottom_left = '0 0.025 0'
    block_id = 1
  [../]
  [./block_2_box]
    type = SubdomainBoundingBox
    top_right = '0.1 0.075 0'
    bottom_left = '0 0.050 0'
    block_id = 2
  [../]
  [./block_3_box]
    type = SubdomainBoundingBox
    top_right = '0.1 0.1 0'
    bottom_left = '0 0.075 0'
    block_id = 3
  [../]
[]

[Variables]
  [./temperature]
    initial_condition = 300
  [../]
[]

[Functions]
  [./thermal_ramp]
    type = PiecewiseLinear
    y = '300 1000 1000'
    x = '0 10 60'
  [../]
[]

[Kernels]
  [./temperature_space]
    type = HeatConductionDMI
    variable = temperature
  [../]
  [./temperature_time]
    type = SpecificHeatConductionTimeDerivative
    variable = temperature
  [../]
[]

[BCs]
  [./left]
    type = FunctionDirichletBC
    variable = temperature
    boundary = left
    function = thermal_ramp
  [../]
  [./right]
    type = DirichletBC
    variable = temperature
    boundary = right
    value = 300.
  [../]
[]

[Materials]
  [./Al2024]
    type = Aluminum2024
    block = 0
    temperature = temperature
  [../]
  [./Steatite]
    type = Steatite
    block = 1
    temperature = temperature
  [../]
  [./Al7075]
    type = Aluminum7075
    block = 2
    temperature = temperature
  [../]
  [./Air]
    type = Atmosphere
    block = 3
    temperature = temperature
  [../]
[]

[Executioner]
  type = Transient
  dt = 1e-3
  solve_type = PJFNK
  end_time = 30
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 1e-2
  [../]
[]

[Adaptivity]
  marker = error_marker
  max_h_level = 4
  [./Indicators]
    [./gradient_indicator]
      type = GradientJumpIndicator
      variable = temperature
    [../]
  [../]
  [./Markers]
    [./error_marker]
      type = ErrorFractionMarker
      coarsen = 0.1
      indicator = gradient_indicator
      refine = 0.7
    [../]
  [../]
[]

[Outputs]
  [./exodus_output]
    output_material_properties = true
    type = Exodus
  [../]
[]

