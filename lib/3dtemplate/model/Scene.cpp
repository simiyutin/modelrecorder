#include "../include/Scene.h"

using namespace glm;

Scene::Scene(const glm::vec3 &lightDir, const Model &model) :
        lightDir(lightDir),
        model(model)
{}
