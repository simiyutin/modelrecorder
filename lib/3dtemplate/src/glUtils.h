#pragma once

#include "glCommon.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "stb_image.h"
#include "stb_image_write.h"
#include "Model.h"

inline GLuint getShader(const std::string & path) {

    GLuint shader;
    if (path.find("vertex") != std::string::npos) {
        shader = glCreateShader(GL_VERTEX_SHADER);
    } else if (path.find("fragment") != std::string::npos) {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    } else {
        std::cout << "ERROR::INVALID SHADER NAME FORMAT" << std::endl;
        exit(1);
    }

    std::ifstream is(path);
    std::string strSource((std::istreambuf_iterator<char>(is)),
                    std::istreambuf_iterator<char>());
    const GLchar *source = strSource.c_str();

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }

    return shader;

}

inline GLuint getProgram(const std::string & vertexPath, const std::string & fragmentPath) {

    GLuint vertexShader = getShader(vertexPath);
    GLuint fragmentShader = getShader(fragmentPath);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR: PROGRAM LINKING FAILED. CHECK THAT WORKING DIRECTORY IS SET TO PROJECT PATH\n" << infoLog << std::endl;
        exit(1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

inline void initObjects(GLuint program, Model &m) {
    std::shared_ptr<Attribute> attributes[] = {m.normals, m.vertices, m.texcoords};

    glGenVertexArrays(1, &m.vaos[program]);
    glBindVertexArray(m.vaos[program]);

    for (auto attr : attributes)
    {
        if (!attr)
        {
            continue;
        }

        GLint attribIndex = glGetAttribLocation(program, attr->attribName);
        if (attribIndex == -1)
        {
            continue;
        }

        // create buffer and copy data
        // привязывается к Vertex Attribute Object (VAO)
        if (attr->vbo == 0)
        {
            glGenBuffers(1, &attr->vbo);
            glBindBuffer(GL_ARRAY_BUFFER, attr->vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(float) * attr->data.size(),
                         attr->data.data(),
                         GL_STATIC_DRAW);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, attr->vbo);
        }

        // привязывается к Vertex Attribute Object (VAO)
        glVertexAttribPointer(static_cast<GLuint>(attribIndex), attr->itemSize, GL_FLOAT, GL_FALSE, attr->itemSize * sizeof(float), nullptr);
        glEnableVertexAttribArray(static_cast<GLuint>(attribIndex));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);
}

inline Texture loadTexture(const std::string & path) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR: Failed to load texture" << std::endl;
        exit(1);
    }



    std::vector<unsigned char> vecData(data, data + width * height * nrChannels);
    stbi_image_free(data);

    Image img{width, height, nrChannels, vecData};

    return Texture{texture, img};
}

inline Texture makeTexture(const Image & img) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    return Texture{texture, img};
}

inline GLuint getDepthTexture(GLuint width, GLuint height) {
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    return depthMap;
}

inline GLuint getFloatTexture(GLuint width, GLuint height, GLuint type,GLuint channels) {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, channels, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texture;
}

inline GLuint getRGBFloatTexture(GLuint width, GLuint height) {
    return getFloatTexture(width, height, GL_RGB16F, GL_RGB);
}

inline GLuint getRGBAFloatTexture(GLuint width, GLuint height) {
    return getFloatTexture(width, height, GL_RGBA16F, GL_RGBA);
}

inline GLuint getDepthFrameBuffer(GLuint depthTexture) {
    GLuint depthFrameBuffer;
    glGenFramebuffers(1, &depthFrameBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return depthFrameBuffer;
}

inline void saveCurrentFrameBufferAsImage(const char *filepath, GLsizei screenWidth, GLsizei screenHeight) {
    size_t size = (size_t) 3 * screenWidth * screenHeight;
    std::vector<unsigned char> image(size);
    glReadPixels(0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, image.data());
    stbi_write_set_flip_vertically_on_save(1);
    stbi_write_png(filepath, screenWidth, screenHeight, 3, (void *) image.data(), sizeof(char) * 3 * screenWidth);
}

inline void extractAttachmentFloat(float * dst, GLsizei width, GLsizei height, GLuint frameBuffer, GLenum attachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glReadBuffer(attachment);
    glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, dst);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void extractAttachmentFloatPixel(float * dst, GLsizei x, GLsizei y, GLuint frameBuffer, GLenum attachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glReadBuffer(attachment);
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, dst);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void extractAttachmentUnsignedChar(unsigned char * dst, GLsizei width, GLsizei height, GLuint frameBuffer, GLenum attachment) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glReadBuffer(attachment);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, dst);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

inline void saveFrameBufferAttachment(GLsizei width, GLsizei height, GLuint frameBuffer, GLenum attachment) {
    size_t size = (size_t) 3 * width * height;
    std::vector<unsigned char> image(size);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glReadBuffer(attachment);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image.data());
    stbi_write_set_flip_vertically_on_save(1);
    stbi_write_png("data/models/test.png", width, height, 3, (void *) image.data(), sizeof(char) * 3 * width);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template <typename T>
inline void checkLocations(const T & locations, const std::string & locationName) {
    for (auto & p : locations) {
        if (p.second == -1) {
            throw std::runtime_error("uniform  " + p.first + " in " + locationName + " is not presented in shader, or presented, but not used");
        }
    }
}