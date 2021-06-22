#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include "interaction.hpp"

/**
 * Base class of integrator
 */

class Integrator
{
protected:
	Scene* scene;
	Camera* camera;

public:
	Integrator ( Scene* scn, Camera* cam );
	virtual ~Integrator () = default;

	virtual void render ( int sample_num_level ) = 0;
	virtual Eigen::Vector3f radiance ( const Interaction& interaction, const Ray& ray ) const = 0;
};

/**
 * Phong lighting integrator
 */

class PhongLightingIntegrator : public Integrator
{
public:
	PhongLightingIntegrator ( Scene* scn, Camera* cam );
	/// <param name="sample_num_level">The anti-ailising sample, sample num = level^2 </param>
	virtual void render ( int sample_num_level ) override;
	virtual Eigen::Vector3f radiance ( const Interaction& interaction, const Ray& ray ) const override;
};