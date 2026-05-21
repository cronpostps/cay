#include "InputInjector.h"

// Maximum characters in a single word + ZWJ + backspaces headroom.
// 1 ZWJ + 64 backspaces + 256 unicode chars = 321 INPUT structs worst case.
#define MAX_INPUTS 321

namespace CayIME {

// ---------------------------------------------------------------------------
// Internal helper – fill an INPUT for a Unicode keydown/keyup pair.
// ---------------------------------------------------------------------------
static void FillUnicodeInput(INPUT* inp, wchar_t ch, DWORD flags) {
    inp->type           = INPUT_KEYBOARD;
    inp->ki.wVk         = 0;
    inp->ki.wScan       = (WORD)ch;
    inp->ki.dwFlags     = flags | KEYEVENTF_UNICODE;
    inp->ki.time        = 0;
    inp->ki.dwExtraInfo = InputInjector::MAGIC_EXTRA_INFO;
}

// ---------------------------------------------------------------------------
// Internal helper – fill an INPUT for a virtual-key keydown/keyup pair.
// ---------------------------------------------------------------------------
static void FillVkInput(INPUT* inp, WORD vk, DWORD flags) {
    inp->type           = INPUT_KEYBOARD;
    inp->ki.wVk         = vk;
    inp->ki.wScan       = (WORD)MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    inp->ki.dwFlags     = flags;
    inp->ki.time        = 0;
    inp->ki.dwExtraInfo = InputInjector::MAGIC_EXTRA_INFO;
}

// ---------------------------------------------------------------------------
// ReplaceText – the core batched injection.
//
// Layout of the single INPUT array:
//   [0]      ZWJ down
//   [1]      ZWJ up
//   [2..2+2*bs-1]   bs*(VK_BACK down + VK_BACK up)
//   remaining       newText unicode pairs
// ---------------------------------------------------------------------------
void InputInjector::ReplaceText(int backspaceCount, const wchar_t* newText, int newTextLen) {
    // Guard against impossible sizes.
    if (backspaceCount < 0) backspaceCount = 0;
    if (newTextLen      < 0) newTextLen      = 0;

    int totalBs = backspaceCount;

    // Total INPUT slots needed.
    int total = (totalBs > 0 ? 4 : 0) // Left + Right arrow down/up
              + totalBs * 2           // each BS needs keydown + keyup
              + newTextLen * 2;       // each Unicode char needs keydown + keyup

    if (total > MAX_INPUTS) total = MAX_INPUTS; // hard clamp – should never happen

    INPUT inputs[MAX_INPUTS];
    int idx = 0;

    // 1. Chrome/Excel autocomplete breaker: Left Arrow + Right Arrow
    // This clears the highlighted prediction so that the subsequent backspaces 
    // actually delete the typed character instead of just clearing the highlight.
    if (totalBs > 0) {
        FillVkInput(&inputs[idx++], VK_LEFT, 0);
        FillVkInput(&inputs[idx++], VK_LEFT, KEYEVENTF_KEYUP);
        FillVkInput(&inputs[idx++], VK_RIGHT, 0);
        FillVkInput(&inputs[idx++], VK_RIGHT, KEYEVENTF_KEYUP);
    }

    // 2. Backspaces – erase the old committed text.
    for (int i = 0; i < totalBs && idx + 1 < MAX_INPUTS; i++) {
        FillVkInput(&inputs[idx++], VK_BACK, 0);
        FillVkInput(&inputs[idx++], VK_BACK, KEYEVENTF_KEYUP);
    }

    // 2. New text Unicode injection.
    for (int i = 0; i < newTextLen && idx + 1 < MAX_INPUTS; i++) {
        FillUnicodeInput(&inputs[idx++], newText[i], 0);
        FillUnicodeInput(&inputs[idx++], newText[i], KEYEVENTF_KEYUP);
    }

    SendInput((UINT)idx, inputs, sizeof(INPUT));
}

// ---------------------------------------------------------------------------
// SendChar – inject a single Unicode character (two INPUT events).
// ---------------------------------------------------------------------------
void InputInjector::SendChar(wchar_t ch) {
    INPUT inputs[2];
    FillUnicodeInput(&inputs[0], ch, 0);
    FillUnicodeInput(&inputs[1], ch, KEYEVENTF_KEYUP);
    SendInput(2, inputs, sizeof(INPUT));
}

// ---------------------------------------------------------------------------
// SendBackspaces – inject N backspace keypresses.
// ---------------------------------------------------------------------------
void InputInjector::SendBackspaces(int count) {
    if (count <= 0) return;
    if (count > 64) count = 64;

    INPUT inputs[128]; // 64 * 2
    int idx = 0;
    for (int i = 0; i < count; i++) {
        FillVkInput(&inputs[idx++], VK_BACK, 0);
        FillVkInput(&inputs[idx++], VK_BACK, KEYEVENTF_KEYUP);
    }
    SendInput((UINT)idx, inputs, sizeof(INPUT));
}

} // namespace CayIME
