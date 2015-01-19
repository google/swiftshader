attribute highp   vec4 inVertex;
attribute mediump vec2 inTexCoord;

varying   mediump vec2 texCoords;
		
void main() 
{ 
	gl_Position = inVertex;
	texCoords   = inTexCoord;
} 
