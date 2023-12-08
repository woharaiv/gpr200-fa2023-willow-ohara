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

	void Model::loadModel(const std::string& filepath) 
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
		for (unsigned int i = 0; i < node->mNumMeshes; i++) 
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		//TODO: then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++) 
		{
			processNode(node->mChildren[i], scene);
		}
	}

	ew::Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
	{
		ew::MeshData meshData;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
		{
			ew::Vertex vertex;
			//process vertex positions, normals and texture coordinates
			ew::Vec3 vector;
			//pos
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.pos = vector;
			//normals
			if (mesh->HasNormals()) 
			{
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
			}
			//UVs / texture coordinates
			if (mesh->mTextureCoords[0])
			{
				ew::Vec2 uv;
				uv.x = mesh->mTextureCoords[0][i].x;
				uv.y = mesh->mTextureCoords[0][i].y;
				vertex.uv = uv;
			}
			else 
			{
				vertex.uv = ew::Vec2(0.0f,0.0f);
			}

			meshData.vertices.push_back(vertex);
		}
		//process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) 
			{
				meshData.indices.push_back(face.mIndices[j]);
			}
		}
		ew::Mesh convertedMesh = {};
		convertedMesh.load(meshData);
		return convertedMesh;
	}
}