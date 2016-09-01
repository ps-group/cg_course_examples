varying vec3 n;
varying vec3 v;

void main(void)
{
    v = vec3(gl_ModelViewMatrix * gl_Vertex);
    n = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = ftransform();
}
