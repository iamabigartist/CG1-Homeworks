#pragma once
#include <utility>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "interaction.hpp"
#include "brdf.hpp"
#include "ray.hpp"
#include "accel_struct.hpp"

/**
 * Base class of geometries
 */

class Geometry
{
protected:
	/* Material attached on the geometry */
	BRDF* material {};
	/* The axis-aligned bounding box of the geometry */
	AABB bounding_box;

public:
	Geometry () {};
	virtual ~Geometry () = default;
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) = 0;
	virtual void buildBoundingBox () = 0;
	void setMaterial ( BRDF* mat );
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
	Parallelogram ( Vector3f p0, Vector3f s0, Vector3f s1, Vector3f normal, BRDF* mat );
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) override;
	virtual void buildBoundingBox () override;
	/*sample a random point uniformly.*/
	V3 samplePoint ()
	{
		return p0 + s0 * unif ( 0, s0_len ) + s1 * unif ( 0, s1_len );
	}
	float getArea () { return ( s0_len * s0 ).cross ( s1_len * s1 ).norm (); }
	V3 getNormal () { return ( s0_len * s0 ).cross ( s1_len * s1 ).normalized (); }
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
	Sphere ( Vector3f p0, float r, BRDF* mat );
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) override;
	virtual void buildBoundingBox () override;
};

/**
 * Triangle meshes
 */

class TriangleMesh : public Geometry
{
private:
	int kd_threshold;
	AccelerationStructure* acceleration_structure;

protected:
	/* Number of triangles */
	int num_triangles;
	/* List of verteices and vertex indices */
	vector<Vector3f> vertices;
	vector<int> vertices_index;
	/* List of normals and normal indices */
	vector<Vector3f> normals;
	vector<int> normals_index;
	/* Is uniform grid established */
	bool has_grid;

	//bool raySingleTriangleIntersection ( Interaction& interaction, const Ray& ray, int v0_idx, int v1_idx, int v2_idx ) const;

public:
	TriangleMesh ( string file_path, BRDF* mat, int kd_threshold );
	virtual bool rayIntersection ( Interaction& interaction, const Ray& ray ) override;
	/// <summary>
	/// Set the geometry interaction
	/// </summary>
	void setInteraction ( AccelerationStructure::Result& result, Interaction& interaction );
	virtual void buildBoundingBox () override;
	void applyTransformation ( const Affine3f& t );
	void buildAccelerationStructure ();
};