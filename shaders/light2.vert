#version 410 core

layout(location = 0) in vec3 inPosition;  // Vertex position
layout(location = 1) in vec3 inNormal;    // Vertex normal
layout(location = 2) in vec2 inTexCoords; // Texture coordinates

out vec3 fPosition;   // World-space position of the fragment
out vec3 fNormal;     // Normal in world space
out vec2 fTexCoords;  // Texture coordinates

uniform mat4 model;      // Model matrix
uniform mat4 view;       // View matrix
uniform mat4 projection; // Projection matrix

void main() {
    // Transform the vertex position to clip space
    gl_Position = projection * view * model * vec4(inPosition, 1.0);

    // Compute the world-space position
    fPosition = vec3(model * vec4(inPosition, 1.0));

    // Compute the world-space normal
    fNormal = mat3(transpose(inverse(model))) * inNormal;

    // Pass texture coordinates to the fragment shader
    fTexCoords = inTexCoords;
}
