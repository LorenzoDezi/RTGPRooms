#include "TreeModel.h"


TreeModel::TreeModel(std::string path)
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

TreeModel::~TreeModel()
{
}

void TreeModel::Draw(Shader & shader, Shader & leavesShader)
{
	Model::Draw(shader);
	leavesShader.use();
	leavesMesh->Draw(leavesShader);
}

void TreeModel::processNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		if (strcmp(mesh->mName.C_Str(), "leaves") == 0)
			leavesMesh = processMesh(mesh, scene);
		else
			meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}
