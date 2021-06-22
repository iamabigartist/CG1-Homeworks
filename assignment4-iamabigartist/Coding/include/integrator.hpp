#pragma once
#include "scene.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "brdf.hpp"

/**
 * Base class of integrator
 */

class Integrator
{
protected:
	Scene& scene;
	Camera& camera;
	Config& conf;

public:
	Integrator ( Scene& scn, Camera& cam, Config& conf );
	virtual ~Integrator () = default;

	virtual void render () = 0;
	virtual Eigen::Vector3f radiance ( Ray ray ) = 0;
};

/**
 * Path-tracing integrator
 */

class PathTracingIntegrator : public Integrator
{
public:
	PathTracingIntegrator ( Scene& scene, Camera& camera, Config& conf );
	virtual void render ()  override;
	virtual Eigen::Vector3f radiance ( Ray ray ) override;
};