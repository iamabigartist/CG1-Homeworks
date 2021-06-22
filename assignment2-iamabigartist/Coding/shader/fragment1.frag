#version 330 core

in vec3 v_W;
in vec3 n_W;
in vec3 eye_vec_W;
in vec3 light_vec_W;
in vec2 uv;
//in float light_distance;

out vec3 color;

uniform vec3 e_W;//camera(eye) position in World Space
uniform vec3 l_W;//light postion position in World Space
uniform sampler2D texture1;

void main()
{
	//light info
	vec3 light_color_ori = vec3(1.0,1.0,0.6);
	float light_power = 50.0f;
	vec3 light_color = light_color_ori*light_power;

	//material info
	vec3 material_diffuse_color = texture2D(texture1,uv).rgb;
	//vec3 material_diffuse_color = vec3(uv,0);
	vec3 material_ambient_color = vec3(0.7,0.7,0.7)*material_diffuse_color;
	vec3 material_specular_color = vec3(0.4,0.4,0.4);
	float mirror_scale = 10;//How much the materail behave like a mirror.


	vec3 n = normalize(n_W);
	vec3 l = normalize(light_vec_W);
	vec3 e = normalize(eye_vec_W);
	vec3 rl = reflect(-l,n);//reflect light direction

	float cos_n_l = clamp(dot(n,l),0,1);
	float cos_e_rl = clamp(dot(e,rl),0,1);

	float light_distance = length(l_W-v_W);
	float d2 = pow(light_distance,2);

	color =
		material_ambient_color
		+material_diffuse_color*light_color*cos_n_l/d2
		+material_specular_color*light_color*pow(cos_e_rl,mirror_scale)/d2;
		


}