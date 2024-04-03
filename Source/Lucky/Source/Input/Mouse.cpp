#include <cassert>

#include <Lucky/Input/Mouse.hpp>

const Lucky::MouseState &GetPreviousMouseState_impl();
const Lucky::MouseState &GetCurrentMouseState_impl();

namespace Lucky
{
    const MouseState &GetPreviousMouseState()
    {
        return GetPreviousMouseState_impl();
    }

    const MouseState &GetCurrentMouseState()
    {
        return GetCurrentMouseState_impl();
    }

    bool MouseButtonPressed(int mouseButton)
    {
        assert(mouseButton > 0 && mouseButton < MaxMouseButtons);

        auto &ps = GetPreviousMouseState();
        auto &cs = GetCurrentMouseState();

        return ps.buttons[mouseButton] == false && cs.buttons[mouseButton] == true;
    }

    bool MouseButtonReleased(int mouseButton)
    {
        assert(mouseButton > 0 && mouseButton < MaxMouseButtons);

        auto &ps = GetPreviousMouseState();
        auto &cs = GetCurrentMouseState();

        return ps.buttons[mouseButton] == true && cs.buttons[mouseButton] == false;
    }
} // namespace Lucky