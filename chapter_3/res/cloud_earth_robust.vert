attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 textureUV;

varying vec2 fragTextureUV;
varying vec3 fragNormal;
varying vec3 fragPosInViewSpace;

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
