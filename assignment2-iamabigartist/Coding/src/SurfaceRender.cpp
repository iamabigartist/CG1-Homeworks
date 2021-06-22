#include "SurfaceRender.hpp"

SurfaceRender::SurfaceRender() :faceNum(0), edgeNum(0), vertexNum(0)
{
	glGenVertexArrays(3, VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(2, EBO);
}

SurfaceRender::~SurfaceRender()
{
	glDeleteBuffers(2, EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(3, VAO);
}

void SurfaceRender::LoadSurface(const SurfaceMesh& mesh)
{
	vertexNum = mesh.v.size();
	faceNum = mesh.n_face;
	edgeNum = mesh.n_edge;

#pragma region Upload Data

	//Vertex
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.v.size() * sizeof(Vertex), &(mesh.v)[0], GL_STATIC_DRAW);

	//edge indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.i_edge.size() * sizeof(unsigned int), &(mesh.i_edge)[0], GL_STATIC_DRAW);

	//face indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.i_face.size() * sizeof(unsigned int), &(mesh.i_face)[0], GL_STATIC_DRAW);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region VAO edge
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	//uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region VAO face
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	//uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region VAO point
	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	//uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion
}

void SurfaceRender::LoadCurve(const SurfaceMesh& mesh)
{
	vertexNum = mesh.v.size();
	faceNum = 0;
	edgeNum = mesh.n_edge;

#pragma region Upload Data

	//Vertex
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.v.size() * sizeof(Vertex), &(mesh.v)[0], GL_STATIC_DRAW);

	//Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region VAO point
	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	//uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(vec3)));

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion
}

void SurfaceRender::Draw()
{
	glBindVertexArray(VAO[render_mode]);
	switch (render_mode)
	{
	case 0:
	{
		glDrawElements(GL_LINES, 2 * edgeNum, GL_UNSIGNED_INT, 0);
		break;
	}
	case 1:
	{
		glDrawElements(GL_TRIANGLES, 3 * faceNum, GL_UNSIGNED_INT, 0);
		break;
	}
	case 2:
	{
		glDrawArrays(GL_POINTS, 0, vertexNum);
		break;
	}
	default:
	{
		break;
	}
	}

	glBindVertexArray(0);
}