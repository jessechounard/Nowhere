#pragma once

#include <map>
#include <memory>
#include <stdint.h>
#include <string>

#include <glm/glm.hpp>

namespace Lucky
{
    struct ShaderAttributeLocation
    {
        static const int Position = 0;
        static const int Color = 1;
        static const int TexCoord = 2;
    };

    struct GraphicsDevice;
    struct Texture;

    struct VertexShader
    {
      public:
        VertexShader(const uint8_t *source, uint32_t sourceLength);
        VertexShader(const VertexShader &) = delete;
        ~VertexShader();

        VertexShader &operator=(const VertexShader &) = delete;

        uint32_t GetShaderId() const
        {
            return id;
        }

      private:
        uint32_t id;
    };

    struct FragmentShader
    {
      public:
        FragmentShader(const uint8_t *source, uint32_t sourceLength);
        FragmentShader(const FragmentShader &) = delete;
        ~FragmentShader();

        FragmentShader &operator=(const FragmentShader &) = delete;

        uint32_t GetShaderId() const
        {
            return id;
        }

      private:
        uint32_t id;
    };

    struct ShaderProgram
    {
      public:
        ShaderProgram(std::shared_ptr<GraphicsDevice> graphicsDevice, VertexShader &vertexShader,
            FragmentShader &fragmentShader);
        ShaderProgram(const ShaderProgram &) = delete;
        ~ShaderProgram();

        ShaderProgram &operator=(const ShaderProgram &) = delete;

        // todo: store the parameter values, so we can query them
        void SetParameter(const std::string &name, const Texture &texture, int slotNumber);
        void SetParameter(const std::string &name, const glm::mat4 &matrix);
        // todo: other parameter types

        uint32_t GetShaderId() const
        {
            return id;
        }

        int32_t GetParameterLocation(const std::string &parameter);
        uint32_t GetParameterType(const std::string &parameter);

        int32_t GetAttributeLocation(const std::string &attribute);
        uint32_t GetAttributeType(const std::string &attribute);

      private:
        struct ShaderParameter
        {
            int32_t location;
            uint32_t type;
        };

        struct ShaderAttribute
        {
            int32_t location;
            uint32_t type;
        };

        std::shared_ptr<GraphicsDevice> graphicsDevice;
        std::map<std::string, ShaderParameter> parameters;
        std::map<std::string, ShaderAttribute> attributes;
        uint32_t id;
    };
} // namespace Lucky