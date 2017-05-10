[Preconditioning]
  [./SMP]
    type = SMP
    solve_type = PJFNK
  [../]
[]

[GlobalParams]
  order = CONSTANT
  family = MONOMIAL
  rho = rho
  rhou = momx
  rhov = momy
  rhoe = rhoe
  fluid_properties = fp
  slope_reconstruction = rslope
  slope_limiting = lslope
  boundary_list = 'bottom right_to_wind_tunnel interface left_to_wind_tunnel'
  boundary_condition_user_object_list = 'bottom_bcuo outflow_bcuo interface_bcuo inflow_bcuo'
  infinity_density = 1.
  infinity_x_velocity = 0.1
  infinity_pressure = 0.71428571428571428571
  implicit = false
[]

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 4
  ny = 3
  xmax = 0.5
  ymax = 0.15
  elem_type = QUAD
[]

[MeshModifiers]
  [./wind_tunnel_bounding_box]
    type = SubdomainBoundingBox
    bottom_left = '0 0 0'
    top_right = '0.5 0.1 0'
    block_name = wind_tunnel
    block_id = 1
  [../]
  [./rename_solid_wall]
    type = RenameBlock
    new_block_name = solid_wall
    depends_on = wind_tunnel_bounding_box
    old_block_id = 0
  [../]
  [./define_interface]
    type = SideSetsBetweenSubdomains
    master_block = 0
    depends_on = wind_tunnel_bounding_box
    new_boundary = interface
    paired_block = 1
  [../]
  [./break_side_boundaries]
    type = BreakBoundaryOnSubdomain
    boundaries = 'left right'
    depends_on = rename_solid_wall
  [../]
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
[]

[Modules]
  [./FluidProperties]
    [./fp]
      type = IdealGasFluidProperties
      gamma = 1.4
      R = 0.71428571428571428571
      mu = 1e-5
    [../]
  [../]
[]

[UserObjects]
  [./inflow_bcuo]
    type = CNSFVRiemannInvariantBCUserObject
    execute_on = linear
  [../]
  [./outflow_bcuo]
    type = CNSFVFreeOutflowBCUserObject
    execute_on = linear
  [../]
  [./interface_bcuo]
    type = CNSFVSlipBCUserObject
    execute_on = linear
  [../]
  [./bottom_bcuo]
    type = CNSFVSlipBCUserObject
    execute_on = linear
  [../]
  [./rslope]
    type = CNSFVLeastSquaresSlopeReconstruction
    execute_on = linear
    block = wind_tunnel
  [../]
  [./lslope]
    type = CNSFVNoSlopeLimiting
    execute_on = linear
    block = wind_tunnel
  [../]
  [./riemann]
    type = CNSFVHLLCInternalSideFlux
    execute_on = linear
  [../]
  [./inflow_bc]
    type = CNSFVRiemannInvariantBoundaryFlux
    bc_uo = inflow_bcuo
    execute_on = linear
  [../]
  [./outflow_bc]
    type = CNSFVFreeOutflowBoundaryFlux
    execute_on = linear
  [../]
  [./interface_bc]
    type = CNSFVHLLCSlipBoundaryFlux
    execute_on = linear
    bc_uo = interface_bcuo
  [../]
  [./bottom_bc]
    type = CNSFVHLLCSlipBoundaryFlux
    execute_on = linear
    bc_uo = bottom_bcuo
  [../]
[]

[Variables]
  [./rho]
    block = wind_tunnel
  [../]
  [./momx]
    block = wind_tunnel
  [../]
  [./momy]
    block = wind_tunnel
  [../]
  [./rhoe]
    block = wind_tunnel
  [../]
  [./temperature]
    block = solid_wall
    initial_condition = 1.0
  [../]
[]

[AuxVariables]
  [./mach]
    block = wind_tunnel
  [../]
  [./pres]
    block = wind_tunnel
  [../]
  [./velx]
    block = wind_tunnel
  [../]
  [./vely]
    block = wind_tunnel
  [../]
