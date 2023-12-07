#include "model.h"

namespace celLib 
{
	void Model::Draw()
	{
		//call draw on all the meshes
		for (unsigned int i = 0; i < meshes.size(); i++) 
		{
			meshes[i].draw();
		}
	}

	void Model::loadModel(std::string filepath) 
	{
		//import model from filepath
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
		{
			std::cout << "ERROR:ASSIMP::" << import.GetErrorString()<< std::endl;
			return;
		}
		directory = filepath.substr(0, filepath.find_last_of('/'));
		processNode(scene->mRootNode, scene); //recursively call processNode to process all of the scene's nodes
	}

	void Model::processNode(aiNode* node, const aiScene* scene) 
	{
		//TODO: process all the node's meshes (if any)
		//TODO: then do the same for each of its children
	}

	ew::Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
	{
		ew::Mesh convertedMesh = {};

		return convertedMesh;
	}
}