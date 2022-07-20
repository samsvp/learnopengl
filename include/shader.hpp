#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
    Shader(const char* vertex_path, const char* shader_path);
    ~Shader();

    unsigned int program_ID;

    // use the shader
    void use();
    // utility functions
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vec2(const std::string &name, const glm::vec2 &value) const;
    void set_vec2(const std::string &name, float x, float y) const;
    void set_vec3(const std::string &name, const glm::vec3 &value) const;
    void set_vec3(const std::string &name, float x, float y, float z) const;
    void set_vec4(const std::string &name, const glm::vec4 &value) const;
    void set_vec4(const std::string &name, float x, float y, float z, float w) const;
    void set_mat2(const std::string &name, const glm::mat2 &mat) const;
    void set_mat3(const std::string &name, const glm::mat3 &mat) const;
    void set_mat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void check_compile_errors(GLuint shader, std::string type);
};


Shader::Shader(const char* vertex_path, const char* shader_path)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try 
    {
        // open files
        vShaderFile.open(vertex_path);
        fShaderFile.open(shader_path);
        std::stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();	

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();			
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    this->check_compile_errors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    this->check_compile_errors(fragment, "FRAGMENT");

    // shader Program
    this->program_ID = glCreateProgram();
    glAttachShader(this->program_ID, vertex);
    glAttachShader(this->program_ID, fragment);
    glLinkProgram(this->program_ID);
    this->check_compile_errors(this->program_ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


Shader::~Shader() { }


void Shader::check_compile_errors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog 
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog 
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


void Shader::use() 
{ 
    glUseProgram(this->program_ID); 
}


// utility uniform functions
// ------------------------------------------------------------------------
void Shader::set_bool(const std::string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(this->program_ID, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::set_int(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(this->program_ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::set_float(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(this->program_ID, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::set_vec2(const std::string &name, const glm::vec2 &value) const
{ 
    glUniform2fv(glGetUniformLocation(this->program_ID, name.c_str()), 1, &value[0]); 
}
void Shader::set_vec2(const std::string &name, float x, float y) const
{ 
    glUniform2f(glGetUniformLocation(this->program_ID, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::set_vec3(const std::string &name, const glm::vec3 &value) const
{ 
    glUniform3fv(glGetUniformLocation(this->program_ID, name.c_str()), 1, &value[0]); 
}
void Shader::set_vec3(const std::string &name, float x, float y, float z) const
{ 
    glUniform3f(glGetUniformLocation(this->program_ID, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::set_vec4(const std::string &name, const glm::vec4 &value) const
{ 
    glUniform4fv(glGetUniformLocation(this->program_ID, name.c_str()), 1, &value[0]); 
}
void Shader::set_vec4(const std::string &name, float x, float y, float z, float w) const
{ 
    glUniform4f(glGetUniformLocation(this->program_ID, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::set_mat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(this->program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::set_mat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(this->program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::set_mat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}



#endif