[]

[ICs]
  [./rho_ic]
    variable = rho
    type = ConstantIC
    value = 1.
  [../]
  [./rhou_ic]
    variable = momx
    type = ConstantIC
    value = 0.1
  [../]
  [./rhov_ic]
    variable = momy
    type = ConstantIC
    value = 0.
  [../]
  [./rhoe_ic]
    variable = rhoe
    type = ConstantIC
    value = 1.79071428571
  [../]
  [./mach_ic]
    type = CNSFVMachIC
    variable = mach
  [../]
  [./pres_ic]
    type = CNSFVPressureIC
    variable = pres
  [../]
[]

[Kernels]
  # ### Time derivative of mass
  # ### Time derivative of momentum in x-direction
  # ### Time derivative of momentum in y-direction
  # ### Time derivative of total energy
  [./time_rho]
    implicit = true
    type = TimeDerivative
    variable = rho
    block = wind_tunnel
  [../]
  [./time_momx]
    implicit = true
    type = TimeDerivative
    variable = momx
    block = wind_tunnel
  [../]
  [./time_momy]
    implicit = true
    type = TimeDerivative
    variable = momy
    block = wind_tunnel
  [../]
  [./time_rhoe]
    implicit = true
    type = TimeDerivative
    variable = rhoe
    block = wind_tunnel
  [../]
  [./null_temperature]
    implicit = true
    type = NullKernel
    variable = temperature
    block = solid_wall
  [../]
[]

[DGKernels]
  # ### Mass conservation eqn
  # ### Momentum balance eqn in x-direction
  # ### Momentum balance eqn in y-direction
  # ### Total energy conservation eqn
  [./mass]
    type = CNSFVKernel
    variable = rho
    component = mass
    flux = riemann
    block = wind_tunnel
  [../]
  [./momx]
    type = CNSFVKernel
    variable = momx
    component = x-momentum
    flux = riemann
    block = wind_tunnel
  [../]
  [./momy]
    type = CNSFVKernel
    variable = momy
    component = y-momentum
    flux = riemann
    block = wind_tunnel
  [../]
  [./etot]
    type = CNSFVKernel
    variable = rhoe
    component = total-energy
    flux = riemann
    block = wind_tunnel
  [../]
[]

[InterfaceKernels]
  [./mass]
    type = CNSFVThermalFluxInterface
    variable = rho
    neighbor_var = temperature
    component = mass
    flux = bottom_bc
    boundary = interface
  [../]
  [./momx]
    type = CNSFVThermalFluxInterface
    variable = momx
    neighbor_var = temperature
    component = x-momentum
    flux = bottom_bc
    boundary = interface
  [../]
  [./momy]
    type = CNSFVThermalFluxInterface
    variable = momy
    neighbor_var = temperature
    component = y-momentum
    flux = bottom_bc
    boundary = interface
  [../]
  [./etot]
    type = CNSFVThermalFluxInterface
    variable = rhoe
    neighbor_var = temperature
    component = total-energy
    flux = bottom_bc
    boundary = interface
  [../]
[]

[AuxKernels]
  [./mach]
    type = CNSFVMachAux
    variable = mach
    block = wind_tunnel
  [../]
  [./pres]
    type = CNSFVPressureAux
    variable = pres
    block = wind_tunnel
  [../]
  [./velx]
    type = QuotientAux
    variable = velx
    denominator = rho
    numerator = momx
    block = wind_tunnel
  [../]
  [./vely]
    type = QuotientAux
    variable = vely
    denominator = rho
    numerator = momy
    block = wind_tunnel
  [../]
[]

