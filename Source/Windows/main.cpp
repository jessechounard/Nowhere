#include <deque>
#include <map>

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_gamepad.h>

#include <stb_rect_pack.h>
#include <stb_truetype.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl3.h>

#include <Lucky/Audio/AudioPlayer.hpp>
#include <Lucky/Audio/Sound.hpp>
#include <Lucky/Audio/Stream.hpp>
#include <Lucky/Graphics/BatchRenderer.hpp>
#include <Lucky/Graphics/BloomEffect.hpp>
#include <Lucky/Graphics/Font.hpp>
#include <Lucky/Graphics/GraphicsDevice.hpp>
#include <Lucky/Graphics/Texture.hpp>
#include <Lucky/Input/Gamepad.hpp>
#include <Lucky/Input/Keyboard.hpp>
#include <Lucky/Input/Mouse.hpp>

// #define BORDERLESS

std::unique_ptr<Lucky::BatchRenderer> batchRenderer;
std::shared_ptr<Lucky::GraphicsDevice> graphicsDevice;

std::shared_ptr<Lucky::Texture> white;
std::shared_ptr<Lucky::Texture> renderTarget;

std::shared_ptr<Lucky::ShaderProgram> circleShader;

glm::vec3 mousePosition = glm::vec3(0.0f, 0.0f, 0.0f);
float time = 0;

void DebugCodeInit(std::shared_ptr<Lucky::GraphicsDevice> graphicsDevice)
{
    batchRenderer = std::make_unique<Lucky::BatchRenderer>(graphicsDevice, 1024);

    white = std::make_shared<Lucky::Texture>("white.png", Lucky::TextureFilter::Point);

    Lucky::VertexShader defaultVertexShader("BatchRenderer.vert");
    circleShader = std::make_shared<Lucky::ShaderProgram>(
        graphicsDevice, defaultVertexShader, Lucky::FragmentShader("Circle.frag"));

    renderTarget = std::make_shared<Lucky::Texture>(Lucky::TextureType::RenderTarget, 1920, 1080, nullptr, 0,
        Lucky::TextureFilter::Linear, Lucky::TextureFormat::HDR);
}

void DebugCodeCleanup()
{
    batchRenderer.reset();
}

void DebugCodeUpdate(double t)
{
    auto &mouseState = Lucky::GetMouseState();
    mousePosition.x = (float)mouseState.x;
    mousePosition.y = (float)mouseState.y;
    mousePosition.z = mouseState.buttons[Lucky::MouseButton::Left] ? 1.0f : 0.0f;

    time += (float)t;
}

void DebugCodeRender()
{
    graphicsDevice->BindRenderTarget(*renderTarget);
    graphicsDevice->ClearScreen(Lucky::Color::Black);

    batchRenderer->Begin(Lucky::BlendMode::Alpha, nullptr, circleShader);
    circleShader->SetParameter("Dimensions", 1920.0f, 1080.0f);
    circleShader->SetParameter("Radius", 100.0f);
    circleShader->SetParameter("Border", 10.0f);
    batchRenderer->BatchQuadUV(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0, 0.0f),
        glm::vec2(1920.0f, 1080.0f), Lucky::Color::White);
    batchRenderer->End();

    graphicsDevice->UnbindRenderTarget();

    batchRenderer->Begin(Lucky::BlendMode::None, renderTarget);
    batchRenderer->BatchQuadUV(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f),
        glm::vec2(1920.0f, 1080.0f), Lucky::Color::White);
    batchRenderer->End();
}

Lucky::KeyboardState keyboardState;
Lucky::MouseState mouseState;

std::deque<Lucky::GamepadEvent> gamepadEvents;
std::map<SDL_JoystickID, Lucky::GamepadState> gamepadStates;

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

