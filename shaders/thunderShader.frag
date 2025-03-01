#version 330 core

out vec4 FragColor;

// Uniform to control flash intensity (0.0 = no flash, 1.0 = full white screen)
uniform float intensity;

void main() {
    // White flash with adjustable intensity
    FragColor = vec4(vec3(intensity), 1.0);
}
