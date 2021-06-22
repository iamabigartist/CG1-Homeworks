#include "light.hpp"
#include "geometry.hpp"
#include "utils.hpp"

using namespace std;
using namespace Eigen;

/**
 * Light class
 */

Light::Light ( Vector3f pos, Vector3f rgb )
	: position ( pos )
	, color ( rgb )
{
}

Vector3f Light::getPosition () const
{
	return position;
}

Vector3f Light::getColor () const
{
	return color;
}

/**
 * PointLight class
 */

PointLight::PointLight ( Vector3f pos, Vector3f rgb )
	: Light ( pos, rgb )
{
	generate_samples ();
}

void PointLight::generate_samples ()
{
	samples.clear ();
	samples.push_back ( LightSamplePair ( position, color ) );
}

bool PointLight::rayIntersection ( Interaction& interaction, const Ray& ray ) const
{
	Vector3f dist = position - ray.origin;
	float t = dist.norm ();
	if ( ray.direction == dist.normalized () && t >= ray.range_min && t <= ray.range_max )
	{
		interaction.entry_dist = t;
		interaction.exit_dist = t;
		interaction.entry_point = ray.getPoint ( t );
		interaction.type = Interaction::Type::LIGHT;
		return true;
	}
	return false;
}

/**
 * AreaLight class
 */
AreaLight::AreaLight ( Vector3f pos, Vector3f rgb, Vector2f size, Vector2f sample_size )
	: Light ( pos, rgb )
	, area_size ( size )
	, geometry_delegation (
	pos - Vector3f ( size [ 0 ], 0, size [ 1 ] ) / 2,
	Vector3f ( size [ 0 ], 0, 0 ),
	Vector3f ( 0, 0, size [ 1 ] ),
	Vector3f ( 0, 1, 0 ), nullptr )
	, sample_size ( sample_size )
{
	generate_samples ();
}

void AreaLight::generate_samples ()
{
	constexpr auto UNIFORM_SAMPLE_NUM = 100;

//#define GRID_SAMPLE
#define UNIFORM_SAMPLE

	samples.clear ();

	//TODO Uniform Sample
	//利用uniform sample来重写

	//The light num in height direction
	int H = area_size [ 0 ] / sample_size [ 0 ];
	//The pixel num in width direction
	int W = area_size [ 1 ] / sample_size [ 1 ];

#ifdef GRID_SAMPLE

	for ( int i = 0; i < H; i++ )
	{
		for ( int j = 0; j < W; j++ )
		{
			samples.push_back ( LightSamplePair ( geometry_delegation.get_point ( Vector2f ( i / H, j / W ) ), color / ( H * W ) ) );
		}
	}
#endif // GRID_SAMPLE

#ifdef UNIFORM_SAMPLE
	for ( int i = 0; i < W; i++ )
	{
		for ( int j = 0; j < H; j++ )
		{
			samples.push_back (
				LightSamplePair ( geometry_delegation.get_point ( Vector2f (
				uniform_sample ( 0, 1 ),
				uniform_sample ( 0, 1 ) ) ),
				color / ( H * W ) ) );
		}
	}
#endif // UNIFORM_SAMPLE
}

bool AreaLight::rayIntersection ( Interaction& interaction, const Ray& ray ) const
{
	bool intersection = geometry_delegation.rayIntersection ( interaction, ray );
	interaction.type = Interaction::Type::LIGHT;
	return intersection;
}