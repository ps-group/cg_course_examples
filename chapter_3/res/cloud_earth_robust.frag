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

const float SHININESS = 10.0;

uniform LightSource light0;
uniform sampler2D colormap;
uniform sampler2D surfaceDataMap;
uniform sampler2D nightColormap;
uniform mat4 view;

in vec2 fragTextureUV;
in vec3 fragNormal;
in vec3 fragViewDirection;

LightFactors GetLight0Factors()
{
    vec3 viewDirection = normalize(-fragViewDirection);
    vec3 fixedNormal = normalize(fragNormal);
    // Fix lightDirection for both directed and undirected light sources.
    vec3 delta = light0.position.w * viewDirection;
    vec4 lightPosInViewSpace = view * light0.position;
    vec3 lightDirection = normalize(lightPosInViewSpace.xyz + delta);

    vec3 reflectDirection = normalize(-reflect(lightDirection, fixedNormal));

    LightFactors result;
    result.diffuse = max(dot(fixedNormal, lightDirection), 0.0);
    float base = max(dot(reflectDirection, viewDirection), 0.0);
    result.specular = pow(base, SHININESS);

    result.diffuse = clamp(result.diffuse, 0.0, 1.0);
    result.specular = clamp(result.specular, 0.0, 1.0);

    return result;
}

void main()
{
    LightFactors factors = GetLight0Factors();

    // Get base color by fetching the texture
    vec4 color = texture2D(colormap, fragTextureUV.st);
    // Get night earth color by fetching the texture
    vec4 nightColor = texture2D(nightColormap, fragTextureUV.st);
    // Extract surface data where each channel has own meaning
    vec4 surfaceData = texture2D(surfaceDataMap, fragTextureUV.st);
    // Red channel keeps cloud luminance
    float cloudGray = surfaceData.r;
    // Green channel keeps 1 for water and 0 for earth.
    float waterFactor = surfaceData.g;

    vec4 diffuseColor = mix(color, vec4(factors.diffuse), cloudGray);
    vec4 diffuseIntensity = mix(nightColor, diffuseColor,vec4(factors.diffuse))
            * light0.diffuse;

    vec4 specularIntensity = waterFactor * factors.specular
            * light0.specular;

    gl_FragColor = diffuseIntensity + specularIntensity;
}
