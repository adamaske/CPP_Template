#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <string>

class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const char* name);

	void Use();

	GLuint GetProgram();

	void SetUniformMatrix4(glm::mat4 matrix, std::string name);
	void SetUniformVec4(glm::vec4 v1, std::string name);
	void SetUniformVec3(glm::vec3 v1, std::string name);
	void SetUniform1f(float v1, std::string name);
	void SetUniform1i(int v1, std::string name);
	const char* mName;
private:
	//Opengl refrence to this entity
	GLuint mProgram;

};