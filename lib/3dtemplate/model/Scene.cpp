#include "../include/Scene.h"

using namespace glm;

Scene::Scene(const glm::vec3 &lightDir, const Model &model, Trackball & trackball) :
        lightDir(lightDir),
        model(model),
        trackball(trackball)
{}
