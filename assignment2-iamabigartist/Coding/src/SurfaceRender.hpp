#ifndef SURFACERENDER_HPP
#define SURFACERENDER_HPP
#include "SurfaceMesh.hpp"

class SurfaceRender {
public:
	SurfaceRender();
	~SurfaceRender();

	void LoadSurface(const SurfaceMesh& mesh);
	void LoadCurve(const SurfaceMesh& mesh);
	void Draw();

	//0 for line mode, 1 for face mode
	int render_mode = 0;
private:
	unsigned int VBO;
	unsigned int EBO[2];
	unsigned int VAO[3];
	int vertexNum;
	int faceNum, edgeNum;
	//you can add anything to the skelton code, but remember to construct it in the constructor function and destuctor function
};

#endif