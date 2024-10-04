#include <assert.h>
#include <stdio.h>

#include <spdlog/spdlog.h>

#include <Lucky/Graphics/GraphicsDevice.hpp>
#include <Lucky/Graphics/ShaderProgram.hpp>
#include <Lucky/Graphics/Texture.hpp>

#include "IncludeOpenGL.h"

namespace Lucky
{
    VertexShader::VertexShader(const uint8_t *source, uint32_t sourceLength)
    {
        assert(source != nullptr);

        id = glCreateShader(GL_VERTEX_SHADER);
        if (id == 0)
        {
            spdlog::error("Failed to create OpenGL vertex shader id.");
            throw;
        }

        glShaderSource(id, 1, (GLchar **)&source, (GLint *)&sourceLength);
        glCompileShader(id);

        int status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            char infoLog[1024];
            int logLength;
            glGetShaderInfoLog(id, 1024, &logLength, infoLog);
            glDeleteShader(id);

            spdlog::error("Vertex shader compilation failed:\n{}", infoLog);
            throw;
        }
    }

    VertexShader::~VertexShader()
    {
        glDeleteShader(id);
    }

    FragmentShader::FragmentShader(const uint8_t *source, uint32_t sourceLength)
    {
        assert(source != nullptr);

        id = glCreateShader(GL_FRAGMENT_SHADER);
        if (id == 0)
        {
            spdlog::error("Failed to create OpenGL fragment shader id.");
            throw;
        }

        glShaderSource(id, 1, (GLchar **)&source, (GLint *)&sourceLength);
        glCompileShader(id);

        int status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            char infoLog[1024];
            int logLength;
            glGetShaderInfoLog(id, 1024, &logLength, infoLog);
            glDeleteShader(id);

            spdlog::error("Fragment shader compilation failed:\n{}", infoLog);
            throw;
        }
    }

    FragmentShader::~FragmentShader()
    {
        glDeleteShader(id);
    }

    ShaderProgram::ShaderProgram(
        std::shared_ptr<GraphicsDevice> graphicsDevice, VertexShader &vertexShader, FragmentShader &fragmentShader)
        : graphicsDevice(graphicsDevice)
    {
        assert(graphicsDevice);

        id = glCreateProgram();
        if (id == 0)
        {
            spdlog::error("Failed to create OpenGL shader program id.");
            throw;
        }

        glAttachShader(id, vertexShader.GetShaderId());
        glAttachShader(id, fragmentShader.GetShaderId());

        glLinkProgram(id);

        int status;
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if (status != GL_TRUE)
        {
            char infoLog[1024];
            int logLength;
            glGetShaderInfoLog(id, 1024, &logLength, infoLog);
            glDeleteProgram(id);

            spdlog::error("Shader program linking failed");
            throw;
        }

        int uniformCount;
        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);

        for (int i = 0; i < uniformCount; i++)
        {
            int size;
            GLenum type;
            GLchar name[256];

            glGetActiveUniform(id, i, 256, nullptr, &size, &type, name);
            parameters[name] = {glGetUniformLocation(id, name), type};
        }

        int attributeCount;
        glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &attributeCount);

        for (int i = 0; i < attributeCount; i++)
        {
            int size;
            GLenum type;
            GLchar name[256];

            glGetActiveAttrib(id, i, 256, nullptr, &size, &type, name);
            attributes[name] = {glGetAttribLocation(id, name), type};
        }

        auto positionType = GetAttributeType("position");
        if (positionType != 0 && positionType != GL_FLOAT_VEC4)
        {
            spdlog::error("Shader program has an invalid type for attribute: position\n");
        }

        auto colorType = GetAttributeType("color");
        if (colorType != 0 && colorType != GL_FLOAT_VEC4)
        {
            spdlog::error("Shader program has an invalid type for attribute: color\n");
        }

        auto texcoordType = GetAttributeType("texcoord");
        if (texcoordType != 0 && texcoordType != GL_FLOAT_VEC2)
        {
            spdlog::error("Shader program has an invalid type for attribute: texcoord\n");
        }

        auto projectionMatrixType = GetParameterType("ProjectionMatrix");
        if (projectionMatrixType != 0 && projectionMatrixType != GL_FLOAT_MAT4)
        {
            spdlog::error("Shader program has an invalid type for uniform: ProjectionMatrix\n");
        }
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteShader(id);
    }

    void ShaderProgram::SetParameter(const std::string &name, const Texture &texture, int slotNumber)
    {
        assert(name.length() > 0);

        if (parameters.find(name) == parameters.end())
        {
            spdlog::error("Shader program parameter not found: {}", name);
            throw;
        }

        if (parameters[name].type != GL_SAMPLER_2D)
        {
            spdlog::error("Shader program parameter type mismatch: {}", name);
            throw;
        }

        glActiveTexture(GL_TEXTURE0 + slotNumber);
        graphicsDevice->BindTexture(texture);
        glUniform1i(parameters[name].location, slotNumber);
    }

    void ShaderProgram::SetParameter(const std::string &name, const glm::mat4 &matrix)
    {
        assert(name.length() > 0);

        if (parameters.find(name) == parameters.end())
        {
            spdlog::error("Shader program parameter not found: {}", name);
            throw;
        }

        if (parameters[name].type != GL_FLOAT_MAT4)
        {
            spdlog::error("Shader program parameter type mismatch: {}", name);
            throw;
        }

        glUniformMatrix4fv(parameters[name].location, 1, GL_FALSE, &matrix[0][0]);
    }

    int32_t ShaderProgram::GetParameterLocation(const std::string &name)
    {
        return (parameters.find(name) == parameters.end()) ? -1 : parameters[name].location;
    }

    uint32_t ShaderProgram::GetParameterType(const std::string &name)
    {
        return (parameters.find(name) == parameters.end()) ? 0 : parameters[name].type;
    }

    int32_t ShaderProgram::GetAttributeLocation(const std::string &name)
    {
        return (attributes.find(name) == attributes.end()) ? -1 : attributes[name].location;
    }

    uint32_t ShaderProgram::GetAttributeType(const std::string &name)
    {
        return (attributes.find(name) == attributes.end()) ? 0 : attributes[name].type;
    }
} // namespace Lucky