void GetInitialGamepadState(SDL_Gamepad *gamepad, bool *buttons, int buttonCount, int16_t *axes, int axisCount)
{
    for (int i = 0; i < buttonCount; i++)
    {
        buttons[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);
    }
    for (int i = 0; i < axisCount; i++)
    {
        axes[i] = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);
    }
}

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    memset(&keyboardState, 0, sizeof(keyboardState));
    memset(&mouseState, 0, sizeof(mouseState));

    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_TIMER) < 0)
    {
        SDL_Log("Failed to initialize SDL\n\t%s\n", SDL_GetError());
        return -1;
    }

    const int windowWidth = 1920;
    const int windowHeight = 1080;

    auto attributes = Lucky::GraphicsDevice::PrepareWindowAttributes(Lucky::GraphicsAPI::OpenGL);

    SDL_Window *window = SDL_CreateWindow("Nowhere", windowWidth, windowHeight,
        attributes
#ifdef BORDERLESS
            | SDL_WINDOW_BORDERLESS
#endif
    );
    if (window == NULL)
    {
        SDL_Log("Window could not be created\n\t%s\n", SDL_GetError());
    }

    graphicsDevice = std::make_shared<Lucky::GraphicsDevice>(
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
            gamepadEvents.clear();

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
                        keyboardState.keys[event.key.keysym.scancode] = true;
                    }

                    break;

                case SDL_EVENT_KEY_UP:
                    if (io.WantCaptureKeyboard)
                    {
                        break;
                    }
                    if (event.key.keysym.scancode >= 0 && event.key.keysym.scancode < Lucky::MaxKeyboardKeys)
                    {
                        keyboardState.keys[event.key.keysym.scancode] = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    if (event.button.button > 0 && event.button.button < Lucky::MaxMouseButtons)
                    {
                        mouseState.buttons[event.button.button] = true;
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    if (event.button.button > 0 && event.button.button < Lucky::MaxMouseButtons)
                    {
                        mouseState.buttons[event.button.button] = false;
                    }
                    break;

                case SDL_EVENT_MOUSE_MOTION:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    mouseState.x = event.motion.x;
                    mouseState.y = event.motion.y;
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }
                    if (event.wheel.direction != SDL_MOUSEWHEEL_FLIPPED)
                    {
                        mouseState.wheelDelta += event.wheel.y;
                    }
                    else
                    {
                        mouseState.wheelDelta -= event.wheel.y;
                    }
                    break;

                case SDL_EVENT_GAMEPAD_ADDED: {
                    printf("added\n");
                    Lucky::GamepadState gamepadState;
                    gamepadState.gamepad = SDL_OpenGamepad(event.gdevice.which);
                    if (!gamepadState.gamepad)
                    {
                        // todo: handle this error
                        break;
                    }
                    GetInitialGamepadState(gamepadState.gamepad, gamepadState.buttons, SDL_GAMEPAD_BUTTON_MAX - 1,
                        gamepadState.axes, SDL_GAMEPAD_AXIS_MAX - 1);
                    gamepadStates[event.gdevice.which] = gamepadState;
                    break;
                }

                case SDL_EVENT_GAMEPAD_REMOVED:
                    printf("removed\n");
                    SDL_CloseGamepad(gamepadStates[event.gdevice.which].gamepad);
                    gamepadStates.erase(event.gdevice.which);
                    break;

                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    printf("button down\n");
                    gamepadEvents.emplace_back((SDL_JoystickID)event.gbutton.which,
                        Lucky::GamepadEventType::ButtonPressed, (SDL_GamepadButton)event.gbutton.button);
                    gamepadStates[event.gbutton.which].buttons[event.gbutton.button] = true;
                    break;

                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    printf("button up\n");
                    gamepadEvents.emplace_back((SDL_JoystickID)event.gbutton.which,
                        Lucky::GamepadEventType::ButtonReleased, (SDL_GamepadButton)event.gbutton.button);
                    gamepadStates[event.gbutton.which].buttons[event.gbutton.button] = false;
                    break;

                case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                    printf("axis motion\n");
                    gamepadStates[event.gaxis.which].buttons[event.gaxis.axis] = event.gaxis.value;
                    break;

                case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
                    // todo:
                    // event->gtouchpad
                    break;

                case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
                    // todo:
                    // event->gtouchpad
                    break;

                case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
                    // todo:
                    // event->gtouchpad
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

const Lucky::KeyboardState &GetKeyboardState_impl()
{
    return keyboardState;
}

const Lucky::MouseState &GetMouseState_impl()
{
    return mouseState;
}

bool GetGamepadEvent_impl(Lucky::GamepadEvent *event)
{
    if (gamepadEvents.empty() || event == nullptr)
    {
        return false;
    }

    *event = gamepadEvents.front();
    gamepadEvents.pop_front();

    return true;
}

SDL_Gamepad *GetGamepadFromJoystickId_impl(SDL_JoystickID joystickId)
{
    auto it = gamepadStates.find(joystickId);
    if (it == gamepadStates.end())
    {
        return nullptr;
    }

    return it->second.gamepad;
}

bool GetGamepadState_impl(SDL_JoystickID joystickId, Lucky::GamepadState *gamepadState)
{
    auto it = gamepadStates.find(joystickId);
    if (it == gamepadStates.end())
    {
        return false;
    }

    *gamepadState = it->second;
    return true;
}
