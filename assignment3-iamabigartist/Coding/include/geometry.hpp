#pragma once
#include <utility>
#include "interaction.hpp"
#include "material.hpp"
#include "ray.hpp"

using namespace Eigen;

/**
 * Base class of geometries
 */

class Geometry
{
protected:
	Material* material {};

public:
	Geometry () {};
	virtual ~Geometry () = default;
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) const = 0;
	void setMaterial ( Material* mat );
};

/**
 * Parallelograms
 */

class Parallelogram : public Geometry
{
protected:
	/* The origin point */
	Vector3f p0;
	/* Directions of the two sides */
	Vector3f s0, s1;
	/* Lengths of the two sides */
	float s0_len, s1_len;
	/* Normal (orientation) */
	Vector3f normal;

public:
	Parallelogram ( Vector3f p0, Vector3f s0, Vector3f s1, Vector3f normal, Material* mat );
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) const override;
	//Given a uv coordinate [0,1]x[0,1] to specify a point on the parallelogram.
	Vector3f get_point ( Vector2f uv ) const { return p0 + uv.x () * s0 * s0_len + uv.y () * s1 * s1_len; }
};

/**
 * Spheres
 */
class Sphere : public Geometry
{
protected:
	/* Radius of the sphere */
	float radius;
	/* Center of the sphere */
	Vector3f p0;

public:
	Sphere ( Vector3f p0, float r, Material* mat );
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) const override;
};