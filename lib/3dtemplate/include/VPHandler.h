#pragma once

#include "glCommon.h"

struct VPHandler {
    VPHandler(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);

    glm::mat4 getVMatrix() const;
    glm::mat4 getPMatrix() const;
    glm::mat3 getVNMatrix() const;

private:
    glm::mat4 vMatrix;
    glm::mat4 pMatrix;
};

inline glm::mat4 getLightVMatrix(const glm::vec3 & lightDir) {
    glm::mat4 result = glm::lookAt(lightDir, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0});
    return result;
}

inline glm::mat4 getLightPMatrix() {
    float near_plane = 0.0f, far_plane = 2.0f;
    glm::mat4 result = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
    return result;
}