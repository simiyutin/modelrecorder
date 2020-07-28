#pragma once


#include <vector>
#include "glCommon.h"
#include "../../stb/stb_image_wrapper.h"

struct Texture {
    Texture(GLuint index, const Image &image);

    Texture();

    GLuint index;
    Image image;
};


