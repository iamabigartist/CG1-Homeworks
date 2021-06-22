#include <cmath>
#include "implicit_geom.hpp"
#include "constant.hpp"
#include "utils.hpp"

using namespace MathUtils;

/**
 * ImplicitGeometry class
 */

AABB ImplicitGeometry::getBBox () const
{
	return bounding_box;
}

bool ImplicitGeometry::bboxRayIntersection ( Ray r, float& t_entry, float& t_exit ) const
{
	return bounding_box.rayIntersection ( r, t_entry, t_exit );
}

/**
 * GenusTwoSurface class
 */

GenusTwoSurface::GenusTwoSurface ( Eigen::Vector3f pos, Eigen::Vector3f range )
{
	bounding_box = AABB ( pos - range / 2.0, pos + range / 2.0 );
}

float GenusTwoSurface::getValue ( Eigen::Vector3f p )
{
	float x = p.x ();
	float y = p.y ();
	float z = p.z ();

	return 2 * y * ( y * y - 3 * x * x ) * ( 1 - z * z ) + pow ( x * x + y * y, 2 ) - ( 9 * z * z - 1 ) * ( 1 - z * z );
}

Eigen::Vector3f GenusTwoSurface::computeGradient ( Eigen::Vector3f p )
{
	return genus_two_gradient ( p );
}

/**
 * WineGlassSurface class
 */

WineGlassSurface::WineGlassSurface ( Eigen::Vector3f pos, Eigen::Vector3f range )
{
	bounding_box = AABB ( pos - range / 2.0, pos + range / 2.0 );
}

float WineGlassSurface::getValue ( Eigen::Vector3f p )
{
	float x = p.x ();
	float y = p.y ();
	float z = p.z ();

	/* Out of domain: Not-A-Number */
	if ( z + 3.2 <= 0 )
	{
		return INFINITY;
	}

	return x * x + y * y - pow ( log ( z + 3.2 ), 2 ) - 0.09;
}

Eigen::Vector3f WineGlassSurface::computeGradient ( Eigen::Vector3f p )
{
	return wine_glass_gradient ( p );
}

/**
 * PorousSurface class
 */

PorousSurface::PorousSurface ( Eigen::Vector3f pos, Eigen::Vector3f range )
{
	bounding_box = AABB ( pos - range / 2.0, pos + range / 2.0 );
}

float PorousSurface::getValue ( Eigen::Vector3f p )
{
	float x = p.x ();
	float y = p.y ();
	float z = p.z ();

	return -0.02 + pow ( -0.88 + pow ( y, 2 ), 2 ) * pow ( 2.92 * ( -1 + x ) * pow ( x, 2 ) * ( 1 + x ) + 1.7 * pow ( y, 2 ), 2 ) + pow ( -0.88 + pow ( z, 2 ), 2 ) * pow ( 2.92 * ( -1 + y ) * pow ( y, 2 ) * ( 1 + y ) + 1.7 * pow ( z, 2 ), 2 ) + pow ( -0.88 + pow ( x, 2 ), 2 ) * pow ( 1.7 * pow ( x, 2 ) + 2.92 * ( -1 + z ) * pow ( z, 2 ) * ( 1 + z ), 2 );
}

Eigen::Vector3f PorousSurface::computeGradient ( Eigen::Vector3f p )
{
	return porous_gradient ( p );
}