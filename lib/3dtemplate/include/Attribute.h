#pragma once

#include <vector>

const char * const VERTEX_ATTRIBUTE_NAME   = "aPosition";
const char * const NORMAL_ATTRIBUTE_NAME   = "aNormal";
const char * const TEXCOORD_ATTRIBUTE_NAME = "aTexcoord";


struct Attribute {
    Attribute(const std::vector<float> & data, int itemSize, const char * attribName);

    std::vector<float> data;
    int itemSize;
    int numItems;
    const char * attribName;
    unsigned int vbo;
};
