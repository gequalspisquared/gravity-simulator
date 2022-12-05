#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

// default values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right; 
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = YAW, float pitch = PITCH);

    Camera(float posX, float posY, float posZ, 
           float upX,  float upY,  float upZ, 
           float yaw = YAW,  float pitch = PITCH);

    glm::mat4 getMatrixView() const;
    void processKeyboard(CameraMovement direction, float dt);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void processMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

#endif