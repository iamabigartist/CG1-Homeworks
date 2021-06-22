#include "SurfaceRender.hpp"
#include "header.hpp"
#include "Utility.hpp"
#include "SurfaceMesh.hpp"
#include "BsplineSurface.hpp"
#include "ControlPoint.hpp"
#include "Camera.h"
#include "MTime.h"
#include "shader_m.h"
#include "stb_image.h"
using namespace std;
using namespace glm;

void reshape(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

GLuint Load_box_texture(const char* path)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	int width, height, channels;
	unsigned char* data = stbi_load(path, &width, &height, &channels, 4);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("stbi_load fail: %s\n", stbi_failure_reason());
		throw new exception("stbi_load fail");
	}
	stbi_image_free(data);
	return tex;
}

extern double delta_time;

int main()
{
#pragma region InitWindowAndEnvironment
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "HW2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, reshape);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// configure global opengl state
	// -----------------------------

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	//glEnable(GL_MULTISAMPLE);
#pragma endregion

#pragma region Set the Mesh and the Renderer

#pragma region Construct spline

#pragma region ReadControlPoint
	//read given control point.(We already provide one piece of control points in dat file)
	//you can also define your own control point.
	ControlPoint m_controllpoint_reader;
	m_controllpoint_reader.readBinary("../resources/controlPoint_0.dat");
	vector<vector<vec3>> controlpoint = m_controllpoint_reader.getControlPoint();
#pragma endregion

#pragma region InitKnots
	//Init knots vectors
	vector<float> knots_u;
	vector<float> knots_v;
	uniform_open_knots_vector(knots_u, controlpoint.size(), 3);
	uniform_open_knots_vector(knots_v, controlpoint[0].size(), 3);
#pragma endregion

	BsplineSurface surface_b_spline = BsplineSurface(3, controlpoint, knots_u, knots_v);
#pragma endregion

	auto sur_renderer = SurfaceRender();
	auto sur_mesh = SurfaceMesh();

	surface_b_spline.SetMesh(sur_mesh);
	sur_renderer.LoadSurface(sur_mesh);
#pragma endregion

#pragma region WindowRun

	//Init shader
	Shader m_shader = Shader("../shader/vertex1.vert", "../shader/fragment1.frag");

	//Init box texture
	GLuint texture1 = Load_box_texture("../resources/checkerBox.png");
	//Init camera
	Camera m_cam(window);

	sur_renderer.render_mode = 0;

	//float refresh_mesh_timer = 0;

	//Window Loop
	do
	{
		RefreshFrame();
		//refresh_mesh_timer += delta_time;
		//if (refresh_mesh_timer > 0.5f)
		//{
		//	surface_b_spline.SetCurve(sur_mesh);
		//	sur_renderer.LoadCurve(sur_mesh);
		//	refresh_mesh_timer = 0;
		//}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#pragma region Input
		m_cam.RefreshMatrixByInput();
#pragma endregion

		//Refresh MVP
		mat4 P = m_cam.GetProjectionMatrix();
		mat4 V = m_cam.GetViewMatrix();
		mat4 M = mat4(1.0f);
		mat4 MVP = P * V * M;//Get MVP matrix from camera

		//use!!
		m_shader.use();

		//Assign uniform variables
		m_shader.setMat4("MVP", MVP);
		m_shader.setMat4("M", M);
		m_shader.setVec3("e_W", m_cam.position);//camera postion in world space
		m_shader.setVec3("l_W", 2, 2, 5);//light postion in world space

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		m_shader.setInt("texture1", 0);

		//Draw
		sur_renderer.Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

	glfwTerminate();
#pragma endregion

	return 0;
}