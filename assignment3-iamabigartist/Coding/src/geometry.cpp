#include <math.h>
#include "geometry.hpp"
#include "constant.hpp"
#include "utils.hpp"
#include "constant.hpp"

using namespace Eigen;
using namespace std;

/**
 * Geometry class
 */

void Geometry::setMaterial ( Material* new_mat )
{
	material = new_mat;
}

/**
 * Parallelogram class
 */

Parallelogram::Parallelogram ( Eigen::Vector3f p0, Eigen::Vector3f s0, Eigen::Vector3f s1, Eigen::Vector3f normal, Material* mat )
	: p0 ( p0 )
	, normal ( normal.normalized () )
{
	s0_len = s0.norm ();
	s1_len = s1.norm ();
	this->s0 = s0.normalized ();
	this->s1 = s1.normalized ();

	setMaterial ( mat );
}

bool Parallelogram::rayIntersection ( Interaction& interaction, const Ray& ray ) const
{
	if ( ray.direction.dot ( normal ) == 0 )
	{
		return false;
	}

	float t = ( p0 - ray.origin ).dot ( normal ) / ray.direction.dot ( normal );
	Vector3f p0_p = ray.getPoint ( t ) - p0;
	float q0 = p0_p.dot ( s0 ) / s0_len;
	float q1 = p0_p.dot ( s1 ) / s1_len;
	if ( q0 >= 0 && q0 <= s0.norm () && q1 >= 0 && q1 <= s1.norm () && t >= ray.range_min && t <= ray.range_max )
	{
		interaction.entry_dist = t;
		interaction.exit_dist = t;
		interaction.normal = normal;
		interaction.entry_point = ray.getPoint ( t );
		interaction.uv [ 0 ] = q0;
		interaction.uv [ 1 ] = q1;
		if ( material != nullptr )
		{
			interaction.lighting_model = material->evaluate ( interaction );
		}
		interaction.type = Interaction::Type::GEOMETRY;
		interaction.geometry_hit = this;
		return true;
	}
	return false;
}

/**
 * Sphere class
 */

Sphere::Sphere ( Eigen::Vector3f p0, float r, Material* mat )
	: p0 ( p0 )
	, radius ( r )
{
	setMaterial ( mat );
}

bool Sphere::rayIntersection ( Interaction& interaction, const Ray& ray ) const
{
	Vector3f L = ray.origin - p0;
	float x0, x1;
	int interaction_num = solve_quadratic_equation (
		ray.direction.dot ( ray.direction ),
		2 * ray.direction.dot ( L ),
		L.dot ( L ) - radius * radius,
		x0, x1 );

	switch ( interaction_num )
	{
		case 0:
		{
			interaction.type = Interaction::Type::NONE;
			return false;
		}
		case 1:
		{
			interaction.type = Interaction::Type::GEOMETRY;
			interaction.geometry_hit = this;
			interaction.exit_dist = x0;
			break;
		}
		case 2:
		{
			interaction.type = Interaction::Type::GEOMETRY;
			interaction.geometry_hit = this;
			interaction.exit_dist = x1;
			break;
		}
		default:
		{
			throw new exception ( "Wrong number of solution" );
		}
	}

	interaction.entry_dist = x0;
	interaction.entry_point = ray.getPoint ( x0 );
	interaction.normal = ( interaction.entry_point - p0 ).normalized ();

	interaction.uv [ 1 ] = angle ( interaction.normal, Vector3f ( 0, 1, 0 ) ) / PI;
	interaction.uv [ 0 ] = angle ( interaction.normal, Vector3f ( 1, 0, 0 ) ) / PI;

	if ( material != nullptr )
	{
		interaction.lighting_model = material->evaluate ( interaction );
	}

	return true;
}