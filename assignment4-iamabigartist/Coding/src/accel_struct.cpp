#pragma once
#include "accel_struct.hpp"
#include <iostream>
#include "constant.hpp"

#pragma region AABB

using namespace mathutils;

/**
 * AABB class
 */

AABB::AABB ()
	: lb ( 0, 0, 0 )
	, ub ( 0, 0, 0 )
{
}

AABB::AABB ( float lb_x, float lb_y, float lb_z, float ub_x, float ub_y, float ub_z )
{
	lb = Eigen::Vector3f ( lb_x, lb_y, lb_z );
	ub = Eigen::Vector3f ( ub_x, ub_y, ub_z );
}

AABB::AABB ( Eigen::Vector3f lb, Eigen::Vector3f ub )
	: lb ( lb )
	, ub ( ub )
{
}

AABB::AABB ( const Eigen::Vector3f& pos, float radius )
{
	Eigen::Vector3f r ( radius, radius, radius );
	lb = pos - r;
	ub = pos + r;
}

AABB::AABB ( const Eigen::Vector3f& v1, const Eigen::Vector3f& v2, const Eigen::Vector3f& v3 )
{
	lb = v1.cwiseMin ( v2 ).cwiseMin ( v3 );
	ub = v1.cwiseMax ( v2 ).cwiseMax ( v3 );
}

AABB::AABB ( const AABB& a, const AABB& b )
{
	lb = Eigen::Vector3f ( a.lb.cwiseMin ( b.lb ) );
	ub = Eigen::Vector3f ( a.ub.cwiseMax ( b.ub ) );
}

void AABB::seperateBy ( float bound, int split_dimension, AABB& l_box, AABB& u_box )
{
	l_box = AABB ( *this ); u_box = AABB ( *this );
	l_box.ub [ split_dimension ] = bound;
	u_box.lb [ split_dimension ] = bound;
}

Eigen::Vector3f AABB::getCenter () const
{
	return ( lb + ub ) / 2;
}

float AABB::getDist ( int c ) const
{
	return ub [ c ] - lb [ c ];
}

float AABB::getVolume () const
{
	return getDist ( 2 ) * getDist ( 1 ) * getDist ( 0 );
}

bool AABB::isOverlap ( const AABB& a ) const
{
	return ( ( a.lb [ 0 ] >= this->lb [ 0 ] && a.lb [ 0 ] <= this->ub [ 0 ] ) || ( this->lb [ 0 ] >= a.lb [ 0 ] && this->lb [ 0 ] <= a.ub [ 0 ] ) ) &&
		( ( a.lb [ 1 ] >= this->lb [ 1 ] && a.lb [ 1 ] <= this->ub [ 1 ] ) || ( this->lb [ 1 ] >= a.lb [ 1 ] && this->lb [ 1 ] <= a.ub [ 1 ] ) ) &&
		( ( a.lb [ 2 ] >= this->lb [ 2 ] && a.lb [ 2 ] <= this->ub [ 2 ] ) || ( this->lb [ 2 ] >= a.lb [ 2 ] && this->lb [ 2 ] <= a.ub [ 2 ] ) );
}

float AABB::diagonalLength () const
{
	return ( ub - lb ).norm ();
}

bool AABB::rayIntersection ( const Ray& ray, float& t_in, float& t_out ) const
{
	float dir_frac_x = ( ray.direction [ 0 ] == 0.0 ) ? 1.0e32 : 1.0f / ray.direction [ 0 ];
	float dir_frac_y = ( ray.direction [ 1 ] == 0.0 ) ? 1.0e32 : 1.0f / ray.direction [ 1 ];
	float dir_frac_z = ( ray.direction [ 2 ] == 0.0 ) ? 1.0e32 : 1.0f / ray.direction [ 2 ];

	float tx1 = ( lb [ 0 ] - ray.origin [ 0 ] ) * dir_frac_x;
	float tx2 = ( ub [ 0 ] - ray.origin [ 0 ] ) * dir_frac_x;
	float ty1 = ( lb [ 1 ] - ray.origin [ 1 ] ) * dir_frac_y;
	float ty2 = ( ub [ 1 ] - ray.origin [ 1 ] ) * dir_frac_y;
	float tz1 = ( lb [ 2 ] - ray.origin [ 2 ] ) * dir_frac_z;
	float tz2 = ( ub [ 2 ] - ray.origin [ 2 ] ) * dir_frac_z;

	t_in = std::max ( std::max ( std::min ( tx1, tx2 ), std::min ( ty1, ty2 ) ), std::min ( tz1, tz2 ) );
	t_out = std::min ( std::min ( std::max ( tx1, tx2 ), std::max ( ty1, ty2 ) ), std::max ( tz1, tz2 ) );

	/* When t_out < 0 and the ray is intersecting with AABB, the whole AABB is behind us */
	if ( t_out < 0 )
	{
		return false;
	}

	return t_out >= t_in;
}
#pragma endregion

#pragma region A_C
void AccelerationStructure::buildBoundingBox ()
{
	structure_box.lb = vertices [ 0 ].cwiseMin ( vertices [ 1 ] );
	structure_box.ub = vertices [ 0 ].cwiseMax ( vertices [ 1 ] );
	for ( int i = 2; i < vertices.size (); i++ )
	{
		structure_box.lb = structure_box.lb.cwiseMin ( vertices [ i ] );
		structure_box.ub = structure_box.ub.cwiseMax ( vertices [ i ] );
	}
}
#pragma endregion

