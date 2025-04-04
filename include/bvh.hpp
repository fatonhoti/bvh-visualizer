#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include "shader.hpp"

struct BVH {
    struct AABB {
        glm::vec3 min, max;
    };

    struct Node {
        AABB bbox;
        int left, right;
        int triangleIndex;
        int triangleCount;
        int depth;
    };

    explicit BVH(const Mesh& mesh, int maxDepth = 1);
    void rebuild(int depth);
    void draw(const glm::mat4& viewProj);

    std::vector<Triangle> triangles;
    Shader shader;
    int maxDepth;
    std::vector<Node> nodes;
    unsigned int bboxVAO, bboxVBO, bboxEBO;
    unsigned int instanceVBO;
    int numInstances = 0;

    void setupBoundingBoxBuffers();
    void updateInstanceData();
    void setupInstanceDataBuffer();
    int buildBVH(int start, int end, int depth);
    void drawAABB(const AABB& bbox, const glm::mat4& viewProj, int depth);
};