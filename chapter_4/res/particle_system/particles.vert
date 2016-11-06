#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `attribute` renamed to `in` for vertex shader
//  - `varying` renamed to `out` for vertex shader

in vec3 particlePosition;
in vec2 textureUV;

out vec2 fragTextureUV;

uniform mat4 modelView;
uniform mat4 projection;

void main(void)
{
    // Modified transformation from object local coords to camera
    //  which does translate and scale, but doesn't rotate.
    mat4 billboardModelView = modelView;
    billboardModelView[0][1] = 0;
    billboardModelView[0][2] = 0;
    billboardModelView[1][2] = 0;
    billboardModelView[1][0] = 0;
    billboardModelView[2][0] = 0;
    billboardModelView[2][1] = 0;

    vec4 particleCenter = modelView * vec4(particlePosition, 1.0);
    vec4 vertexOffset = billboardModelView * vec4(textureUV.x, textureUV.y, 0.0, 0.0);
    vec4 vertexPos = particleCenter + vertexOffset;

    // Normalize UV from [-1..1] to [0..1]
    fragTextureUV = 0.5 * textureUV + vec2(0.5);

    gl_Position = projection * vertexPos;
}
