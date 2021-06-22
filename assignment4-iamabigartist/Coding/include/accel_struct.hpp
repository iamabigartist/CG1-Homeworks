#pragma once
#include <utility>
#include <cmath>
#include <vector>
#include "ray.hpp"
#include "interaction.hpp"
#include <brdf.hpp>

using namespace std;
using namespace Eigen;
using V3 = Vector3f;

/**
 * Axis-Aligned Bounding Box
 */

struct AABB
{
	Eigen::Vector3f lb{};
	Eigen::Vector3f ub{};

	AABB ();
	/* Construct AABB by coordinates of lower bound and upper bound */
	AABB ( float lb_x, float lb_y, float lb_z, float ub_x, float ub_y, float ub_z );
	AABB ( Eigen::Vector3f lb, Eigen::Vector3f ub );
	/* Construct AABB for a sphere */
	AABB ( const Eigen::Vector3f& pos, float radius );
	/* Construct AABB for a triangle */
	AABB ( const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const Eigen::Vector3f& v3 );
	/* Construct AABB by merging two AABBs */
	AABB ( const AABB& a, const AABB& b );

	/// <summary>
	/// Seperate the box into 2 by the bound. Will not check the range of the bound
	/// </summary>
	void seperateBy ( float bound, int split_dimension, AABB& l_box, AABB& u_box );
	/* Get the AABB center */
	Eigen::Vector3f getCenter () const;
	/* Get the length of a specified side on the AABB */
	float getDist ( int c ) const;
	/* Get the volume of the AABB */
	float getVolume () const;
	/* Check whether the AABB is overlapping with another AABB */
	bool isOverlap ( const AABB& a ) const;
	/* Get the diagonal length */
	float diagonalLength () const;
	/// <param name="t_in">The t when ray enters the box</param>
	/// <param name="t_out">The t when ray exits the box</param>
	/// <returns>if the ray has intersection with the box</returns>
	bool rayIntersection ( const Ray& ray, float& t_in, float& t_out ) const;
};

class AccelerationStructure
{
public:

	struct Result
	{
		Result ( int triangle_index = -1, float u = -1, float v = -1, float t_hit = -1 ) :
			triangle_index ( triangle_index ), u ( u ), v ( v ), t_hit ( t_hit ) {}

		int triangle_index = -1;
		float u = -1;
		float v = -1;
		float t_hit = -1;
	};

	AccelerationStructure (
		const int& num_triangles,
		const vector<Vector3f>& vertices,
		const vector<int>& vertices_index ) :
		num_triangles ( num_triangles ),
		vertices ( vertices ),
		vertices_index ( vertices_index ) {}

	virtual void build () = 0;
	virtual bool rayIntersection ( Result& result, const Ray& ray ) = 0;
protected:

	void buildBoundingBox ();
	/* Number of triangles */
	const int& num_triangles;
	/* List of verteices and vertex indices */
	const vector<Vector3f>& vertices;
	const vector<int>& vertices_index;

	AABB structure_box;
	//The bounding boxes for all the triangle of the mesh
	vector<AABB> triangle_boxes;
};

class KD_Tree :public AccelerationStructure
{
public:

	struct Node
	{
		Node ( AABB box, int split_dimension ) :
			box ( box ), split_dimension ( split_dimension ) {}

		bool is_leaf () { return !( lower_child || upper_child ); }

		static int increment_dimension ( int prev_dimension )
		{
			int cur_dimension;
			if ( prev_dimension >= 2 ) { cur_dimension = 0; }
			else
			{
				cur_dimension = prev_dimension + 1;
			}
			return cur_dimension;
		}

		int split_dimension;
		float split_value{};
		Node* lower_child{}; Node* upper_child{};
		AABB box;

		//The vector to stroe the indices of triangle inside,for the 3 vertices indices, vn=3*i+n.
		vector<int> triangle_indices;
	};

	KD_Tree (
		const int& num_triangles,
		const vector<Vector3f>& vertices,
		const vector<int>& vertices_index,
		int threshold ) :
		AccelerationStructure ( num_triangles, vertices, vertices_index ),
		threshold ( threshold ) {}

	virtual void build () override;

	/// <summary>
	/// Using restart and push down traversal to find the interaction
	/// </summary>
	virtual bool rayIntersection ( Result& result, const Ray& ray ) override;

private:
	Node* buildNode ( AABB box, int seperate_dimension, vector<int>& triangle_indices );
	void buildAllAABBs ();

	//The seperate number threshold
	int threshold;

	//The root node of kd_tree
	Node* root{};
};