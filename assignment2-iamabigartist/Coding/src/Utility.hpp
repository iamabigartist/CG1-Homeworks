#pragma once
#include"header.hpp"
using namespace std;
using namespace glm;

//Knot
void uniform_open_knots_vector(vector<float>& knots, int m, int p);
void uniform_clamped_knots_vector(vector<float>& knots, int m, int p);
bool in_curve_range(int& k, const int& p, const float& u, const vector<float>& knots);