[BCs]
  # # wall
  # # inflow
  # # outflow
  [./inflow_mass]
    type = CNSFVBC
    boundary = left_to_wind_tunnel
    variable = rho
    component = mass
    flux = inflow_bc
  [../]
  [./inflow_momx]
    type = CNSFVBC
    boundary = left_to_wind_tunnel
    variable = momx
    component = x-momentum
    flux = inflow_bc
  [../]
  [./inflow_momy]
    type = CNSFVBC
    boundary = left_to_wind_tunnel
    variable = momy
    component = y-momentum
    flux = inflow_bc
  [../]
  [./inflow_etot]
    type = CNSFVBC
    boundary = left_to_wind_tunnel
    variable = rhoe
    component = total-energy
    flux = inflow_bc
  [../]
  [./outflow_mass]
    type = CNSFVBC
    boundary = right_to_wind_tunnel
    variable = rho
    component = mass
    flux = outflow_bc
  [../]
  [./outflow_momx]
    type = CNSFVBC
    boundary = right_to_wind_tunnel
    variable = momx
    component = x-momentum
    flux = outflow_bc
  [../]
  [./outflow_momy]
    type = CNSFVBC
    boundary = right_to_wind_tunnel
    variable = momy
    component = y-momentum
    flux = outflow_bc
  [../]
  [./outflow_etot]
    type = CNSFVBC
    boundary = right_to_wind_tunnel
    variable = rhoe
    component = total-energy
    flux = outflow_bc
  [../]
  #[./interface_mass]
  #  type = CNSFVBC
  #  variable = rho
  #  boundary = interface
  #  component = mass
  #  flux = interface_bc
  #[../]
  #[./interface_etot]
  #  type = CNSFVBC
  #  variable = rhoe
  #  boundary = interface
  #  component = total-energy
  #  flux = interface_bc
  #[../]
  [./bottom_mass]
    type = CNSFVBC
    variable = rho
    boundary = bottom
    component = mass
    flux = bottom_bc
  [../]
  [./bottom_etot]
    type = CNSFVBC
    variable = rhoe
    boundary = bottom
    component = total-energy
    flux = bottom_bc
  [../]
  #[./interface_momx]
  #  type = CNSFVBC
  #  variable = momx
  #  boundary = interface
  #  component = x-momentum
  #  flux = interface_bc
  #[../]
  #[./interface_momy]
  #  type = CNSFVBC
  #  variable = momy
  #  boundary = interface
  #  component = y-momentum
  #  flux = interface_bc
  #[../]
  [./bottom_momy]
    type = CNSFVBC
    variable = momy
    boundary = bottom
    component = y-momentum
    flux = bottom_bc
  [../]
  [./bottom_momx]
    type = CNSFVBC
    variable = momx
    boundary = bottom
    component = x-momentum
    flux = bottom_bc
  [../]
[]

[Materials]
  [./cnsfv]
    type = CNSFVMaterial
    block = wind_tunnel
  [../]
  [./temp]
    type = Aluminum7075
    block = solid_wall
    temperature = temperature
  [../]
[]

[Postprocessors]
  [./dt]
    type = CNSFVTimeStepLimit
    execute_on = 'initial timestep_end'
    cfl = 0.1
    block = wind_tunnel
  [../]
[]

[Executioner]
  num_steps = 3
  type = Transient
  solve_type = LINEAR
  l_tol = 1e-4
  end_time = 0.01
  nl_abs_tol = 1e-12
  ss_check_tol = 1e-12
  trans_ss_check = true
  [./TimeIntegrator]
    type = ExplicitTVDRK2
  [../]
  [./TimeStepper]
    type = PostprocessorDT
    postprocessor = dt
  [../]
[]

[Outputs]
  print_perf_log = true
  [./Exodus]
    type = Exodus
    execute_on = 'initial timestep_end final'
    elemental_as_nodal = true
    interval = 1
  [../]
  [./CONSOLE]
    type = Console
    output_linear = true
    output_nonlinear = true
    execute_postprocessors_on = none
    interval = 1
  [../]
[]
