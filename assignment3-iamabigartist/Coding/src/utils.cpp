#include <random>
#include "utils.hpp"
using namespace Eigen;

Vector3f V4_xyz ( Vector4f v ) { return Vector3f ( v.x (), v.y (), v.z () ); }

Vector4f V3_4f ( Vector3f v, float w ) { return Vector4f ( v.x (), v.y (), v.z (), w ); }

float clamp ( float x, float left, float right )
{
	return x < left ? left : x > right ? right : x;
}

float saw_wave ( float t ) { return t - floor ( t ); }

float repeat ( float x, float start, float end )
{
	int interval = end - start;
	return start + interval * saw_wave ( ( x - start ) / interval );
}

unsigned char gamma_correction ( float x )
{
	return ( unsigned char ) ( pow ( clamp ( x, 0.0, 1.0 ), 1 / 2.2 ) * 255 );
}

static std::default_random_engine random_generator;

float uniform_sample ( float a, float b )
{
	std::uniform_real_distribution<float> dis ( a, b );
	return dis ( random_generator );
}

int solve_quadratic_equation ( const float& a, const float& b, const float& c, float& x0, float& x1 )
{
	float discriminant = b * b - 4 * a * c;
	if ( discriminant < 0 )
	{
		x1 = x0 = NAN;
		return 0;
	}
	else if ( discriminant == 0 )
	{
		x0 = -0.5 * b / a;
		x1 = NAN;
		return 1;
	}
	else
	{
		float p = -b / ( 2 * a );
		float q = sqrt ( discriminant ) / ( 2 * abs ( a ) );//The abs of q, make x0 always smaller
		x0 = p - q;
		x1 = p + q;
		return 2;
	}
}

bool close_enough ( float a, float b, float alow_diff )
{
	return abs ( a - b ) < alow_diff;
}

float angle ( Vector3f a, Vector3f b ) { return atan2 ( a.cross ( b ).norm (), a.dot ( b ) ); }