#version 150
// GLSL version 150 enabled with OpenGL Core Profile 3.0.
//  - `attribute` renamed to `in` for vertex shader
//  - `varying` renamed to `out` for vertex shader
//  - array attributes allowed

const int BONES_PER_VERTEX = 4;

in vec3 vertex;
in vec3 normal;
in vec2 textureUV;
in int boneIds[BONES_PER_VERTEX];
in float boneWeights[BONES_PER_VERTEX];

out vec2 fragTextureUV;
out vec3 fragNormal;
out vec3 fragPosInViewSpace;

uniform mat4 modelView;
uniform mat4 normalModelView;
uniform mat4 projection;
uniform mat4 boneTransforms[64];

void main(void)
{
    mat4 boneTransform = mat4(0);
    for (int bi = 0; bi < BONES_PER_VERTEX; ++bi)
    {
        int boneId = boneIds[bi];
        float weight = boneWeights[bi];
        boneTransform += weight * boneTransforms[boneId];
    }

    mat4 normalTransform = normalModelView * transpose(inverse(boneTransform));

    vec4 posInViewSpace = modelView * boneTransform * vec4(vertex, 1.0);
    fragPosInViewSpace = vec3(posInViewSpace);
    fragNormal = normalize(vec3(normalTransform * vec4(normal, 0.0)));
    fragTextureUV = textureUV;

    gl_Position = projection * posInViewSpace;
}
