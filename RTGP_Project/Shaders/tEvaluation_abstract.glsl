#version 430 core

layout(triangles, equal_spacing, ccw) in;

uniform mat4 view;
uniform float time;
uniform mat4 projection;

in vec3 Pos_ES[];
in vec2 TexCoords_ES[];
in vec3 Normal_ES[];

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out float truchetOffset;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2);
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2);
vec2 truchetPattern(vec2 v, float index);
float random(vec2 v);

void main() {
	TexCoords = interpolate2D(TexCoords_ES[0], TexCoords_ES[1], TexCoords_ES[2]);
	//Interpolating normals and positions
	Normal =  normalize(interpolate3D(Normal_ES[0], Normal_ES[1], Normal_ES[2]));
	FragPos = interpolate3D(Pos_ES[0], Pos_ES[1], Pos_ES[2]);
	float textureScale = 40.0f;
	vec2 pos = TexCoords * textureScale;
	vec2 ipos = floor(pos);  // integer
	vec2 fpos = fract(pos);  // fraction
	vec2 tile = truchetPattern(fpos, random(ipos));
	float offset = 0.0;
	// Maze
	offset = smoothstep(tile.x - 0.3, tile.x, tile.y) -
		smoothstep(tile.x, tile.x + 0.3, tile.y);
	// Truchet (2 triangles)
	offset = step(tile.x, tile.y);
	truchetOffset = offset;
	FragPos += offset * Normal * 0.2;
	gl_Position = projection * view * vec4(FragPos, 1.0);
}

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

float random(vec2 v) {
	return fract(sin(dot(v.xy,
		vec2(12.9898, 78.233)))*
		time * 0.2);
}

vec2 truchetPattern(vec2 v, float index) {
	index = fract(((index - 0.5)*2.0));
	if (index > 0.75) {
		v = vec2(1.0) - v;
	}
	else if (index > 0.5) {
		v = vec2(1.0 - v.x, v.y);
	}
	else if (index > 0.25) {
		v = 1.0 - vec2(1.0 - v.x, v.y);
	}
	return v;
}
