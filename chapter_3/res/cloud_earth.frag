uniform sampler2D colormap;
uniform sampler2D surfaceDataMap;
uniform sampler2D nightColormap;

varying vec3 normal;
varying vec3 viewDir;

struct LightFactors
{
    float diffuse;
    float specular;
};

LightFactors GetLight0Factors()
{
    vec3 viewDirection = normalize(-viewDir);
    vec3 fixedNormal = normalize(normal);
    // Fix lightDirection for both directed and undirected light sources.
    vec3 delta = gl_LightSource[0].position.w * viewDirection;
    vec3 lightDirection = normalize(gl_LightSource[0].position.xyz + delta);

    vec3 reflectDirection = normalize(-reflect(lightDirection, fixedNormal));

    LightFactors result;
    result.diffuse = max(dot(fixedNormal, lightDirection), 0.0);
    float base = max(dot(reflectDirection, viewDirection), 0.0);
    result.specular = pow(base, gl_FrontMaterial.shininess / 4.0);

    result.diffuse = clamp(result.diffuse, 0.0, 1.0);
    result.specular = clamp(result.specular, 0.0, 1.0);

    return result;
}

void main()
{
    LightFactors factors = GetLight0Factors();

    // Get base color by fetching the texture
    vec4 color = texture2D(colormap, gl_TexCoord[0].st);
    // Get night earth color by fetching the texture
    vec4 nightColor = texture2D(nightColormap, gl_TexCoord[0].st);
    // Extract surface data where each channel has own meaning
    vec4 surfaceData = texture2D(surfaceDataMap, gl_TexCoord[0].st);
    // Red channel keeps cloud luminance
    float cloudGray = surfaceData.r;
    // Green channel keeps 1 for water and 0 for earth.
    float waterFactor = surfaceData.g;

    vec4 diffuseColor = mix(color, vec4(factors.diffuse), cloudGray);
    vec4 diffuseIntensity = mix(nightColor, diffuseColor, vec4(factors.diffuse))
            * gl_FrontLightProduct[0].diffuse;

    vec4 specularIntensity = waterFactor * factors.specular
            * gl_FrontLightProduct[0].specular;

    gl_FragColor = diffuseIntensity + specularIntensity;
}
