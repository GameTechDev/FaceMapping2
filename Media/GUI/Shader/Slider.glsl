#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout (std140, row_major) uniform cbGUIConstants
{
   mat4 wvpMatrix;
   vec4 position;
   vec4 backgroundColor;
   vec4 foregroundColor;
   vec4 screenresolution;
   vec4 mousePosition;
};

#ifdef GLSL_VERTEX_SHADER

layout(location = 0) in  vec3 in_Position;
layout(location = 1) in  vec2 in_Tex;

out   vec2  Tex;

void main(void) {

	vec4 pos = vec4(in_Position.xyz, 1.0);
	pos.xy -= screenresolution.xy/2.0;

	gl_Position = wvpMatrix * pos;
	Tex = in_Tex.xy;

}

#endif
#ifdef GLSL_FRAGMENT_SHADER
#ifdef textured
uniform sampler2D SpriteTexture;
#endif
in  vec2 Tex;

layout (location = 0) out vec4 fragColor; 


void main(void) {
	vec4 color;
	if(mousePosition.x < Tex.x)
	{
		color = foregroundColor;
	}
	else
	{
		color = backgroundColor;
	}
#ifdef textured
	fragColor = texture(SpriteTexture, Tex) * color;
#endif
	fragColor = color;
}
#endif

