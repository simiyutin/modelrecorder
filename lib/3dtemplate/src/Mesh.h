#pragma once

#include <cstddef>
#include <vector>
#include <array>

struct Mesh {

    using index = int64_t;
    using vec3i = std::array<index, 3>;
    using vec3f = std::array<float, 3>;
    using vec2f = std::array<float, 2>;


    struct Vertex {
        Vertex(index p, index n, index t);
        index p;
        index n;
        index t;
    };

    struct Face {
        vec3i v;
        index mat;
        vec3i FFf;
        vec3i FFe;
    };

    struct Builder {
        Mesh build();
        void addVertex(index p, index n, index t);
        void addVertex(const Vertex & v);
        void addFace(index mat = 0);
        void addVertexData(const std::vector<float> & data);
        void addNormalsData(const std::vector<float> & data);
        void addTexcoordsData(const std::vector<float> & data);

        std::vector<Face> faces;
        std::vector<Vertex> vertices;
        std::vector<float> verticesData;
        std::vector<float> normalsData;
        std::vector<float> texcoordsData;

    private:
        struct HalfEdge {
            HalfEdge(index v0, index v1, index face, index indexInFace);

            inline bool operator<(const HalfEdge & other) const;
            inline bool operator==(const HalfEdge & other) const;

            index v0;
            index v1;
            index face;
            index edge;
        };
        void computeTopology();
    };

    Mesh() = default;

    std::vector<float> expandVertices() const;
    std::vector<float> expandNormals() const;
    std::vector<float> expandTexcoords() const;

    const std::vector<Face> faces;
    const std::vector<Vertex> vertices;

    const std::vector<float> verticesData;
    const std::vector<float> normalsData;
    const std::vector<float> texcoordsData;

private:
    Mesh(const std::vector<Face> &faces, const std::vector<Vertex> &vertices, const std::vector<float> &pdata,
         const std::vector<float> &ndata, const std::vector<float> &tdata);

};


