#include <vector>
#include "Eigen/Dense"
#include "interaction.hpp"
#include "utils.hpp"
#include "constant.hpp"
#include "brdf.hpp"

/**
 * BRDF class
 */

BRDF::BRDF ()
{
}

/**
 * IdealDiffusion class
 */

IdealDiffusion::IdealDiffusion ( Eigen::Vector3f diff )
	: reflectivity ( diff )
{
}

Eigen::Vector3f IdealDiffusion::eval ( const Interaction& interact )
{
	return reflectivity / ( 2 * PI );
}

float IdealDiffusion::samplePdf ( const Interaction& interact )
{
	return 1.0f / ( 2 * PI );
}

float IdealDiffusion::sample ( Interaction& interact )
{
	interact.wi = BRDF::sample_direction_ideal_diffusion ( interact );
	interact.wi = V3 (
		interact.wi.x () * interact.n_b.x () + interact.wi.y () * interact.normal.x () + interact.wi.z () * interact.n_t.x (),
		interact.wi.x () * interact.n_b.y () + interact.wi.y () * interact.normal.y () + interact.wi.z () * interact.n_t.y (),
		interact.wi.x () * interact.n_b.z () + interact.wi.y () * interact.normal.z () + interact.wi.z () * interact.n_t.z () );
	return samplePdf ( interact );
}

bool IdealDiffusion::isDelta () const
{
	return false;
}

/**
 * IdealSpecular class
 */

IdealSpecular::IdealSpecular ( Eigen::Vector3f spec )
	: reflectivity ( spec )
{
}

Eigen::Vector3f IdealSpecular::eval ( const Interaction& interact )
{
	return reflectivity;
}

float IdealSpecular::samplePdf ( const Interaction& interact )
{
	/** HACK: UNREACHABLE */
	UNREACHABLE;
}

float IdealSpecular::sample ( Interaction& interact )
{
	interact.wi = BRDF::sample_direction_ideal_specular ( interact ).normalized ();
	return 1.0f;
}

bool IdealSpecular::isDelta () const
{
	return true;
}

/**
 * IdealTransmission class
 */

IdealTransmission::IdealTransmission ( Eigen::Vector3f reflect, float idx_refract )
	: reflectivity ( reflect )
	, ior ( idx_refract )
{
}

Eigen::Vector3f IdealTransmission::eval ( const Interaction& interact )
{
	return reflectivity;
}

float IdealTransmission::samplePdf ( const Interaction& interact )
{
	/** HACK: UNREACHABLE */
	UNREACHABLE;
}

float IdealTransmission::sample ( Interaction& interact )
{
	if ( !BRDF::sample_direction_ideal_transmission ( interact, ior, interact.wi ) )
	{
		interact.refraction_impossible = true;
	}
	else
	{
		//cout << "Yep.";
	}
	return 1.0f;
}

bool IdealTransmission::isDelta () const
{
	return true;
}

Vector3f BRDF::sample_direction_ideal_diffusion ( const Interaction& interact )
{
	return uniformSampleHemisphere ();
}

Vector3f BRDF::sample_direction_ideal_specular ( const Interaction& interact )
{
	return reflect ( interact.wo, interact.normal );
}

bool BRDF::sample_direction_ideal_transmission ( const Interaction& interact, float ior, V3& wi )
{
	bool entering_geometry = interact.wo.dot ( interact.normal ) < 0;
	float eta_i = entering_geometry ? ior : IOR_VACUUM;
	float eta_f = entering_geometry ? IOR_VACUUM : ior;
	return refract ( interact.wo, interact.normal, eta_i, eta_f, wi );
}