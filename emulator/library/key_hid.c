#include <SDL3/SDL.h>
#include <stdint.h>

/**
 * Translate SDL3 keycode to USB HID usage ID (symbolic name).
 * Returns 0 if no mapping exists.
 */
uint16_t SDLKeycodeToHID(SDL_Keycode key)
{
    switch (key) {
        // Function keys
        case SDLK_F1: return HID_KEYBOARD_F1;
        case SDLK_F2: return HID_KEYBOARD_F2;
        case SDLK_F3: return HID_KEYBOARD_F3;
        case SDLK_F4: return HID_KEYBOARD_F4;
        case SDLK_F5: return HID_KEYBOARD_F5;
        case SDLK_F6: return HID_KEYBOARD_F6;
        case SDLK_F7: return HID_KEYBOARD_F7;
        case SDLK_F8: return HID_KEYBOARD_F8;
        case SDLK_F9: return HID_KEYBOARD_F9;
        case SDLK_F10: return HID_KEYBOARD_F10;
        case SDLK_F11: return HID_KEYBOARD_F11;
        case SDLK_F12: return HID_KEYBOARD_F12;
        case SDLK_F13: return HID_KEYBOARD_F13;
        case SDLK_F14: return HID_KEYBOARD_F14;
        case SDLK_F15: return HID_KEYBOARD_F15;
        case SDLK_F16: return HID_KEYBOARD_F16;
        case SDLK_F17: return HID_KEYBOARD_F17;
        case SDLK_F18: return HID_KEYBOARD_F18;
        case SDLK_F19: return HID_KEYBOARD_F19;
        case SDLK_F20: return HID_KEYBOARD_F20;
        case SDLK_F21: return HID_KEYBOARD_F21;
        case SDLK_F22: return HID_KEYBOARD_F22;
        case SDLK_F23: return HID_KEYBOARD_F23;
        case SDLK_F24: return HID_KEYBOARD_F24;

        // Navigation keys
        case SDLK_INSERT:   return HID_KEYBOARD_INSERT;
        case SDLK_HOME:     return HID_KEYBOARD_HOME;
        case SDLK_PAGEUP:   return HID_KEYBOARD_PAGE_UP;
        case SDLK_DELETE:   return HID_KEYBOARD_DELETE_FORWARD;
        case SDLK_END:      return HID_KEYBOARD_END;
        case SDLK_PAGEDOWN: return HID_KEYBOARD_PAGE_DOWN;

        case SDLK_RIGHT: return HID_KEYBOARD_RIGHTARROW;
        case SDLK_LEFT:  return HID_KEYBOARD_LEFTARROW;
        case SDLK_DOWN:  return HID_KEYBOARD_DOWNARROW;
        case SDLK_UP:    return HID_KEYBOARD_UPARROW;

        // Control keys
        case SDLK_ESCAPE:    return HID_KEYBOARD_ESCAPE;
        case SDLK_TAB:       return HID_KEYBOARD_TAB;
        case SDLK_RETURN:    return HID_KEYBOARD_RETURN;
        case SDLK_BACKSPACE: return HID_KEYBOARD_DELETE; // backspace
        case SDLK_CAPSLOCK:  return HID_KEYBOARD_CAPS_LOCK;

        // Modifier keys
        case SDLK_LCTRL:  return HID_KEYBOARD_LEFTCONTROL;
        case SDLK_LSHIFT: return HID_KEYBOARD_LEFTSHIFT;
        case SDLK_LALT:   return HID_KEYBOARD_LEFTALT;
        case SDLK_LGUI:   return HID_KEYBOARD_LEFT_GUI;
        case SDLK_RCTRL:  return HID_KEYBOARD_RIGHTCONTROL;
        case SDLK_RSHIFT: return HID_KEYBOARD_RIGHTSHIFT;
        case SDLK_RALT:   return HID_KEYBOARD_RIGHTALT;
        case SDLK_RGUI:   return HID_KEYBOARD_RIGHT_GUI;

        // Print / Pause / Scroll
        case SDLK_PRINTSCREEN: return HID_KEYBOARD_PRINT_SCREEN;
        case SDLK_SCROLLLOCK:  return HID_KEYBOARD_SCROLL_LOCK;
        case SDLK_PAUSE:       return HID_KEYBOARD_PAUSE;

        // Lock keys
        case SDLK_NUMLOCKCLEAR: return HID_KEYBOARD_NUM_LOCK;

        // Keypad
        case SDLK_KP_DIVIDE:    return HID_KEYPAD_SLASH;
        case SDLK_KP_MULTIPLY:  return HID_KEYPAD_ASTERISK;
        case SDLK_KP_MINUS:     return HID_KEYPAD_MINUS;
        case SDLK_KP_PLUS:      return HID_KEYPAD_PLUS;
        case SDLK_KP_ENTER:     return HID_KEYPAD_ENTER;
        case SDLK_KP_1:         return HID_KEYPAD_1;
        case SDLK_KP_2:         return HID_KEYPAD_2;
        case SDLK_KP_3:         return HID_KEYPAD_3;
        case SDLK_KP_4:         return HID_KEYPAD_4;
        case SDLK_KP_5:         return HID_KEYPAD_5;
        case SDLK_KP_6:         return HID_KEYPAD_6;
        case SDLK_KP_7:         return HID_KEYPAD_7;
        case SDLK_KP_8:         return HID_KEYPAD_8;
        case SDLK_KP_9:         return HID_KEYPAD_9;
        case SDLK_KP_0:         return HID_KEYPAD_0;
        case SDLK_KP_PERIOD:    return HID_KEYPAD_PERIOD;
        case SDLK_KP_EQUALS:    return HID_KEYPAD_EQUAL;
        case SDLK_KP_COMMA:     return HID_KEYPAD_COMMA;
        case SDLK_KP_LEFTPAREN: return HID_KEYPAD_LEFTPAREN;
        case SDLK_KP_RIGHTPAREN:return HID_KEYPAD_RIGHTPAREN;

        // Application / Help keys
        case SDLK_MENU:        return HID_KEYBOARD_MENU;
        case SDLK_HELP:        return HID_KEYBOARD_HELP;
        case SDLK_APPLICATION: return HID_KEYBOARD_APPLICATION;
        case SDLK_SYSREQ:      return HID_KEYBOARD_SYSREQ;

        default:
            return 0; // unmapped
    }
}
