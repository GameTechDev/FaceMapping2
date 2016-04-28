
layout(location = 0) in  vec3 in_Position;
layout(location = 1) in  vec2 in_Tex;

out  vec2  Tex;

void main(void) {
	gl_Position = vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
	Tex = vec2(in_Tex);
}

