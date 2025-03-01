#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform bool light1off;

uniform vec3 pointLightPosition;
uniform vec3 pointLightColor;
uniform float constantAtt;
uniform float linearAtt;
uniform float quadraticAtt;


float shininess = 32.0f;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;

float computeFog()
{
 vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
 float fogDensity = 0.05f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

void computePointLight() {

    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);

    vec3 cameraPosEye = vec3(0.0f);
    vec3 normalEye = normalize(fNormal);

    vec3 lightVec = pointLightPosition - fPosEye.xyz;
    float distance = length(lightVec);
    vec3 lightDirN = normalize(lightVec);
    
    float attenuation = 1.0f / (constantAtt + linearAtt * distance + quadraticAtt * distance * distance);

    vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

    ambient += ambientStrength * pointLightColor * attenuation;

    float diff = max(dot(normalEye, lightDirN), 0.0f);
    diffuse += diff * pointLightColor * attenuation;

    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoef = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    specular += specularStrength * specCoef * pointLightColor * attenuation;
}

void main() 
{
    //if(light1off){
        computeDirLight();
    //}

    if(light1off){
        computePointLight();
    }
    
    

    //compute final vertex color
    vec3 color = min((ambient + diffuse) * texture(diffuseTexture, fTexCoords).rgb + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

    vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
    if(colorFromTexture.a < 0.1)
        discard;
    //fColor = colorFromTexture;

    vec4 baseColor = vec4(color, 1.0f) * colorFromTexture;
    fColor = fogColor * (1 - fogFactor) + baseColor * fogFactor;
    //fColor = vec4(color, 1.0f);
    fColor.a = 0.6;
}
