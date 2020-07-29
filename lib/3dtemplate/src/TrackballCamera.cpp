#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include "TrackballCamera.h"

static const float inf = std::numeric_limits<float>::infinity();


Camera::Camera(float sensivity) :
        prevX(inf),
        prevY(inf),
        cameraCenter(0, 0, 1),
        cameraUp(0, 1, 0),
        phi(0),
        theta(0),
        sensivity(sensivity)
{}

void Camera::update(float x, float y) {

    x *= sensivity;
    y *= sensivity;

    if ((prevX != inf && prevY != inf) && (x != prevX || y != prevY)) {

        float dx = x - prevX;
        float dy = y - prevY;

        phi += dx;
        theta = glm::clamp(theta + dy, -89.f, 89.f);
    }

    prevX = x;
    prevY = y;
}

glm::mat4 Camera::getTransformMatrix() {
    glm::vec3 direction;
    direction.z = std::cos(glm::radians(phi)) * std::cos(glm::radians(theta));
    direction.x = -std::sin(glm::radians(phi)) * std::cos(glm::radians(theta));
    direction.y = std::sin(glm::radians(theta));
    return glm::lookAt(cameraCenter, cameraCenter - direction, cameraUp);
}

void Camera::translate(float dx, float dy, float dz) {
    glm::mat3 rotation(getTransformMatrix());
    cameraCenter += glm::inverse(rotation) * glm::vec3(dx, dy, dz);
}

void Camera::reset() {
    *this = Camera(sensivity);
}

void Camera::stopMotion() {
    prevX = prevY = inf;
}

Trackball::Trackball(float sensivity)
    : prevX(inf)
    , prevY(inf)
    , rotation(1, 0, 0, 0)
    , sensivity(sensivity)
{}

void Trackball::update(float x, float y, const glm::mat4 &view, const glm::mat4 &projection) {
    x *= sensivity;
    y *= sensivity;

    y *= -1;

    if ((prevX != inf && prevY != inf) && (x != prevX || y != prevY)) {

        glm::vec4 prev(prevX, prevY, -1, 0);
        glm::vec4 cur(x, y, -1, 0);

        prev = glm::inverse(view) * glm::inverse(projection) * prev;
        cur = glm::inverse(view) * glm::inverse(projection) * cur;

        glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(prev), glm::vec3(cur)));

        auto angle = glm::distance<float, glm::precision::highp>(glm::vec2(x, y), glm::vec2(prevX, prevY));

        glm::quat delta = glm::angleAxis(glm::radians(angle), axis);
        rotation = glm::normalize(delta * rotation);
    }

    prevX = x;
    prevY = y;
}

void Trackball::stopMotion() {
    prevX = prevY = inf;
}

glm::mat4 Trackball::getTransformMatrix() {
    return glm::mat4_cast(rotation);
}

void Trackball::reset() {
    *this = Trackball(sensivity);
}
