#pragma once

#include "Scene.h"
#include "VPHandler.h"

struct Renderer {
    virtual void render(const VPHandler & handler) = 0;
};