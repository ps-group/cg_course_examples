#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `attribute` renamed to `in` for vertex shader
//  - `varying` renamed to `out` for vertex shader

in vec3 vertex;
in vec3 normal;
in vec2 textureUV;

out vec2 fragTextureUV;
out vec3 fragNormal;
out vec3 fragViewDirection;

uniform mat4 modelView;
uniform mat4 normalModelView;
uniform mat4 projection;

void main(void)
{
    vec4 posInViewSpace = modelView * vec4(vertex, 1.0);
    fragViewDirection = vec3(posInViewSpace);
    fragNormal = normalize(vec3(normalModelView * vec4(normal, 0.0)));
    fragTextureUV = textureUV;
    gl_Position = projection * posInViewSpace;
}
