//
//  InputCodes.h
//

#pragma once

#include <Windows.h>

namespace dg {


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


  using T = std::underlying_type_t<Key>;
  inline Key operator + (Key lhs, int rhs) {
    return (Key)(static_cast<T>(lhs) + rhs);
  };

} // namespace dg
