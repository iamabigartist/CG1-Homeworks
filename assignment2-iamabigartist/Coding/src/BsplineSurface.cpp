#include "BsplineSurface.hpp"
#include <algorithm>
#include <random>
#include "Utility.hpp"
using namespace glm;

/**
 * Evaluate Normal of two vector
 * @param[in] vector1
 * @param[in] vector2
 * @return vec3 is the normal of these two vector.
 */
inline vec3 BsplineSurface::normal(const vec3& v1, const vec3& v2)
{
	return normalize(cross(v1, v2));
}

float BsplineSurface::N(int i, int p, const float& u, const vector<float>& u_)
{
	if (p == 0)
	{
		if (u_[i] <= u && u < u_[i + 1])
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return
			N(i, p - 1, u, u_) *
			(u - u_[i]) / (u_[i + p] - u_[i]) +
			N(i + 1, p - 1, u, u_) *
			(u_[i + p + 1] - u) / (u_[i + p + 1] - u_[i + 1]);
	}
}

/**
 * Evaluate point on a Bspline curve
 * @param[in] control_point Control points of the curve.
 * @param[in] knots Knot vector of the curve.
 * @param[in] u Parameter to evaluate the curve at.
 * #param[in] k the index of u_k which is the last smaller knot than u.
 * @return vec3 position on the curve at parameter u
 */
inline vec3 BsplineSurface::CurvePosK(const vector<vec3>& controlpoint, const vector<float>& knots, const float& u, const int& k)
{
	//The temp column to store the cur column of points.
	auto p_column = vector<vec3>(controlpoint.begin() + k - p, controlpoint.begin() + k + 1);

	float alpha = (u - knots[k]) / (knots[k + 1] - knots[k]);

	for (int r = 1; r <= p; r++)
	{
		for (int i_p = 0; i_p < p + 1 - r; i_p++)
		{
			p_column[i_p] = (1 - alpha) * p_column[i_p] + alpha * p_column[i_p + 1];
		}
	}

	//pos is at the last point of p_column.
	return p_column[0];
}

/**
 * Evaluate point on a Bspline curve
 * @param[in] control_point Control points of the curve.
 * @param[in] knots Knot vector of the curve.
 * @param[in] u Parameter to evaluate the curve at.
 * #param[in] k the index of u_k which is the last smaller knot than u.
 * @return vec3 position on the curve at parameter u
 */
inline vec3 BsplineSurface::CurvePosN(const vector<vec3>& controlpoint, const vector<float>& knots, const float& u)
{
	vector<float> N_;
	for (int i = 0; i < controlpoint.size(); i++)
	{
		N_.push_back(N(i, p, u, knots));
	}
	vec3 point(0, 0, 0);
	for (int i = 0; i < controlpoint.size(); i++)
	{
		point += N_[i] * controlpoint[i];
	}
	return point;
}

/**
 * Evaluate point on a Bspline curve
 * @param[in] control_point Control points of the curve.
 * @param[in] knots Knot vector of the curve.
 * @param[in] u Parameter to evaluate the curve at.
 * #param[in] k the index of u_k which is the last smaller knot than u.
 * @return vec3 position on the curve at parameter u
 */
inline vec3 BsplineSurface::CurvePosDeBoor(const vector<vec3>& controlpoint, const vector<float>& knots, const float& u, const int& k)
{
	//The temp column to store the cur column of points.
	auto p_column = vector<vec3>(controlpoint.begin() + k - p, controlpoint.begin() + k + 1);

	for (int r = 1; r <= p; r++)//r times construct
	{
		//the rth column has p-r+1 points.

		for (int i_k = k, i_p = p; i_k >= k - p + r; i_k--, i_p--)//i_k is the knot index, i_p is the point index in the p_column.
		{
			////if the denominator is 0, which represents a multiplicity of s, then a will be 0.
			//float den = knots[i_k + p - r + 1] - knots[i_k];

			////a_(i,r)
			//float a_ir = den == 0 ? 1 : (u - knots[i_k]) / den;

			float a_ir = (u - knots[i_k]) / (knots[i_k + p - r + 1] - knots[i_k]);

			p_column[i_p] = ((1.0f - a_ir) * p_column[i_p - 1]) + (a_ir * p_column[i_p]);//1 may cause bug
		}
	}

	//pos is at the last point of p_column.
	return p_column[p];
}

