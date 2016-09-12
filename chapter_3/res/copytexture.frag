uniform sampler2D colormap;

void main()
{
    // Calculate fragment color by fetching the texture
    gl_FragColor = texture2D(colormap, gl_TexCoord[0].st);
}
