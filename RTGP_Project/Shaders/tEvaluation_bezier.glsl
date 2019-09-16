#version 420 core
layout(quads) in;

in vec4 Pos_ES[];

out vec2 TexCoords; //Tex coordinate of the vertex
//DEBUG
out vec3 norm;
out float DisplaceFactor;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 normal;
uniform sampler2D displaceMap;

void basisFunctions(out float[4] b, out float[4] db, float t) {
	float t1 = (1.0 - t);
	float t12 = t1 * t1;

	// Bernstein polynomials 
	b[0] = t12 * t1;
	b[1] = 3.0 * t12 * t;
	b[2] = 3.0 * t1 * t * t;
	b[3] = t * t * t;

	// Derivatives 
	db[0] = -3.0 * t1 * t1;
	db[1] = -6.0 * t * t1 + 3.0 * t12;
	db[2] = -3.0 * t * t + 6.0 * t * t1;
	db[3] = 3.0 * t * t;
}

void main() {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// The sixteen control points
	vec4 p00 = Pos_ES[0];
	vec4 p01 = Pos_ES[1];
	vec4 p02 = Pos_ES[2];
	vec4 p03 = Pos_ES[3];
	vec4 p10 = Pos_ES[4];
	vec4 p11 = Pos_ES[5];
	vec4 p12 = Pos_ES[6];
	vec4 p13 = Pos_ES[7];
	vec4 p20 = Pos_ES[8];
	vec4 p21 = Pos_ES[9];
	vec4 p22 = Pos_ES[10];
	vec4 p23 = Pos_ES[11];
	vec4 p30 = Pos_ES[12];
	vec4 p31 = Pos_ES[13];
	vec4 p32 = Pos_ES[14];
	vec4 p33 = Pos_ES[15];

	// Compute basis functions 
	float bu[4], bv[4];   // Basis functions for u and v 
	float dbu[4], dbv[4]; // Derivitives for u and v 
	basisFunctions(bu, dbu, u);
	basisFunctions(bv, dbv, v);

	//TODO: For loops
	// Bezier interpolation 
	vec4 Position =
		p00 * bu[0] * bv[0] + p01 * bu[0] * bv[1] + p02 * bu[0] * bv[2] +
		p03 * bu[0] * bv[3] +
		p10 * bu[1] * bv[0] + p11 * bu[1] * bv[1] + p12 * bu[1] * bv[2] +
		p13 * bu[1] * bv[3] +
		p20 * bu[2] * bv[0] + p21 * bu[2] * bv[1] + p22 * bu[2] * bv[2] +
		p23 * bu[2] * bv[3] +
		p30 * bu[3] * bv[0] + p31 * bu[3] * bv[1] + p32 * bu[3] * bv[2] +
		p33 * bu[3] * bv[3];

	// The partial derivatives 
	vec4 du =
		p00 * dbu[0] * bv[0] + p01 * dbu[0] * bv[1] + p02 * dbu[0] * bv[2] +
		p03 * dbu[0] * bv[3] +
		p10 * dbu[1] * bv[0] + p11 * dbu[1] * bv[1] + p12 * dbu[1] * bv[2] +
		p13 * dbu[1] * bv[3] +
		p20 * dbu[2] * bv[0] + p21 * dbu[2] * bv[1] + p22 * dbu[2] * bv[2] +
		p23 * dbu[2] * bv[3] +
		p30 * dbu[3] * bv[0] + p31 * dbu[3] * bv[1] + p32 * dbu[3] * bv[2] +
		p33 * dbu[3] * bv[3];

	vec4 dv =
		p00 * bu[0] * dbv[0] + p01 * bu[0] * dbv[1] + p02 * bu[0] * dbv[2] +
		p03 * bu[0] * dbv[3] +
		p10 * bu[1] * dbv[0] + p11 * bu[1] * dbv[1] + p12 * bu[1] * dbv[2] +
		p13 * bu[1] * dbv[3] +
		p20 * bu[2] * dbv[0] + p21 * bu[2] * dbv[1] + p22 * bu[2] * dbv[2] +
		p23 * bu[2] * dbv[3] +
		p30 * bu[3] * dbv[0] + p31 * bu[3] * dbv[1] + p32 * bu[3] * dbv[2] +
		p33 * bu[3] * dbv[3];

	// The normal is the cross product of the partials 
	vec3 n = normalize(cross(du.xyz, dv.xyz));

	// Convert to camera coordinates 
	//TODO: We really need it?
	TexCoords = vec2(gl_TessCoord.x, gl_TessCoord.y);
	norm = normalize((normal * vec4(n, 1.0)).xyz);
	DisplaceFactor = texture(displaceMap, TexCoords).r;
	//DEBUG
	vec3 pos = Position.xyz + norm * DisplaceFactor * 0.1;
	gl_Position = projection * view * model * vec4(pos, 1.0);

}