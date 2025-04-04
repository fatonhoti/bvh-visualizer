#include "bvh.hpp"
#include "glad/glad.h"
#include <iostream>

BVH::BVH(const Mesh& mesh, int maxDepth) : shader("shaders/bvh.vert", "shaders/bvh.frag"), maxDepth(maxDepth) {
    triangles = mesh.getTriangles();
    setupBoundingBoxBuffers();
    setupInstanceDataBuffer();
    rebuild(maxDepth);
}

void BVH::rebuild(int depth) {
    nodes.clear();
    maxDepth = depth;
    buildBVH(0, triangles.size(), 0);
    updateInstanceData();
}

int BVH::buildBVH(int start, int end, int depth) {
    Node node;
    node.left = -1;
    node.right = -1;
    node.triangleIndex = -1;
    node.triangleCount = 0;

    glm::vec3 minBounds = glm::vec3(FLT_MAX);
    glm::vec3 maxBounds = glm::vec3(-FLT_MAX);
    for (int i = start; i < end; ++i) {
        const Triangle& triangle = triangles[i];
        for (int j = 0; j < 3; ++j) {
            minBounds = glm::min(minBounds, triangle.vertices[j]);
            maxBounds = glm::max(maxBounds, triangle.vertices[j]);
        }
    }

    node.bbox.min = minBounds;
    node.bbox.max = maxBounds;

    if (depth >= maxDepth || (end - start) <= 10) {
        node.triangleIndex = start;
        node.triangleCount = end - start;
        node.depth = depth;
        nodes.push_back(node);
        return nodes.size() - 1;
    }

    glm::vec3 extent = maxBounds - minBounds;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    float splitPos = (minBounds[axis] + maxBounds[axis]) * 0.5f;
    int mid = start;
    for (int i = start; i < end; ++i) {
        const Triangle& triangle = triangles[i];
        float centroid = (triangle.vertices[0][axis] + triangle.vertices[1][axis] + triangle.vertices[2][axis]) / 3.0f;
        if (centroid < splitPos) {
            std::swap(triangles[i], triangles[mid]);
            ++mid;
        }
    }

    if (mid == start || mid == end) {
        node.triangleIndex = start;
        node.triangleCount = end - start;
        nodes.push_back(node);
        node.depth = depth;
        return nodes.size() - 1;
    }

    node.left = buildBVH(start, mid, depth + 1);
    node.right = buildBVH(mid, end, depth + 1);

    nodes.push_back(node);
    node.depth = depth;
    return nodes.size() - 1;
}

void BVH::setupBoundingBoxBuffers() {
    static constexpr glm::vec3 bboxVertices[] = {
        { 1.0f,  1.0f,  1.0f}, 
        { 1.0f, -1.0f,  1.0f}, 
        {-1.0f, -1.0f,  1.0f}, 
        {-1.0f,  1.0f,  1.0f},
        { 1.0f,  1.0f, -1.0f}, 
        { 1.0f, -1.0f, -1.0f}, 
        {-1.0f, -1.0f, -1.0f}, 
        {-1.0f,  1.0f, -1.0f}
    };

    static constexpr GLuint indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    glGenVertexArrays(1, &bboxVAO);
    glBindVertexArray(bboxVAO);

    glGenBuffers(1, &bboxVBO);
    glGenBuffers(1, &bboxEBO);

    glBindBuffer(GL_ARRAY_BUFFER, bboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bboxVertices), bboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BVH::setupInstanceDataBuffer() {
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 2 * 100000, nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(bboxVAO);

    // Center (vec4)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(1, 1);

    // Scale (vec4)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)sizeof(glm::vec4));
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void BVH::updateInstanceData() {
    std::vector<glm::vec4> instanceData;
    instanceData.reserve(nodes.size() * 2);

    for (const auto& node : nodes) {
        if (node.triangleIndex >= 0) {
            glm::vec4 center(glm::vec3((node.bbox.min + node.bbox.max) * 0.5f), 1.0);
            glm::vec4 scale(glm::vec3((node.bbox.max - node.bbox.min) * 0.5f), 1.0);
            scale.w = node.depth;
            instanceData.push_back(center);
            instanceData.push_back(scale);
        }
    }

    numInstances = instanceData.size() / 2;
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, numInstances * 2 * sizeof(glm::vec4), instanceData.data());
}

void BVH::draw(const glm::mat4& viewProj) {
    shader.use();
    shader.setMat4("pv", viewProj);
    shader.setInt("maxDepth", maxDepth);
    glBindVertexArray(bboxVAO);
    glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, numInstances);
}
