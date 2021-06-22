#version 330 core

layout(location = 0) in vec3 v_M;
layout(location = 1) in vec3 n_M;

//info specified for one vertex
out vec3 v_W;
out vec3 n_W;
out vec3 eye_vec_W;
out vec3 light_vec_W;
//out float light_distance;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 e_W;//camera(eye) position in World Space
uniform vec3 l_W;//light postion position in World Space

void main()
{
	gl_Position = MVP*vec4(v_M,1);//assign clip postion


	//Preparation for the light calculate

	v_W = (M*vec4(v_M,1)).xyz; //world_position = ModelM * model_position

	// world_normal=ModelM(inverse transpose for scaling)
//	n_W = (transpose(inverse(M))*vec4(n_M,0)).xyz;
	n_W = (M*vec4(n_M,0)).xyz;

	//the vector from the vertex to the camera in World Space= e_W - v_W
	eye_vec_W = e_W-v_W;

	//the vector from the vertex to the light point in World Space = l_W - v_W
	light_vec_W = l_W-v_W;

//	light_distance = length(light_vec_W);
}