/**
 * Evaluate point on a Bspline curve
 * @param[in] control_point Control points of the curve.
 * @param[in] knots Knot vector of the curve.
 * @param[in] u Parameter to evaluate the curve at.
 * #param[in] k the index of u_k which is the last smaller knot than u.
 * @return vec3 position on the curve at parameter u
 */
inline vec3 BsplineSurface::CurvePosDeBoorNURBS(const vector<vec4>& controlpoint, const vector<float>& knots, const float& u, const int& k)
{
	//The temp column to store the cur column of points.
	auto p_column = vector<vec3>(controlpoint.begin() + k - p, controlpoint.begin() + k + 1);

	for (int r = 1; r <= p; r++)//r times construct
	{
		//the rth column has p-r+1 points.

		for (int i_k = k, i_p = p; i_k >= k - p + r; i_k--, i_p--)//i_k is the knot index, i_p is the point index in the p_column.
		{
			////if the denominator is 0, which represents a multiplicity of s, then a will be 0.
			//float den = knots[i_k + p - r + 1] - knots[i_k];

			////a_(i,r)
			//float a_ir = den == 0 ? 1 : (u - knots[i_k]) / den;

			float a_ir = (u - knots[i_k]) / (knots[i_k + p - r + 1] - knots[i_k]);

			p_column[i_p] = ((1.0f - a_ir) * p_column[i_p - 1]) + (a_ir * p_column[i_p]);//1 may cause bug
		}
	}

	//pos is at the last point of p_column.
	return p_column[p];
}

int BsplineSurface::mesh_index(int u, int v, int n_part) { return v + u * n_part; }

void BsplineSurface::SetCurve(SurfaceMesh& mesh)
{
	//init
	mesh.v.clear();
	mesh.i_edge.clear();
	mesh.i_face.clear();
	mesh.n_edge = 0;
	mesh.n_face = 0;

	float step = 1.0 / (n);
	float u;
	int k;
	for (int i = 0; i < n; i++)
	{
		u = i * step;
		if (!in_curve_range(k, p, u, m_knots_v)) { continue; }
		mesh.v.push_back({ CurvePosK(m_cnPoint[0], m_knots_v,u,k),vec3(cos(glfwGetTime())),vec2(glfwGetTime()) });
	}
}

