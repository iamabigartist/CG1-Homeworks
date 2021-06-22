#pragma once
#include <Eigen/Dense>
#include <utility>
#include <vector>
#include "ray.hpp"
#include "interaction.hpp"
#include "geometry.hpp"

using namespace std;
using namespace Eigen;

//The first is position, the second is color
typedef pair<Vector3f, Vector3f> LightSamplePair;

/**
 * Base class of lights
 */

class Light
{
protected:
	Vector3f position;
	Vector3f color;
	vector<LightSamplePair> samples;

public:
	Light ( Vector3f pos, Vector3f rgb );
	Vector3f getPosition () const;
	Vector3f getColor () const;
	virtual void generate_samples () = 0;
	virtual const vector<LightSamplePair>& get_samples () const { return samples; }
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) const = 0;
};

/**
 * Point lights
 */

class PointLight : public Light
{
public:
	PointLight ( Vector3f pos, Vector3f rgb );
	virtual void generate_samples () override;
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) const override;
};

/**
 * Area lights
 */
class AreaLight : public Light
{
protected:
	Parallelogram geometry_delegation;
	Vector2f area_size;
	Vector2f sample_size;
public:
	AreaLight ( Vector3f pos, Vector3f rgb, Vector2f size, Vector2f sample_size = Vector2f ( 0.2, 0.2 ) );
	virtual void generate_samples () override;
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) const override;
};