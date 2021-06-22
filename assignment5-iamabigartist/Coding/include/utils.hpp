#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Eigen/Dense"

using namespace Eigen;
using namespace std;
using V3 = Eigen::Vector3f;

#define UNREACHABLE std::cout << "Error: Unreachable code executed. Exit(-1)..." << std::endl; exit(-1);
//
//class MatlabUtil
//{
//public:
//	MatlabUtil ()
//	{
//	}
//	~MatlabUtil ()
//	{
//	}
//
//private:
//};

/**
 * String processing utilities
 */

namespace StrUtils
{
	/* Trim from left */
	void ltrim ( std::string& s );
	/* Trim from right */
	void rtrim ( std::string& s );
	/* Trim from both left and right */
	void trim ( std::string& s );
};

/**
 * Mathematical utilities
 */

namespace MathUtils
{
	/* Clamps the input x to the closed range [lo, hi] */
	float clamp ( float x, float lo, float hi );
	/* Performs Gamma correction on x and outputs an integer between 0-255. */
	unsigned char gamma_correction ( float x );

	//Get the guassian(x), which have a fix sigma and no bias
	float guassian ( float x );

	V3 genus_two_gradient ( V3& p );
	V3 wine_glass_gradient ( V3& p );
	V3 porous_gradient ( V3& p );
};