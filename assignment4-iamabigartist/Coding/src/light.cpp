#include <utility>
#include "light.hpp"
#include "geometry.hpp"
#include "utils.hpp"

/**
 * Light class
 */

Light::Light ( Eigen::Vector3f pos, Eigen::Vector3f power )
	: position ( pos )
	, radiance ( power )
{
}

Eigen::Vector3f Light::getPosition () const
{
	return position;
}

Eigen::Vector3f Light::getRadiance () const
{
	return radiance;
}

/**
 * AreaLight class
 */
AreaLight::AreaLight ( Eigen::Vector3f pos, Eigen::Vector3f power, Eigen::Vector2f size )
	: Light ( pos, power )
	, area_size ( size )
	, geometry_delegation (
		pos - Eigen::Vector3f ( size [ 0 ], 0, size [ 1 ] ) / 2,
		Eigen::Vector3f ( size [ 0 ], 0, 0 ),
		Eigen::Vector3f ( 0, 0, size [ 1 ] ),
		Eigen::Vector3f ( 0, 1, 0 ), nullptr )
{
}

Eigen::Vector3f AreaLight::emission ( Eigen::Vector3f pos, Eigen::Vector3f dir )
{
	return radiance * abs ( geometry_delegation.getNormal ().dot ( dir ) );
}

float AreaLight::samplePdf ( const Interaction& ref_it, Eigen::Vector3f pos )
{
	return ( float ) pow ( ( ref_it.entry_point - pos ).norm (), 2 ) / ( abs ( geometry_delegation.getNormal ().dot ( ref_it.wi ) ) * geometry_delegation.getArea () );
}

Eigen::Vector3f AreaLight::sample ( Interaction& ref_it, float* pdf )
{
	auto pos = geometry_delegation.samplePoint ();
	ref_it.wi = ( ref_it.entry_point - pos ).normalized ();
	*pdf = samplePdf ( ref_it, pos );
	return ref_it.wi;
}

bool AreaLight::rayIntersection ( Interaction& interaction, const Ray& ray )
{
	bool intersection = geometry_delegation.rayIntersection ( interaction, ray );
	interaction.type = Interaction::Type::LIGHT;
	return intersection;
}