#include <Lucky/Input/Gamepad.hpp>

bool FindGamepadButtonPressed_impl(int &index, int gamepadButton);
bool FindGamepadButtonReleased_impl(int &index, int gamepadButton);
const Lucky::GamepadState &GetPreviousGamepadState_impl(int index);
const Lucky::GamepadState &GetCurrentGamepadState_impl(int index);

namespace Lucky
{
	bool FindGamepadButtonPressed(int &index, int gamepadButton)
	{
		return FindGamepadButtonPressed_impl(index, gamepadButton);
	}

	bool FindGamepadButtonReleased(int &index, int gamepadButton)
	{
		return FindGamepadButtonReleased_impl(index, gamepadButton);
	}

	const GamepadState &GetPreviousGamepadState(int index)
	{
		return GetPreviousGamepadState_impl(index);
	}

	const GamepadState &GetCurrentGamepadState(int index)
	{
		return GetCurrentGamepadState_impl(index);
	}
}