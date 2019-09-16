#version 420 core

layout(vertices = 16) out;

in vec4 Pos_CS[];
out vec4 Pos_ES[];

void main() {
	Pos_ES[gl_InvocationID] = Pos_CS[gl_InvocationID];
	if (gl_InvocationID == 0) {
		gl_TessLevelOuter[0] = 64;
		gl_TessLevelOuter[1] = 64;
		gl_TessLevelOuter[2] = 64;
		gl_TessLevelOuter[3] = 64;

		gl_TessLevelInner[0] = 64;
		gl_TessLevelInner[1] = 64;
	}
}