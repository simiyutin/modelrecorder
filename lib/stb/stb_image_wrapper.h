#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include "stb_image_write.h"
#include "stb_image.h"
#include <glm/vec3.hpp>

struct Image {

    Image(int width, int height, int dim, std::vector<unsigned char> data) : width(width), height(height),
                                                                                       dim(dim), data(std::move(data)) {}

    Image(int width, int height, int dim, int def) :width(width), height(height),
                                                    dim(dim), data(width * height * dim, def) {}

    Image(int width, int height, int dim) : Image(width, height, dim, 0) {}

    Image() = default;

    int width;
    int height;
    int dim;
    std::vector<unsigned char> data;

    glm::vec3 get(int i, int j) const {
        int offset = j * width + i;
        glm::vec3 result{0, 0, 0};

        result.r = data[offset * dim + 0];

        if (dim > 1) {
            result.g = data[offset * dim + 1];
        }

        if (dim > 2) {
            result.b = data[offset * dim + 2];
        }

        return result;
    };

    void set(int i, int j, const glm::vec3 & color) {
        if (dim < 3) {
            throw std::runtime_error("image must be at least 3 dim to get rgb value");
        }
        int offset = j * width + i;
        data[offset * dim + 0] = static_cast<unsigned char>(color.r);
        data[offset * dim + 1] = static_cast<unsigned char>(color.g);
        data[offset * dim + 2] = static_cast<unsigned char>(color.b);
    }

    void set(int dstX, int dstY, const Image &src) {
        for (int i = 0; i < src.width; ++i) {
            for (int j = 0; j < src.height; ++j) {
                set(dstX + i, dstY + j, src.get(i, j));
            }
        }
    }

    std::vector<Image> split(int xParts, int yParts) {
        if (xParts == 1 && yParts == 1) {
            return {*this};
        }

        int partWidth = width / xParts;
        int partHeight = height / yParts;
        int numParts = xParts * yParts;
        std::vector<Image> result(numParts, Image(partWidth, partHeight, dim));

        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                int partI = i / partWidth;
                int partJ = j / partHeight;
                int partX = i % partWidth;
                int partY = j % partHeight;
                int partOffset = partJ * xParts + partI;
                result[partOffset].set(partX, partY, get(i, j));
            }
        }

        return result;
    }
};


inline void saveImage(const std::string & filepath, const unsigned char * data, int width, int height, int dim) {
    stbi_write_set_flip_vertically_on_save(1);
    stbi_write_png(
            filepath.c_str(),
            width, height, dim,
            (void *) data,
            sizeof(char) * dim * width);
}


inline void saveImage(const std::string & filepath, const std::vector<float> & data, int width, int height, int dim) {
    std::vector<unsigned char> uBrushMap(data.size());
    for (int i = 0; i < data.size(); ++i) {
        uBrushMap[i] = static_cast<unsigned char>(data[i] * 255);
    }
    saveImage(filepath, uBrushMap.data(), width, height, dim);
}

inline void saveImage(const std::string & filepath, Image image) {
    saveImage(filepath, image.data.data(), image.width, image.height, image.dim);
}

inline Image loadImage(const char * filepath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (!data)
    {
        throw std::runtime_error("ERROR: Failed to load texture");
    }

    if (nrChannels < 3) {
        throw std::runtime_error("only rgb images are allowed");
    }

    Image result;
    if (nrChannels > 3) {
        std::vector<unsigned char> vecData(width * height * 3);
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                int index = j * width + i;
                for (int k = 0; k < 3; ++k) {
                    vecData[index * 3 + k] = data[index * nrChannels + k];
                }
            }
        }
        result = Image{width, height, 3, vecData};
    } else {
        std::vector<unsigned char> vecData(data, data + width * height * 3);
        result = Image{width, height, 3, vecData};
    }

    stbi_image_free(data);

    return result;
}

inline void drawCircle(Image & img, int x, int y, int radius, const glm::vec3 & color) {
    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            int xpos = x + i;
            int ypos = y + j;
            if (xpos < img.width && xpos >=0 && ypos < img.height && ypos >= 0 && (i * i + j * j <= radius * radius)) {
                img.set(xpos, ypos, color);
            }
        }
    }
}

inline void drawLine(Image & img, int x1, int y1, int x2, int y2, int width, const glm::vec3 & color) {
    const bool steep = (std::abs(y2 - y1) > std::abs(x2 - x1));
    if(steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if(x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = static_cast<const float>(std::abs(y2 - y1));

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const auto maxX = (int)x2;

    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep)
        {
            img.set(y,x, color);
        }
        else
        {
            img.set(x,y, color);
        }

        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}
