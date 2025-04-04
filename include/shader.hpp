#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;
    
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setInt(const std::string& name, const int value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;

private:
    void checkCompileErrors(unsigned int shader, const std::string& type);
};