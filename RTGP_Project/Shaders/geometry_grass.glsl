#version 330  core

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;
out vec4 ViewDir;
out float ColorWeight;
flat out int TexIndex;

in mat4 modelMatrix[];
flat in int texIndex[];

uniform float time;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

//Rotation matrix used to perform the wind rotation
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 LocalSeed;

// This function returns random number from zero to one
float randZeroOne()
{
    uint n = floatBitsToUint(LocalSeed.y * 214013.0 + LocalSeed.x * 2531011.0 + LocalSeed.z * 141251.0);
    n = n * (n * n * 15731u + 789221u);
    n = (n >> 9u) | 0x3F800000u;
 
    float fRes =  2.0 - uintBitsToFloat(n);
    LocalSeed = vec3(LocalSeed.x + 147158.0 * fRes, LocalSeed.y*fRes  + 415161.0 * fRes, LocalSeed.z + 324154.0*fRes);
    return fRes;
}

int randomInt(int min, int max)
{
	float fRandomFloat = randZeroOne();
	return int(float(min)+fRandomFloat*float(max-min));
}

void main()
{
	mat4 modelMat = modelMatrix[0];
	mat4 mMVP = projMatrix*viewMatrix*modelMat;
	
	vec3 GrassFieldPos = gl_in[0].gl_Position.xyz;
	Normal = vec3(0.0, 1.0, 0.0);

	float PIover180 = 3.1415/180.0;
	//Vector basis to generate the patch quads
	vec3 baseDir[3] = vec3[]
	(
		vec3(1.0, 0.0, 0.0),
		//cos function accepts angles in radians
		vec3(float(cos(45.0*PIover180)), 0.0, float(sin(45.0*PIover180))),
		vec3(float(cos(-45.0*PIover180)), 0.0, float(sin(-45.0*PIover180)))
	);
	
	//the size of the quad
	float grassPatchSize = 0.6;
	float windStrenght = 0.2;
	
	vec3 windDir = vec3(1.0, 0.0, 1.0);
	windDir = normalize(windDir);
	for(int i = 0; i < 3; i++)
	{
		// Grass patch top left vertex
		vec3 baseDirRotated = (rotationMatrix(vec3(0, 1, 0), sin(time*0.7)*0.1) * vec4(baseDir[i], 1.0)).xyz;
		//Changing the seed of the random function at every quad
		LocalSeed = GrassFieldPos*float(i);
		//Also the x start position of the quad is changed randomly...
		int grassPatch = randomInt(0, 3);
		//.. and the height
		float grassPatchHeight = 0.5+randZeroOne()*1.3;
	
		//Starting x (scaled down) and ending x values
		float quadStartX = float(grassPatch)*0.25;
		float quadEndX = quadStartX+0.25;
		
		//Wind power calculation
		float windPower = 0.5+sin(GrassFieldPos.x/30 + GrassFieldPos.z/30 + time*(1.2+windStrenght/20.0));
		if(windPower < 0.0)
			windPower = windPower*0.2;
		else windPower = windPower*0.3;
		windPower *= windStrenght;
		

		vec3 topLeftPos = GrassFieldPos - baseDirRotated * grassPatchSize*0.5 + windDir*windPower;
		topLeftPos.y += grassPatchHeight;
		gl_Position = mMVP*vec4(topLeftPos, 1.0);
		TexCoord = vec2(0.0, 1.0);
		FragPos = topLeftPos;
		ColorWeight = 0.8;
		TexIndex = texIndex[0];
		EmitVertex();
		
		// Grass patch bottom left vertex
		vec3 bottomLeftPos = GrassFieldPos - baseDir[i]*grassPatchSize*0.5;  
		gl_Position = mMVP*vec4(bottomLeftPos, 1.0);
		TexCoord = vec2(0.0, 0.0);
		FragPos = bottomLeftPos;
		ColorWeight = 0.3;
		TexIndex = texIndex[0];
		EmitVertex();
		                               
		// Grass patch top right vertex
		vec3 topRightPos = GrassFieldPos + baseDirRotated * grassPatchSize*0.5 + windDir*windPower;
		topRightPos.y += grassPatchHeight;  
		gl_Position = mMVP*vec4(topRightPos, 1.0);
		TexCoord = vec2(1.0, 1.0);
		FragPos = topRightPos;
		ColorWeight = 0.8;
		TexIndex = texIndex[0];
		EmitVertex();
		
		// Grass patch bottom right vertex
		vec3 bottomRightPos = GrassFieldPos + baseDir[i]*grassPatchSize*0.5;  
		gl_Position = mMVP*vec4(bottomRightPos, 1.0);
		TexCoord = vec2(1.0, 0.0);
		FragPos = bottomRightPos;
		TexIndex = texIndex[0];
		ColorWeight = 0.3;
		EmitVertex();
		
		EndPrimitive();
	}

}