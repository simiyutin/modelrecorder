#pragma once

#include "glCommon.h"

struct RenderOptions {
    struct {
        int x;
        int y;
        int width;
        int height;
    } viewPortOptions;

    GLuint clearBits;
};