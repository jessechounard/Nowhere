#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <stb_rect_pack.h>
#include <stb_truetype.h>

#include <Lucky/Graphics/BatchRenderer.hpp>
#include <Lucky/Graphics/Texture.hpp>

namespace Lucky
{
    class Font
    {
      public:
        Font(const std::string &fileName);
        Font(void *memory);
        ~Font();

        // todo: future optimization - allow packing multiple sizes into one texture
        // todo: do we need to specify a maximum texture size?
        std::shared_ptr<Texture> CreateFontEntry(
            const std::string &entryName, const float fontSize, int *codePoints, int pointCount, uint32_t oversampling = 1, bool kerningEnabled = true);
        std::shared_ptr<Texture> GetTexture(const std::string &entryName);

        void DrawString(BatchRenderer &batchRenderer, const std::string &text, const std::string &entryName,
            const float x, const float y);

      private:
        void Initialize();

        void *fontMemoryBuffer;
        bool shouldFreeMemory;

        stbtt_fontinfo fontInfo;

        struct FontEntry
        {
            float size;
            float scaleFactor;
            std::vector<stbtt_packedchar> packedData;
            std::vector<int> codePoints;
            std::shared_ptr<Lucky::Texture> texture;
            std::map<std::pair<int, int>, int> kerning;
        };

        std::map<std::string, FontEntry> fontEntries;
    };
} // namespace Lucky