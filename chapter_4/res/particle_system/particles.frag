#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `varying` renamed to `in` for fragment shader

uniform sampler2D emissiveMap;

in vec2 fragTextureUV;

void main()
{
    // Get material emissive color by fetching the texture
    vec4 matEmissive = texture2D(emissiveMap, fragTextureUV.st);

    gl_FragColor = matEmissive;
}
