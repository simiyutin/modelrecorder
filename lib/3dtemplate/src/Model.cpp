#include <iostream>
#include <utility>
#include <unordered_set>
#include <set>
#include <tiny_obj_loader.h>
#include <contains.h>
#include "Model.h"
#include "glUtils.h"


Model::Model(const std::shared_ptr<Attribute> &vertices, const std::shared_ptr<Attribute> &normals,
             const std::shared_ptr<Attribute> &texcoords, const Texture &texture, const Mesh &mesh) : vertices(
        vertices), normals(normals), texcoords(texcoords), texture(texture), mesh(mesh), matrix(1) {}

Model Model::fromMesh(const Mesh &mesh) {
    std::shared_ptr<Attribute> vertexAttribute = std::make_shared<Attribute>(mesh.expandVertices(), 3, VERTEX_ATTRIBUTE_NAME);
    std::shared_ptr<Attribute> normalsAttribute = std::make_shared<Attribute>(mesh.expandNormals(),   3, NORMAL_ATTRIBUTE_NAME);
    std::shared_ptr<Attribute> texcoordsAttribute = std::make_shared<Attribute>(mesh.expandTexcoords(), 2, TEXCOORD_ATTRIBUTE_NAME);

    return Model{vertexAttribute, normalsAttribute, texcoordsAttribute, Texture{}, mesh};
}

Image concat(const std::unordered_map<Mesh::index, Image> & textures) {
    if (textures.empty()) {
        return Image{};
    }

    std::set<Mesh::index> ids;
    int width = 0;
    int height = 0;
    int dim = 0;
    for (auto & p : textures) {
        ids.insert(p.first);
        width += p.second.width;
        height = p.second.height;
        dim = p.second.dim;
    }

    Image result(width, height, dim);
    int filledX = 0;
    for (Mesh::index id : ids) {
        auto it = textures.find(id);
        result.set(filledX, 0, it->second);
        filledX += it->second.width;
    }

    return result;
}

void scaleTranslate(std::vector<float> & vertices) {
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();

    float maxX = std::numeric_limits<float>::min();
    float maxY = std::numeric_limits<float>::min();
    float maxZ = std::numeric_limits<float>::min();
    for (int i = 0; i < vertices.size() / 3; ++i) {
        minX = std::min(minX, vertices[i * 3]);
        minY = std::min(minY, vertices[i * 3 + 1]);
        minZ = std::min(minZ, vertices[i * 3 + 2]);
        maxX = std::max(maxX, vertices[i * 3]);
        maxY = std::max(maxY, vertices[i * 3 + 1]);
        maxZ = std::max(maxZ, vertices[i * 3 + 2]);
    }

    glm::vec3 center = {maxX + minX, maxY + minY, maxZ + minZ};
    center /= 2;

    float scale = std::max((maxX - minX), std::max((maxY - minY), (maxZ - minZ)));
    scale *= 1.2;

    for (int i = 0; i < vertices.size() / 3; ++i) {
        vertices[i * 3]     = (vertices[i * 3    ] - center.x) / scale;
        vertices[i * 3 + 1] = (vertices[i * 3 + 1] - center.y) / scale;
        vertices[i * 3 + 2] = (vertices[i * 3 + 2] - center.z) / scale;
    }
};

Model loadModelByFileName(const std::string & basePath, const std::string & objFile) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, (basePath + objFile).c_str(), basePath.c_str());

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    Mesh::Builder meshBuilder;
    meshBuilder.addVertexData(attrib.vertices);
    scaleTranslate(meshBuilder.verticesData);
    meshBuilder.addNormalsData(attrib.normals);
    meshBuilder.addTexcoordsData(attrib.texcoords);

    std::unordered_map<Mesh::index, Image> textures;
    std::unordered_map<Mesh::index, Mesh::index> texMatMap;
    std::map<Mesh::index, Mesh::index> sortedMaterials;
    // Loop over shapes
    for (auto &shape : shapes) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            Mesh::index mat = shape.mesh.material_ids[f];
            bool hasTexture = !materials[mat].diffuse_texname.empty();

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                meshBuilder.addVertex(idx.vertex_index, idx.normal_index, idx.texcoord_index);

                if (hasTexture && !contains(texMatMap, idx.texcoord_index)) {
                    texMatMap.insert({idx.texcoord_index, mat});
                } else if (hasTexture && texMatMap.find(idx.texcoord_index)->second != mat) {
                    throw std::runtime_error("assumption violated: texcoords overlap. cant shift texcoords in data array");
                }
            }

            if (!contains(textures, mat) && hasTexture) {
                textures[mat] = loadImage((basePath + materials[mat].diffuse_texname).c_str());
            }
            if (hasTexture) {
                meshBuilder.addFace(mat);
                sortedMaterials[mat] = 0;
            } else {
                meshBuilder.addFace(-1);
            }

            index_offset += fv;
        }
    }

    int order = 0;
    for (auto & p : sortedMaterials) {
        p.second = order++;
    }

    for (int i = 0; i < meshBuilder.texcoordsData.size() / 2; ++i) {
        meshBuilder.texcoordsData[i * 2] = (meshBuilder.texcoordsData[i * 2] + sortedMaterials[texMatMap[i]]) / sortedMaterials.size();
    }

    Mesh mesh = meshBuilder.build();

    std::shared_ptr<Attribute> vertexAttribute = std::make_shared<Attribute>(mesh.expandVertices(), 3, VERTEX_ATTRIBUTE_NAME);
    std::shared_ptr<Attribute> normalsAttribute = std::make_shared<Attribute>(mesh.expandNormals(),   3, NORMAL_ATTRIBUTE_NAME);
    std::shared_ptr<Attribute> texcoordsAttribute = std::make_shared<Attribute>(mesh.expandTexcoords(), 2, TEXCOORD_ATTRIBUTE_NAME);

    Texture concatTex = makeTexture(concat(textures));

    return Model{vertexAttribute, normalsAttribute, texcoordsAttribute, concatTex, mesh};
}

Model loadQuad() {
    std::vector<float> verticesData = {
            // first triangle
            1.f,  1.f, 0.f,  // top right
            1.f,  0.f, 0.f,  // bottom right
            0.f,  1.f, 0.f,  // top left
            // second triangle
            1.f,  0.f, 0.f,  // bottom right
            0.f,  0.f, 0.f,  // bottom left
            0.f,  1.f, 0.f   // top left
    };

    std::vector<float> texcoordsData = {
            // first triangle
            1.f,  1.f,  // top right
            1.f,  0.f,  // bottom right
            0.f,  1.f,  // top left
            // second triangle
            1.f,  0.f,  // bottom right
            0.f,  0.f,  // bottom left
            0.f,  1.f,  // top left
    };

    std::shared_ptr<Attribute> vertexAttribute = std::make_shared<Attribute>(verticesData, 3, VERTEX_ATTRIBUTE_NAME);
    std::shared_ptr<Attribute> texcoordsAttribute = std::make_shared<Attribute>(texcoordsData, 2, TEXCOORD_ATTRIBUTE_NAME);

    return Model(vertexAttribute, nullptr, texcoordsAttribute, Texture{}, Mesh{});
}
