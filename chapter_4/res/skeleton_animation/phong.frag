#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `varying` renamed to `in` for fragment shader

struct LightSource
{
    // (x, y, z, 1) means positioned light.
    // (x, y, z, 0) means directed light.
    vec4 position;
    vec4 diffuse;
    vec4 specular;
};

struct LightFactors
{
    float diffuse;
    float specular;
};

struct Material
{
    vec4 diffuse;
    vec4 specular;
    vec4 emissive;
    float shininess;
};

const float AMBIENT_DIFFUSE = 0.2;

uniform LightSource light0;
uniform Material material;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D emissiveMap;
uniform mat4 view;

in vec2 fragTextureUV;
in vec3 fragNormal;
in vec3 fragPosInViewSpace;

// Returns direction from surface fragment to light source.
// Light can be either directed or undirected.
// TODO: calculate attenuation of directed light.
vec3 GetLight0Direction()
{
    // For spotlight, it's position of the light0 in the view space.
    // For directed light, it's light direction in the view space.
    vec4 lightInViewSpace = view * light0.position;

    // We should return:
    //   - `-lightInViewSpace.xyz` for directed light
    //   - `lightInViewSpace.xyz - fragPosInViewSpace` for spotlight
    // Both results should be normalized,
    // and we avoid `if` to improve perfomance.
    return normalize(
        -lightInViewSpace.xyz
        + light0.position.w
        * (2.0 * lightInViewSpace.xyz - fragPosInViewSpace));
}

LightFactors GetLight0Factors(float shininess)
{
    vec3 normal = normalize(fragNormal);

    // Direction from surface fragment to light source.
    vec3 lightDirection = GetLight0Direction();
    // Reflected ray direction.
    vec3 reflectDirection = normalize(-reflect(lightDirection, normal));
    // Direction from surface fragment to camera (eye).
    vec3 eyeDirection = normalize(-fragPosInViewSpace);

    LightFactors result;
    result.diffuse = max(dot(normal, lightDirection), AMBIENT_DIFFUSE);
    float base = max(dot(reflectDirection, eyeDirection), 0.0);
    result.specular = max(pow(base, shininess), 0.0);

    result.diffuse = clamp(result.diffuse, 0.0, 1.0);
    result.specular = clamp(result.specular, 0.0, 1.0);

    return result;
}

void main()
{
    LightFactors factors = GetLight0Factors(material.shininess);

    // Get material diffuse color by fetching the texture
    //  and adding material diffuse color.
    vec4 matDiffuse = material.diffuse + texture2D(diffuseMap, fragTextureUV.st);
    // Get material specular color by fetching the texture
    //  and adding material diffuse color.
    vec4 matSpecular = material.specular + texture2D(specularMap, fragTextureUV.st);
    // Get material emissive color by fetching the texture
    //  and adding material diffuse color.
    vec4 matEmissive = material.emissive + texture2D(emissiveMap, fragTextureUV.st);

    vec4 diffuseIntensity = matDiffuse * factors.diffuse * light0.diffuse;
    vec4 specularIntensity = matSpecular * factors.specular * light0.specular;

    gl_FragColor = diffuseIntensity + specularIntensity + matEmissive;
}
