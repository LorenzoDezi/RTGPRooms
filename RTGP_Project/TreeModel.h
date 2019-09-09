#pragma once
#include "Model.h"

class TreeModel : public Model
{
public:
	TreeModel(std::string path);
	~TreeModel();
	void Draw(Shader &shader, Shader &leavesShader);
protected:
	void processNode(aiNode *node, const aiScene *scene) override;

	std::shared_ptr<Mesh> leavesMesh = nullptr;
};

