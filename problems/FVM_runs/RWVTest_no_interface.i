[Preconditioning]
  [./SMP]
    # Why does this option hurt convergence?    
    solve_type = PJFNK
    type = SMP
  [../]
[]

[GlobalParams]
  order = CONSTANT
  family = MONOMIAL
  rho = fluid_density
  rhou = momx
  rhov = momy
  rhoe = energy
  fluid_properties = fp
  slope_reconstruction = rslope
  slope_limiting = lslope
  boundary_list = 'midplane outlet interface inlet slip_wall'
  boundary_condition_user_object_list = 'slipwall_bcuo outflow_bcuo interface_bcuo inflow_bcuo slipwall_bcuo'
  infinity_density = 1.17659149022
  infinity_x_velocity = 1.0
  infinity_pressure = 101325.
  implicit = false
[]

[Mesh]
  type = FileMesh
  file = /home/ENP/staff/achaill/Projects/phoenix/meshes/WAXTS.e
[]

[Problem]
  type = FEProblem
  kernel_coverage_check = false
[]

[Modules]
  [./FluidProperties]
    [./fp]
      type = AirFluidProperties
      gamma = 1.4
      R = 287.058 # J/(kg*K) for dry air
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
  [./slipwall_bcuo]
    type = CNSFVNoSlipBCUserObject
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
  [./slipwall_bc]
    type = CNSFVHLLCSlipBoundaryFlux
    execute_on = linear
    bc_uo = slipwall_bcuo
  [../]
[]

[Variables]
  [./solid_temperature]
    initial_condition = 300.
    family = LAGRANGE
    order = FIRST
    block = 'holder coupon solid_wall backing heat_flux_plate'
  [../]
  [./fluid_density]
    block = wind_tunnel
  [../]
  [./momx]
    block = wind_tunnel
  [../]
  [./momy]
    block = wind_tunnel
  [../]
  [./energy]
    block = wind_tunnel
  [../]
[]

[AuxVariables]
  [./machNumber]
    block = wind_tunnel
  [../]
  [./fluid_pressure]
    block = wind_tunnel
  [../]
  [./velocityx]
    block = wind_tunnel
  [../]
  [./velocityy]
    block = wind_tunnel
  [../]
  [./fluid_temperature]
    block = wind_tunnel
  [../]
  [./global_temperature]
  [../]
[]

[ICs]
  [./density_ic]
    variable = fluid_density
    type = ConstantIC
    value = 1.17659149022
  [../]
  [./rhou_ic]
    # value = 1.17659149022
    variable = momx
    type = ConstantIC
    value = 0.
  [../]
  [./rhov_ic]
    variable = momy
    type = ConstantIC
    value = 0.
  [../]
  [./energy_ic]
    variable = energy
    type = ConstantIC
    value = 253313.088296
  [../]
  [./mach_ic]
    type = CNSFVMachIC
    variable = machNumber
  [../]
  [./pressure_ic]
    type = CNSFVPressureIC
    variable = fluid_pressure
  [../]
[]

[Kernels]
  # ### Time derivative of mass
  # ### Time derivative of momentum in x-direction
  # ### Time derivative of momentum in y-direction
  # ### Time derivative of total energy
  [./time_fluid_density]
    implicit = true
    type = TimeDerivative
    variable = fluid_density
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
  [./time_energy]
    implicit = true
    type = TimeDerivative
    variable = energy
    block = wind_tunnel
  [../]
  [./time_temperature]
    implicit = true
    type = SpecificHeatConductionTimeDerivative
    variable = solid_temperature
    block = 'holder coupon solid_wall backing heat_flux_plate'
  [../]
  [./diff_temperature]
    # thermal_conductivity = thermal_conductivity
    implicit = true
    type = HeatConductionDMI
    variable = solid_temperature
    block = 'holder coupon solid_wall backing heat_flux_plate'
  [../]
[]

[DGKernels]
  # ### Mass conservation eqn
  # ### Momentum balance eqn in x-direction
  # ### Momentum balance eqn in y-direction
  # ### Total energy conservation eqn
  [./mass]
    type = CNSFVKernel
    variable = fluid_density
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
    variable = energy
    component = total-energy
    flux = riemann
    block = wind_tunnel
  [../]
[]

