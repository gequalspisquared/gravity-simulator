#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ncp/Sim.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Components.h"
#include "ncp/Vec3.h"
#include "Callback.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Skybox.h"
#include "stb_image.h"

double dt = 0.0;
double lastFrame = 0.0;

GLFWwindow *initializeGLFW(unsigned int windowWidth, unsigned int windowHeight);

ncp::Sim::Sim(const std::string &windowName, const std::string &config)
{
    init(windowName, config);
}

void ncp::Sim::run()
{
    while (m_running) {
        double currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        m_entities.update();

        sMovement();
        sUserInput();
        sRender();

        m_currentFrame++;
    }

    glfwSetWindowShouldClose(m_window, true);
}

void ncp::Sim::init(const std::string &windowName, const std::string &config)
{
    m_window = initializeGLFW(1600, 900);
    if (m_window == NULL)
    {
        std::cout << "Failed to initialize GLFW\n";
        // return -1;
        return;
    }
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // skybox
    std::vector<std::string> faces = {
        "../res/skybox/GalaxyTex_PositiveX.png",
        "../res/skybox/GalaxyTex_NegativeX.png",
        "../res/skybox/GalaxyTex_PositiveY.png",
        "../res/skybox/GalaxyTex_NegativeY.png",
        "../res/skybox/GalaxyTex_PositiveZ.png",
        "../res/skybox/GalaxyTex_NegativeZ.png",
    };
    Skybox skybox(faces);

    stbi_set_flip_vertically_on_load(true);

    m_skyboxShader = Shader("../shaders/skybox.vs", "../shaders/skybox.fs");


    m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
}

void ncp::Sim::setPaused(bool paused)
{
    m_paused = paused;
}

void ncp::Sim::sMovement()
{
    // newton method for now
    for (auto &e : m_entities.getEntities()) {
        e->cTransform->pos += e->cTransform->vel * dt;
    }
}

void ncp::Sim::sUserInput()
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_running = false;
        // glfwSetWindowShouldClose(m_window, true);
    
    if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.processKeyboard(FORWARD, dt);
    if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.processKeyboard(BACKWARD, dt);
    if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.processKeyboard(RIGHT, dt);
    if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.processKeyboard(LEFT, dt);
    if(glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera.processKeyboard(UP, dt);
    if(glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_camera.processKeyboard(DOWN, dt);
}

void ncp::Sim::sRender()
{

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