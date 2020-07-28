#pragma once

#include <memory>
#include <glCommon.h>
#include "Renderer.h"
#include "RenderOptions.h"
#include <Scene.h>

struct PositioningRenderer : Renderer {
    explicit PositioningRenderer(const std::shared_ptr<Scene> & scene, RenderOptions renderOptions);
    void render(const VPHandler & handelr) override;
    const std::vector<unsigned char> & getFrontBuffer();

private:
    std::shared_ptr<Scene> scene;
    RenderOptions renderOptions;
    GLuint mainProgram;
    std::unordered_map<std::string, GLint> mainLocations;
    std::vector<unsigned char> imageData;
};


