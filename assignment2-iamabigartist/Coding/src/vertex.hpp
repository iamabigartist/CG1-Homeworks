#ifndef VERTEX_HPP
#define VERTEX_HPP
#include "header.hpp"
using namespace glm;
struct Vertex {
	// position
	vec3 position;
	// normal
	vec3 normal;
	//uv
	vec2 uv;

	Vertex() {}
	Vertex(vec3 position, vec3 normal, vec2 uv) :position(position), normal(normal), uv(uv) {}
};
#endif