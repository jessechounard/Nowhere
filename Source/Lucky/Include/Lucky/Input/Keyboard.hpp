#pragma once

namespace Lucky
{
    struct KeyboardKey
    {
        static constexpr int A = 4;
        static constexpr int B = 5;
        static constexpr int C = 6;
        static constexpr int D = 7;
        static constexpr int E = 8;
        static constexpr int F = 9;
        static constexpr int G = 10;
        static constexpr int H = 11;
        static constexpr int I = 12;
        static constexpr int J = 13;
        static constexpr int K = 14;
        static constexpr int L = 15;
        static constexpr int M = 16;
        static constexpr int N = 17;
        static constexpr int O = 18;
        static constexpr int P = 19;
        static constexpr int Q = 20;
        static constexpr int R = 21;
        static constexpr int S = 22;
        static constexpr int T = 23;
        static constexpr int U = 24;
        static constexpr int V = 25;
        static constexpr int W = 26;
        static constexpr int X = 27;
        static constexpr int Y = 28;
        static constexpr int Z = 29;
        static constexpr int D1 = 30;
        static constexpr int D2 = 31;
        static constexpr int D3 = 32;
        static constexpr int D4 = 33;
        static constexpr int D5 = 34;
        static constexpr int D6 = 35;
        static constexpr int D7 = 36;
        static constexpr int D8 = 37;
        static constexpr int D9 = 38;
        static constexpr int D0 = 39;
        static constexpr int Return = 40;
        static constexpr int Escape = 41;
        static constexpr int Backspace = 42;
        static constexpr int Tab = 43;
        static constexpr int Space = 44;
        static constexpr int Minus = 45;
        static constexpr int Equals = 46;
        static constexpr int LeftBracket = 47;
        static constexpr int RightBracket = 48;
        static constexpr int BackSlash = 49;
        static constexpr int NonUSHash = 50;
        static constexpr int Semicolon = 51;
        static constexpr int Apostrophe = 52;
        static constexpr int Grave = 53;
        static constexpr int Comma = 54;
        static constexpr int Period = 55;
        static constexpr int Slash = 56;
        static constexpr int CapsLock = 57;
        static constexpr int F1 = 58;
        static constexpr int F2 = 59;
        static constexpr int F3 = 60;
        static constexpr int F4 = 61;
        static constexpr int F5 = 62;
        static constexpr int F6 = 63;
        static constexpr int F7 = 64;
        static constexpr int F8 = 65;
        static constexpr int F9 = 66;
        static constexpr int F10 = 67;
        static constexpr int F11 = 68;
        static constexpr int F12 = 69;
        static constexpr int PrintScreen = 70;
        static constexpr int ScrollLock = 71;
        static constexpr int Pause = 72;
        static constexpr int Insert = 73;
        static constexpr int Home = 74;
        static constexpr int PageUp = 75;
        static constexpr int Delete = 76;
        static constexpr int End = 77;
        static constexpr int PageDown = 78;
        static constexpr int Right = 79;
        static constexpr int Left = 80;
        static constexpr int Down = 81;
        static constexpr int Up = 82;
        static constexpr int NumlockClear = 83;
        static constexpr int KP_Divide = 84;
        static constexpr int KP_Multiply = 85;
        static constexpr int KP_Minus = 86;
        static constexpr int KP_Plus = 87;
        static constexpr int KP_Enter = 88;
        static constexpr int KP_1 = 89;
        static constexpr int KP_2 = 90;
        static constexpr int KP_3 = 91;
        static constexpr int KP_4 = 92;
        static constexpr int KP_5 = 93;
        static constexpr int KP_6 = 94;
        static constexpr int KP_7 = 95;
        static constexpr int KP_8 = 96;
        static constexpr int KP_9 = 97;
        static constexpr int KP_0 = 98;
        static constexpr int KP_Period = 99;
        static constexpr int NonUSBackSlash = 100;
        static constexpr int Application = 101;
        static constexpr int Power = 102;
        static constexpr int KP_Equals = 103;
        static constexpr int F13 = 104;
        static constexpr int F14 = 105;
        static constexpr int F15 = 106;
        static constexpr int F16 = 107;
        static constexpr int F17 = 108;
        static constexpr int F18 = 109;
        static constexpr int F19 = 110;
        static constexpr int F20 = 111;
        static constexpr int F21 = 112;
        static constexpr int F22 = 113;
        static constexpr int F23 = 114;
        static constexpr int F24 = 115;
        static constexpr int Execute = 116;
        static constexpr int Help = 117;
        static constexpr int Menu = 118;
        static constexpr int Select = 119;
        static constexpr int Stop = 120;
        static constexpr int Redo = 121;
        static constexpr int Undo = 122;
        static constexpr int Cut = 123;
        static constexpr int Copy = 124;
        static constexpr int Paste = 125;
        static constexpr int Find = 126;
        static constexpr int Mute = 127;
        static constexpr int VolumeUp = 128;
        static constexpr int VolumeDown = 129;
        static constexpr int KP_Comma = 133;
        static constexpr int KP_EqualsAs400 = 134;
        static constexpr int International1 = 135;
        static constexpr int International2 = 136;
        static constexpr int International3 = 137;
        static constexpr int International4 = 138;
        static constexpr int International5 = 139;
        static constexpr int International6 = 140;
        static constexpr int International7 = 141;
        static constexpr int International8 = 142;
        static constexpr int International9 = 143;
        static constexpr int Language1 = 144;
        static constexpr int Language2 = 145;
        static constexpr int Language3 = 146;
        static constexpr int Language4 = 147;
        static constexpr int Language5 = 148;
        static constexpr int Language6 = 149;
        static constexpr int Language7 = 150;
        static constexpr int Language8 = 151;
        static constexpr int Language9 = 152;
        static constexpr int AltErase = 153;
        static constexpr int SysReq = 154;
        static constexpr int Cancel = 155;
        static constexpr int clear = 156;
        static constexpr int Prior = 157;
        static constexpr int Return2 = 158;
        static constexpr int Separator = 159;
        static constexpr int Out = 160;
        static constexpr int Oper = 161;
        static constexpr int ClearAgain = 162;
        static constexpr int CRSEL = 163;
        static constexpr int EXSEL = 164;
        static constexpr int KP_00 = 176;
        static constexpr int KP_000 = 177;
        static constexpr int ThousandsSeparator = 178;
        static constexpr int DecimalSeparator = 179;
        static constexpr int CurrencyUnit = 180;
        static constexpr int CurrencySubUnit = 181;
        static constexpr int KP_LeftParen = 182;
        static constexpr int KP_RightParent = 183;
        static constexpr int KP_LeftBrace = 184;
        static constexpr int KP_RightBrace = 185;
        static constexpr int KP_Tab = 186;
        static constexpr int KP_BackSpace = 187;
        static constexpr int KP_A = 188;
        static constexpr int KP_B = 189;
        static constexpr int KP_C = 190;
        static constexpr int KP_D = 191;
        static constexpr int KP_E = 192;
        static constexpr int KP_F = 193;
        static constexpr int KP_XOR = 194;
        static constexpr int KP_Power = 195;
        static constexpr int KP_Percent = 196;
        static constexpr int KP_Less = 197;
        static constexpr int KP_Greater = 198;
        static constexpr int KP_Ampersand = 199;
        static constexpr int KP_DoubleAmpersand = 200;
        static constexpr int KP_VerticalBar = 201;
        static constexpr int KP_DoubleVerticalBar = 202;
        static constexpr int KP_Colon = 203;
        static constexpr int KP_Hash = 204;
        static constexpr int KP_Space = 205;
        static constexpr int KP_At = 206;
        static constexpr int KP_EXCLAM = 207;
        static constexpr int KP_MemStore = 208;
        static constexpr int KP_MemRecall = 209;
        static constexpr int KP_MemClear = 210;
        static constexpr int KP_MemAdd = 211;
        static constexpr int KP_MemSubstract = 212;
        static constexpr int KP_MemMultiply = 213;
        static constexpr int KP_MemDivide = 214;
        static constexpr int KP_PlusMinus = 215;
        static constexpr int KP_Clear = 216;
        static constexpr int KP_ClearEntry = 217;
        static constexpr int KP_Binary = 218;
        static constexpr int KP_Octal = 219;
        static constexpr int KP_Decimal = 220;
        static constexpr int KP_Hexadecimal = 221;
        static constexpr int LeftControl = 224;
        static constexpr int LeftShift = 225;
        static constexpr int LeftAlt = 226;
        static constexpr int LeftGui = 227;
        static constexpr int RightControl = 228;
        static constexpr int RightShift = 229;
        static constexpr int RightAlt = 230;
        static constexpr int RightGui = 231;

        static constexpr int MaxValue = 512;
    };

    constexpr int MaxKeyboardKeys = KeyboardKey::MaxValue;

    struct KeyboardState
    {
        bool keys[MaxKeyboardKeys];
    };

    const KeyboardState &GetPreviousKeyboardState();
    const KeyboardState &GetCurrentKeyboardState();
} // namespace Lucky