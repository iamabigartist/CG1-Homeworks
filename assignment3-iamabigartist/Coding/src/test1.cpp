#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <chrono>

#include "stb_image_write.h"

#include "geometry.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "integrator.hpp"
#include "config.hpp"
#include "constant.hpp"

#define clock_t std::chrono::time_point<std::chrono::system_clock>

using namespace std;
using namespace Eigen;

float saw_wave1 ( float t ) { return t - floor ( t ); }

float repeat1 ( float x, float start, float end )
{
	int interval = end - start;
	return start + interval * saw_wave1 ( ( x - start ) / interval );
}

float angleBetweenVectors1 ( Vector3f a, Vector3f b ) { return atan2 ( a.cross ( b ).norm (), a.dot ( b ) ); }

int main1 ()
{
	cout << angleBetweenVectors1 ( Vector3f ( 1, 0, 1 ), Vector3f ( 1, 0, 0 ) ) * ( 180 / PI ) << endl;
	return 0;
}