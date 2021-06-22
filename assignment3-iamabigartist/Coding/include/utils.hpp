#pragma once
#include <iostream>
#include "Eigen/Dense"

using namespace Eigen;

#define UNREACHABLE std::cout << "Error: Unreachable code executed. Exit(-1)..." << std::endl; exit(-1);

/// <summary>
/// Clamps the input x to the closed range [left, right]
/// </summary>
float clamp ( float x, float left, float right );

/// <summary>
/// Move x repeatly to range [start, end]
/// </summary>
float repeat ( float x, float start, float end );

float saw_wave ( float t );

/// <summary>
/// Performs Gamma correction on x and outputs an integer between 0-255.
/// </summary>
unsigned char gamma_correction ( float x );

/// <summary>
/// Uniformly samples a real number in [a, b]
/// </summary>
float uniform_sample ( float a, float b );

/// <summary>
/// Convert a Vector3f to a Vector4f with a scalar w.
/// </summary>
Vector4f V3_4f ( Vector3f v, float w = 0 );

Vector3f V4_xyz ( Vector4f v );

/// <summary>
/// Solve a quadratic equation with given coefficients and output the solutions and solution number.
/// </summary>
/// <returns>The number of solution</returns>
/// <param name="x0">the smaller solution or the only solution</param>
/// <param name="x1">the bigger solution</param>
int solve_quadratic_equation ( const float& a, const float& b, const float& c, float& x0, float& x1 );

bool close_enough ( float a, float b, float alow_diff );

float angle ( Vector3f a, Vector3f b );