#include "mesh.hpp"

#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh::Mesh(const std::string& filename) : shader("shaders/mesh.vert", "shaders/mesh.frag") {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        throw std::runtime_error(warn + err);
    }

    for (const auto& shape : shapes) {
        for (size_t i = 0; i < shape.mesh.indices.size(); i += 3) {
            Triangle tri;

            for (int j = 0; j < 3; j++) {
                tinyobj::index_t idx = shape.mesh.indices[i + j];
                tri.vertices[j] = glm::vec3(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );


                if (idx.normal_index >= 0) {
                    tri.normals[j] = glm::vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                } else {
                    std::cout << "model is missing normal!\n";
                    std::exit(0);
                    tri.normals[j] = glm::vec3(0.0f, 0.0f, 1.0f);
                }
            }

            triangles.push_back(tri);
        }
    }

    printf("# of triangles = %zu\n", triangles.size());

    setupMesh();
}

void Mesh::setupMesh() {
    std::vector<float> vertices;
    for (const auto& triangle : triangles) {
        for (int i = 0; i < 3; ++i) {
            vertices.push_back(triangle.vertices[i].x);
            vertices.push_back(triangle.vertices[i].y);
            vertices.push_back(triangle.vertices[i].z);
            vertices.push_back(triangle.normals[i].x);
            vertices.push_back(triangle.normals[i].y);
            vertices.push_back(triangle.normals[i].z);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw(const glm::mat4 &viewProj, const glm::vec3 &viewPos) const
{
    shader.use();
    shader.setMat4("mvp", viewProj);
    shader.setVec3("viewPos", viewPos);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, triangles.size() * 3);
    glBindVertexArray(0);
}