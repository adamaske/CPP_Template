#include "Shader.h"
#include <iostream>
#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>


Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const char* name)
{
    mName = name;
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Open files and check for errors
    vShaderFile.open(vertexPath);
    if (!vShaderFile) {
        std::cout << "Could not open vertex shader file!" << std::endl;
        std::cout << vertexPath << std::endl;
    }
    else {
        std::cout << "Did open vertex shader file!" << std::endl;
    }
    fShaderFile.open(fragmentPath);
    if (!fShaderFile) {
        std::cout << "Could not open fragment shader file!" << std::endl;
    }
    else {
        std::cout << "Did open fragment shader file!" << std::endl;
    }


    std::stringstream vShaderStream;
    std::stringstream fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // Convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    // 2. Compile shaders
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];
    glViewport(100, 100, 600, 600);

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        //mLogger->logText("ERROR SHADER VERTEX " + std::string(vertexPath) +
         //   " COMPILATION_FAILED\n" + infoLog, LogType::REALERROR);
    }
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "No fragment sucsess!" << std::endl;
    }
    // Shader Program linking
    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertex);
    glAttachShader(mProgram, fragment);
    glLinkProgram(mProgram);
    // Print linking errors if any
    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);

    //making nice output name for printing:
    std::string shadername{ vertexPath };
    shadername = shadername.substr(shadername.size() - 5, shadername.size()); //deleting ".vert"

    if (!success)
    {
        glGetProgramInfoLog(this->mProgram, 512, nullptr, infoLog);
        std::cout << "No vertex shader succsess!" << std::endl;
    }
    else
    {
        std::cout << "Shader succsess!" << std::endl;
        std::cout << "Create shader " << name << " with programID = " << GetProgram() << std::endl;
        //Name = shadername.c_str();
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    // The shader program is now on the GPU and we reference it by using the mProgram variable
    glDeleteShader(vertex);
    glDeleteShader(fragment);



};

void Shader::Use()
{
    glUseProgram(mProgram);
}

GLuint Shader::GetProgram()
{
    return mProgram;
}


void Shader::SetUniformMatrix4(glm::mat4 matrix, std::string name) {
    //What matrix do we want to effect
    auto m = glGetUniformLocation(mProgram, name.c_str());
    //Sets the value from the matrix inserted
    glUniformMatrix4fv(m, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniformVec4(glm::vec4 v1, std::string name) {
    auto m = glGetUniformLocation(mProgram, name.c_str());
    glUniform4f(m, v1.x, v1.y, v1.z, v1.w);
}

void Shader::SetUniformVec3(glm::vec3 v1, std::string name)
{
    auto m = glGetUniformLocation(mProgram, name.c_str());
    glUniform3f(m, v1.x, v1.x, v1.x);
}

void Shader::SetUniform1f(float v1, std::string name) {
    auto m = glGetUniformLocation(mProgram, name.c_str());
    glUniform1f(m, v1);
}
void Shader::SetUniform1i(int v1, std::string name) {
    auto m = glGetUniformLocation(mProgram, name.c_str());
    glUniform1i(m, v1);
}