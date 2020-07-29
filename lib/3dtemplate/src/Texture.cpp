#include "Texture.h"


Texture::Texture() = default;

Texture::Texture(GLuint index, const Image &image) : index(index), image(image) {}
