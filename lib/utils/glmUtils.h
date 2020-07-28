#pragma once

#include <glm/detail/precision.hpp>
#include <glm/common.hpp>
#include <glm/glm.hpp>

inline glm::vec3 clamp(const glm::vec3 & vec, float down, float up) {
    return glm::clamp<float, glm::precision::highp>(vec, down, up);
}

inline float clamp(float val, float down, float up) {
    return glm::clamp(val, down, up);
}

inline float dot(const glm::vec3 & v1, const glm::vec3 & v2) {
    return glm::dot<float, glm::precision::highp>(v1, v2);
}
