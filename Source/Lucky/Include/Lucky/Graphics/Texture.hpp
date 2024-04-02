#pragma once

#include <stdint.h>
#include <string>

namespace Lucky
{
    enum class TextureFilter
    {
        Linear,
        Point,
    };

    enum class TextureType
    {
        Default,
        RenderTarget
    };

    struct GraphicsDevice;

    struct Texture
    {
      public:
        Texture(TextureFilter textureFilter, const std::string &filename);
        Texture(TextureFilter textureFilter, uint8_t *memory, uint32_t memoryLength);
        Texture(TextureFilter textureFilter, TextureType textureType, uint32_t width,
            uint32_t height, uint8_t *pixelData, uint32_t dataLength);
        Texture(const Texture &) = delete;
        ~Texture();

        Texture &operator=(const Texture &) = delete;

        void SetTextureData(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t *pixelData,
            uint32_t dataLength);

        TextureFilter GetTextureFilter() const
        {
            return textureFilter;
        }

        void SetTextureFilter(TextureFilter textureFilter);

        TextureType GetTextureType() const
        {
            return textureType;
        }

        uint32_t GetWidth() const
        {
            return width;
        }

        uint32_t GetHeight() const
        {
            return height;
        }

        uint32_t GetTextureId() const
        {
            return textureId;
        }

        uint32_t GetFramebufferId() const
        {
            return fbo;
        }

      private:
        void Initialize(TextureFilter textureFilter, TextureType textureType, uint32_t width,
            uint32_t height, uint8_t *pixelData, uint32_t dataLength);

        TextureFilter textureFilter = TextureFilter::Linear;
        TextureType textureType = TextureType::Default;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t textureId = 0;
        uint32_t fbo = 0;
    };
} // namespace Lucky
