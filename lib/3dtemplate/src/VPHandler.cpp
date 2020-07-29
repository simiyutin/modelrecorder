#include "VPHandler.h"

glm::mat4 VPHandler::getVMatrix() const {
    return vMatrix;
}

glm::mat4 VPHandler::getPMatrix() const {
    return pMatrix;
}

glm::mat3 VPHandler::getVNMatrix() const {
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(vMatrix)));
    return normalMatrix;
}

VPHandler::VPHandler(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix) :
    vMatrix(viewMatrix),
    pMatrix(projectionMatrix)
{}
