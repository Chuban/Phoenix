#! /usr/bin/env python

import vtk

er = vtk.vtkExodusIIReader()
er.SetFileName("/home/adam/Projects/phoenix/meshes/mug.e")
er.Update()
print(er)
