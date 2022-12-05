#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

Camera::Camera(glm::vec3 position, 
               glm::vec3 up, 
               float yaw, float pitch)
               : front(glm::vec3(0.0f, 0.0f, -1.0f))
               , movementSpeed(SPEED)
               , mouseSensitivity(SENSITIVITY)
               , zoom(ZOOM)
               , position(position)
               , worldUp(up)
               , yaw(yaw)
               , pitch(pitch)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, 
               float upX,  float upY,  float upZ, 
               float yaw,  float pitch)
               : front(glm::vec3(0.0f, 0.0f, -1.0f))
               , movementSpeed(SPEED)
               , mouseSensitivity(SENSITIVITY)
               , zoom(ZOOM)
               , position(glm::vec3(posX, posY, posZ))
               , worldUp(glm::vec3(upX, upY, upZ))
               , yaw(yaw)
               , pitch(pitch)
{
    updateCameraVectors();
}

glm::mat4 Camera::getMatrixView() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float dt)
{
    float velocity = movementSpeed*dt;
    if (direction == FORWARD)
        position += velocity * front;
    if (direction == BACKWARD)
        position -= velocity * front;
    if (direction == RIGHT)
        position += velocity * right;
    if (direction == LEFT)
        position -= velocity * right;
    if (direction == UP)
        position += velocity * worldUp;
    if (direction == DOWN)
        position -= velocity * worldUp;
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    zoom -= yoffset;
    if (zoom < 1.0f)  zoom = 1.0f;
    if (zoom > 45.0f) zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}