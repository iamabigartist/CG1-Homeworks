#ifndef NO_OMP
#include <omp.h>
#endif
#include "progressbar.hpp"
#include "integrator.hpp"
#include "constant.hpp"
#include "interaction.hpp"

extern Config conf;

/**
 * Integrator class
 */

Integrator::Integrator ( Scene& scn, Camera& cam, Config& conf )
	: scene ( scn )
	, camera ( cam )
	, conf ( conf )
{
}

/**
 * PathTracingIntegrator class
 */

PathTracingIntegrator::PathTracingIntegrator ( Scene& scene, Camera& camera, Config& conf )
	: Integrator ( scene, camera, conf )
{
}

void PathTracingIntegrator::render ()
{
	float sample_step = 1.0f / conf.num_samples;

	int dx, dy;
	int res_x = camera.getFilm ().resolution.x (), res_y = camera.getFilm ().resolution.y ();

	/* Initialize a progress bar */
	//progressbar progress_bar ( res_x * res_y );

	int total_progress = res_x * res_y;
	int cur_progress = 0;

#ifndef NO_OMP
#pragma omp parallel for private(dy)
#endif
	for ( dx = 0; dx < res_x; ++dx )
	{
		for ( dy = 0; dy < res_y; ++dy )
		{
			Vector3f L ( 0, 0, 0 );
			//Anti-ailising
			for ( int offset_x = 0; offset_x < conf.num_samples; offset_x++ )
			{
				for ( int offset_y = 0; offset_y < conf.num_samples; offset_y++ )
				{
					Ray ray = camera.generateRay ( dx + offset_x * sample_step, dy + offset_y * sample_step );

					L += radiance ( ray );
				}
			}

			L /= pow ( conf.num_samples, 2 );

			camera.setPixel ( dx, dy, L );

			//progress_bar.update ();
			if ( ( ++cur_progress ) % ( total_progress / 100 ) == 0 ) {
				cout << 100.0f * ( float ) ( cur_progress ) / total_progress << "%......" << endl;
			}
		}
	}
}

Eigen::Vector3f PathTracingIntegrator::radiance ( Ray ray )
{
	//TODO Russian Roullete PRR, where to put, which term
	//TODO Multiple direct light
	//TODO ask the blog
	//TODO check the todo
	//TODO Normal sample transfrom
	//TODO Öªºõ MIS

	V3 Beta = V3 ( 1, 1, 1 ); V3 L = V3 ( 0, 0, 0 );
	bool flag_done = false;

	float P_RR = pow ( conf.max_depth, 1 );

	//bool was_mesh = false;
	//while ( unif ( 0, 1 ) > 1 / conf.max_depth )
	for ( int i = 0; i < conf.max_depth; i++ )
	{
		Interaction interaction;
		scene.intersection ( ray, interaction );
		switch ( interaction.type )
		{
		case Interaction::Type::NONE: {flag_done = true; break; }//Nothing, just return the before color
		case Interaction::Type::LIGHT:
		{flag_done = true; L += Beta.cwiseProduct ( scene.getLight ()->getRadiance () ); break; }//Finally add the light color
		case Interaction::Type::GEOMETRY: {break; }//The iteration continue.
		default:throw;
		}

		if ( flag_done )break;

		//set ray dir to interaction out direction
		interaction.wo = ray.direction;
		normalCoordinateSystem ( interaction.normal, interaction.n_t, interaction.n_b );

		V3 L_direct ( 0, 0, 0 ); float pdf_direct_light_ray = 0;
		if ( !( ( BRDF* ) interaction.material )->isDelta () )
		{
			//sample direct light

			scene.getLight ()->sample ( interaction, &pdf_direct_light_ray );
			//cout << "Light pdf:" << pdf_direct_light_ray << endl;
			if ( !scene.isShadowed (
				Ray ( interaction.entry_point + interaction.wi * EPSILON, interaction.wi ) ) )
			{
				L_direct =
					Beta.cwiseProduct (
						scene.getLight ()->getRadiance ().cwiseProduct (
						( ( BRDF* ) interaction.material )->eval ( interaction ) *
						abs ( interaction.normal.dot ( interaction.wi ) ) /
						pdf_direct_light_ray ) ) * P_RR;
			}
			else
			{
				pdf_direct_light_ray = 0;
			}
		}

		//sample next ray
		float pdf_cur_ray = ( ( BRDF* ) interaction.material )->sample ( interaction );
		//L += L_direct * ( pdf_direct_light_ray / ( pdf_direct_light_ray + pdf_cur_ray ) );
		L += L_direct;
		//cout << "Next Ray pdf:" << pdf_cur_ray << endl;
		Beta = Beta.cwiseProduct (
			( ( BRDF* ) interaction.material )->eval ( interaction ) *
			abs ( interaction.normal.dot ( interaction.wi ) ) /
			pdf_cur_ray ) * P_RR;
		//Beta = Beta * ( pdf_cur_ray / ( pdf_direct_light_ray + pdf_cur_ray ) );
		ray = Ray ( interaction.entry_point - interaction.wi * EPSILON, interaction.wi );

		//was_mesh = interaction.is_mesh ? true : false;
	}

	return L;
}