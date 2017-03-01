[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 5
  xmax = 2
  uniform_refine = 2
[]

[MeshModifiers]
  [./interface]
    type = SideSetsBetweenSubdomains
    depends_on = subdomain
    master_block = 0
    paired_block = 1
    new_boundary = interface
    force_prepare = true
  [../]
  [./subdomain]
    type = SubdomainBoundingBox
    top_right = '2.0 1.0 1.0'
    bottom_left = '1.0 0.0 0.0'
    block_id = 1
  [../]
[]

[Variables]
  [./u]
    initial_condition = 0.75
  [../]
  [./v]
    initial_condition = 0.25
  [../]
[]

[Kernels]
  [./diff_u]
    type = CoefDiffusion
    variable = u
    coef = 4
  [../]
  [./diff_v]
    type = CoefDiffusion
    variable = v
    coef = 2
  [../]
[]

[InterfaceKernels]
  [./interface_flux]
    neighbor_var = v
    variable = u
    boundary = interface
    type = InterfaceDiffusion
    D_neighbor = 2
    D = 4
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 400
  [../]
  [./right]
    type = DirichletBC
    variable = v
    boundary = right
    value = 300
  [../]
  [./v_match_u]
    type = MatchedValueBC
    variable = v
    boundary = interface
    v = u
  [../]
[]

[Postprocessors]
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
[]

[Outputs]
  [./exodus]
    type = Exodus
  [../]
[]

