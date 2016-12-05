#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `attribute` renamed to `in` for vertex shader
//  - `varying` renamed to `out` for vertex shader

in vec2 attrPosition;
in vec2 attrTextureUV;

out vec2 fragTextureUV;

uniform mat3x3 modelView2D;
uniform mat4 projection;

void main(void)
{
    vec3 position3D = modelView2D * vec3(attrPosition, 1.0);
    fragTextureUV = attrTextureUV;
    gl_Position = projection * vec4(position3D.x, position3D.y, 0, position3D.z);
}
