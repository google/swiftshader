uniform sampler2D sTexture;

varying mediump vec2 texCoords;

void main()
{
    highp vec3 vCol = texture2D(sTexture, texCoords).rgb;
    gl_FragColor = vec4(vCol, 1.0);
}
