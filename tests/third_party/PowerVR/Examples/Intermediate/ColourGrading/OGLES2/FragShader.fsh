uniform  sampler2D     sTexture;
uniform  mediump sampler3D		sColourLUT;

varying mediump vec2 texCoords;

void main()
{
    highp vec3 vCol = texture2D(sTexture, texCoords).rgb;
	lowp vec3 vAlteredCol = texture3D(sColourLUT, vCol).rgb;
    gl_FragColor = vec4(vAlteredCol, 1.0);
}
