#ifndef BSPLINESURFACE_HPP
#define BSPLINESURFACE_HPP
#include "vertex.hpp"
#include "SurfaceMesh.hpp"
using namespace std;
using namespace glm;

class BsplineSurface {
public:
	BsplineSurface() :p(NULL) {}

	BsplineSurface(int degree, const vector<vector<vec3>>& cnPoint, const vector<float>& knots_u, const vector<float>& knots_v)
		:m_cnPoint(cnPoint), m_knots_u(knots_u), m_knots_v(knots_v), p(degree) {}

	BsplineSurface& operator=(const BsplineSurface& sur) { return *this; }

	~BsplineSurface() = default;

	inline vec3 normal(const vec3& vector1, const vec3& vector2);
	inline vec3 CurvePosDeBoor(const vector<vec3>& controlpoint, const vector<float>& knots, const float& u, const int& k);
	inline vec3 CurvePosK(const vector<vec3>& controlpoint, const vector<float>& knots, const float& u, const int& k);
	inline vec3 CurvePosN(const vector<vec3>& controlpoint, const vector<float>& knots, const float& u);
	inline vec3 CurvePosDeBoorNURBS(const vector<vec4>& controlpoint, const vector<float>& knots, const float& u, const int& k);
	float N(int i, int p, const float& u, const vector<float>& knots);
	/// <summary>
	/// Generate Mesh for opengl to render
	/// </summary>
	void SetMesh(SurfaceMesh& mesh);
	void SetCurve(SurfaceMesh& mesh);

	int mesh_index(int u, int v, int n_part);

private:

	//The number of rows or columns
	int n = 1000;

	//The degree of both of the 2 dimensions of the surface
	const int p;

	vector<vector<vec3>> m_cnPoint;
	//The knots vector of the u dimension
	vector<float> m_knots_u;
	//The knots vector of the v dimension
	vector<float> m_knots_v;
};

#endif