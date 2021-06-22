#pragma once
#include "vertex.hpp"
using namespace std;
using namespace glm;

struct SurfaceMesh
{
	//The vertices info
	vector<Vertex> v;
	//The element indices of face mesh
	vector<unsigned int> i_face;
	//The number of faces
	int n_face;
	//The element indices of line mesh
	vector<unsigned int> i_edge;
	//The number of lines
	int n_edge;
};