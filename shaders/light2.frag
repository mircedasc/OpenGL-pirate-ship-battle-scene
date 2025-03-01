#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

// Uniforms
uniform vec3 pointLightPosition; // Position of the point light
uniform vec3 pointLightColor;    // Color of the point light
uniform vec3 viewPosition;       // Position of the camera/viewer

uniform sampler2D diffuseTexture;

// Light properties
float constant = 1.0f;  // Constant attenuation
float linear = 0.09f;   // Linear attenuation
float quadratic = 0.032f; // Quadratic attenuation

void main() {
    // Ambient light
    vec3 ambient = 0.1 * pointLightColor;

    // Diffuse light
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(pointLightPosition - fPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * pointLightColor;

    // Specular light
    vec3 viewDir = normalize(viewPosition - fPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * pointLightColor;

    // Attenuation
    float distance = length(pointLightPosition - fPosition);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    // Combine results
    vec3 result = (ambient + diffuse + specular) * texture(diffuseTexture, fTexCoords).rgb;
    result *= attenuation; // Apply attenuation
    fColor = vec4(result, 1.0f);
}
