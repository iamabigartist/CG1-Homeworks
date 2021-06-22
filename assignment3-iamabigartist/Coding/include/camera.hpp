#pragma once
#include "Eigen/Dense"
#include "film.hpp"
#include "ray.hpp"

class Camera
{
protected:
	Eigen::Vector3f position;
	/// <summary>
	/// The z axis of camera space
	/// </summary>
	Eigen::Vector3f forward;
	/// <summary>
	/// The x axis of camera space
	/// </summary>
	Eigen::Vector3f right;
	/// <summary>
	/// The y axis of camera space
	/// </summary>
	Eigen::Vector3f up;
	/// <summary>
	/// The camera space to world space transform matrix
	/// </summary>
	Eigen::Matrix4f camera_matrix;
	float image_width;
	float image_height;
	float aspect_ratio;
	/// <summary>
	/// Vertical field of view
	/// </summary>
	float vertical_fov;
	/// <summary>
	/// tan(vertical_fov / 2 * PI / 180)
	/// </summary>
	float tan_alpha;
	/// <summary>
	/// The distance to the image plane from the camera
	/// </summary>
	float focal_length;
	/// <summary>
	/// The camera view image
	/// </summary>
	Film film;

public:
	Camera ( const Eigen::Vector3f& pos, float focal_len, float v_fov, const Eigen::Vector2i& film_res );
	void lookAt ( const Eigen::Vector3f& look_at_point, const Eigen::Vector3f& ref_up );
	Ray generateRay ( float dx, float dy ) const;
	void setPixel ( int dx, int dy, Eigen::Vector3f value );
	const Film& getFilm () const;
};