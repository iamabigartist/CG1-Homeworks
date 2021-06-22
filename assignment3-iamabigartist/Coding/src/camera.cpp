#include <utility>
#include "camera.hpp"
#include "constant.hpp"
#include "utils.hpp"
#include <iostream>
using namespace Eigen;
using namespace std;

//#define DEBUG_CAMERA
Camera::Camera ( const Vector3f& pos, float focal_len, float v_fov, const Vector2i& film_res )
	: film ( film_res )
	, vertical_fov ( v_fov )
	, position ( pos )
	, focal_length ( focal_len )
	, forward ( 0, 0, 1 )
	, right ( 1, 0, 0 )
	, up ( 0, 1, 0 )
{
	image_width = film.resolution.x ();
	image_height = film.resolution.y ();
	aspect_ratio = film.getAspectRatio ();
	tan_alpha = tan ( vertical_fov / 2 * ( PI / 180 ) );
	camera_matrix.Constant ( 0 );
}

void Camera::lookAt ( const Vector3f& look_at_point, const Vector3f& ref_up )
{
	Vector3f look_dir = look_at_point - position;
	forward = look_dir.normalized ();
	right = look_dir.cross ( ref_up ).normalized ();
	up = right.cross ( forward ).normalized ();

	camera_matrix << V3_4f ( right ), V3_4f ( up ), V3_4f ( forward ), V3_4f ( position, 1 ); //may cause uninitialised bug
}

/// <summary>
/// Generate a ray according to the given pixel indices
/// </summary>
/// <param name="dx">The x dimension index</param>
/// <param name="dy">The y dimension index</param>
Ray Camera::generateRay ( float dx, float dy ) const
{
#ifdef DEBUG_CAMERA
	float r_x = dx / image_width;
	float r_y = dy / image_height;
	float alow_diff = 0.001;
	bool check =
		( close_enough ( saw_wave ( 10 * r_x ), 0.1, alow_diff ) && close_enough ( saw_wave ( 10 * r_y ), 0.1, alow_diff ) );
	if ( check ) cout << "( " << r_x << " , " << r_y << " )->";

#endif // DEBUG_CAMERA

	//Turn the pixel indices into the camera space coordinates
	dx = ( 2 * ( ( dx + 0.5 ) / image_width ) - 1 ) * tan_alpha * aspect_ratio;
	dy = ( 1 - 2 * ( ( dy + 0.5 ) / image_height ) ) * tan_alpha;

	//Cause the dir of camera forward may different at the turorial and this homework, the poistion need the flipping adjustment.
	Vector3f ray_point = V4_xyz ( camera_matrix * Vector4f ( dx, -dy, 1, 1 ) );

#ifdef DEBUG_CAMERA
	if ( check ) cout << "( " << ray_point.x () << " , " << ray_point.y () << " , " << ray_point.z () << " )" << endl;
#endif // DEBUG_CAMERA

	return Ray { position, ( ray_point - position ).normalized () };
}

void Camera::setPixel ( int dx, int dy, Vector3f value )
{
	film.pixel_array [ dy * film.resolution.x () + dx ] = value;
}

const Film& Camera::getFilm () const
{
	return film;
}