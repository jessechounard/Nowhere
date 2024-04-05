#pragma once

namespace Lucky
{
	struct GamepadAxis
	{
		static constexpr int LeftX = 0;
		static constexpr int LeftY = 1;
		static constexpr int RightX = 2;
		static constexpr int RightY = 3;
		static constexpr int TriggerLeft = 4;
		static constexpr int TriggerRight = 5;

		static constexpr int MaxValue = 6;
	};

	struct GamepadButton
	{
		static constexpr int A = 0;
		static constexpr int B = 1;
		static constexpr int X = 2;
		static constexpr int Y = 3;
		static constexpr int Back = 4;
		static constexpr int Guide = 5;
		static constexpr int Start = 6;
		static constexpr int LeftStick = 7;
		static constexpr int RightStick = 8;
		static constexpr int LeftShoulder = 9;
		static constexpr int RightShoulder = 10;
		static constexpr int DPadUp = 11;
		static constexpr int DPadDown = 12;
		static constexpr int DPadLeft = 13;
		static constexpr int DPadRight = 14;
		static constexpr int Misc1 = 15;
		static constexpr int Paddle1 = 16;
		static constexpr int Paddle2 = 17;
		static constexpr int Paddle3 = 18;
		static constexpr int Paddle4 = 19;
		static constexpr int Touchpad = 20;

		static constexpr int MaxValue = 21;
	};

	constexpr int MaxGamepadAxes = GamepadAxis::MaxValue;
	constexpr int MaxGamepadButtons = GamepadButton::MaxValue;

	struct GamepadState
	{
		bool isConnected;
		float axes[MaxGamepadAxes];
		bool buttons[MaxGamepadButtons];
	};

	bool FindGamepadButtonPressed(int &index, int gamepadButton);
	bool FindGamepadButtonReleased(int &index, int gamepadButton);

	const GamepadState &GetPreviousGamepadState(int index);
	const GamepadState &GetCurrentGamepadState(int index);
}
