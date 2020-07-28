#include "PositioningRenderer.h"
#include <glUtils.h>

PositioningRenderer::PositioningRenderer(const std::shared_ptr<Scene> & scene, RenderOptions renderOptions) :
        scene(scene),
        renderOptions(renderOptions),
        mainProgram(getProgram("mask_maker/shaders/view_main_vertex.glsl", "mask_maker/shaders/view_main_fragment.glsl")),
        imageData(renderOptions.viewPortOptions.width * renderOptions.viewPortOptions.height * 3)
{
    initObjects(mainProgram, scene->model);

    mainLocations["uMMatrix"]      = glGetUniformLocation(mainProgram, "uMMatrix");
    mainLocations["uVMatrix"]      = glGetUniformLocation(mainProgram, "uVMatrix");
    mainLocations["uPMatrix"]      = glGetUniformLocation(mainProgram, "uPMatrix");
    mainLocations["uColorSampler"] = glGetUniformLocation(mainProgram, "uColorSampler");
}

void PositioningRenderer::render(const VPHandler & handler)
{
    glViewport(
            renderOptions.viewPortOptions.x,
            renderOptions.viewPortOptions.y,
            renderOptions.viewPortOptions.width,
            renderOptions.viewPortOptions.height
    );
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(renderOptions.clearBits);
    glUseProgram(mainProgram);

    Model & m = scene->model;
    glBindVertexArray(m.vaos[mainProgram]);

    glUniformMatrix4fv(mainLocations["uMMatrix"],      1, GL_FALSE, glm::value_ptr(scene->trackball.getModelMatrix()));
    glUniformMatrix4fv(mainLocations["uVMatrix"],      1, GL_FALSE, glm::value_ptr(handler.getVMatrix()));
    glUniformMatrix4fv(mainLocations["uPMatrix"],      1, GL_FALSE, glm::value_ptr(handler.getPMatrix()));

    glUniform1i(mainLocations["uColorSampler"], 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.texture.index);

    glDrawArrays(GL_TRIANGLES, 0, m.vertices->numItems);
}

const std::vector<unsigned char> &PositioningRenderer::getFrontBuffer()
{
    extractAttachmentUnsignedChar(imageData.data(), renderOptions.viewPortOptions.width, renderOptions.viewPortOptions.height, 0, GL_FRONT_LEFT);
    return imageData;
}
