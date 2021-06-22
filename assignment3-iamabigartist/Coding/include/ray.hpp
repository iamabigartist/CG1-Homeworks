#pragma once
#include "Eigen/Dense"
#include <iostream>
#include <set>

using namespace std;
using namespace Eigen;
//#define DEBUG_RAY

class Geometry;

/**
 * Data structure representing a ray
 */

struct Ray
{
/* Original point of the ray */
	Eigen::Vector3f origin;
	/* Direction of the ray */
	Eigen::Vector3f direction;
	/* The maximum and minimum value of the parameter t */
	float range_min;
	float range_max;

	std::set<const Geometry*> exception_geometries;

   /// <param name="dir">This dir will be normalised automatically</param>
	Ray ( const Eigen::Vector3f& ori, const Eigen::Vector3f& dir, float mini_t = 1e-5f, float maxi_t = 1e+30f )
	{
		origin = ori;
		direction = dir.normalized ();
		range_min = mini_t;
		range_max = maxi_t;

	#ifdef DEBUG_RAY
		std::cout << "Ray: " << "ori: " << ori.transpose () << "dir: " << dir.transpose () << endl;
	#endif // DEBUG_RAY
	}

	Eigen::Vector3f getPoint ( float t ) const
	{
		return origin + t * direction;
	}
};