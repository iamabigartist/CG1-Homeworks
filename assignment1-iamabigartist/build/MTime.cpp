#include <GLFW/glfw3.h>
#include "MTime.h"

double delta_time;
void RefreshFrame()
{
	static double last_time = glfwGetTime();
	delta_time = glfwGetTime() - last_time;
	last_time = glfwGetTime();
}