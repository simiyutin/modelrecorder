#pragma once

// заменит поле Orientation у модели

#include <glm/glm.hpp>

// состояние: предыдущее положение мыши и матрица преобразования
// при обновлении положения мыши считаем дельту преобразования и применяем к текущей матрице.
// как обрабатывать положения мыши дальше чем полусфера - примагничивать к ближайшей точке сферы.
struct Trackball {
    Trackball(float sensivity);

    void update(float x, float y);
    void setPrev(float x, float y);
    void translate(float dx, float dy, float dz);
    glm::mat4 getModelMatrix();
    glm::vec3 applyModelMatrix(const glm::vec3 & v);
    void reset();

private:
    float prevX;
    float prevY;
    glm::mat4 matrix;
    float sensivity;
};


