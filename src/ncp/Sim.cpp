#include <iostream>
#include <string>

#include "ncp/Sim.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Components.h"
#include "ncp/Vec3.h"

GLFWwindow *initializeGLFW(unsigned int windowWidth, unsigned int windowHeight);

ncp::Sim::Sim(const std::string &windowName, const std::string &config)
{
    init(windowName, config);
}

void ncp::Sim::run()
{
    while (m_running) {
        m_entities.update();

        sMovement();
        sUserInput();
        sRender();

        m_currentFrame++;
    }
}

void ncp::Sim::init(const std::string &windowName, const std::string &config)
{
    m_window = initializeGLFW(1600, 900);
}




GLFWwindow *initializeGLFW(unsigned int windowWidth, unsigned int windowHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }    

    return window;
}