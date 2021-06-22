#include <random>
#include <algorithm>
#include "utils.hpp"
#include "constant.hpp"

/**
 * strutils namespace
 */

void strutils::ltrim ( std::string& s )
{
	s.erase ( s.begin (), std::find_if ( s.begin (), s.end (), std::not1 ( std::ptr_fun<int, int> ( std::isspace ) ) ) );
}

void strutils::rtrim ( std::string& s )
{
	s.erase ( std::find_if ( s.rbegin (), s.rend (), std::not1 ( std::ptr_fun<int, int> ( std::isspace ) ) ).base (), s.end () );
}

void strutils::trim ( std::string& s )
{
	ltrim ( s );
	rtrim ( s );
}

/**
 * mathutils namespace
 */

float mathutils::clamp ( float x, float lo, float hi )
{
	return x < lo ? lo : x > hi ? hi : x;
}

unsigned char mathutils::gamma_correction ( float x )
{
	return ( unsigned char ) ( pow ( mathutils::clamp ( x, 0.0, 1.0 ), 1 / 2.2 ) * 255 );
}

static std::default_random_engine random_generator;

std::vector<float> mathutils::unif ( float a, float b, int N )
{
	std::vector<float> res;
	std::uniform_real_distribution<float> dis ( a, b );

	for ( int i = 0; i < N; i++ )
	{
		res.push_back ( dis ( random_generator ) );
	}
	return res;
}

float mathutils::unif ( float a, float b )
{
	std::uniform_real_distribution<float> dis ( a, b );
	return dis ( random_generator );
}

Vector3f mathutils::uniformSampleHemisphere ()
{
	//The 2 uniform rr , xi_1, xi_2
	auto xis = unif ( 0, 1, 2 );
	float sin_theta = sqrt ( 1 - xis [ 0 ] * xis [ 0 ] );
	float phi = 2 * PI * xis [ 1 ];
	float x = sin_theta * cosf ( phi );
	float y = xis [ 0 ];
	float z = sin_theta * sinf ( phi );
	return Vector3f ( x, y, z );
}

bool mathutils::ray_triangle_intersection ( const Ray& ray, V3 A, V3 B, V3 C,
	float& t_hit, float& u, float& v )
{
	/**
	 * HACK: Ray intersection test with single triangle
	 * Note: Remember that normals are interpolated using barycentric coordinates.
	 */

	 //matrix vector elements
	V3 E_1 = B - A;
	V3 E_2 = C - A;
	V3 P = ray.direction.cross ( E_2 );

	//If the triangle and the ray are parallel
	float det = E_1.dot ( P );
	if ( fabs ( det ) < EPSILON ) return false;

	float inv_det = 1.0f / det;

	//If the point is in the triangle
	V3 T = ray.origin - A;
	u = T.dot ( P ) * inv_det;
	if ( u < 0 || u > 1 ) return false;

	V3 Q = T.cross ( E_1 );
	v = ray.direction.dot ( Q ) * inv_det;
	if ( v < 0 || u + v>1 )return false;

	t_hit = E_2.dot ( Q ) * inv_det;
	if ( t_hit < ray.range_min || t_hit > ray.range_max )return false;
	return true;
}

void mathutils::normalCoordinateSystem ( const V3& n, V3& n_t, V3& n_b )
{
	if ( abs ( n.x () ) > abs ( n.y () ) )
		n_t = V3 ( n.z (), 0, -n.x () ).normalized ();
	else
		n_t = V3 ( 0, -n.z (), n.y () ).normalized ();
	n_b = n.cross ( n_t );
}