#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram
{
public:
	unsigned int ID;
	ShaderProgram(const char* vertexPath, const char* fragmentPath) {
		// Buffer to hold error data
		char infoLog[1024];
		// Temporary variable for conversion to c_str
		std::string temp;

		// Load vertex shader from file and compile
		std::ifstream vertexFile;
		std::stringstream vertexStream;
		const char* vertexShaderCode;
		unsigned int vertexShaderID;
		//		Try to open the file and read contents, provide error message if fail
		try {
			vertexFile.open(vertexPath);
			vertexStream << vertexFile.rdbuf();
			vertexFile.close();
			temp = vertexStream.str();
			vertexShaderCode = temp.c_str();
		}
		catch (const char* msg) {
			std::cout << "Exception: " << msg << std::endl;
		}
		//		Create the shader and compile it
  		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderID, 1, &vertexShaderCode, NULL);
		glCompileShader(vertexShaderID);

		// Load fragment shader from file and compile
		std::ifstream fragmentFile;
		std::stringstream fragmentStream;
		const char* fragmentShaderCode;
		unsigned int fragmentShaderID;
		//		Same as vertex shader
		try {
			fragmentFile.open(fragmentPath);
			fragmentStream << fragmentFile.rdbuf();
			fragmentFile.close();
			temp = fragmentStream.str();
			fragmentShaderCode = temp.c_str();
		}
		catch (const char* msg) {
			std::cout << "Exception: " << msg << std::endl;
		}
		//		Same as vertex shader
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, NULL);
		glCompileShader(fragmentShaderID);

		// Create and link vertex and fragment shaders into shader program
		memset(infoLog, 0, sizeof(infoLog));
		this->ID = glCreateProgram();
		glAttachShader(this->ID, vertexShaderID);
		glAttachShader(this->ID, fragmentShaderID);
		glLinkProgram(this->ID);
		glDeleteShader(vertexShaderID);
		glDeleteProgram(fragmentShaderID);
		
		// Get any shader error logs into infoLog and display
		glGetShaderInfoLog(vertexShaderID, 1024, NULL, infoLog);
		std::cout << "Vertex shader info log: " << infoLog << std::endl;
		glGetShaderInfoLog(fragmentShaderID, 1024, NULL, infoLog);
		std::cout << "Fragment shader info log: " << infoLog << std::endl;
		// Get any program error logs into infoLog and display
		glGetProgramInfoLog(this->ID, 1024, NULL, infoLog);
		std::cout << "Shader program info log: " << infoLog << std::endl;
	}


	void use() {
		glUseProgram(this->ID);
	}
	// Direct access function to set any uniform integer variables
	void setUniformInt(const char* varName, int varValue) {
		glUniform1i(glGetUniformLocation(this->ID, varName), varValue);
	}

	void setUniformIntArray(const char* varName,int varValueArrayCount, int* varValueArray) {
		glUniform1iv(glGetUniformLocation(this->ID, varName), varValueArrayCount, varValueArray);
	}

	void setUniformFloat(const char* varName, float varValue) {
		glUniform1f(glGetUniformLocation(this->ID, varName), varValue);
	}

	void setUniformMat4(const char* varName, glm::mat4 varValue) {
		glUniformMatrix4fv(glGetUniformLocation(this->ID, varName), 1, GL_FALSE, glm::value_ptr(varValue));
	}

	void setUniformMat3(const char* varName, glm::mat3 varValue) {
		glUniformMatrix3fv(glGetUniformLocation(this->ID, varName), 1, GL_FALSE, glm::value_ptr(varValue));
	}
	
	void setUniformVec3(const char* varName, glm::vec3 varValue) {
		glUniform3fv(glGetUniformLocation(this->ID, varName), 1, glm::value_ptr(varValue));
	}
};