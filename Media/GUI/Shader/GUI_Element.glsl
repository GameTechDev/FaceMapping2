
//#ifdef GL_FRAGMENT_PRECISION_HIGH
//precision highp float;
//#else
//precision mediump float;
//#endif

layout (std140, row_major) uniform cbGUIConstants
{
	mat4 wvpMatrix;
	vec4 position;
	vec4 backgroundcolor;
	vec4 foregroundcolor;
	vec4 screenresolution;
	vec4 mousePosition;
};

#ifdef GLSL_VERTEX_SHADER

layout(location = 0) in  vec3 in_Position;
layout(location = 1) in  vec2 in_Tex;
//layout(location = 2) in  vec4 in_Color;
out   vec2  Tex;

void main(void) {

	vec4 outPosition = vec4( in_Position.xyz, 1.0);
	outPosition.xy -= screenresolution.xy/2.0;

	gl_Position = outPosition * wvpMatrix;
	Tex = in_Tex.xy;

}

#endif
#ifdef GLSL_FRAGMENT_SHADER
uniform sampler2D SpriteTexture;
in  vec2 Tex;

layout (location = 0) out vec4 fragColor; 


void main(void) {
	fragColor = texture(SpriteTexture, Tex).r * foregroundcolor;
}
#endif

