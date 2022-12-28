#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Skybox.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLFWwindow *initializeGLFW(unsigned int windowWidth, unsigned int windowHeight);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH/2.0f, lastY = SCR_HEIGHT/2.0f;

float dt = 0.0f;
float lastFrame = 0.0f;

// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    GLFWwindow *window = initializeGLFW(SCR_WIDTH, SCR_HEIGHT);
    if (window == NULL)
    {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    Shader shader("../shaders/model_loading.vs", "../shaders/model_loading.fs");
    Shader skyboxShader("../shaders/skybox.vs", "../shaders/skybox.fs");

    // Model curModel("../res/models/backpack/backpack.obj");
    Model curModel("../res/mars/mars.obj");
    // Model curModel("../res/models/cube/untitled.obj");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getMatrixView();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        curModel.draw(shader);

        skyboxShader.use();
        
        // view/projection transformations
        glm::mat4 sbProjection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 sbView = glm::mat4(glm::mat3(camera.getMatrixView()));
        skyboxShader.setMat4("projection", sbProjection);
        skyboxShader.setMat4("view", sbView);

        skybox.draw(skyboxShader);

        // glBindVertexArray(sbVAO);
        // glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // glDepthFunc(GL_LESS);

        // lightingShader.use();
        // lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        // float g = (sin(currentFrame) + 1.0f)/4.0f + 0.5f;
        // lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        // glm::mat4 model = glm::mat4(1.0f);
        // lightingShader.setMat4("model", model);

        // lightingShader.setMat4("view", camera.getMatrixView());

        // glm::mat4 projection;
        // projection = glm::perspective(glm::radians(camera.zoom), 1600.0f / 900.0f, 0.1f, 100.0f);
        // lightingShader.setMat4("projection", projection);

        // // lightPos = glm::vec3(2.0*sin(currentFrame), 2.0*cos(currentFrame), 0.0f);
        // lightingShader.setVec3("viewPos", camera.position);

        // lightingShader.setVec3("light.position", lightPos);
        // glm::vec3 lightColor;
        // lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
        // lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
        // lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
        // glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
        // glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
        // lightingShader.setVec3("light.ambient", ambientColor);
        // lightingShader.setVec3("light.diffuse", diffuseColor);
        // lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("material.ambient", 0.5f, 0.5f, 0.5f);
        // lightingShader.setFloat("material.shininess", 32.0f);

        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // lightCubeShader.use();
        // lightCubeShader.setMat4("projection", projection);
        // model = glm::translate(model, lightPos);
        // model = glm::scale(model, glm::vec3(0.2f));
        // lightCubeShader.setMat4("model", model);
        // lightCubeShader.setMat4("view", camera.getMatrixView());

        // glBindVertexArray(lightVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, dt);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, dt);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, dt);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, dt);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(UP, dt);
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.processKeyboard(DOWN, dt);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float)yoffset);
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