void BsplineSurface::SetMesh(SurfaceMesh& mesh)
{
	//init
	mesh.v.clear();
	mesh.i_edge.clear();
	mesh.i_face.clear();
#pragma region Construct Vertex Mesh

	//The 2d grid mesh
	auto v_2d_N = vector<vector<vec3>>(n, vector<vec3>(n, vec3(-10, 10, 10)));
	auto v_2d_DeBoor = vector<vector<vec3>>();
	float step = 1.0 / n;

	//the 1st dimension sample value
	float u;
	//the 2nd dimension sample value
	float v;

	//the index of u_k which is the last smaller knot than u.
	int k_u;
	//the index of v_k which is the last smaller knot than v.
	int k_v;
	for (int i_v = 0; i_v < n; i_v++)
	{
		v = i_v * step;
		//Check if the v is in range of surface
		if (!in_curve_range(k_v, p, v, m_knots_v)) { continue; }

		//construct the u direction curve.
		auto curve_u = vector<vec3>();

		//get every v direction curve.
		//fetch every control point on every v direction curve.
		for (int i = 0; i < m_cnPoint.size(); i++)
		{
			curve_u.push_back(CurvePosDeBoor(m_cnPoint[i], m_knots_v, v, k_v));
		}

		v_2d_DeBoor.push_back(vector<vec3>());

		for (int i_u = 0; i_u < n; i_u++)
		{
			u = i_u * step;
			//Check if the u is in range of surface
			if (!in_curve_range(k_u, p, u, m_knots_u)) { continue; }

			v_2d_DeBoor[v_2d_DeBoor.size() - 1].push_back(CurvePosDeBoor(curve_u, m_knots_u, u, k_u));
		}
	}
#pragma endregion

	assert(v_2d_DeBoor.size() == v_2d_DeBoor[0].size());
	int n_part = v_2d_DeBoor.size();

#pragma region AddNormal,Into v
	vec3 ref_normal(0, 0, 1);
	vec3 cur_normal;
	for (int i_v = 0; i_v < n_part; i_v++)
	{
		for (int i_u = 0; i_u < n_part; i_u++)
		{
			//horizontal tangent, v dimension
			vec3 t_h;
			if (i_v == 0) { t_h = v_2d_DeBoor[i_u][i_v + 1] - v_2d_DeBoor[i_u][i_v]; }
			else if (i_v < n_part - 1) {
				t_h = (
					(v_2d_DeBoor[i_u][i_v + 1] - v_2d_DeBoor[i_u][i_v]) +
					(v_2d_DeBoor[i_u][i_v] - v_2d_DeBoor[i_u][i_v - 1])) / 2.0f;
			}
			else { t_h = v_2d_DeBoor[i_u][i_v] - v_2d_DeBoor[i_u][i_v - 1]; }

			//vertical tangent, u dimension
			vec3 t_v;
			if (i_u == 0) { t_v = v_2d_DeBoor[i_u + 1][i_v] - v_2d_DeBoor[i_u][i_v]; }
			else if (i_u < n_part - 1) {
				t_v = (
					(v_2d_DeBoor[i_u + 1][i_v] - v_2d_DeBoor[i_u][i_v]) +
					(v_2d_DeBoor[i_u][i_v] - v_2d_DeBoor[i_u - 1][i_v])) / 2.0f;
			}
			else { t_v = v_2d_DeBoor[i_u][i_v] - v_2d_DeBoor[i_u - 1][i_v]; }
			cur_normal = normal(t_h, t_v);
			float uv_factor = 2 - abs(dot(cur_normal, ref_normal));

			//Push into the v
			mesh.v.push_back(Vertex(v_2d_DeBoor[i_u][i_v], cur_normal, vec2(i_u, i_v) / (float)n_part));
		}
	}
#pragma endregion

#pragma region Face Indices
	mesh.n_face = 2 * (n_part - 1) * (n_part - 1);
	for (int i_u = 0; i_u < n_part - 1; i_u++)
	{
		for (int i_v = 0; i_v < n_part - 1; i_v++)
		{
			//Face up left
			mesh.i_face.push_back(mesh_index(i_u, i_v, n_part));
			mesh.i_face.push_back(mesh_index(i_u + 1, i_v, n_part));
			mesh.i_face.push_back(mesh_index(i_u, i_v + 1, n_part));

			//Face down right
			mesh.i_face.push_back(mesh_index(i_u + 1, i_v + 1, n_part));
			mesh.i_face.push_back(mesh_index(i_u, i_v + 1, n_part));
			mesh.i_face.push_back(mesh_index(i_u + 1, i_v, n_part));
		}
	}
#pragma endregion

#pragma region Edge Indices
	mesh.n_edge = 2 * n_part * (n_part - 1);
	for (int i_u = 0; i_u < n_part; i_u++)
	{
		for (int i_v = 0; i_v < n_part; i_v++)
		{
			if (i_v < n_part - 1)//Horizontal
			{
				mesh.i_edge.push_back(mesh_index(i_u, i_v, n_part));
				mesh.i_edge.push_back(mesh_index(i_u, i_v + 1, n_part));
			}
			if (i_u < n_part - 1)//Vertical
			{
				mesh.i_edge.push_back(mesh_index(i_u, i_v, n_part));
				mesh.i_edge.push_back(mesh_index(i_u + 1, i_v, n_part));
			}
		}
	}
#pragma endregion

	return;
}