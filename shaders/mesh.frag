#version 460 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

const vec3 objectColor = vec3(0.75, 0.75, 0.75);

struct Light {
    vec3 color;
    vec3 dir;
};

const Light lights[6] = {
    {vec3(1.0, 1.0, 1.0), vec3( 0.0,  1.0,  0.0)}, // Up (Y+)
    {vec3(1.0, 1.0, 1.0), vec3( 0.0, -1.0,  0.0)}, // Down (Y-)
    {vec3(1.0, 1.0, 1.0), vec3(-1.0,  0.0,  0.0)}, // Left (X-)
    {vec3(1.0, 1.0, 1.0), vec3( 1.0,  0.0,  0.0)}, // Right (X+)
    {vec3(1.0, 1.0, 1.0), vec3( 0.0,  0.0, -1.0)}, // Front (Z-)
    {vec3(1.0, 1.0, 1.0), vec3( 0.0,  0.0,  1.0)}  // Back (Z+)
};

void main() {
    const vec3 norm = normalize(Normal);
    FragColor = vec4(norm*0.5+0.5, 1.0);
    return;

    vec3 diffuse = vec3(0.0);
    for (int i = 0; i < 6; i++) {
        const Light light = lights[i];
        const vec3 d = normalize(-light.dir);
        diffuse += max(dot(norm, d), 0.0) * light.color * 0.5;
    }

    const vec3 ambient = 0.05 * vec3(1.0);
    const vec3 res = (ambient + diffuse) * objectColor;
    FragColor = vec4(res, 1.0);
}