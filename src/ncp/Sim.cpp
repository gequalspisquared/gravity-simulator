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

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse = true;

float lastX = 800.0f, lastY = 450.0f;

GLFWwindow *initializeGLFW(unsigned int windowWidth, unsigned int windowHeight);

ncp::Sim::Sim(const std::string &windowName, const std::string &config)
{
    init(windowName, config);
}

void ncp::Sim::run()
{
    std::cout << "Running\n";
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
    glfwTerminate();
}

void ncp::Sim::init(const std::string &windowName, const std::string &config)
{
    std::cout << "Initializing...\n";
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
    // Skybox skybox(faces);
    auto skybox = m_entities.addEntity("Skybox");
    skybox->cSkybox = std::make_shared<CSkybox>(faces);

    stbi_set_flip_vertically_on_load(true);

    m_skyboxShader = Shader("../shaders/skybox.vs", "../shaders/skybox.fs");
    m_modelShader  = Shader("../shaders/model_loading.vs", "../shaders/model_loading.fs");

    auto mars = m_entities.addEntity("Gravity");
    mars->cTransform = std::make_shared<CTransform>(
        ncp::Vec3(0.0, 0.0, 0.0),
        ncp::Vec3(0.0, 0.2, 0.0),
        ncp::Vec3(0.0, 0.0, 0.0)
    );
    mars->cModel = std::make_shared<CModel>("../res/mars/mars.obj");
    mars->cGravity = std::make_shared<CGravity>(1.0);

    auto earth = m_entities.addEntity("Gravity");
    earth->cTransform = std::make_shared<CTransform>(
        ncp::Vec3(2.0, 0.0, 0.0),
        ncp::Vec3(0.0, -0.2, 0.0),
        ncp::Vec3(0.0, 0.0, 0.0)
    );
    earth->cModel = std::make_shared<CModel>("../res/earth/earth.obj");
    earth->cGravity = std::make_shared<CGravity>(1.0);
}

void ncp::Sim::setPaused(bool paused)
{
    m_paused = paused;
}

void ncp::Sim::sMovement()
{
    computeForces(m_entities.getEntities("Gravity"));
    // newton method for now
    for (auto &e : m_entities.getEntities("Gravity")) {
        e->cTransform->pos += e->cTransform->vel * dt;
    }
}

void ncp::Sim::sUserInput()
{
    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_running = false;
        // glfwSetWindowShouldClose(m_window, true);
    
    if(glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, dt);
    if(glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, dt);
    if(glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, dt);
    if(glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, dt);
    if(glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(UP, dt);
    if(glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.processKeyboard(DOWN, dt);
}

void ncp::Sim::sRender()
{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_modelShader.use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 1600.0f/900.0f, 0.1f, 100.0f);
    glm::mat4 view = camera.getMatrixView();
    m_modelShader.setMat4("projection", projection);
    m_modelShader.setMat4("view", view);

    // for each planet
    size_t nGravity = m_entities.getEntities("Gravity").size();
    for (size_t i = 0; i < nGravity; i++) {
        glm::mat4 model = glm::mat4(1.0f);

        // render every model
        m_entities.getEntities("Gravity")[i]->cModel->model.draw(m_modelShader);

        ncp::Vec3 entityPos = m_entities.getEntities("Gravity")[i]->cTransform->pos; 
        model = glm::translate(model, glm::vec3(entityPos.x, entityPos.y, entityPos.z));
        m_modelShader.setMat4("model", model);
    }

    m_skyboxShader.use();

    glm::mat4 sbProjection = glm::perspective(glm::radians(camera.zoom), 1600.0f/900.0f, 0.1f, 100.0f);
    glm::mat4 sbView = glm::mat4(glm::mat3(camera.getMatrixView()));
    m_skyboxShader.setMat4("projection", sbProjection);
    m_skyboxShader.setMat4("view", sbView);

    // render skybox
    m_entities.getEntities("Skybox")[0]->cSkybox->skybox.draw(m_skyboxShader);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void ncp::Sim::computeForces(const EntityVec &objects)
{
    size_t nObjects = objects.size();

    for (size_t i = 0; i < nObjects - 1; i++)
        for (size_t j = i + 1; j < nObjects; j++) {
            const ncp::Vec3 p1 = objects[i]->cTransform->pos;
            const ncp::Vec3 p2 = objects[j]->cTransform->pos;
            const ncp::Vec3 r  = p2 - p1;

            const double m1 = objects[i]->cGravity->mass;
            const double m2 = objects[j]->cGravity->mass;

            const ncp::Vec3 a = r * (0.5 * m1 * m2 / (r.mag2() * r.mag()));

            objects[i]->cTransform->vel += a * dt;
            objects[j]->cTransform->vel += a * -dt;
        }
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