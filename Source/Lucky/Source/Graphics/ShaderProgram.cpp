#include <assert.h>
#include <stdio.h>

#include <spdlog/spdlog.h>

#include <Lucky/Graphics/GraphicsDevice.hpp>
#include <Lucky/Graphics/ShaderProgram.hpp>
#include <Lucky/Graphics/Texture.hpp>

#include "IncludeOpenGL.h"

namespace Lucky
{
    const int ShaderAttributeLocation::Position;
    const int ShaderAttributeLocation::Color;
    const int ShaderAttributeLocation::TexCoord;

    VertexShader::VertexShader(const uint8_t *source, uint32_t sourceLength)
    {
        assert(source != nullptr);

        id = glCreateShader(GL_VERTEX_SHADER);
        if (id == 0)
        {
            spdlog::error("Failed to create OpenGL vertex shader id.");
            throw;
        }

        glShaderSource(id, 1, (GLchar**)&source, (GLint*)&sourceLength);
        glCompileShader(id);

        int status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            char infoLog[1024];
            int logLength;
            glGetShaderInfoLog(id, 1024, &logLength, infoLog);
            glDeleteShader(id);

            spdlog::error("Vertex shader compilation failed\n");
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

        glShaderSource(id, 1, (GLchar **)&source, (GLint*)&sourceLength);
        glCompileShader(id);

        int status;
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE)
        {
            char infoLog[1024];
            int logLength;
            glGetShaderInfoLog(id, 1024, &logLength, infoLog);
            glDeleteShader(id);

            spdlog::error("Fragment shader compilation failed");
            throw;
        }
    }

    FragmentShader::~FragmentShader()
    {
        glDeleteShader(id);
    }

    ShaderProgram::ShaderProgram(std::shared_ptr<GraphicsDevice> graphicsDevice, VertexShader &vertexShader, FragmentShader &fragmentShader)
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

        glBindAttribLocation(id, ShaderAttributeLocation::Position, "position");
        glBindAttribLocation(id, ShaderAttributeLocation::Color, "color");
        glBindAttribLocation(id, ShaderAttributeLocation::TexCoord, "texcoord");

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
            parameters[name] = { glGetUniformLocation(id, name), type };
        }

        if (parameters.find("TextureSampler") == parameters.end() ||
            parameters["TextureSampler"].type != GL_SAMPLER_2D)
        {
            spdlog::error("Shader program missing required parameter: {}", "TextureSampler");
            throw;
        }
        if (parameters.find("ProjectionMatrix") == parameters.end() ||
            parameters["ProjectionMatrix"].type != GL_FLOAT_MAT4)
        {
            spdlog::error("Shader program missing required parameter: {}", "ProjectionMatrix");
            throw;
        }

        int attributeCount;
        glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &attributeCount);

        for (int i = 0; i < attributeCount; i++)
        {
            int size;
            GLenum type;
            GLchar name[256];

            glGetActiveAttrib(id, i, 256, nullptr, &size, &type, name);
            attributes[name] = { glGetAttribLocation(id, name), type };
        }

        if (attributes.find("position") == attributes.end() ||
            attributes["position"].location != ShaderAttributeLocation::Position ||
            attributes["position"].type != GL_FLOAT_VEC4)
        {
            spdlog::error("Shader program missing required attribute: {}", "position");
            throw;
        }
        if (attributes.find("color") == attributes.end() ||
            attributes["color"].location != ShaderAttributeLocation::Color ||
            attributes["color"].type != GL_FLOAT_VEC4)
        {
            spdlog::error("Shader program missing required attribute: {}", "color");
            throw;
        }
        if (attributes.find("texcoord") == attributes.end() ||
            attributes["texcoord"].location != ShaderAttributeLocation::TexCoord ||
            attributes["texcoord"].type != GL_FLOAT_VEC2)
        {
            spdlog::error("Shader program missing required attribute: {}", "texcoord");
            throw;
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
        assert(name.length() > 0);

        if (parameters.find(name) == parameters.end())
        {
            spdlog::error("Shader program parameter not found: {}", name);
            throw;
        }

        return parameters[name].location;
    }

    uint32_t ShaderProgram::GetParameterType(const std::string &name)
    {
        assert(name.length() > 0);

        if (parameters.find(name) == parameters.end())
        {
            spdlog::error("Shader program parameter not found: {}", name);
            throw;
        }

        return parameters[name].type;
    }

    int32_t ShaderProgram::GetAttributeLocation(const std::string &name)
    {
        assert(name.length() > 0);

        if (attributes.find(name) == attributes.end())
        {
            spdlog::error("Shader program attribute not found: {}", name);
            throw;
        }

        return attributes[name].type;
    }

    uint32_t ShaderProgram::GetAttributeType(const std::string &name)
    {
        assert(name.length() > 0);

        if (attributes.find(name) == attributes.end())
        {
            spdlog::error("Shader program attribute not found: {}", name);
            throw;
        }

        return attributes[name].type;
    }
}
