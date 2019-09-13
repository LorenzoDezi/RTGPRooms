#pragma once
#include "stb_image/stb_image.h"
#include "Shader.h"
#include "Mesh.h"
#include "ImageUtility.h"
#include "DepthMapped.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>


class Model : public DepthMapped
{
public:
	/*  Functions   */
	//DEBUG
	Model() {};
	Model(std::string path)
	{
		loadModel(path);
	}
	std::vector<int> GetVAOs();
	void Draw(Shader &shader);
	std::vector<std::shared_ptr<Mesh>> getMeshes();
	~Model();

protected:
	/*  Model Data  */
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	/*  Functions   */
	void loadModel(std::string path);
	virtual void processNode(aiNode *node, const aiScene *scene);
	std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
};