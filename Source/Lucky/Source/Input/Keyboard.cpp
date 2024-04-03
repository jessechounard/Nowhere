#include <Lucky/Input/Keyboard.hpp>

const Lucky::KeyboardState &GetPreviousKeyboardState_impl();
const Lucky::KeyboardState &GetCurrentKeyboardState_impl();

namespace Lucky
{
    const KeyboardState &GetPreviousKeyboardState()
    {
        return GetPreviousKeyboardState_impl();
    }

    const KeyboardState &GetCurrentKeyboardState()
    {
        return GetCurrentKeyboardState_impl();
    }
} // namespace Lucky