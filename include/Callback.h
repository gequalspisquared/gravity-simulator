#ifndef CALLBACK_H
#define CALLBACK_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

// extern const unsigned int SCR_WIDTH;
// extern const unsigned int SCR_HEIGHT;


// float dt = 0.0f;
// float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

#endif