[AuxKernels]
  # This variable is only used for material property calculations.
  [./mach]
    type = CNSFVMachAux
    variable = machNumber
    block = wind_tunnel
  [../]
  [./pres]
    type = CNSFVPressureAux
    variable = fluid_pressure
    block = wind_tunnel
  [../]
  [./velX]
    type = QuotientAux
    numerator = momx
    denominator = fluid_density
    variable = velocityx
    block = wind_tunnel
  [../]
  [./velY]
    type = QuotientAux
    numerator = momy
    denominator = fluid_density
    variable = velocityy
    block = wind_tunnel
  [../]
  [./fluidTemp]
    type = CNSFVTempAux
    variable = fluid_temperature
    block = wind_tunnel
  [../]
  [./globalTemp]
    type = ParsedAux
    args = 'solid_temperature fluid_temperature'
    function = '(solid_temperature + fluid_temperature)'
    variable = global_temperature
  [../]
[]

[BCs]
  [./inflow_mass]
    type = CNSFVBC
    boundary = inlet
    variable = fluid_density
    component = mass
    flux = inflow_bc
  [../]
  [./inflow_momx]
    type = CNSFVBC
    boundary = inlet
    variable = momx
    component = x-momentum
    flux = inflow_bc
  [../]
  [./inflow_momy]
    type = CNSFVBC
    boundary = inlet
    variable = momy
    component = y-momentum
    flux = inflow_bc
  [../]
  [./inflow_etot]
    type = CNSFVBC
    boundary = inlet
    variable = energy
    component = total-energy
    flux = inflow_bc
  [../]
  [./outflow_mass]
    type = CNSFVBC
    boundary = outlet
    variable = fluid_density
    component = mass
    flux = outflow_bc
  [../]
  [./outflow_momx]
    type = CNSFVBC
    boundary = outlet
    variable = momx
    component = x-momentum
    flux = outflow_bc
  [../]
  [./outflow_momy]
    type = CNSFVBC
    boundary = outlet
    variable = momy
    component = y-momentum
    flux = outflow_bc
  [../]
  [./outflow_etot]
    type = CNSFVBC
    boundary = outlet
    variable = energy
    component = total-energy
    flux = outflow_bc
  [../]
  [./bottom_mass]
    type = CNSFVBC
    variable = fluid_density
    boundary = midplane
    component = mass
    flux = slipwall_bc
  [../]
  [./bottom_etot]
    type = CNSFVBC
    variable = energy
    boundary = midplane
    component = total-energy
    flux = slipwall_bc
  [../]
  [./bottom_momy]
    type = CNSFVBC
    variable = momy
    boundary = midplane
    component = y-momentum
    flux = slipwall_bc
  [../]
  [./bottom_momx]
    type = CNSFVBC
    variable = momx
    boundary = midplane
    component = x-momentum
    flux = slipwall_bc
  [../]
  [./slipwall_mass]
    type = CNSFVBC
    variable = fluid_density
    boundary = slip_wall
    component = mass
    flux = slipwall_bc
  [../]
  [./slipwall_etot]
    type = CNSFVBC
    variable = energy
    boundary = slip_wall
    component = total-energy
    flux = slipwall_bc
  [../]
  [./slipwall_momy]
    type = CNSFVBC
    variable = momy
    boundary = slip_wall
    component = y-momentum
    flux = slipwall_bc
  [../]
  [./slipwall_momx]
    type = CNSFVBC
    variable = momx
    boundary = slip_wall
    component = x-momentum
    flux = slipwall_bc
  [../]
  [./interface_mass]
    type = CNSFVBC
    variable = fluid_density
    boundary = interface
    component = mass
    flux = interface_bc
  [../]
  [./interface_etot]
    type = CNSFVBC
    variable = energy
    boundary = interface
    component = total-energy
    flux = interface_bc
  [../]
  [./interface_momy]
    type = CNSFVBC
    variable = momy
    boundary = interface
    component = y-momentum
    flux = interface_bc
  [../]
  [./interface_momx]
    type = CNSFVBC
    variable = momx
    boundary = interface
    component = x-momentum
    flux = interface_bc
  [../]
  [./ambient_temp]
    type = DirichletBC
    variable = solid_temperature
    boundary = exterior
    value = 300.
  [../]
[]

[Materials]
  [./cnsfv]
    type = CNSFVMaterial
    block = wind_tunnel
  [../]
  [./Al2024]
    type = Aluminum2024
    block = 'coupon solid_wall backing'
    temperature = solid_temperature
  [../]
  [./Steatite]
    type = Steatite
    block = 'heat_flux_plate holder'
    temperature = solid_temperature
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
  # num_steps = 100
  type = Transient
  end_time = 1.0
  solve_type = LINEAR
  l_tol = 1e-4
  nl_abs_tol = 1e-12
  ss_check_tol = 1e-12
  trans_ss_check = false
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
    output_material_properties = false
    interval = 25
  [../]
  [./CONSOLE]
    type = Console
    output_linear = false
    output_nonlinear = true
    execute_postprocessors_on = none
    interval = 1
  [../]
[]

