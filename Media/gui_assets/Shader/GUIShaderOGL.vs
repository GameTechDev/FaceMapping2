
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout (std140, row_major) uniform cbGUIValues
{
	mat4  projectionMatrix;
    mat4  modelMatrix;
};

layout(location = 0) in  vec3 in_Position;
layout(location = 1) in  vec2 in_Tex;
layout(location = 2) in  vec4 in_Color;

out   vec2  Tex;
out   vec4  Color;

void main(void) {
	vec4 inPos = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
	gl_Position = inPos*modelMatrix*projectionMatrix;
	Tex = vec2(in_Tex.x,in_Tex.y);
	Color = in_Color;
}

