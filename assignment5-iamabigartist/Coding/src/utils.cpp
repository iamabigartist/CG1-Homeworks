#include <random>
#include <algorithm>
#include <cmath>
#include "utils.hpp"
#include "constant.hpp"

/**
 * StrUtils namespace
 */

void StrUtils::ltrim ( std::string& s )
{
	s.erase ( s.begin (), std::find_if ( s.begin (), s.end (), std::not1 ( std::ptr_fun<int, int> ( std::isspace ) ) ) );
}

void StrUtils::rtrim ( std::string& s )
{
	s.erase ( std::find_if ( s.rbegin (), s.rend (), std::not1 ( std::ptr_fun<int, int> ( std::isspace ) ) ).base (), s.end () );
}

void StrUtils::trim ( std::string& s )
{
	ltrim ( s );
	rtrim ( s );
}

/**
 * MathUtils namespace
 */

float MathUtils::clamp ( float x, float lo, float hi )
{
	return x < lo ? lo : x > hi ? hi : x;
}

unsigned char MathUtils::gamma_correction ( float x )
{
	return ( unsigned char ) ( pow ( MathUtils::clamp ( x, 0.0, 1.0 ), 1 / 2.2 ) * 255 );
}

V3 MathUtils::genus_two_gradient ( V3& p )
{
	float x = p.x (); float y = p.y (); float z = p.z ();

	return V3 (
		( float ) ( 4 * x * ( pow ( x, 2 ) + pow ( y, 2 ) ) + 12 * x * y * ( pow ( z, 2 ) - 1 ) ),
		( float ) ( 2 * ( pow ( z, 2 ) - 1 ) * ( 3 * pow ( x, 2 ) - pow ( y, 2 ) ) - 4 * pow ( y, 2 ) * ( pow ( z, 2 ) - 1 ) + 4 * y * ( pow ( x, 2 ) + pow ( y, 2 ) ) ),
		( float ) ( 18 * z * ( pow ( z, 2 ) - 1 ) + 2 * z * ( 9 * pow ( z, 2 ) - 1 ) + 4 * y * z * ( 3 * pow ( x, 2 ) - pow ( y, 2 ) ) ) );
}

V3 MathUtils::wine_glass_gradient ( V3& p )
{
	float x = p.x (); float y = p.y (); float z = p.z ();
	return V3 (
		( float ) ( 2 * x ), ( float ) ( 2 * y ),
		( float ) ( -( 2 * log ( z + 3.2000 ) ) / ( z + 3.2000 ) ) );
}

V3 MathUtils::porous_gradient ( V3& p )
{
	float x = p.x (); float y = p.y (); float z = p.z ();
	return V3 (
		( float ) ( 6.8000 * x * ( 1.7000 * pow ( x, 2 ) + pow ( z, 2 ) * ( z + 1 ) * ( 2.9200 * z - 2.9200 ) ) * pow ( ( pow ( x, 2 ) - 0.8800 ), 2 ) + 4 * x * pow ( ( 1.7000 * pow ( x, 2 ) + pow ( z, 2 ) * ( z + 1 ) * ( 2.9200 * z - 2.9200 ) ), 2 ) * ( pow ( x, 2 ) - 0.8800 ) + 2 * ( 1.7000 * pow ( y, 2 ) + pow ( x, 2 ) * ( x + 1 ) * ( 2.9200 * x - 2.9200 ) ) * pow ( ( pow ( y, 2 ) - 0.8800 ), 2 ) * ( 2.9200 * pow ( x, 2 ) * ( x + 1 ) + pow ( x, 2 ) * ( 2.9200 * x - 2.9200 ) + 2 * x * ( x + 1 ) * ( 2.9200 * x - 2.9200 ) ) ),
		( float ) ( 2 * ( 1.7000 * pow ( z, 2 ) + pow ( y, 2 ) * ( y + 1 ) * ( 2.9200 * y - 2.9200 ) ) * pow ( ( pow ( z, 2 ) - 0.8800 ), 2 ) * ( 2.9200 * pow ( y, 2 ) * ( y + 1 ) + pow ( y, 2 ) * ( 2.9200 * y - 2.9200 ) + 2 * y * ( y + 1 ) * ( 2.9200 * y - 2.9200 ) ) ),
		( float ) ( 2 * ( 1.7000 * pow ( x, 2 ) + pow ( z, 2 ) * ( z + 1 ) * ( 2.9200 * z - 2.9200 ) ) * pow ( ( pow ( x, 2 ) - 0.8800 ), 2 ) * ( 2.9200 * pow ( z, 2 ) * ( z + 1 ) + pow ( z, 2 ) * ( 2.9200 * z - 2.9200 ) + 2 * z * ( z + 1 ) * ( 2.9200 * z - 2.9200 ) ) ) );
}

//static MatlabUtil matlab_util;