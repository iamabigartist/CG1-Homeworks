#define TINYCOLORMAP_WITH_EIGEN
#include "tinycolormap.hpp"
#include "classifier.hpp"
#include "constant.hpp"
#include "utils.hpp"

using namespace MathUtils;
using namespace tinycolormap;
/**
 * IsosurfaceClassifier class
 */

IsosurfaceClassifier::IsosurfaceClassifier ( float isoval )
	: Classifier ( isoval )
{
}

OpticsData IsosurfaceClassifier::transfer ( VolumePointData v_data, Camera* cam, const std::vector<Light*>& lights, float dt ) const
{
	OpticsData optics;

	//Evaluate the transparency
	//optics.transparency = guassian ( v_data.value ) * V3 ( 1, 1, 1 );
	optics.transparency = V3 ( 0, 0, 0 );

#pragma region Original color
	//printf ( "norm: %f\n", v_data.gradient.norm () );
	V3 original_color = GetColor ( v_data.gradient.norm () * 0.05f, ColormapType::Viridis ).ConvertToEigen ().cast<float> ();
#pragma endregion

#pragma region PhongLightingModel
	V3 normal = v_data.gradient.normalized ();
	V3 ambient_color = V3 ( 1.0f, 1.0f, 1.0f ) * AMBIENT_MAGNITUDE;
	V3 specular_color = V3 ( 0.0f, 0.0f, 0.0f );
	V3 diffusion_color = V3 ( 0.0f, 0.0f, 0.0f );
	V3 w_eye = ( cam->getPosition () - v_data.position ).normalized ();
	if ( w_eye.dot ( normal ) < 0 )normal *= -1;
	for ( int i = 0; i < lights.size (); i++ )
	{
		V3 w_i = ( -v_data.position + lights [ i ]->getPosition () ).normalized ();
		float cos_wi_n = abs ( normal.dot ( w_i ) );
		diffusion_color += lights [ i ]->getColor () * cos_wi_n;

		V3 w_o = ( 2 * normal * cos_wi_n - w_i ).normalized ();//may >1, do not know why
		specular_color += lights [ i ]->getColor () * pow ( abs ( w_eye.dot ( w_o ) ), SPECULAR_SHININESS ); //UNDONE abs may cause bug
		//printf ( "diff: %f\n", w_o.norm () );
	}

	optics.color = original_color.cwiseProduct ( ambient_color + diffusion_color + specular_color );
	//optics.color = original_color;
#pragma endregion

	return optics;
}