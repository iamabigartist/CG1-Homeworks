#include <omp.h>
#include "optics_data.hpp"
#include "volume_renderer.hpp"
#include "config.hpp"
#include "constant.hpp"

extern Config conf;

/**
 * VolumeRenderer class
 */

VolumeRenderer::VolumeRenderer ()
	: camera ( NULL )
	, classifier ( NULL )
	, geom ( NULL )
{
}

void VolumeRenderer::setCamera ( Camera* cam )
{
	camera = cam;
}

void VolumeRenderer::addLight ( Light* li )
{
	lights.push_back ( li );
}

void VolumeRenderer::setGeometry ( ImplicitGeometry* implicit_geom )
{
	geom = implicit_geom;
}

void VolumeRenderer::setClassifier ( Classifier* cls )
{
	classifier = cls;
}

void VolumeRenderer::renderFrontToBack ()
{
	int res_x = camera->getFilm ().resolution.x ();
	int res_y = camera->getFilm ().resolution.y ();
	float dt = conf.step_len;

	float total_progress = res_x * res_y;
	float cur_progress = 0;
#ifndef NO_OMP
#pragma omp parallel for
#endif
	for ( int i = 0; i < res_x * res_y; ++i )
	{
		int dy = i / ( float ) res_x;
		int dx = i - dy * ( float ) res_x;
		Ray ray = camera->generateRay ( ( float ) dx, ( float ) dy );
		Eigen::Vector3f color ( 0, 0, 0 );
		Eigen::Vector3f alpha ( 0, 0, 0 );
#pragma region Determine color
		float t_in, t_out;
		geom->bboxRayIntersection ( ray, t_in, t_out );
		float t_cur = t_in;
		while ( t_cur <= t_out )
		{
			V3 position = ray.getPoint ( t_cur );
			float value = geom->getValue ( position );
			if ( abs ( value - classifier->get_isovalue () ) < DELTA )//can render
			{
				VolumePointData v_data;
				v_data.value = value;
				v_data.position = position;
				v_data.gradient = geom->computeGradient ( position );
				auto optic_data = classifier->transfer ( v_data, camera, lights, dt );
				color = optic_data.color;
				alpha = optic_data.getOpacity ();
				break;
			}

			t_cur += dt;
		}
#pragma endregion

		camera->setPixel ( dx, dy, color );

		cur_progress++;
		if ( ( int ) ( 100 * cur_progress / total_progress ) % 10 == 0 )
			printf ( "%f...\n", 100 * cur_progress / total_progress );
	}
};