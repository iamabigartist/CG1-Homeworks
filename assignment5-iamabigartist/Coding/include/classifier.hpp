#pragma once
#include <vector>
#include "Eigen/Dense"
#include "camera.hpp"
#include "light.hpp"
#include "optics_data.hpp"
#include "implicit_geom.hpp"

using namespace std;
using namespace Eigen;
using V3 = Eigen::Vector3f;

/**
 * Base class of classifiers
 */

class Classifier
{
protected:
	float isovalue;
public:
	virtual OpticsData transfer ( VolumePointData v_data, Camera* cam, const std::vector<Light*>& lights, float dt ) const = 0;
	float get_isovalue () { return isovalue; }
	Classifier ( float isoval ) :isovalue ( isoval ) {}
};

/**
 * The classifier for visualizing isosurfaces
 */

class IsosurfaceClassifier : public Classifier
{
public:
	IsosurfaceClassifier ( float isoval );
	/**
	 * Transfer function
	 * @param v_data the volume data at the given point
	 * @param cam    the camera used to compute Phong shading
	 * @param lights a set of lights used to compute Phong shading
	 * @param dt     the sampling step length
	 */
	virtual OpticsData transfer ( VolumePointData v_data, Camera* cam, const std::vector<Light*>& lights, float dt ) const;
};