#include<Windows.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "MTime.h"
using namespace glm;

extern double delta_time;

float pi_f = pi<float>();

Camera::Camera(GLFWwindow* window)
{
	this->window = window;
	position = vec3(0, 0, 4);
	h_angle = pi_f; v_angle = 0;
	Fov = 45.0f;
	move_speed = 0.50f;
	mouse_sensor = 0.05f;
}

Camera::~Camera() {}

mat4 Camera::GetViewMatrix() { return ViewMatrix; }
mat4 Camera::GetProjectionMatrix() { return ProjectionMatrix; }

void Camera::RefreshMatrixByInput()
{
	//record window size
	double x_mouse, y_mouse;
	glfwGetCursorPos(window, &x_mouse, &y_mouse);

	//record and restrict mouse
	GLint width, height; GLfloat aspect_ratio;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) { aspect_ratio = 0; }
	else { aspect_ratio = width / height; }
	GLfloat x_center = width / 2;
	GLfloat y_center = height / 2;
	glfwSetCursorPos(window, x_center, y_center);//glVursorDisable?

	//Mouse control
	h_angle += (-x_mouse + x_center) * mouse_sensor * (float)delta_time; v_angle += (-y_mouse + y_center) * mouse_sensor * (float)delta_time;

	//Generate the 3 direction
	vec3 forward(cos(v_angle) * sin(h_angle), sin(v_angle), cos(v_angle) * cos(h_angle));
	vec3 right(sin(h_angle - pi_f / 2), 0, cos(h_angle - pi_f / 2));
	vec3 up = cross(right, forward);

	//printf("forward: %f,%f,%f\n", forward.x, forward.y, forward.z);

	float speed_up_ratio = 1;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { speed_up_ratio = 2; }
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { speed_up_ratio = 0.5; }
	//Camera move
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { position += forward * speed_up_ratio * move_speed * (float)delta_time; }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { position -= forward * speed_up_ratio * move_speed * (float)delta_time; }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { position -= right * speed_up_ratio * move_speed * (float)delta_time; }
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) { position += right * speed_up_ratio * move_speed * (float)delta_time; }

	//ProjectionMatrix = ortho(1000.0f, 1000.0f, 1000.0f, 1000.0f);
	ProjectionMatrix = perspective(Fov, aspect_ratio, 0.1f, 300.0f);//Generate PMatrix
	ViewMatrix = lookAt(position, position + forward, up);
	//printf("position: %f %f %f\n", position.x, position.y, position.z);
}