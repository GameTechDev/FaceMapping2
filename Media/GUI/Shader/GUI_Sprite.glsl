

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

out  vec2  Tex;

void main(void) {
	gl_Position.z = 1.0;//in_Position.z;
	gl_Position.w = 1.0;
	vec2 screenposition = position.xy/screenresolution.xy*2.0;
	vec2 screensize = position.zw/screenresolution.xy*2.0;
	gl_Position.x = screenposition.x;
	gl_Position.y = -screenposition.y;
	gl_Position.x += in_Position.x * screensize.x - 1.0;
	gl_Position.y += in_Position.y * screensize.y + 1.0;

	Tex = vec2(in_Tex);
}
#endif
#ifdef GLSL_FRAGMENT_SHADER
#ifdef textured
uniform sampler2D SpriteTexture;
#endif
in  vec2 Tex;

layout (location = 0) out vec4 fragColor; 


void main(void) {
#ifdef textured
	fragColor = texture(SpriteTexture, Tex) * backgroundcolor;
#else
	fragColor = backgroundcolor;
#endif
}
#endif

