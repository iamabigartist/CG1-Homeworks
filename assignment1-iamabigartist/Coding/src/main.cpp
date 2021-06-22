#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <windef.h>
#endif
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "Camera.h"
#include "MTime.h"
using namespace std;
using namespace glm;

extern double delta_time;

void CheckShaderCompileError(GLuint shader, string shader_type)
{
	GLint pass; GLchar info_log[512];
	if (shader_type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &pass);
		if (!pass)
		{
			glGetShaderInfoLog(shader, 1024, NULL, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << shader_type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_LINK_STATUS, &pass);
		if (!pass)
		{
			glGetProgramInfoLog(shader, 1024, NULL, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << shader_type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void reshape(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void LoadMeshIntoBuffer(const char* path,
	vector<vec3>& v,
	vector<vec3>& vn)
{
	vector<vec3> ori_v;
	vector<vec3> ori_vn;
	vector<GLint> indices;

#pragma region MeshLoading
	printf("Loading OBJ file %s...\n", path);
	FILE* mesh_file = fopen(path, "r"); if (mesh_file == nullptr) { throw "Can not open the mesh file."; }
	GLint num_v, num_f;
	fscanf(mesh_file, "%d %d\n", &num_v, &num_f);

	vec3 cur_v; vec3 cur_vn;
	GLint cur_f1, cur_f2, cur_f3;

	for (GLint i = 0; i < num_v; i++)
	{
		//vec3 cur_v;
		fscanf(mesh_file, "%f %f %f\n", &cur_v.x, &cur_v.y, &cur_v.z);
		ori_v.push_back(cur_v);
	}
	for (GLint i = 0; i < num_v; i++)
	{
		//vec3 cur_vn;
		fscanf(mesh_file, "%f %f %f\n", &cur_vn.x, &cur_vn.y, &cur_vn.z);
		ori_vn.push_back(cur_vn);
	}
	for (GLint i = 0; i < num_f; i++)
	{
		//GLint cur_f1, cur_f2, cur_f3;
		fscanf(mesh_file, "%d %d %d\n", &cur_f1, &cur_f2, &cur_f3);
		indices.push_back(cur_f1); indices.push_back(cur_f2); indices.push_back(cur_f3);
	}
	fclose(mesh_file);

	for (GLint i = 0; i < 3 * num_f; i++)
	{
		v.push_back(ori_v[indices[i] - 1]);
		vn.push_back(ori_vn[indices[i] - 1]);
	}

	//for (GLint i = 0; i < num_v; i++)
	//{
	//	v.push_back(ori_v[i]);
	//	vn.push_back(ori_vn[i]);
	//}

#pragma endregion
}

void initShaderProgram(GLuint& ID, const char* vertex_path, const char* fragment_path)
{
	ifstream v_file, f_file;
	stringstream v_stream, f_stream;
	string v_str, f_str;

	try
	{
		v_file.open(vertex_path); f_file.open(fragment_path);
		v_stream << v_file.rdbuf(); f_stream << f_file.rdbuf();
		v_file.close(); f_file.close();
		v_str = v_stream.str(); f_str = f_stream.str();
	}
	catch (ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* v_char = v_str.c_str();
	const char* f_char = f_str.c_str();
	GLuint vertex_shader, fragment_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER); fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex_shader, 1, &v_char, NULL); glShaderSource(fragment_shader, 1, &f_char, NULL);
	glCompileShader(vertex_shader); CheckShaderCompileError(vertex_shader, "VERTEX");
	glCompileShader(fragment_shader); CheckShaderCompileError(fragment_shader, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertex_shader); glAttachShader(ID, fragment_shader);
	glLinkProgram(ID); CheckShaderCompileError(ID, "PROGRAM");
	glDeleteShader(vertex_shader); glDeleteShader(fragment_shader);
}

int main() {
#pragma region InitWindowAndEnvironment
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "HW1", NULL, NULL);
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

#pragma region WindowRun

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Load shaders
	GLuint shader_program_ID;
	initShaderProgram(shader_program_ID, "shader/vertex1.vert", "shader/fragment1.frag");

	//get uniform variables IDs
	GLuint MVP_ID = glGetUniformLocation(shader_program_ID, "MVP");
	GLuint M_ID = glGetUniformLocation(shader_program_ID, "M");
	GLuint e_W_ID = glGetUniformLocation(shader_program_ID, "e_W");
	GLuint l_W_ID = glGetUniformLocation(shader_program_ID, "l_W");

	//Load Mesh and Init Buffers
	GLuint VBO_v, VBO_vn;
	vector<vec3> v, vn;
	//bunny_simlified.object pumpkin_simlified.object fourface.object
	LoadMeshIntoBuffer("obj/bunny_simlified.object", v, vn);

	glGenBuffers(1, &VBO_v); glBindBuffer(GL_ARRAY_BUFFER, VBO_v);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(vec3), &v[0], GL_STATIC_DRAW);
	glGenBuffers(1, &VBO_vn); glBindBuffer(GL_ARRAY_BUFFER, VBO_vn);
	glBufferData(GL_ARRAY_BUFFER, vn.size() * sizeof(vec3), &vn[0], GL_STATIC_DRAW);
	//glGenBuffers(1, &EBO); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * num_f * sizeof(GLint), &f[0], GL_STATIC_DRAW);

	//Init camera
	Camera m_cam(window);

	//Window Loop
	do
	{
		RefreshFrame();//ˢ��ÿ֡��ʱ��

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program_ID);

		//Refresh MVP
		m_cam.RefreshMatrixByInput();
		mat4 P = m_cam.GetProjectionMatrix();
		mat4 V = m_cam.GetViewMatrix();
		mat4 M = mat4(1.0f);
		mat4 MVP = P * V * M;//Get MVP matrix from camera

		//Assign uniform variables
		glUniformMatrix4fv(MVP_ID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(M_ID, 1, GL_FALSE, &M[0][0]);
		glUniform3fv(e_W_ID, 1, &(m_cam.position[0]));//camera postion in world space
		glUniform3f(l_W_ID, 5 * sin(glfwGetTime() / 10), 2, 5 * cos(glfwGetTime() / 10));//light postion in world space

		//Send VBOs into shader
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_v);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_vn);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//Draw
		//glDrawElements(GL_TRIANGLES, v.size(), GL_INT, NULL);
		glDrawArrays(GL_TRIANGLES, 0, v.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

	// Cleanup
	glDeleteBuffers(1, &VBO_v);
	glDeleteBuffers(1, &VBO_vn);
	glDeleteProgram(shader_program_ID);
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
#pragma endregion

	return 0;
}