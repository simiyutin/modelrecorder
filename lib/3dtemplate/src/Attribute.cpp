#include "Attribute.h"

Attribute::Attribute(const std::vector<float> & data, int itemSize, const char * attribName) :
        data(data),
        itemSize(itemSize),
        numItems(static_cast<int>(this->data.size() / itemSize)),
        attribName(attribName),
        vbo(0)
{}