#pragma region KD_Tree
KD_Tree::Node* KD_Tree::buildNode ( AABB box, int split_dimension, vector<int>& triangle_indices )
{
	//cout << "build node: " << triangle_indices.size () << endl;
	Node* node = new Node ( box, split_dimension );

	//Enough less triangles situation
	if ( triangle_indices.size () <= threshold )
	{
		node->triangle_indices = triangle_indices;
		return node;
	}

	//get the seperate face bound
	float bound = 0;
	for ( int i = 0; i < triangle_indices.size (); i++ )
	{
		bound += triangle_boxes [ triangle_indices [ i ] ].getCenter () [ split_dimension ];
	}
	bound /= triangle_indices.size (); //UNDONE: may cause bug if the bound is not between the 2 original boundaries

	//Build the 2 box
	AABB l_box, u_box;
	box.seperateBy ( bound, split_dimension, l_box, u_box );

	//and their triangles lists
	vector<int> l_triangle_indices, u_triangle_indices;
	for ( int i = 0; i < triangle_indices.size (); i++ )
	{
		if ( triangle_boxes [ triangle_indices [ i ] ].isOverlap ( l_box ) )
		{
			l_triangle_indices.push_back ( triangle_indices [ i ] );
		}
		if ( triangle_boxes [ triangle_indices [ i ] ].isOverlap ( u_box ) )
		{
			u_triangle_indices.push_back ( triangle_indices [ i ] );
		}
	}

	//Can't seperate any more situation
	if (
		l_triangle_indices.size () == triangle_indices.size () || //UNDONE: Or/And lead to difference
		u_triangle_indices.size () == triangle_indices.size () )
	{
		node->triangle_indices = triangle_indices;
		return node;
	}

	//Have children, don't have triangles
	node->split_value = bound;
	int next_dimension = Node::increment_dimension ( split_dimension );

	node->lower_child = buildNode ( l_box, next_dimension, l_triangle_indices );
	node->upper_child = buildNode ( u_box, next_dimension, u_triangle_indices );
	return node;
}

void KD_Tree::buildAllAABBs ()
{
	for ( int i = 0; i < num_triangles; i++ )
	{
		triangle_boxes.push_back ( AABB (
			vertices [ vertices_index [ 3 * i ] ],
			vertices [ vertices_index [ 3 * i + 1 ] ],
			vertices [ vertices_index [ 3 * i + 2 ] ] ) );
	}
}

void KD_Tree::build ()
{
	cout << "Build start"<<endl;
	buildBoundingBox ();
	buildAllAABBs ();
	vector<int> root_triangles_indices = vector<int> ( num_triangles );
	for ( int i = 0; i < num_triangles; i++ )
	{
		root_triangles_indices [ i ] = i;
	}
	root = buildNode ( structure_box, 0, root_triangles_indices );
	cout << "Build done" << endl;
}

bool KD_Tree::rayIntersection ( Result& result, const Ray& ray )
{
	//The final hit point info
	AccelerationStructure::Result cur_result;
	result.t_hit = INFINITY;

	//The t range info
	float mesh_t_in, mesh_t_out;
	if ( !root->box.rayIntersection ( ray, mesh_t_in, mesh_t_out ) )return false;
	float t_in = mesh_t_in;
	float t_out = mesh_t_in;

	//The node info
	Node* cur_root = root;
	Node* cur_node {};

	//The ray info
	vector<float> inv_ray_direction = {
		ray.direction [ 0 ] == 0 ? INF : 1.0f / ray.direction [ 0 ],
		ray.direction [ 1 ] == 0 ? INF : 1.0f / ray.direction [ 1 ] ,
		ray.direction [ 2 ] == 0 ? INF : 1.0f / ray.direction [ 2 ] };

	while ( t_out < mesh_t_out )
	{
		//Restart from cur root
		cur_node = cur_root;
		t_in = t_out;
		t_out = mesh_t_out;

		//Whether to push down the t
		bool pushdown = true;
		while ( !cur_node->is_leaf () )
		{
			//get split info
			int a = cur_node->split_dimension;

			Node* first {}; Node* second {};

			//determine which child will be the 1st to hit
			if ( ray.origin [ a ] < cur_node->split_value ||
				ray.origin [ a ] == cur_node->split_value && ray.direction [ a ] <= 0 )
			{
				first = cur_node->lower_child;
				second = cur_node->upper_child;
			}
			else
			{
				second = cur_node->lower_child;
				first = cur_node->upper_child;
			}

			float t_split = ( cur_node->split_value - ray.origin [ a ] ) * inv_ray_direction [ a ];

			//determine which nodes need to be visit in fact
			if ( t_split < 0 || t_out <= t_split )
			{
				cur_node = first;
			}
			else if ( t_split <= t_in )
			{
				cur_node = second;
			}
			else
			{
				//both nodes need to be visit
				cur_node = first;
				t_out = t_split;
				pushdown = false;
			}
			//can ignore upper nodes
			if ( pushdown )
			{
				cur_root = cur_node;
			}
		}
		//Iterate triangles in a leaf
		for ( int i = 0; i < cur_node->triangle_indices.size (); i++ )
		{
			//set the triangle index
			cur_result.triangle_index = cur_node->triangle_indices [ i ];

			//find a hit
			if ( ray_triangle_intersection ( ray,
				vertices [ vertices_index [ 3 * cur_result.triangle_index ] ],
				vertices [ vertices_index [ 3 * cur_result.triangle_index + 1 ] ],
				vertices [ vertices_index [ 3 * cur_result.triangle_index + 2 ] ],
				cur_result.t_hit, cur_result.u, cur_result.v ) )
			{
				//find a nearer hit
				if ( cur_result.t_hit < result.t_hit )
				{
					result = cur_result;
				}
			}
		}

		//if the hit is range, it's guaranteed to be the smallest
		if ( result.t_hit < t_out )
		{
			return true;
		}
	}

	return false;
}

#pragma endregion