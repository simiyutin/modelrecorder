#pragma once

#include "glCommon.h"
#include "Model.h"
#include <vector>
#include <glm/vec3.hpp>

struct Scene {
    Scene(const glm::vec3 &lightDir, const Model &model);
    glm::vec3 lightDir;
    Model model;
};