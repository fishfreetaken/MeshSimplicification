#pragma once
#include <Eigen/Dense>
using namespace Eigen;

#include <flann/flann.h>
#include <iostream>
using namespace std;

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>


typedef OpenMesh::PolyMesh_ArrayKernelT<>  MyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<>  TriMyMesh;
//typedef Quaternion<float, 0> Quaternionx;

#include <set>
#include <vector>
#include <map>
#include <windows.h>
#include <time.h>

#include <math.h>

#include "MyStruct.h"
