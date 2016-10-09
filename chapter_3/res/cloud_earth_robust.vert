#version 130
// GLSL version 130 enabled with OpenGL Core Profile 3.0.
//  - `attribute` renamed to `in`
//  - `varying` renamed to `out`

in vec3 vertex;
in vec3 normal;
in vec2 textureUV;

out vec2 fragTextureUV;
out vec3 fragNormal;
out vec3 fragPosInViewSpace;

uniform mat4 modelView;
uniform mat4 normalModelView;
uniform mat4 projection;

void main(void)
{
    vec4 posInViewSpace = modelView * vec4(vertex, 1.0);

    fragPosInViewSpace = vec3(posInViewSpace);
    fragNormal = normalize(vec3(normalModelView * vec4(normal, 1.0)));
    fragTextureUV = textureUV;
    gl_Position = projection * posInViewSpace;
}
