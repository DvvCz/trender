#version 450 core

struct Light {
    vec3 position;
    vec3 color;
    float radius;
};

struct Texture {
    vec2 uvScale;
    vec2 uvOffset;

    /// -1 if no texture
    int index;

    /// Rotation in radians
    float uvRotation;
};

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in mat3 fragTBN;

layout(location = 0) uniform mat4x4 projMatrix;
layout(location = 1) uniform mat4x4 viewMatrix;
layout(location = 2) uniform mat4x4 modelMatrix;

layout(location = 3) uniform sampler2DArray textureList;
layout(location = 4) uniform vec3 cameraPos;

#define MAX_LIGHTS 40

layout(std140, binding = 0) uniform LightsArray {
    uint lightCount;
    Light lights[MAX_LIGHTS];
};

layout(std140, binding = 1) uniform Material3D {
    vec3 materialAmbient;
    float materialShininess;
    vec3 materialDiffuse;
    float materialDissolve;
    vec3 materialSpecular;
    float emissiveStrength;
    vec3 materialEmissive;
    float _padding;
    Texture diffuseTexture;
    Texture normalTexture;
    Texture emissiveTexture;
};

out vec4 outColor;

// Function to apply UV transformations
vec2 transformUV(vec2 uv, Texture tex) {
    vec2 transformedUV = uv;

    transformedUV *= tex.uvScale;

    if (tex.uvRotation != 0.0) {
        float cosAngle = cos(tex.uvRotation);
        float sinAngle = sin(tex.uvRotation);
        mat2 rotationMatrix = mat2(cosAngle, -sinAngle, sinAngle, cosAngle);
        transformedUV = rotationMatrix * transformedUV;
    }

    transformedUV += tex.uvOffset;

    return transformedUV;
}

void main() {
    vec3 fragToCameraDir = normalize(cameraPos - fragPos);

    vec3 baseColor = vec3(1.0);
    if (diffuseTexture.index >= 0) {
        vec2 diffuseUV = transformUV(fragUV, diffuseTexture);
        baseColor = texture(textureList, vec3(diffuseUV, float(diffuseTexture.index))).rgb;
    }

    vec3 normal = fragNormal;
    if (normalTexture.index >= 0) {
        vec2 normalUV = transformUV(fragUV, normalTexture);
        vec3 normalMap = texture(textureList, vec3(normalUV, float(normalTexture.index))).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0); // [0,1] -> [-1,1]
        normal = normalize(fragTBN * normalMap); // tangent space to world space
    }

    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (uint i = 0; i < lightCount; i++) {
        vec3 lightToFrag = fragPos - lights[i].position;
        vec3 lightToFragDir = normalize(lightToFrag);
        vec3 fragToLightDir = -lightToFragDir;

        // Diffuse calculation
        float diff = max(dot(normal, fragToLightDir), 0.0);

        // Specular calculation - only calculate if light is hitting the front face
        float spec = 0.0;
        if (diff > 0.0) {
            vec3 reflectDir = reflect(lightToFragDir, normal);
            spec = pow(max(dot(fragToCameraDir, reflectDir), 0.0), materialShininess);
        }

        // Avoid extreme light at very close distances
        float distToLight = length(lightToFrag);
        float distAttenuation = 1.0 / (1.0 + 0.09 * distToLight + 0.032 * distToLight * distToLight);

        diffuse += diff * lights[i].color * distAttenuation;
        specular += spec * lights[i].color * distAttenuation;
    }

    vec3 emissive = materialEmissive * emissiveStrength;
    if (emissiveTexture.index >= 0) {
        vec2 emissiveUV = transformUV(fragUV, emissiveTexture);
        vec3 emissiveTextureSample = texture(textureList, vec3(emissiveUV, float(emissiveTexture.index))).rgb;
        emissive *= emissiveTextureSample;
    }

    vec3 ambientPart = materialAmbient * baseColor;
    vec3 diffusePart = materialDiffuse * baseColor * diffuse;
    vec3 specularPart = materialSpecular * specular;

    vec3 resultColor = ambientPart + diffusePart + specularPart + emissive;
    outColor = vec4(resultColor, materialDissolve);
}
