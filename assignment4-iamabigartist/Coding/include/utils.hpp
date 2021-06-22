#pragma once
#include <iostream>
#include <string>
#include "Eigen/Dense"
#include <ray.hpp>

#define UNREACHABLE std::cout << "Error: Unreachable code executed. Exit(-1)..." << std::endl; exit(-1);

using namespace std;
using namespace Eigen;
using V3 = Vector3f;

/**
 * String processing utilities
 */

namespace strutils
{
	/* Trim from left */
	void ltrim ( std::string& s );
	/* Trim from right */
	void rtrim ( std::string& s );
	/* Trim from both left and right */
	void trim ( std::string& s );
};

/**
 * Mathematical utilities
 */

namespace mathutils
{
	/* Clamps the input x to the closed range [lo, hi] */
	float clamp ( float x, float lo, float hi );
	/* Performs Gamma correction on x and outputs an integer between 0-255. */
	unsigned char gamma_correction ( float x );
	/* Uniformly sample N numbers among [a, b] */
	vector<float> unif ( float a, float b, int N );
	/* Uniformly sample 1 numbers among [a, b] */
	float unif ( float a, float b );
	/// <summary>
	/// return specular wi accroding to wi and n
	/// </summary>
	inline V3 reflect ( const V3& wo, const V3& n ) { return 2 * wo.dot ( n ) * n - wo; }
	inline bool refract ( const V3& w_incident, const V3& n, float eta_i, float eta_t, V3& w_refracted )
	{
		float eta = eta_i / eta_t;
		float cos_theta_i = n.dot ( w_incident );
		float sin2_theta_i = max ( 0.0f, 1.0f - cos_theta_i * cos_theta_i );
		float sin2_theta_t = eta * eta * sin2_theta_i;//UNDONE: May be not i or t.
		if ( sin2_theta_t >= 1 )return false;
		float cos_theta_t = sqrt ( 1 - sin2_theta_t );
		w_refracted = eta * ( -w_incident ) + eta * ( cos_theta_i - cos_theta_t ) * n;
		return true;
	}
	V3 uniformSampleHemisphere ();
	/* Test whether the given ray is intersected with the triangle */
	bool ray_triangle_intersection ( const Ray& ray, V3 A, V3 B, V3 C,
		float& t_hit, float& u, float& v );

	void normalCoordinateSystem ( const V3& n, V3& n_t, V3& n_b );
};