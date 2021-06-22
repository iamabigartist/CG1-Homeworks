#include <vector>
#include "material.hpp"
#include "utils.hpp"

/**
 * ColorMaterial class
 */

ColorMaterial::ColorMaterial ()
	: diffusion_color ( Eigen::Vector3f ( 1.0, 1.0, 1.0 ) )
	, specular_color ( Eigen::Vector3f ( 0, 0, 0 ) )
	, shininess ( 0.0 )
{
}

ColorMaterial::ColorMaterial ( Eigen::Vector3f diff_rgb, Eigen::Vector3f spec_rgb, float sh )
	: diffusion_color ( diff_rgb )
	, specular_color ( spec_rgb )
	, shininess ( sh )
{
}

ColorMaterial::ColorMaterial ( Eigen::Vector3f rgb, float sh = 16.0f )
	: diffusion_color ( rgb )
	, specular_color ( rgb )
	, shininess ( sh )
{
}

InteractionPhongModel ColorMaterial::evaluate ( const Interaction& in ) const
{
	InteractionPhongModel m;
	m.diffusion = diffusion_color;
	m.specular = specular_color;
	m.shininess = shininess;
	return m;
}

/**
 * TextureMaterial class
 */

TextureMaterial::TextureMaterial ()
	: texture ( nullptr )
	, shininess ( 16.0f )
{
}

TextureMaterial::TextureMaterial ( Texture* tex, float sh )
	: texture ( tex )
	, shininess ( sh )
{
}

InteractionPhongModel TextureMaterial::evaluate ( const Interaction& in ) const
{
	InteractionPhongModel m;
	m.diffusion = texture->Sample ( in.uv );
	m.specular = m.diffusion;
	m.shininess = shininess;
	return m;
}