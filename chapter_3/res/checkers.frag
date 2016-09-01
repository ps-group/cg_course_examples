void main()
{
    // determine whether fraction part of x and y
    // texture coordinate is greater than 0.5
    vec2 stepXY = step(vec2(0.5, 0.5), fract(gl_TexCoord[0].xy));
    // determine whether the texture coordinate
    // is within a black or white check
    gl_FragColor = vec4((stepXY.x != stepXY.y) ? 1.0 : 0.0);
}
