#include "sdlhid.h"

#include "../../../firmware/library/usb/hid.h"

/**
 * Translate SDL3 keycode to USB HID usage ID (symbolic name).
 * Returns 0 if no mapping exists.
 */
uint16_t SDLKeycodeToHID(SDL_Keycode key)
{
    switch (key) {
        // Function keys
        case SDLK_F1: return HID_KEY_F1;
        case SDLK_F2: return HID_KEY_F2;
        case SDLK_F3: return HID_KEY_F3;
        case SDLK_F4: return HID_KEY_F4;
        case SDLK_F5: return HID_KEY_F5;
        case SDLK_F6: return HID_KEY_F6;
        case SDLK_F7: return HID_KEY_F7;
        case SDLK_F8: return HID_KEY_F8;
        case SDLK_F9: return HID_KEY_F9;
        case SDLK_F10: return HID_KEY_F10;
        case SDLK_F11: return HID_KEY_F11;
        case SDLK_F12: return HID_KEY_F12;
        case SDLK_F13: return HID_KEY_F13;
        case SDLK_F14: return HID_KEY_F14;
        case SDLK_F15: return HID_KEY_F15;
        case SDLK_F16: return HID_KEY_F16;
        case SDLK_F17: return HID_KEY_F17;
        case SDLK_F18: return HID_KEY_F18;
        case SDLK_F19: return HID_KEY_F19;
        case SDLK_F20: return HID_KEY_F20;
        case SDLK_F21: return HID_KEY_F21;
        case SDLK_F22: return HID_KEY_F22;
        case SDLK_F23: return HID_KEY_F23;
        case SDLK_F24: return HID_KEY_F24;

        // Navigation keys
        case SDLK_INSERT:   return HID_KEY_INSERT;
        case SDLK_HOME:     return HID_KEY_HOME;
        case SDLK_PAGEUP:   return HID_KEY_PAGE_UP;
        case SDLK_DELETE:   return HID_KEY_DELETE;
        case SDLK_END:      return HID_KEY_END;
        case SDLK_PAGEDOWN: return HID_KEY_PAGE_DOWN;

        case SDLK_RIGHT: return HID_KEY_ARROW_RIGHT;
        case SDLK_LEFT:  return HID_KEY_ARROW_LEFT;
        case SDLK_DOWN:  return HID_KEY_ARROW_DOWN;
        case SDLK_UP:    return HID_KEY_ARROW_UP;

        // Control keys
        case SDLK_ESCAPE:    return HID_KEY_ESCAPE;
        case SDLK_TAB:       return HID_KEY_TAB;
        case SDLK_RETURN:    return HID_KEY_RETURN;
        case SDLK_BACKSPACE: return HID_KEY_DELETE; // backspace
        case SDLK_CAPSLOCK:  return HID_KEY_CAPS_LOCK;

        // Modifier keys
        case SDLK_LCTRL:  return HID_KEY_CONTROL_LEFT;
        case SDLK_LSHIFT: return HID_KEY_SHIFT_LEFT;
        case SDLK_LALT:   return HID_KEY_ALT_LEFT;
        case SDLK_LGUI:   return HID_KEY_GUI_LEFT;
        case SDLK_RCTRL:  return HID_KEY_CONTROL_RIGHT;
        case SDLK_RSHIFT: return HID_KEY_SHIFT_RIGHT;
        case SDLK_RALT:   return HID_KEY_ALT_RIGHT;
        case SDLK_RGUI:   return HID_KEY_GUI_RIGHT;

        // Print / Pause / Scroll
        case SDLK_PRINTSCREEN: return HID_KEY_PRINT_SCREEN;
        case SDLK_SCROLLLOCK:  return HID_KEY_SCROLL_LOCK;
        case SDLK_PAUSE:       return HID_KEY_PAUSE;

        // Lock keys
        case SDLK_NUMLOCKCLEAR: return HID_KEY_NUM_LOCK;

        // Keypad
        case SDLK_KP_DIVIDE:    return HID_KEY_KEYPAD_DIVIDE;
        case SDLK_KP_MULTIPLY:  return HID_KEY_KEYPAD_MULTIPLY;
        case SDLK_KP_MINUS:     return HID_KEY_KEYPAD_SUBTRACT;
        case SDLK_KP_PLUS:      return HID_KEY_KEYPAD_ADD;
        case SDLK_KP_ENTER:     return HID_KEY_KEYPAD_ENTER;
        case SDLK_KP_1:         return HID_KEY_KEYPAD_1;
        case SDLK_KP_2:         return HID_KEY_KEYPAD_2;
        case SDLK_KP_3:         return HID_KEY_KEYPAD_3;
        case SDLK_KP_4:         return HID_KEY_KEYPAD_4;
        case SDLK_KP_5:         return HID_KEY_KEYPAD_5;
        case SDLK_KP_6:         return HID_KEY_KEYPAD_6;
        case SDLK_KP_7:         return HID_KEY_KEYPAD_7;
        case SDLK_KP_8:         return HID_KEY_KEYPAD_8;
        case SDLK_KP_9:         return HID_KEY_KEYPAD_9;
        case SDLK_KP_0:         return HID_KEY_KEYPAD_0;
        case SDLK_KP_PERIOD:    return HID_KEY_KEYPAD_DECIMAL;
        case SDLK_KP_EQUALS:    return HID_KEY_KEYPAD_EQUAL;
        case SDLK_KP_COMMA:     return HID_KEY_KEYPAD_COMMA;

        // Application / Help keys
        case SDLK_MENU:        return HID_KEY_MENU;
        case SDLK_HELP:        return HID_KEY_HELP;
        case SDLK_APPLICATION: return HID_KEY_APPLICATION;

        default:
            return 0; // unmapped
    }
}
