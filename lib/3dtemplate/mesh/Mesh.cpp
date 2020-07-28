#include <stdexcept>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <iostream>
#include "../include/Mesh.h"

Mesh::Mesh(const std::vector<Mesh::Face> &faces,
           const std::vector<Mesh::Vertex> &vertices,
           const std::vector<float> &pdata,
           const std::vector<float> &ndata,
           const std::vector<float> &tdata) :
        faces(faces),
        vertices(vertices),
        verticesData(pdata),
        normalsData(ndata),
        texcoordsData(tdata)
{}

std::vector<float> Mesh::expandVertices() const {
    std::vector<float> result;
    for (auto & face : faces) {
        for (index i = 0; i < 3; ++i) {
            index vertexIndex = vertices[face.v[i]].p;
            for (index j = 0; j < 3; ++j) {
                result.push_back(verticesData[vertexIndex * 3 + j]);
            }
        }
    }
    return result;
}

std::vector<float> Mesh::expandNormals() const {
    std::vector<float> result;

    if (normalsData.empty()) return result;

    for (auto & face : faces) {
        for (index i = 0; i < 3; ++i) {
            index normalIndex = vertices[face.v[i]].n;
            for (index j = 0; j < 3; ++j) {
                result.push_back(normalsData[normalIndex * 3 + j]);
            }
        }
    }
    return result;
}

std::vector<float> Mesh::expandTexcoords() const {
    std::vector<float> result;
    for (auto & face : faces) {
        for (index i = 0; i < 3; ++i) {
            index texCoordIndex = vertices[face.v[i]].t;
            float u = texcoordsData[texCoordIndex * 2];
            float v = texcoordsData[texCoordIndex * 2 + 1];
            if (face.mat < 0) {
                u = v = 0;
            }
            result.push_back(u);
            result.push_back(v);
        }
    }
    return result;
}


Mesh::Vertex::Vertex(Mesh::index p, Mesh::index n, Mesh::index t) : p(p), n(n), t(t) {}


void Mesh::Builder::addVertex(index p, index n, index t) {
    vertices.emplace_back(p, n, t);
}

void Mesh::Builder::addVertex(const Mesh::Vertex &v) {
    vertices.push_back(v);
}

void Mesh::Builder::addFace(index mat) {
    if (vertices.size() < 3) {
        throw std::runtime_error("cannot add face");
    }
    index i = static_cast<index>(vertices.size() - 3);
    Face face;
    face.v = {i, i + 1, i + 2};
    face.mat = mat;
    faces.push_back(face);
}

void Mesh::Builder::addVertexData(const std::vector<float> &data) {
    verticesData = data;
}

void Mesh::Builder::addNormalsData(const std::vector<float> &data) {
    normalsData = data;
}

void Mesh::Builder::addTexcoordsData(const std::vector<float> &data) {
    texcoordsData = data;
}

Mesh Mesh::Builder::build() {
    computeTopology();
    return Mesh{faces, vertices, verticesData, normalsData, texcoordsData};
}

void Mesh::Builder::computeTopology() {
    size_t nfaces = faces.size();
    size_t nedges = nfaces * 3;

    std::vector<HalfEdge> edges;
    for (index i = 0; i < faces.size(); ++i) {
        index v0 = vertices[faces[i].v[0]].p;
        index v1 = vertices[faces[i].v[1]].p;
        index v2 = vertices[faces[i].v[2]].p;
        edges.emplace_back(v0, v1, i, 0);
        edges.emplace_back(v1, v2, i, 1);
        edges.emplace_back(v2, v0, i, 2);
    }

    std::sort(edges.begin(), edges.end());

    size_t k0 = 0;
    while (k0 < nedges) {
        size_t k1 = k0 + 1;
        while (k1 < nedges && edges[k1] == edges[k0]) {
            HalfEdge &e0 = edges[k1 - 1];
            HalfEdge &e1 = edges[k1];
            faces[e0.face].FFf[e0.edge] = e1.face;
            faces[e0.face].FFe[e0.edge] = e1.edge;
            k1++;
        }
        HalfEdge &e0 = edges[k1 - 1];
        HalfEdge &e1 = edges[k0];
        faces[e0.face].FFf[e0.edge] = e1.face;
        faces[e0.face].FFe[e0.edge] = e1.edge;
        k0 = k1;
    }
}

Mesh::Builder::HalfEdge::HalfEdge(Mesh::index v0, Mesh::index v1, Mesh::index face, Mesh::index indexInFace)
        : v0(std::min(v0, v1)),
          v1(std::max(v0, v1)),
          face(face),
          edge(indexInFace)
{}

bool Mesh::Builder::HalfEdge::operator<(const Mesh::Builder::HalfEdge &other) const {
    if (v0 < other.v0) return true;
    if (v0 > other.v0) return false;
    return v1 < other.v1;
}

bool Mesh::Builder::HalfEdge::operator==(const Mesh::Builder::HalfEdge &other) const {
    return v0 == other.v0 && v1 == other.v1;
}
