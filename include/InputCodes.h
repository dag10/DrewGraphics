//
//  InputCodes.h
//

#pragma once

#if defined(_DIRECTX)
#include <Windows.h>
#endif

namespace dg {

#if defined(_OPENGL)

  // Values copied from GLFW/glfw3.h
  enum class Key : int32_t {
    UNKNOWN = -1,

    SPACE = 32,
    APOSTROPHE = 39,
    COMMA = 44,
    MINUS = 45,
    PERIOD = 46,
    SLASH = 47,
    NUM_0 = 48,
    NUM_1 = 49,
    NUM_2 = 50,
    NUM_3 = 51,
    NUM_4 = 52,
    NUM_5 = 53,
    NUM_6 = 54,
    NUM_7 = 55,
    NUM_8 = 56,
    NUM_9 = 57,
    SEMICOLON = 59,
    EQUAL = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LEFT_BRACKET = 91,
    BACKSLASH = 92,
    RIGHT_BRACKET = 93,
    GRAVE_ACCENT = 96,
    WORLD_1 = 16,
    WORLD_2 = 16,
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE_ = 261,
    RIGHT = 262,
    LEFT = 263,
    DOWN = 264,
    UP = 265,
    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,
    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    KP_0 = 320,
    KP_1 = 321,
    KP_2 = 322,
    KP_3 = 323,
    KP_4 = 324,
    KP_5 = 325,
    KP_6 = 326,
    KP_7 = 327,
    KP_8 = 328,
    KP_9 = 329,
    KP_DECIMAL = 330,
    KP_DIVIDE = 331,
    KP_MULTIPLY = 332,
    KP_SUBTRACT = 333,
    KP_ADD = 334,
    KP_ENTER = 335,
    KP_EQUAL = 336,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348,

    SHIFT,   // Set manually
    CONTROL, // Set manually

    LAST, // Bookend
  };

  enum class MouseButton {
    BUTTON_1 = 0,
    BUTTON_2 = 1,
    BUTTON_3 = 2,
    BUTTON_4 = 3,
    BUTTON_5 = 4,
    BUTTON_6 = 5,
    BUTTON_7 = 6,
    BUTTON_8 = 7,

    LAST, // Bookend
  };

  const MouseButton BUTTON_LEFT   = MouseButton::BUTTON_1;
  const MouseButton BUTTON_RIGHT  = MouseButton::BUTTON_2;
  const MouseButton BUTTON_MIDDLE = MouseButton::BUTTON_3;

#elif defined _DIRECTX

  // TODO: Populate these with DirectInput values.
  enum class Key {
    UNKNOWN = -1,

    SPACE = VK_SPACE,
    APOSTROPHE = VK_OEM_7,
    COMMA = VK_OEM_COMMA,
    MINUS = VK_OEM_MINUS,
    PERIOD = VK_OEM_PERIOD,
    SLASH = VK_OEM_2,
    NUM_0 = '0',
    NUM_1 = '1',
    NUM_2 = '2',
    NUM_3 = '3',
    NUM_4 = '4',
    NUM_5 = '5',
    NUM_6 = '6',
    NUM_7 = '7',
    NUM_8 = '8',
    NUM_9 = '9',
    SEMICOLON = VK_OEM_1,
    EQUAL = VK_OEM_PLUS,
    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',
    LEFT_BRACKET = VK_OEM_4,
    BACKSLASH = VK_OEM_5,
    RIGHT_BRACKET = VK_OEM_6,
    GRAVE_ACCENT = VK_OEM_3,
    WORLD_1 = -1, // What is this?
    WORLD_2 = -1, // What is this?
    ESCAPE = VK_ESCAPE,
    ENTER = VK_RETURN,
    TAB = VK_TAB,
    BACKSPACE = VK_BACK,
    INSERT = VK_INSERT,
    DELETE_ = VK_DELETE,
    RIGHT = VK_RIGHT,
    LEFT = VK_LEFT,
    DOWN = VK_DOWN,
    UP = VK_UP,
    PAGE_UP = VK_PRIOR,
    PAGE_DOWN = VK_NEXT,
    HOME = VK_HOME,
    END = VK_END,
    CAPS_LOCK = VK_CAPITAL,
    SCROLL_LOCK = VK_SCROLL,
    NUM_LOCK = VK_NUMLOCK,
    PRINT_SCREEN = VK_SNAPSHOT,
    PAUSE = VK_PAUSE,
    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,
    F13 = VK_F13,
    F14 = VK_F14,
    F15 = VK_F15,
    F16 = VK_F16,
    F17 = VK_F17,
    F18 = VK_F18,
    F19 = VK_F19,
    F20 = VK_F20,
    F21 = VK_F21,
    F22 = VK_F22,
    F23 = VK_F23,
    F24 = VK_F24,
    F25 = -1, // No VK
    KP_0 = VK_NUMPAD0,
    KP_1 = VK_NUMPAD1,
    KP_2 = VK_NUMPAD2,
    KP_3 = VK_NUMPAD3,
    KP_4 = VK_NUMPAD4,
    KP_5 = VK_NUMPAD5,
    KP_6 = VK_NUMPAD6,
    KP_7 = VK_NUMPAD7,
    KP_8 = VK_NUMPAD8,
    KP_9 = VK_NUMPAD9,
    KP_DECIMAL = VK_DECIMAL,
    KP_DIVIDE = VK_DIVIDE,
    KP_MULTIPLY = VK_MULTIPLY,
    KP_SUBTRACT = VK_SUBTRACT,
    KP_ADD = VK_ADD,
    KP_ENTER = VK_RETURN,
    KP_EQUAL = VK_RETURN,
    LEFT_SHIFT = VK_LSHIFT,
    LEFT_CONTROL = VK_LCONTROL,
    LEFT_ALT = -1, // Implemented as a syskey
    LEFT_SUPER = VK_LWIN,
    RIGHT_SHIFT = VK_RSHIFT,
    RIGHT_CONTROL = VK_RCONTROL,
    RIGHT_ALT = -1, // Implemented as a syskey
    RIGHT_SUPER = VK_RWIN,
    MENU = VK_MENU,

    SHIFT = VK_SHIFT,
    CONTROL = VK_CONTROL,

    LAST = 0xFF, // Bookend
  };

  // TODO: Populate these with DirectInput values.
  enum class MouseButton {
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7,
    BUTTON_8,

    LAST, // Bookend
  };

  const MouseButton BUTTON_LEFT   = MouseButton::BUTTON_1;
  const MouseButton BUTTON_RIGHT  = MouseButton::BUTTON_2;
  const MouseButton BUTTON_MIDDLE = MouseButton::BUTTON_4;

#endif

  using T = std::underlying_type_t<Key>;
  inline Key operator + (Key lhs, int rhs) {
    return (Key)(static_cast<T>(lhs) + rhs);
  };

} // namespace dg
