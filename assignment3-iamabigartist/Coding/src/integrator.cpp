#ifndef NO_OMP
#include <omp.h>
#endif
#include "integrator.hpp"
#include "material.hpp"
#include "constant.hpp"
#include "light.hpp"
#include "utils.hpp"
#include "config.hpp"
#include <iostream>
using namespace std;
using namespace Eigen;

extern Config conf;

//#define DEBUG_INTEGRATOR
//#define DEBUG_SHADOW
#define SHOW_PROGRESS

/**
 * Integrator class
 */

Integrator::Integrator ( Scene* scn, Camera* cam )
	: scene ( scn )
	, camera ( cam )
{
}

/**
 * PhongLightingIntegrator class
 */

PhongLightingIntegrator::PhongLightingIntegrator ( Scene* scn, Camera* cam )
	: Integrator ( scn, cam )
{
}

void PhongLightingIntegrator::render ( int sample_num_level )
{
	//The sample step in one pixel
	float sample_step = 1.0f / sample_num_level;

	int dx, dy;

	int progress = 0;
	int total = camera->getFilm ().resolution.x () * camera->getFilm ().resolution.y ();

#ifndef NO_OMP
#pragma omp parallel for private(dy)
#endif

	for ( dx = 0; dx < camera->getFilm ().resolution.x (); ++dx )
	{
		for ( dy = 0; dy < camera->getFilm ().resolution.y (); ++dy )
		{
		#ifdef DEBUG_INTEGRATOR
			cout << "Cur Pixel: ( " << dx << " , " << dy << " )" << endl;
		#endif // DEBUG_INTEGRATOR

		#ifdef SHOW_PROGRESS
			if ( progress % ( total / 100 ) == 0 )
			{
				cout << "progress " << 100.0f * ( float ) progress / ( float ) total << "% ......" << endl;
			}

		#endif // SHOW_PROGRESS

			//The mean randiance of all rays shooted in 1 pixel
			Vector3f cur_pixel_radiance ( 0, 0, 0 );

			//Anti-ailising
			for ( int offset_x = 0; offset_x < sample_num_level; offset_x++ )
			{
				for ( int offset_y = 0; offset_y < sample_num_level; offset_y++ )
				{
					Ray ray = camera->generateRay ( dx + offset_x * sample_step, dy + offset_y * sample_step );

				#ifdef DEBUG_INTEGRATOR
					//cout << "Ray: " << "dir: ( " << ray.direction.transpose () << " )" << endl;
				#endif // DEBUG_INTEGRATOR

					Interaction interaction;
					if ( scene->intersection ( ray, interaction ) )
					{
						cur_pixel_radiance += radiance ( interaction, ray );
					}
				}
			}

			camera->setPixel ( dx, dy, cur_pixel_radiance / pow ( sample_num_level, 2 ) );
			//每个pixel生成多条ray,scene::intersect,然后计算颜色,
			// 然后平均所有条颜色
			// setpixel
			// 完成

			progress++;
		}
	}
}

Vector3f PhongLightingIntegrator::radiance ( const Interaction& interaction, const Ray& ray ) const
{
	constexpr auto BIAS = 0.001;

		/* If the ray hits a light source, return the light color */
	if ( interaction.type == Interaction::Type::LIGHT )
	{
		return scene->getLight ()->getColor ();
	}

	else if ( interaction.type == Interaction::Type::NONE )
	{
		return Vector3f ( 0, 0, 0 );
	}

	else
	{
	#define BLIN_PHONG

		/* Ambient component */
		Vector3f amb_comp = scene->getAmbientLight ();

		//The total radiance of the ray and interaction
		Vector3f radiance_sum ( 0, 0, 0 );

		//Phong ligting model
		Vector3f diff_comp, spec_comp;
		for each ( auto light in scene->getLight ()->get_samples () )//Sample randiance of every light
		{
			//The vector from entry point to the light source position, reflect the light distance
			Vector3f light_distance_vector = light.first - interaction.entry_point;
			Ray sample_ray ( interaction.entry_point + interaction.normal * BIAS, light_distance_vector );
			//sample_ray.exception_geometries.insert ( interaction.geometry_hit );
			if ( !scene->isShadowed ( sample_ray ) )//can reach the light source
			{
			#ifdef DEBUG_SHADOW
				cout << "Reach is shadow if!" << endl;
			#endif // DEBUG_INTEGRATOR

			#ifndef BLIN_PHONG
				//calculate reflect
				Vector3f reflect_light = ( 2 * ( interaction.normal.dot ( sample_ray.direction ) ) * interaction.normal - sample_ray.direction ).normalized ();
				//dot reflect light dir and original ray dir
				float cos_e_rl = clamp ( reflect_light.dot ( -ray.direction ), 0, 1 );
			#endif // BLIN_PHONG

			#ifdef BLIN_PHONG
				//calculate reflect
				Vector3f H = ( -ray.direction + sample_ray.direction ).normalized ();
				//dot reflect light dir and original ray dir
				float cos_e_rl = clamp ( H.dot ( interaction.normal ), 0, 1 );
			#endif // BLIN_PHONG

				//dot normal and source light dir
				float cos_n_l = clamp ( interaction.normal.dot ( sample_ray.direction ), 0, 1 );

				float d_pow = pow ( light_distance_vector.norm (), 0 );//UNDONE Is norm the distance of a vector?
				diff_comp =
					( cos_n_l / d_pow )
					* interaction.lighting_model.diffusion.cwiseProduct (
					light.second );
				spec_comp =
					( pow ( cos_e_rl, interaction.lighting_model.shininess ) / d_pow )
					* interaction.lighting_model.specular.cwiseProduct (
					light.second );
				radiance_sum += ( diff_comp + spec_comp );
			}
		}
		//生成与所有点光源相交的射线 Ray()
		//检查他们是否阴影 isShadowed()
		//计算到达光源的射线的颜色: 视线v->源射线ray反方向, 光线方向l->生成的此射线方向 大概; 三part phong model
		//加和所有的颜色
		//返回这个值

	#ifdef DEBUG_INTEGRATOR
		cout << "Color: " << "( " << radiance_sum.transpose () << " )" << endl;
	#endif // DEBUG_INTEGRATOR

		//At last need to add the ambient part just once
		radiance_sum += interaction.lighting_model.diffusion.cwiseProduct ( amb_comp );
		return radiance_sum;
	}
}