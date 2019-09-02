#include "ShaderManager.h"
#include <shaders/ExtractSourceCode/extractSourceCode.h>

#include <glad/glad.h>

#include <map>
#include <string>
#include <iostream>

#include <filesystem>
#include <fstream>
#include <sstream>

using namespace Engine;

std::vector<std::string> ShaderManager::sourceCode = { "#version 450 core\nout vec4 FragColour;\n\nin vec3 ourColour;\n\nvoid main()\n{\n    FragColour = vec4(ourColour, 1.0f);\n}", "#version 450 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec3 aColour;\n\nout vec3 ourColour;\n\nuniform float timeValue;\n\nvec3 finalColour;\n\nvoid main()\n{\n    gl_Position = vec4(aPos, 1.0);\n\n	if (aColour.x > 0.9){\n		if (timeValue < 1.0f/3.0f){\n			finalColour = vec3(aColour.x - timeValue * 3.0, timeValue * 3.0, 0.0); \n		}\n		else if (timeValue < 2.0f/3.0f){\n			finalColour = vec3(0.0, aColour.x - (timeValue - 0.33) * 3.0, (timeValue - 0.33) * 3.0);\n		}\n		else{\n			finalColour = vec3((timeValue - 0.66) * 3.0, 0.0, aColour.x - (timeValue - 0.66) * 3.0);\n		}\n	}\n	else if (aColour.y > 0.9){\n		if (timeValue < 1.0f/3.0f){\n			finalColour = vec3(0.0, aColour.y - timeValue * 3.0, timeValue * 3.0);\n		}\n		else if (timeValue < 2.0f/3.0f){\n			finalColour = vec3((timeValue - 0.33) * 3.0, 0.0, aColour.y - (timeValue - 0.33) * 3.0);\n		}\n		else{\n			finalColour = vec3(aColour.y - (timeValue - 0.66) * 3.0, (timeValue - 0.66) * 3.0, 0.0); \n		}\n	}\n	else if (aColour.z > 0.9){\n		if (timeValue < 1.0f/3.0f){\n			finalColour = vec3(timeValue * 3.0, 0.0, aColour.z - timeValue * 3.0);\n		}\n		else if (timeValue < 2.0f/3.0f){\n			finalColour = vec3(aColour.z - (timeValue - 0.33) * 3.0, (timeValue - 0.33) * 3.0, 0.0); \n		}\n		else{\n			finalColour = vec3(0.0, aColour.z - (timeValue - 0.66) * 3.0, (timeValue - 0.66) * 3.0);\n		}\n	}\n\n    ourColour = finalColour;\n}" };
std::vector<std::string> ShaderManager::fileNames = { "shader.frag", "shader.vert" };
std::map<std::string, unsigned int> ShaderManager::shaderIndex;

void ShaderManager::setupShaderSource() {
#ifdef CFG_DEBUG
	// extract source code and save it to the sourceCode.txt file
	// which will be used in release
	extractSourceCode();

	// now the it the debug way...
	std::string path = "src/shaders";

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.is_directory()) continue;

		// retrieve the shader source code from filePath
		std::string shaderCode;
		std::ifstream shaderFile;
		
		// ensure ifstream objects can throw exceptions:
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
		try {
			// open file
			shaderFile.open(entry.path());
			std::stringstream shaderStream;

			// read file's buffer contents into stream
			shaderStream << shaderFile.rdbuf();

			// close file handler
			shaderFile.close();

			// convert stream into string
			shaderCode = shaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}

		std::string entryPath = entry.path().string();
		int index = (int)entryPath.size() - 1;
		for (; index >= 0; index--) {
			if (entryPath[index] == '\\' || entryPath[index] == '/') {
				index++;
				break;
			}
		}

		// get the name of the file
		std::string name = entryPath.substr(index);

		// compile the code
		compileShader(shaderCode, name);
	}
#else
	for (int i = 0; i < fileNames.size(); i++) {
		compileShader(sourceCode[i], fileNames[i]);
	}
#endif
}

// Get the shader ID by passing in the file name
unsigned int ShaderManager::getShaderID(std::string shaderName) {
	return shaderIndex[shaderName];
}

void ShaderManager::compileShader(std::string& shaderCode, std::string& name) {
	// the last 4 letters
	std::string extension = name.substr(name.size() - 4);
	
	// glsl, frag, vert, geom, comp, tese, tesc
	if		(extension == "vert") shaderIndex[name] = glCreateShader(GL_VERTEX_SHADER);
	else if (extension == "frag") shaderIndex[name] = glCreateShader(GL_FRAGMENT_SHADER);
	else if (extension == "geom") shaderIndex[name] = glCreateShader(GL_GEOMETRY_SHADER);
	else if (extension == "comp") shaderIndex[name] = glCreateShader(GL_COMPUTE_SHADER);
	else if (extension == "tese") shaderIndex[name] = glCreateShader(GL_TESS_EVALUATION_SHADER);
	else if (extension == "tesc") shaderIndex[name] = glCreateShader(GL_TESS_CONTROL_SHADER);

	const char* shaderCodeC = shaderCode.c_str();
	glShaderSource(shaderIndex[name], 1, &shaderCodeC, NULL);
	glCompileShader(shaderIndex[name]);
	// print compile errors if any
	checkCompileErrors(shaderIndex[name], name);
}

// Utility function for displaying errors if compilation went wrong
void ShaderManager::checkCompileErrors(unsigned int shader, std::string& name) {
	int success;
	char infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of file: " << name << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
}

// Delete all shaders
void ShaderManager::cleanup() {
	for (const auto& index : shaderIndex) {
		glDeleteShader(index.second);
	}
}