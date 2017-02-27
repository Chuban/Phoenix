[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 25
  ny = 25
  elem_type = QUAD
[]

[MeshModifiers]
  [./center_box]
    type = SubdomainBoundingBox
    top_right = '0.75 0.75 0'
    bottom_left = '0.25 0.25 0'
    block_id = 1
  [../]
  [./test]
    type = BoundingBoxNodeSet
    top_right = '0.75 0.75 0'
    new_boundary = inner
    bottom_left = '0.25 0.25 0'
  [../]
[]

[Variables]
  [./test]
    initial_condition = 1
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = test
    block = 0
  [../]
  [./CV]
    type = ConstantValue
    variable = test
    value = 2
    block = 1
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = test
    boundary = left
    value = 1
  [../]
  [./right]
    type = DirichletBC
    variable = test
    boundary = right
    value = 0
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]

