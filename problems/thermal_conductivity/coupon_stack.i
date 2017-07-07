[Mesh]
  type = FileMesh
  file = /home/ENP/staff/achaill/Projects/phoenix/meshes/couponStack_18mil.e
[]

[Variables]
  [./temperature]
    initial_condition = 300.
  [../]
[]

[Kernels]
  [./ThermalDiffusion]
    type = HeatConductionDMI
    variable = temperature
  [../]
[]    

[Materials]
  [./Al2024]
    type = Aluminum2024
    temperature = temperature
    block = 'cold_coupon hot_coupon'
  [../]
  [./paint]
    type = GenericConstantMaterial
    prop_names = 'thermal_conductivity dthermal_conductivity/dtemperature'
    prop_values = '1. 0.'
    block = paint
  [../]
[]

[BCs]
  [./cold_side]
    type = DirichletBC
    variable = temperature
    boundary = cold_side
    value = 300.
  [../]
  [./hot_side]
    type = DirichletBC
    variable = temperature
    boundary = hot_side
    value = 450.
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  [./Exodus]
    type = Exodus
  [../]
[]

