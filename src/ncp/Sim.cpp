/*
    TODO: Add glowing bodies
    TODO: Make screen size a variable rather than hard code
    TODO: Determine units (eg. "mass = 1" is 1 solar mass)
    TODO: Add camera following functionality
*/

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
void spawnRock(EntityManager &entities);

ncp::Sim::Sim(const std::string &windowName, const std::string &config)
{
    init(windowName, config);
}

void ncp::Sim::run()
{
    std::cout << "Running\n";
    double total_time = 0;
    while (m_running) {
        double currentFrame = glfwGetTime();
        dt = std::min(currentFrame - lastFrame, 0.1);
        lastFrame = currentFrame;
        total_time += dt;

        m_entities.update();

        if (!m_paused) sMovement();
        sUserInput();
        sRender();

        m_currentFrame++;
    }

    std::cout << m_entities.getEntities("Gravity").size() << " objects, " 
              << m_currentFrame / total_time << " fps \n";
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

    // stbi_set_flip_vertically_on_load(true);

    m_skyboxShader = Shader("../res/shaders/skybox.vs", "../res/shaders/skybox.fs");
    m_modelShader  = Shader("../res/shaders/model_loading.vs", "../res/shaders/model_loading.fs");

    auto mars = m_entities.addEntity("Gravity");
    mars->cTransform = std::make_shared<CTransform>(
        ncp::Vec3(0.0, 0.0, 0.0),
        ncp::Vec3(0.0, 0.25, 0.0),
        ncp::Vec3(0.0, 0.0, 0.0)
    );
    mars->cModel = std::make_shared<CModel>("../res/planets/mars/mars.obj", 0.2);
    mars->cGravity = std::make_shared<CGravity>(1.0);

    auto earth = m_entities.addEntity("Gravity");
    earth->cTransform = std::make_shared<CTransform>(
        ncp::Vec3(2.0, 0.0, 0.0),
        ncp::Vec3(0.0, -0.25, 0.0),
        ncp::Vec3(0.0, 0.0, 0.0)
    );
    earth->cModel = std::make_shared<CModel>("../res/planets/earth/earth.obj", 0.2);
    earth->cGravity = std::make_shared<CGravity>(1.0);

    auto venus = m_entities.addEntity("Gravity");
    venus->cTransform = std::make_shared<CTransform>(
        ncp::Vec3(0.0, 0.0, 2.0),
        ncp::Vec3(0.0, 0.0, 0.0),
        ncp::Vec3(0.0, 0.0, 0.0)
    );
    venus->cModel = std::make_shared<CModel>("../res/planets/venus/venus.obj", 0.2);
    venus->cGravity = std::make_shared<CGravity>(1.0);
}

void ncp::Sim::setPaused(bool paused)
{
    m_paused = paused;
}

void ncp::Sim::sMovement()
{
    computeGravityForces(m_entities.getEntities("Gravity"));

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

    if (glfwGetKey(m_window, GLFW_KEY_P) == GLFW_PRESS)
        m_paused = !m_paused;
    
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT == GLFW_PRESS))
        spawnRock(m_entities);
    
    // camera following system
    static bool following = false;
    static unsigned int objectToFollow = 0;
    if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS) {
        if (following && objectToFollow != 0)
            following = !following;
        following = !following;
        objectToFollow = 0;
        if (following) {
            const ncp::Vec3 vec = m_entities.getEntities("Gravity")[objectToFollow]->cTransform->pos;
            const glm::vec3 obj(vec.x, vec.y, vec.z);
            camera.lookAt(obj);
        }
    }
    if (glfwGetKey(m_window, GLFW_KEY_2) == GLFW_PRESS) {
        if (following && objectToFollow != 1)
            following = !following;
        following = !following;
        objectToFollow = 1;
        if (following) {
            const ncp::Vec3 vec = m_entities.getEntities("Gravity")[objectToFollow]->cTransform->pos;
            const glm::vec3 obj(vec.x, vec.y, vec.z);
            camera.lookAt(obj);
        }
    }
    if (following && objectToFollow < m_entities.getEntities("Gravity").size()) {
        const ncp::Vec3 vec = m_entities.getEntities("Gravity")[objectToFollow]->cTransform->vel * dt;
        const glm::vec3 ds(vec.x, vec.y, vec.z);
        camera.position += ds;
    }
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

        ncp::Vec3 entityPos = m_entities.getEntities("Gravity")[i]->cTransform->pos; 
        model = glm::translate(model, glm::vec3(entityPos.x, entityPos.y, entityPos.z));
        const float modelScale = m_entities.getEntities("Gravity")[i]->cModel->scale; 
        model = glm::scale(model, glm::vec3(modelScale, modelScale, modelScale));
        m_modelShader.setMat4("model", model);

        // render every model
        m_entities.getEntities("Gravity")[i]->cModel->model.draw(m_modelShader);
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

void ncp::Sim::computeGravityForces(const EntityVec &objects)
{
    size_t nObjects = objects.size();

    #pragma omp for
    for (size_t i = 0; i < nObjects - 1; i++) {
        for (size_t j = i + 1; j < nObjects; j++) {
            const ncp::Vec3 p1 = objects[i]->cTransform->pos;
            const ncp::Vec3 p2 = objects[j]->cTransform->pos;
            const ncp::Vec3 r  = p2 - p1;
            const double    d  = r.mag();

            const double m1 = objects[i]->cGravity->mass;
            const double m2 = objects[j]->cGravity->mass;

            constexpr double minr  = 0.5;
            constexpr double minr3 = minr * minr * minr;

            const ncp::Vec3 a = r * (0.5 * m1 * m2 / std::max(d*d*d, minr3));

            objects[i]->cTransform->vel += a * dt;
            objects[j]->cTransform->vel += a * -dt;
        }
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

void spawnRock(EntityManager &entities)
{
    auto rock = entities.addEntity("Gravity");
    rock->cTransform = std::make_shared<CTransform>(
        ncp::Vec3(camera.position.x, camera.position.y, camera.position.z),
        ncp::Vec3(camera.front.x, camera.front.y, camera.front.z),
        ncp::Vec3(0.0, 0.0, 0.0)
    );
    rock->cModel = std::make_shared<CModel>("../res/planets/asteroid/asteroid.obj", 0.1);
    rock->cGravity = std::make_shared<CGravity>(0.3);
}