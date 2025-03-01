#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace gps {
    enum MOVE_DIRECTION {
        MOVE_FORWARD,
        MOVE_BACKWARD,
        MOVE_LEFT,
        MOVE_RIGHT
    };

    class Camera {
    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);

        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitchDelta, float yawDelta);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraUpDirection;
        glm::vec3 cameraFront;
        float yaw;
        float pitch;
    };
}

#endif
