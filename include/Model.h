#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Mesh.h>

class Model {
private:
	void loadModel(std::string path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "Error loading model with assimp: " << importer.GetErrorString() << std::endl;
			return;
		}

		std::cout << std::endl;
		std::cout << "Model loaded successfully, some information: " << std::endl;
		std::cout << "	1. Animations: " << scene->mNumAnimations << std::endl;
		std::cout << "	2. Cameras: " << scene->mNumCameras << std::endl;
		std::cout << "	3. Lights: " << scene->mNumLights<< std::endl;
		std::cout << "	4. Materials: " << scene->mNumMaterials << std::endl;
		std::cout << "	5. Meshes: " << scene->mNumMeshes << std::endl;
		std::cout << "	6. Textures: " << scene->mNumTextures << std::endl;
		std::cout << std::endl;

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode* node, const aiScene* scene) {
		for (int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(processMesh(mesh, scene));
		}
		for (int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		// Get all positions, normals and texture coordinates
		std::vector<Vertex> vertices;
		for (int i = 0; i < mesh->mNumVertices; i++) {
			Vertex temp;
			temp.position = glm::vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
			);
			temp.normal = glm::vec3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			);
			if (mesh->mTextureCoords[0]) {
				temp.textureCoord = glm::vec3(
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y,
					mesh->mTextureCoords[0][i].z
				);
			}
			else {
				temp.textureCoord = glm::vec3(0.0);
			}
			vertices.push_back(temp);
		}

		// Get all indices
		std::vector<unsigned int> indices;
		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// Get all materials
		// TODO

		return Mesh(vertices, indices);
	}

public:
	std::vector<Mesh> meshes;
	Model(const char* path) {
		loadModel(path);
	}

	void draw() {
		for (int i = 0; i < this->meshes.size(); i++) {
			meshes[i].draw();
		}
	}
};