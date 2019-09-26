#version 430 core
layout(quads) in;

in vec4 Pos_ES[];

out vec2 TexCoords; //Tex coordinate of the vertex
out vec3 norm;
out float DisplaceFactor;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 normal;
uniform sampler2D displaceMap;

#define CONTROL_QUAD_DIMENSION 4

void basisFunctions(out float[CONTROL_QUAD_DIMENSION] b, out float[CONTROL_QUAD_DIMENSION] db, float t) {
	float t1 = (1.0 - t);
	float t12 = t1 * t1;

	// Bernstein polynomials  (Hardcoded for 4 dimensions)
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;

	// Derivatives (Hardcoded for 4 dimensions)
	db[0] = -3.0 * t1 * t1;
	db[1] = -6.0 * t * t1 + 3.0 * t12;
	db[2] = -3.0 * t * t + 6.0 * t * t1;
	db[3] = 3.0 * t * t;
}

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	vec4[CONTROL_QUAD_DIMENSION][CONTROL_QUAD_DIMENSION] points;
	// Compute basis functions 
	float bu[4], bv[4];   // Basis functions for u and v 
	float dbu[4], dbv[4]; // Derivitives for u and v 
	basisFunctions(bu, dbu, u);
	basisFunctions(bv, dbv, v);

	// The sixteen control points
	for(int i = 0; i < CONTROL_QUAD_DIMENSION; i++)
		for (int j = 0; j < CONTROL_QUAD_DIMENSION; j++)
			points[i][j] = Pos_ES[i * CONTROL_QUAD_DIMENSION + j];

	//Calculating support point and tangent/bitangents
	vec4 Position = vec4(0.0, 0.0, 0.0, 1.0); //position
	vec4 du = vec4(0.0, 0.0, 0.0, 1.0); //partial derivative on u
	vec4 dv = vec4(0.0, 0.0, 0.0, 1.0); //partial derivative on v
	for (int i = 0; i < CONTROL_QUAD_DIMENSION; i++)
		for (int j = 0; j < CONTROL_QUAD_DIMENSION; j++) {
			Position += points[i][j] * bu[i] * bv[j];
			du += points[i][j] * dbu[i] * bv[j];
			dv += points[i][j] * bu[i] * dbv[j];
		}

	vec3 n = normalize(cross(du.xyz, dv.xyz));

	TexCoords = vec2(gl_TessCoord.x, gl_TessCoord.y);
	norm = normalize((normal * vec4(n, 1.0)).xyz);
	DisplaceFactor = texture(displaceMap, TexCoords).r;
	vec3 pos = Position.xyz + norm * DisplaceFactor * 0.4;
	gl_Position = projection * view * model * vec4(pos, 1.0);

}