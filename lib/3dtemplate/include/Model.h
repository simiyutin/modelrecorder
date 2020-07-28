#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "glCommon.h"
#include "Attribute.h"
#include "Texture.h"
#include "Mesh.h"
#include "trackball.h"

struct Model {
    Model(const std::shared_ptr<Attribute> &vertices, const std::shared_ptr<Attribute> &normals,
          const std::shared_ptr<Attribute> &texcoords, const Texture &texture, const Mesh &mesh);

    static Model fromMesh(const Mesh & mesh);

    std::shared_ptr<Attribute> vertices;
    std::shared_ptr<Attribute> normals;
    std::shared_ptr<Attribute> texcoords;

    using programID = GLuint;
    using vaoID = GLuint;
    std::unordered_map<programID, vaoID> vaos;
    Texture texture;
    Mesh mesh;
};

Model loadModelByFileName(const std::string &basePath, const std::string &objFile);

inline Model loadBarro() {
    return loadModelByFileName("data/models/barro/", "barro.obj");
}

inline Model loadKolona() {
    return loadModelByFileName("data/models/kolona/", "kolona.obj");
}

inline Model loadTower() {
    return loadModelByFileName("data/models/eastern_tower/", "eastern_tower.obj");
}

inline Model loadTowerDelit() {
    return loadModelByFileName("data/models/eastern_tower/delit/", "eastern_tower.obj");
}

Model loadQuad();