#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <stb_rect_pack.h>
#include <stb_truetype.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <Lucky/Audio/AudioPlayer.hpp>
#include <Lucky/Audio/Sound.hpp>
#include <Lucky/Audio/Stream.hpp>
#include <Lucky/Graphics/BatchRenderer.hpp>
#include <Lucky/Graphics/Font.hpp>
#include <Lucky/Graphics/GraphicsDevice.hpp>
#include <Lucky/Graphics/Texture.hpp>
#include <Lucky/Input/Keyboard.hpp>
#include <Lucky/Input/Mouse.hpp>

std::unique_ptr<Lucky::BatchRenderer> batchRenderer;
std::shared_ptr<Lucky::Texture> white;
std::shared_ptr<Lucky::Font> testFont;

void DebugCodeInit(std::shared_ptr<Lucky::GraphicsDevice> graphicsDevice)
{
    auto devices = Lucky::AudioPlayer::GetAudioOutputDevices();

    for (const auto &device : devices)
    {
        printf("%s, %d\n", device.name.c_str(), device.deviceId);
    }

    batchRenderer = std::make_unique<Lucky::BatchRenderer>(graphicsDevice, 1024);

    white = std::make_shared<Lucky::Texture>(Lucky::TextureFilter::Point, "white.png");

    testFont = std::make_shared<Lucky::Font>("arial.ttf");
    int codePoints[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
        't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};

    testFont->CreateFontEntry("asdf", 128, codePoints, sizeof(codePoints), 3);
}

void DebugCodeCleanup()
{
    testFont.reset();

    batchRenderer.reset();
}

void DebugCodeUpdate(double t)
{
}

void DebugCodeRender()
{
    batchRenderer->Begin(Lucky::BlendMode::Alpha, white);
    batchRenderer->BatchQuad(nullptr, glm::vec2(0.0f, 540.0f), 0.0f, glm::vec2(1920.0f, 540.0f), glm::vec2(0.0f, 0.0f),
        Lucky::UVMode::Normal, Lucky::Color::White);
    batchRenderer->End();

    auto texture = testFont->GetTexture("asdf");
    batchRenderer->Begin(Lucky::BlendMode::Alpha, texture);
    testFont->DrawString(*batchRenderer, "abcdefghijklmnopqrstuvwxyz", "asdf", 20, 540);
    testFont->DrawString(*batchRenderer, "Te VAW", "asdf", 20, 690);
    batchRenderer->End();
}

Lucky::KeyboardState previousKeyboardState, currentKeyboardState;
Lucky::MouseState previousMouseState, currentMouseState;

void InitializeImGui(SDL_Window *window, void *glContext)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(nullptr);
}

void RenderImGui(SDL_Window *window, void *glContext)
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void ShowFPSOverlay(int updateCount, int renderCount)
{
    const float DISTANCE = 10.0f;
    static int corner = 2;
    ImGuiIO &io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                    ImGuiWindowFlags_NoNav;
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    bool temp = true;
    if (ImGui::Begin("FPS Overlay", &temp, window_flags))
    {
        ImGui::Text("FPS");
        ImGui::Text("Update: %d", updateCount);
        ImGui::Text("Render: %d", renderCount);
    }
    ImGui::End();
}

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    memset(&currentKeyboardState, 0, sizeof(currentKeyboardState));
    memset(&previousKeyboardState, 0, sizeof(previousKeyboardState));
    memset(&previousMouseState, 0, sizeof(currentMouseState));
    memset(&currentMouseState, 0, sizeof(currentMouseState));

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

    InitializeImGui(window, graphicsDevice->GetGLContext());

    DebugCodeInit(graphicsDevice);

    bool quit = false;

    double t = 0.0;
    double dt = 1 / 120.0;

    uint64_t currentTime = SDL_GetPerformanceCounter();
    double accumulator = 0.0;

    double fpsTimer = 0;
    int renderCount = 0, updateCount = 0;
    int guiUpdateCount = 0, guiRenderCount = 0;

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
            previousKeyboardState = currentKeyboardState;
            previousMouseState = currentMouseState;

            t += dt;
            accumulator -= dt;

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
                auto &io = ImGui::GetIO();

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

                case SDL_EVENT_KEY_DOWN:
                    if (io.WantCaptureKeyboard)
                    {
                        break;
                    }
                    if (event.key.keysym.scancode >= 0 && event.key.keysym.scancode < Lucky::MaxKeyboardKeys)
                    {
                        currentKeyboardState.keys[event.key.keysym.scancode] = true;
                    }

                    break;

                case SDL_EVENT_KEY_UP:
                    if (io.WantCaptureKeyboard)
                    {
                        break;
                    }
                    if (event.key.keysym.scancode >= 0 && event.key.keysym.scancode < Lucky::MaxKeyboardKeys)
                    {
                        currentKeyboardState.keys[event.key.keysym.scancode] = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    if (event.button.button > 0 && event.button.button < Lucky::MaxMouseButtons)
                    {
                        currentMouseState.buttons[event.button.button] = true;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    if (event.button.button > 0 && event.button.button < Lucky::MaxMouseButtons)
                    {
                        currentMouseState.buttons[event.button.button] = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    currentMouseState.x = event.motion.x;
                    currentMouseState.y = event.motion.y;
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    if (event.wheel.direction != SDL_MOUSEWHEEL_FLIPPED)
                    {
                        currentMouseState.wheelDelta += event.wheel.y;
                    }
                    else
                    {
                        currentMouseState.wheelDelta -= event.wheel.y;
                    }
                    break;

                default:
                    break;
                }
            }

            if (quit)
                break;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            // imgui stuff here
            ShowFPSOverlay(guiUpdateCount, guiRenderCount);

            ImGui::EndFrame();

            // todo: Update code goes here
            DebugCodeUpdate(dt);
            // game->Update(static_cast<float>(dt));

            updateCount++;

            fpsTimer += dt;
            if (Lucky::ApproximatelyEqual((float)fpsTimer, 1.0f) || fpsTimer > 1.0f)
            {
                guiUpdateCount = updateCount;
                guiRenderCount = renderCount;
                updateCount = renderCount = 0;
                fpsTimer = 0;
            }
        }

        // If we updated at least once since the last render, render again
        if (renderRequired && !quit)
        {
            graphicsDevice->BeginFrame();

            graphicsDevice->ClearScreen(Lucky::Color::CornflowerBlue);

            // todo: render code goes ehre
            DebugCodeRender();

            // game->Render();

            renderCount++;

            graphicsDevice->EndFrame();

            RenderImGui(window, graphicsDevice->GetGLContext());

            SDL_GL_SwapWindow(window);
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    DebugCodeCleanup();

    graphicsDevice.reset();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

const Lucky::KeyboardState &GetPreviousKeyboardState_impl()
{
    return previousKeyboardState;
}

const Lucky::KeyboardState &GetCurrentKeyboardState_impl()
{
    return currentKeyboardState;
}

const Lucky::MouseState &GetPreviousMouseState_impl()
{
    return previousMouseState;
}

const Lucky::MouseState &GetCurrentMouseState_impl()
{
    return currentMouseState;
}
