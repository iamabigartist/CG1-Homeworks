#pragma once
#include <string>
#include "Eigen/Dense"
#include "utils.hpp"
using namespace Eigen;

/**
 * Texture objects
 */

class Texture
{
private:
	Vector2f uv_to_pos ( Vector2f uv ) const
	{
		return uv.cwiseProduct ( Vector2f ( height - 1, width - 1 ) );
	}

	Vector2f pos_to_uv ( Vector2f pos ) const
	{
		return pos.cwiseProduct ( Vector2f ( 1 / ( height - 1 ), 1 / ( width - 1 ) ) );
	}

	Vector2f repeat_uv ( Vector2f uv ) const
	{
		return Vector2f ( repeat ( uv.x (), 0, 1 ), repeat ( uv.y (), 0, 1 ) );
	}
	Vector2f clamp_uv ( Vector2f uv ) const
	{
		return Vector2f ( clamp ( uv.x (), 0, 1 ), clamp ( uv.y (), 0, 1 ) );
	}
	Vector2f repeat_pos ( Vector2f pos ) const
	{
		return Vector2f ( repeat ( pos.x (), 0, height - 1 ), repeat ( pos.y (), 0, width - 1 ) );
	}
	Vector2f clamp_pos ( Vector2f pos ) const
	{
		return Vector2f ( clamp ( pos.x (), 0, height - 1 ), clamp ( pos.y (), 0, width - 1 ) );
	}

	Vector3f getPixel ( Vector2f ij ) const
	{
		int i0 = 3 * ( ij.y () * width + ij.x () );
		return Vector3f ( data [ i0 ], data [ i0 + 1 ], data [ i0 + 2 ] );
	}

protected:
	/* An array with width x height x 3 elements
	   Data arranged in r0, g0, b0, r1, g1, b1, ..., ri, gi, bi, ...
	   (0, 0) is located at the left-top corner, (1, 1) is located at the right-bottom corner */
	float* data;
	unsigned int width;
	unsigned int height;

public:
	Texture ();
	bool isNull () const;
	Vector2i getDimension () const;
	Vector3f Sample ( Vector2f uv ) const;

	void clear ();
	virtual ~Texture ();

	static Texture loadFromPath ( std::string path );
	static void generateCheckerboard ( Texture& tex, Vector3f primary_rgb, Vector3f secondary_rgb, unsigned int num_ckers = 4 );
};