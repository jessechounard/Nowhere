#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <Lucky/Graphics/BatchRenderer.hpp>
#include <Lucky/Graphics/GraphicsDevice.hpp>
#include <Lucky/Graphics/Texture.hpp>

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        SDL_Log("Failed to initialize SDL\n\t%s\n", SDL_GetError());
        return -1;
    }

    const int windowWidth = 1920;
    const int windowHeight = 1080;

    auto attributes = Lucky::GraphicsDevice::PrepareWindowAttributes(Lucky::GraphicsAPI::OpenGL);

    SDL_Window *window = SDL_CreateWindow("Nowhere", windowWidth, windowHeight, attributes);
    if (window == NULL)
    {
        SDL_Log("Window could not be created\n\t%s\n", SDL_GetError());
    }

    auto graphicsDevice = std::make_shared<Lucky::GraphicsDevice>(
        Lucky::GraphicsAPI::OpenGL, window, Lucky::VerticalSyncType::AdaptiveEnabled);

    // debug code begin
    auto texture = std::make_shared<Lucky::Texture>(Lucky::TextureFilter::Linear, "temp.png");
    float rotation = 0;
    Lucky::BatchRenderer batchRenderer(graphicsDevice, 1024);
    // debug code end

    bool quit = false;

    double t = 0.0;
    double dt = 1 / 120.0;

    uint64_t currentTime = SDL_GetPerformanceCounter();
    double accumulator = 0.0;

    int renderCount = 0;
    int updateCount = 0;

    while (!quit)
    {
        uint64_t newTime = SDL_GetPerformanceCounter();
        double frameTime = (newTime - currentTime) / (double)SDL_GetPerformanceFrequency();

        if (frameTime > 0.25)
            frameTime = 0.25;
        currentTime = newTime;

        accumulator += frameTime;

        bool renderRequired = (accumulator >= dt);

        // Update loop
        while (accumulator >= dt && !quit)
        {
            t += dt;
            accumulator -= dt;

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    quit = true;
                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    if (event.window.windowID == SDL_GetWindowID(window))
                    {
                        quit = true;
                        break;
                    }
                    break;

                default:
                    break;
                }
            }

            if (quit)
                break;

			//game->Update(static_cast<float>(dt));
            updateCount++;

            // debug code begin
            rotation += dt;
            // debug code end
        }

        // If we updated at least once since the last render, render again
        if (renderRequired && !quit)
        {
			graphicsDevice->BeginFrame();
            graphicsDevice->ClearScreen(Lucky::Color::CornflowerBlue);

            //game->Render();
            renderCount++;

            // debug code begin
            batchRenderer.Begin(Lucky::BlendMode::Alpha, texture);

            batchRenderer.BatchQuad(nullptr, glm::vec2(1920 / 2.0f, 1080 / 2.0f), rotation,
                glm::vec2(1.0f, 1.0f), glm::vec2(0.5f, 0.5f), Lucky::UVMode::Normal,
                Lucky::Color::White);

            batchRenderer.End();
            // debug code end

            graphicsDevice->EndFrame();

            SDL_GL_SwapWindow(window);
        }
    }

    graphicsDevice.reset();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
