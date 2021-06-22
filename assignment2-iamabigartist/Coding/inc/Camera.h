#pragma once
class Camera
{
public:
	Camera(GLFWwindow*);
	~Camera();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	void RefreshMatrixByInput();
	GLfloat Fov;
	GLfloat move_speed, mouse_sensor;
	glm::vec3 position;
	GLfloat h_angle, v_angle;
	GLFWwindow* window;
private:
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
};
