#pragma once
#include "Eigen/Dense"
/**
 * The Phong lighting model at the interacting point
 */

struct InteractionPhongModel
{
	Eigen::Vector3f diffusion;
	Eigen::Vector3f specular;
	float shininess;
};

/**
 * Data structure representing interaction between objects and rays
 */

struct Interaction
{
	enum class Type
	{
		NONE,
		GEOMETRY,
		LIGHT
	};

	//A special term that indicate if the ray according to this interaction will have total internal reflection in transmission materail
	bool refraction_impossible = false;

	/* Distance (in units of t) to intersection point */
	float entry_dist;
	/* Distance (in units of t) to the second intersection point(if existed) */
	float exit_dist;
	/* Position of intersection point */
	Eigen::Vector3f entry_point;
	/* Normal of intersection point (if existed) */
	Eigen::Vector3f normal;
	Eigen::Vector3f n_t, n_b; //Normal Coordinate system
	/* Material at the intersected point (if existed) */
	void* material;
	/* Direction of incoming radiance */
	Eigen::Vector3f wi;
	/* Direction of outcoming radiance */
	Eigen::Vector3f wo;
	/* Type of interacting object */
	Type type;

	//bool is_mesh = false;

	Interaction () : entry_dist ( -1 ), exit_dist ( -1 ), material ( nullptr ), type ( Type::NONE ) {}
};