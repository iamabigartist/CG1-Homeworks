#pragma once
#include <vector>
#include "Eigen/Dense"

/// <summary>
/// The image pixel data
/// </summary>
struct Film
{
/// <summary>
/// <para>The pixel number in 2 dimensions of the image,</para>
/// <para>which are also called the image width and height.</para>
/// </summary>
	Eigen::Vector2i resolution;
	std::vector<Eigen::Vector3f> pixel_array;

	Film ( const Eigen::Vector2i& res )
		: resolution ( res )
	{
		pixel_array.resize ( resolution.x () * resolution.y (), Eigen::Vector3f ( 1, 1, 1 ) );
	}

	/// <summary>
	/// Image Width/Image Height
	/// </summary>
	float getAspectRatio () const
	{
		return ( float ) ( resolution.x () ) / ( float ) ( resolution.y () );
	}
};