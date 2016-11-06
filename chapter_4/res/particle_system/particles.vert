#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `attribute` renamed to `in` for vertex shader
//  - `varying` renamed to `out` for vertex shader

in vec3 particlePosition;
in vec2 textureUV;

out vec2 fragTextureUV;

uniform mat4 modelView;
uniform mat4 projection;
uniform vec3 sizeScale;

void main(void)
{
    // Particle should be always oriented to viewer,
    //  but particle system size scale still applies.
    vec4 particleCenter = modelView * vec4(particlePosition, 1.0);
    vec2 offset = textureUV * sizeScale.xy;
    vec4 vertexPos = particleCenter + vec4(offset.x, offset.y, 0.0, 0.0);

    // Normalize UV from [-1..1] to [0..1]
    fragTextureUV = 0.5 * textureUV + vec2(0.5);

    gl_Position = projection * vertexPos;
}
