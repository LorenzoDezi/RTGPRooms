#include "Model.h"

std::vector<int> Model::GetVAOs()
{
	std::vector<int> VAOs;
	for (int i = 0; i < meshes.size(); i++) {
		VAOs.push_back(meshes[i]->GetVAO());
	}
	return VAOs;
}

void Model::Draw(Shader &shader)
{
	bindDepthMap(shader);
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->Draw(shader);
}

void Model::DrawTessellated(Shader & shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->DrawTessellated(shader);
}

std::vector<std::shared_ptr<Mesh>> Model::getMeshes()
{
	return meshes;
}

Model::~Model()
{
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	//DEBUG
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs 
		| aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	glm::vec3 color_diffuse;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		//processing normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		if (mesh->HasTangentsAndBitangents()) {
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		//processing texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			aiColor3D color;
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
				color_diffuse = glm::vec3(color.r, color.g, color.b);
			}
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material,
				aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = loadMaterialTextures(material,
				aiTextureType_HEIGHT, "texture_normals");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			std::vector<Texture> displacementMaps = loadMaterialTextures(material,
				aiTextureType_DISPLACEMENT, "texture_displacement");
			textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());
			//Using alpha textures to store roughness textures
			std::vector<Texture> roughnessMaps = loadMaterialTextures(material,
				aiTextureType_OPACITY, "texture_roughness");
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
			//Using ambient textures in mtl file to store ao textures
			std::vector<Texture> aoMaps = loadMaterialTextures(material,
				aiTextureType_AMBIENT, "texture_ao");
			textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
		}
	}
	return std::make_shared<Mesh>(vertices, indices, textures, color_diffuse);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		//DEBUG
		if (type == aiTextureType_DISPLACEMENT)
			std::cout << str.C_Str() << std::endl;
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			texture.id = ImageUtility::TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
		}
	}
	return textures;
}

