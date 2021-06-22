#ifdef _WIN32
#include <windef.h>
#endif
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <iostream>
#include<vector>
using namespace std;
int main()
{
	std::cout << "Hello World!\n";
}