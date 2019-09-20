#version 430 core
layout(vertices = 3) out;

uniform vec3 viewPos;

in vec3 Pos_CS[];
in vec2 TexCoords_CS[];
in vec3 Normal_CS[];

out vec3 Pos_ES[];
out vec2 TexCoords_ES[];
out vec3 Normal_ES[];

float GetTessLevel(float dist0, float dist1);

void main() {
	TexCoords_ES[gl_InvocationID] = TexCoords_CS[gl_InvocationID];
	Normal_ES[gl_InvocationID] = Normal_CS[gl_InvocationID];
	Pos_ES[gl_InvocationID] = Pos_CS[gl_InvocationID];

	//Calculating the distance from the camera to the three control points
	//Avoiding doing the calculation three times
	if (gl_InvocationID == 0) {
		float viewToVertDist0 = distance(viewPos, Pos_ES[0]);
		float viewToVertDist1 = distance(viewPos, Pos_ES[1]);
		float viewToVertDist2 = distance(viewPos, Pos_ES[2]);

		//Calculating tessellation levels
		//gl_TessLevelOuter determines roughly how many segments each triangle edge will contain
		gl_TessLevelOuter[0] = GetTessLevel(viewToVertDist1, viewToVertDist2);
		gl_TessLevelOuter[1] = GetTessLevel(viewToVertDist2, viewToVertDist0);
		gl_TessLevelOuter[2] = GetTessLevel(viewToVertDist0, viewToVertDist1);
		//gl_TessLevelInner determines roughly how many rings the triangle will contain
		gl_TessLevelInner[0] = gl_TessLevelOuter[2];
	}
}

float GetTessLevel(float dist0, float dist1) {
	//TODO: check proper values
	float avgDist = (dist0 + dist1) / 2.0;
	if (avgDist <= 8.0) {
		//At max you can tessellate 64!
		return 64.0;
	}
	else {
		return 0.0;
	}
}