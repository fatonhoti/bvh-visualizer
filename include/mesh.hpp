#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

#include "shader.hpp"
#include "triangle.hpp"

class Mesh {
public:
    explicit Mesh(const std::string& filename);
    void draw(const glm::mat4& viewProj, const glm::vec3& viewPos) const;
    const std::vector<Triangle>& getTriangles() const { return triangles; }

private:
    Shader shader;
    std::vector<Triangle> triangles;
    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> normals;
    unsigned int VAO, VBO;
    void setupMesh();
};