#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../include/trackball.h"

void Trackball::update(float x, float y) {

    x *= sensivity;
    y *= sensivity;

    if (x == prevX && y == prevY) {
        return;
    }

    glm::vec3 prev(prevX, prevY, 1);
    glm::vec3 cur(x, y, 1);
    glm::vec3 axis = glm::normalize(glm::cross(prev, cur));

    auto angle = glm::distance<float, glm::precision::highp>(glm::vec2(x, y), glm::vec2(prevX, prevY));

    glm::mat4 delta = glm::rotate(glm::mat4(1.0), angle, axis);
    matrix = delta * matrix;

    prevX = x;
    prevY = y;
}

glm::mat4 Trackball::getModelMatrix() {
    return matrix;
}

Trackball::Trackball(float sensivity) :
        prevX(-1),
        prevY(-1),
        matrix(glm::mat4(1.0)),
        sensivity(sensivity)
{}

void Trackball::setPrev(float x, float y) {
    prevX = x * sensivity;
    prevY = y * sensivity;
}

void Trackball::translate(float dx, float dy, float dz) {
    glm::mat4 delta = glm::translate(glm::mat4(1.0), glm::vec3(dx, dy, dz));
    matrix = delta * matrix;
}

glm::vec3 Trackball::applyModelMatrix(const glm::vec3 &v) {
    glm::vec4 tmp = matrix * glm::vec4(v.x, v.y, v.z, 1.0);
    glm::vec3 result(tmp.x, tmp.y, tmp.z);
    return result / tmp.w;
}

void Trackball::reset() {
    *this = Trackball(sensivity);
}
