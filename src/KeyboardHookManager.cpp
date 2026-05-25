#include "KeyboardHookManager.h"

namespace ChanhIME {

// Static singleton pointer.
InputHookManager* InputHookManager::s_instance = nullptr;

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------
InputHookManager::InputHookManager() {
    s_instance = this;

    // Zero the 256-bit bitmask.
    _keyState[0] = 0;
    _keyState[1] = 0;
    _keyState[2] = 0;
    _keyState[3] = 0;

    _kbHook    = SetWindowsHookExW(WH_KEYBOARD_LL, KbProc,    nullptr, 0);
    _mouseHook = SetWindowsHookExW(WH_MOUSE_LL,    MouseProc, nullptr, 0);
}

InputHookManager::~InputHookManager() {
    if (_kbHook)    { UnhookWindowsHookEx(_kbHook);    _kbHook    = nullptr; }
    if (_mouseHook) { UnhookWindowsHookEx(_mouseHook); _mouseHook = nullptr; }
    if (s_instance == this) s_instance = nullptr;
}

// ---------------------------------------------------------------------------
// 256-bit bitmask helpers
// ---------------------------------------------------------------------------
void InputHookManager::SetKeyBit(DWORD vk) {
    if (vk >= 256) return;
    _keyState[vk >> 6] |=  (DWORD64(1) << (vk & 63));
}

void InputHookManager::ClearKeyBit(DWORD vk) {
    if (vk >= 256) return;
    _keyState[vk >> 6] &= ~(DWORD64(1) << (vk & 63));
}

bool InputHookManager::TestKeyBit(DWORD vk) const {
    if (vk >= 256) return false;
    return (_keyState[vk >> 6] & (DWORD64(1) << (vk & 63))) != 0;
}

// ---------------------------------------------------------------------------
// Low-level keyboard hook procedure
// ---------------------------------------------------------------------------
LRESULT CALLBACK InputHookManager::KbProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || !s_instance) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (kb->dwExtraInfo == InputInjector::MAGIC_EXTRA_INFO) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    DWORD vk = kb->vkCode;

    // Translate VK to a Unicode character (chuẩn xác cho mọi ký tự)
    wchar_t ch = 0;
    BYTE keyState[256];
    GetKeyboardState(keyState);
    
    // Cập nhật thủ công trạng thái Shift/CapsLock để tránh lỗi trễ nhịp của Low-Level Hook
    keyState[VK_SHIFT] = (GetKeyState(VK_SHIFT) & 0x8000) ? 0x80 : 0;
    keyState[VK_CAPITAL] = (GetKeyState(VK_CAPITAL) & 0x0001) ? 0x01 : 0;

    wchar_t buffer[4] = {0};
    UINT scanCode = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    
    // Nhờ Windows dịch mã phím thành ký tự chính xác (bao gồm cả Shift + 0 -> ')')
    if (ToUnicode(vk, scanCode, keyState, buffer, 4, 0) > 0) {
        ch = buffer[0];
    }

    bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool isUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

    // Debounce: skip key-down if already recorded as down.
    if (isDown && s_instance->TestKeyBit(vk)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    HookKeyEventArgs args;
    args.keyCode   = vk;
    args.character = ch;
    args.extraInfo = kb->dwExtraInfo;
    args.handled   = false;

    if (isDown) {
        s_instance->SetKeyBit(vk);
        if (s_instance->KeyDown) {
            s_instance->KeyDown(s_instance, args);
        }
    } else if (isUp) {
        s_instance->ClearKeyBit(vk);
        if (s_instance->KeyUp) {
            s_instance->KeyUp(s_instance, args);
        }
    }

    if (args.handled) return 1; // suppress the keystroke
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Low-level mouse hook procedure
// ---------------------------------------------------------------------------
LRESULT CALLBACK InputHookManager::MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && s_instance) {
        // Any mouse button press resets the engine.
        if (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
            wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN) {
            if (s_instance->MouseClick) {
                s_instance->MouseClick(s_instance);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

} // namespace ChanhIME
