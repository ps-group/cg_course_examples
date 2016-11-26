uniform sampler2D colormap;

void main()
{
    // Calculate fragment color by fetching the texture
    gl_FragColor = vec4(1.0) - texture2D(colormap, gl_TexCoord[0].st);
}

