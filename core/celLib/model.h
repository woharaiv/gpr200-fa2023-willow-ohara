#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include "../ew/mesh.h"
#include "../willowLib/shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace celLib 
{
	class Model 
	{
		public:
			Model(const std::string& path) { loadModel(path); };
			void Draw(); // call draw on each mesh
		private:
			std::vector<ew::Mesh> meshes;
			std::string directory;
			void loadModel(const std::string& path);
			void processNode(aiNode *node, const aiScene *scene);
			ew::Mesh processMesh(aiMesh * mesh, const aiScene * scene);
			
	};
}