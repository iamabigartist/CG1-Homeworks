#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "geometry.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "integrator.hpp"
#include "utils.hpp"
#include "config.hpp"

#define clock_t std::chrono::time_point<std::chrono::system_clock>

using namespace std;
using namespace Eigen;

//#define DEBUG_OUTPUT

Config conf;

int main ( int argc, char* argv [] )
{
	/////////////////////////////////////////////////////////////////////////
	// Parse input arguments
	/////////////////////////////////////////////////////////////////////////

	if ( argc >= 2 )
	{
		if ( !conf.parseConfigFile ( std::string ( argv [ 1 ] ) ) )
		{
			return -1;
		}
		conf.printConfig ();
	}
	else
	{
		std::cout << "Please specify a configuration file." << std::endl;
		return -1;
	}

	/////////////////////////////////////////////////////////////////////////
	// Camera settings
	/////////////////////////////////////////////////////////////////////////

	Eigen::Vector3f camera_pos;
	Eigen::Vector3f camera_look_at;
	if ( conf.camera_id == 1 )
	{
		// Camera setting 1
		camera_pos = Eigen::Vector3f ( 0, 0, 10 );
		camera_look_at = Eigen::Vector3f ( 0, 0, 0 );
	}
	else if ( conf.camera_id == 2 )
	{
		// Camera setting 2
		camera_pos = Eigen::Vector3f ( 5, 3, 12 );
		camera_look_at = Eigen::Vector3f ( 0, 0, 0 );
	}
	else if ( conf.camera_id == 3 )
	{
		// Camera setting 3
		camera_pos = Eigen::Vector3f ( 0, -4, 15 );
		camera_look_at = Eigen::Vector3f ( 0, 0, 0 );
	}
	float focal_len = 1.0f;
	float vertical_fov = 45.0f;
	Eigen::Vector2i film_res ( conf.output_resolution, conf.output_resolution );
	Camera camera ( camera_pos, focal_len, vertical_fov, film_res );
	camera.lookAt ( camera_look_at, Eigen::Vector3f ( 0, 1, 0 ) );

	/////////////////////////////////////////////////////////////////////////
	// Material settings
	/////////////////////////////////////////////////////////////////////////

	ColorMaterial mat_gray ( Eigen::Vector3f ( 0.7, 0.7, 0.7 ), 16.0 );
	ColorMaterial mat_left ( Eigen::Vector3f ( 1.0, 0, 0 ), 16.0 );
	ColorMaterial mat_green ( Eigen::Vector3f ( 0, 1.0, 0 ), 16.0 );

	Texture tex_cb1, tex_cb2, tex_cb3, tex_cb4;

	Texture::generateCheckerboard ( tex_cb1, Eigen::Vector3f ( 0, 0, 1.0 ), Eigen::Vector3f ( 1.0, 1.0, 1.0 ) );
	Texture::generateCheckerboard ( tex_cb2, Eigen::Vector3f ( 0.2, 0.5, 0.0 ), Eigen::Vector3f ( 0.9, 0.0, 0.4 ), 4 );
	Texture::generateCheckerboard ( tex_cb3, Eigen::Vector3f ( 1.0, 1.0, 1.0 ), Eigen::Vector3f ( 0, 0, 0 ), 8 );
	Texture::generateCheckerboard ( tex_cb4, Eigen::Vector3f ( 0.3, 1.0, 0.4 ), Eigen::Vector3f ( 0.3, 0.7, 0.2 ) );
	TextureMaterial mat_cb1 ( &tex_cb1, 22.0 );
	TextureMaterial mat_cb2 ( &tex_cb2, 40.0 );
	TextureMaterial mat_cb3 ( &tex_cb3, 120.0 );
	TextureMaterial mat_cb4 ( &tex_cb4, 16.0 );

	/////////////////////////////////////////////////////////////////////////
	// Setting the Cornell box.
	/////////////////////////////////////////////////////////////////////////

	Eigen::Vector3f p_p0 ( -10, -10, -10 );
	Eigen::Vector3f p_s0 ( 1, 0, 0 );
	Eigen::Vector3f p_s1 ( 0, 1, 0 );
	Eigen::Vector3f p_normal ( 0, 0, 1 );
	Parallelogram back_wall ( p_p0, p_s0 * 20, p_s1 * 20, p_normal, &mat_gray );

	p_p0 = Eigen::Vector3f ( -6, -7, -11 );
	p_s0 = Eigen::Vector3f ( 0, 0, 1 );
	p_s1 = Eigen::Vector3f ( 0, 1, 0 );
	p_normal = Eigen::Vector3f ( 1, 0, 0 );
	Parallelogram left_wall ( p_p0, p_s0 * 20, p_s1 * 20, p_normal, &mat_left );

	p_p0 = Eigen::Vector3f ( 6, -7, -11 );
	p_s0 = Eigen::Vector3f ( 0, 0, 1 );
	p_s1 = Eigen::Vector3f ( 0, 1, 0 );
	p_normal = Eigen::Vector3f ( -1, 0, 0 );
	Parallelogram right_wall ( p_p0, p_s0 * 20, p_s1 * 20, p_normal, &mat_green );

	p_p0 = Eigen::Vector3f ( -7, -6, -11 );
	p_s0 = Eigen::Vector3f ( 1, 0, 0 );
	p_s1 = Eigen::Vector3f ( 0, 0, 1 );
	p_normal = Eigen::Vector3f ( 0, 1, 0 );
	Parallelogram floor ( p_p0, p_s0 * 20, p_s1 * 20, p_normal, &mat_gray );

	p_p0 = Eigen::Vector3f ( -7, 6, -11 );
	p_s0 = Eigen::Vector3f ( 1, 0, 0 );
	p_s1 = Eigen::Vector3f ( 0, 0, 1 );
	p_normal = Eigen::Vector3f ( 0, -1, 0 );
	Parallelogram ceiling ( p_p0, p_s0 * 20, p_s1 * 20, p_normal, &mat_gray );

	/////////////////////////////////////////////////////////////////////////
	// Setting geometries
	/////////////////////////////////////////////////////////////////////////

	Sphere sphere1 ( Eigen::Vector3f ( 3, 1, -7 ), 1.5f, &mat_cb1 );
	Sphere sphere2 ( Eigen::Vector3f ( 2, -3.2, -5 ), 1.0f, &mat_cb2 );
	Sphere sphere3 ( Eigen::Vector3f ( -4, 2, -5 ), 1.8f, &mat_cb3 );
	Sphere sphere4 ( Eigen::Vector3f ( -2, -4.5, -8 ), 2.0f, &mat_cb4 );

	/////////////////////////////////////////////////////////////////////////
	// Light setting
	/////////////////////////////////////////////////////////////////////////

	Eigen::Vector3f light_pos;
	Eigen::Vector3f light_color;

	if ( conf.light_id == 1 )
	{
		// Light setting 1
		light_pos = Eigen::Vector3f ( 0, 5.9, -6 );
		light_color = Eigen::Vector3f ( 1.0, 1.0, 1.0 );
	}
	else if ( conf.light_id == 2 )
	{
		// Light setting 2
		light_pos = Eigen::Vector3f ( 3, 5.9, -6 );
		light_color = Eigen::Vector3f ( 0.9, 0.8, 0 );
	}
	else if ( conf.light_id == 3 )
	{
		// Light setting 3
		light_pos = Eigen::Vector3f ( -3, 5.9, -6 );
		light_color = Eigen::Vector3f ( 0, 0.9, 0.7 );
	}

	AreaLight light ( light_pos, light_color, Eigen::Vector2f ( 2, 2 ),
					  Eigen::Vector2f ( 2.0f / conf.area_light_resolution, 2.0f / conf.area_light_resolution ) );

	/////////////////////////////////////////////////////////////////////////
	// Scene setup
	/////////////////////////////////////////////////////////////////////////

	Scene scene ( &light );
	scene.setAmbientLight ( Eigen::Vector3f ( 0.1, 0.1, 0.1 ) );
	scene.addGeometry ( &back_wall );
	scene.addGeometry ( &left_wall );
	scene.addGeometry ( &right_wall );
	scene.addGeometry ( &floor );
	scene.addGeometry ( &ceiling );

	scene.addGeometry ( &sphere1 );
	scene.addGeometry ( &sphere2 );
	scene.addGeometry ( &sphere3 );
	scene.addGeometry ( &sphere4 );

	/////////////////////////////////////////////////////////////////////////
	// Perform Phong lighting integrator
	/////////////////////////////////////////////////////////////////////////

	std::cout << "Rendering..." << std::endl;

	clock_t start_time = std::chrono::system_clock::now ();

	PhongLightingIntegrator integrator ( &scene, &camera );
	integrator.render ( conf.num_samples );

	clock_t end_time = std::chrono::system_clock::now ();
	double time_elapsed = std::chrono::duration_cast< std::chrono::milliseconds >( end_time - start_time ).count ();
	std::cout << "Time elapsed: " << time_elapsed << " ms" << std::endl;

	/////////////////////////////////////////////////////////////////////////
	// Output the image to a file
	/////////////////////////////////////////////////////////////////////////

	std::string output_path = conf.output_file;
	std::vector<unsigned char> output_data;
	output_data.reserve ( film_res.x () * film_res.y () * 3 );

	for ( Eigen::Vector3f v : camera.getFilm ().pixel_array )
	{
	#ifdef DEBUG_OUTPUT
		cout << v.transpose () << endl;
	#endif // DEBUG_OUTPUT

		for ( int i = 0; i < 3; i++ )
		{
			output_data.push_back ( gamma_correction ( v [ i ] ) );
		}
	}
	stbi_flip_vertically_on_write ( true );
	stbi_write_png ( output_path.c_str (), film_res.x (), film_res.y (), 3, output_data.data (), 0 );

	return 0;
}