#include "Camera.hpp"

namespace gps {
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFront = glm::normalize(cameraTarget - cameraPosition);
        this->yaw = -90.0f; // Default yaw: looking down the negative Z-axis
        this->pitch = 0.0f; // No initial tilt
    }

    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUpDirection);
    }

    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD)
            cameraPosition += speed * cameraFront;
        if (direction == MOVE_BACKWARD)
            cameraPosition -= speed * cameraFront;
        if (direction == MOVE_LEFT)
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUpDirection)) * speed;
        if (direction == MOVE_RIGHT)
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUpDirection)) * speed;
    }

    void Camera::rotate(float pitchDelta, float yawDelta) {
        yaw += yawDelta;
        pitch += pitchDelta;

        // Clamp the pitch to avoid gimbal lock
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        // Recalculate the camera front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}
