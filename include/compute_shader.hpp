#ifndef COMPUTE_SHADER_H
#define COMPUTE_SHADER_H

#include "glad/glad.h"

#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader
{
public:
    unsigned int ID;

    ComputeShader(const char* compute_path)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string compute_code;
        std::ifstream compute_file;
        // ensure ifstream objects can throw exceptions:
        compute_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

        try 
        {
            // open files
            compute_file.open(compute_path);
            std::stringstream cShaderStream;
            // read file's buffer contents into streams
            cShaderStream << compute_file.rdbuf();
            // close file handlers
            compute_file.close();
            // convert stream into string
            compute_code = cShaderStream.str();		
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }

        const char* shader_code = compute_code.c_str();

        unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &shader_code, NULL);
        glCompileShader(compute);
        this->check_compile_errors(compute, "COMPUTE");

        // shader Program
        this->ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        this->check_compile_errors(ID, "PROGRAM");
    }

    void use() 
    { 
        glUseProgram(this->ID); 
    }


    // utility uniform functions
    // ------------------------------------------------------------------------
    void set_bool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void set_int(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set_float(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void set_vec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]); 
    }
    void set_vec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(this->ID, name.c_str()), x, y); 
    }
    // ------------------------------------------------------------------------
    void set_vec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]); 
    }
    void set_vec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(this->ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void set_vec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]); 
    }
    void set_vec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(this->ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void set_mat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set_mat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set_mat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void check_compile_errors(GLuint shader, std::string type)
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
};


#endif // COMPUTE_SHADER_H