#pragma once


#include <glm/glm.hpp>

struct Camera {
    explicit Camera(float sensivity);

    void update(float x, float y);
    void stopMotion();
    void translate(float dx, float dy, float dz);
    glm::mat4 getTransformMatrix();
    void reset();

private:

    float prevX;
    float prevY;

    glm::vec3 cameraCenter;
    glm::vec3 cameraUp;
    float phi;
    float theta;

    float sensivity;
};


struct Trackball {
    explicit Trackball(float sensivity);

    void update(float x, float y, const glm::mat4 &view, const glm::mat4 &projection);
    void stopMotion();
    glm::mat4 getTransformMatrix();
    void reset();

private:

    float prevX;
    float prevY;

    glm::quat rotation;

    float sensivity;
};



