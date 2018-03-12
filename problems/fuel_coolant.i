[GlobalParams]
  # These values are used by the Navier-Stokes kernels.
  gravity = '0 0 0'
  laplace = true
  integrate_p_by_parts = true
  family = LAGRANGE
  order = FIRST
  supg = true
  pspg = true
  alpha = 1e-1
[]

[Mesh]
  # Build a rectangular mesh of QUAD4s.
  # This will represent a bare homogenous core in RZ geometry.
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 20
  xmax = 1.625  # active height
  ymax = 3.5    # active radius
  elem_type = QUAD4
  uniform_refine = 0
[]

[MeshModifiers]
  # Translate the mesh so that the left edge is at x=0.
  [./Translate]
    type = Transform
    transform = TRANSLATE
    vector_value = '0 -1.75 0'
  [../]
  # Name a corner node at which pressure will be fixed.
  [./corner_node]
    type = AddExtraNodeset
    new_boundary = 'pinned_node'
    nodes = '0'
  [../]
[]

[Problem]
  # This is a FE problem in RZ geometry.
  type = FEProblem
  coord_type = rz
  rz_coord_axis = y
[]

[Functions]
  # Set the spatial profile of the volumetric heating.
  # The function expression parser in MOOSE does not understand bessel functions
  #   so we approximate it here with a cosine function.  The error is ~4.1%.
  # The power level is set to 90% using Pf.
  [./heating_profile]
    type = ParsedFunction
    value = Pf*(1/9.55276)*cos(0.5*pi*x/Reff)*cos(pi*y/Heff)
    vars = 'Pf Reff Heff'
    vals = '0.90 1.685 3.66'  # power factor, effective radius, effective height
  [../]
[]

[AuxVariables]
  # The variable 'phi' holds the heating profile function as a convenience.
  [./phi]
  [../]
[]

[Variables]
  # These are the Navier-Stokes variables.
  [./p]
    initial_condition = 0
  [../]
  [./vel_x]
    initial_condition = 0
  [../]
  [./vel_y]
    initial_condition = 1.93
  [../]
  [./T]
    initial_condition = 300
  [../]
[]

[Kernels]
  # These kernels implement the Navier-Stokes equations in RZ geometry.
  [./mass_rz]
    type = INSMassRZ
    variable = p
    u = vel_x
    v = vel_y
    w = 0
    p = p
  [../]
  [./x_momentum_time]
    type = INSMomentumTimeDerivative
    variable = vel_x
    rho_name = rho
  [../]
  [./x_momentum_space_rz]
    type = INSMomentumLaplaceFormRZ
    variable = vel_x
    u = vel_x
    v = vel_y
    w = 0
    p = p
    component = 0
  [../]
  [./y_momentum_time]
    type = INSMomentumTimeDerivative
    variable = vel_y
    rho_name = rho
  [../]
  [./y_momentum_space_rz]
    type = INSMomentumLaplaceFormRZ
    variable = vel_y
    u = vel_x
    v = vel_y
    w = 0
    p = p
    component = 1
  [../]
  [./temperature_time]
    type = INSTemperatureTimeDerivative
    variable = T
    rho_name = rho
    cp_name = cp
  [../]
  [./temperature_space]
    type = INSTemperature
    variable = T
    u = vel_x
    v = vel_y
    w = 0
    rho_name = rho
    k_name = k
    cp_name = cp
  [../]
  [./temperature_source]
    type = HeatSource
    variable = T
    value = 3.144e9
    function = heating_profile
  [../]
[]

[AuxKernels]
  # Evaluate the heating profile and save it in the variable 'phi'.
  [./evaluate_phi]
    type = FunctionAux
    function = heating_profile
    variable = phi
  [../]
[]

[Postprocessors]
  # Compute the average outlet temperature.
  [./Tavg_outlet]
    type = SideAverageValue
    variable = T
    boundary = 'top'
  [../]
[]

[Materials]
  # Define the coolant material properties that are needed by the Navier-Stokes equations.
  [./coolant]
    type = GenericConstantMaterial
    prop_names = 'k cp rho mu'
    prop_values = '0.6 4184 1000 0.000853'
  [../]
[]

[BCs]
  # Set boundary conditions on the variables.
  [./vel_x_inlet_and_walls]
    type = DirichletBC
    variable = vel_x
    boundary = 'left bottom right'
    value = 0
  [../]
  [./vel_y_inlet]
    type = DirichletBC
    variable = vel_y
    boundary = 'bottom'
    value = 1.93
  [../]
  [./pressure_pin]
    type = DirichletBC
    variable = p
    boundary = 'pinned_node'
    value = 0
  [../]
  [./T_inlet]
    type = DirichletBC
    variable = T
    boundary = 'bottom'
    value = 300
  [../]
  [./T_outlet]
    type = INSTemperatureNoBCBC
    variable = T
    boundary = 'top right'
  [../]
[]

[Preconditioning]
  # Precondition using the full Jacobian matrix.
  # Solve using Newton's method.
  [./SMP]
    type = SMP
    full = true
    solve_type = NEWTON
  [../]
[]

[Executioner]
  # Approach steady state using a transient simulation.
  type = Transient
  solve_type = NEWTON
  end_time = 1e6
  dt = 1e-1
  trans_ss_check = true
  ss_check_tol = 1e-8
[]

[Outputs]
  # Output to an Exodus file at the end of each timestep.
  execute_on = 'timestep_end'
  exodus = true
[]
