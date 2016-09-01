varying vec3 n;
varying vec3 v;

void main(void)
{
    vec4 result = vec4(0.0);
    for (int li = 0; li < gl_MaxLights; ++li)
    {
        vec3 delta = gl_LightSource[li].position.w * v;
        vec3 lightDirection = normalize(gl_LightSource[li].position.xyz - delta);
        vec3 viewDirection = normalize(-v);
        vec3 reflectDirection = normalize(-reflect(lightDirection, n));

        vec4 Iamb = gl_FrontLightProduct[li].ambient;

        float diffuseAngle = max(dot(n, lightDirection), 0.0);
        vec4 Idiff = gl_FrontLightProduct[li].diffuse * diffuseAngle;
        Idiff = clamp(Idiff, 0.0, 1.0);

        float specularAngle = max(dot(reflectDirection, viewDirection), 0.0);
        vec4 Ispec = gl_FrontLightProduct[li].specular
                    * pow(specularAngle, gl_FrontMaterial.shininess / 4.0);
        Ispec = clamp(Ispec, 0.0, 1.0);

        result += Iamb + Idiff + Ispec;
    }

    gl_FragColor = gl_FrontLightModelProduct.sceneColor + result;
}
