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

    // CRITICAL FIX: The ZWJ dummy occupies one real character in the OS buffer.
    // We must backspace over it in addition to the old text.
    // This dummy wakes up the caret in Chrome Omnibox / Excel autocomplete.
    // However, if we don't need to backspace anything (backspaceCount == 0),
    // we MUST NOT insert a dummy, because some editors (like VS Code) drop ZWJ
    // which causes the extra backspace to swallow a valid character!
    bool useDummy = (backspaceCount > 0);
    int totalBs = backspaceCount + (useDummy ? 1 : 0);

    // Total INPUT slots needed.
    int total = (useDummy ? 2 : 0)  // ZWJ down + up
              + totalBs * 2         // each BS needs keydown + keyup
              + newTextLen * 2;     // each Unicode char needs keydown + keyup

    if (total > MAX_INPUTS) total = MAX_INPUTS; // hard clamp – should never happen

    INPUT inputs[MAX_INPUTS];
    int idx = 0;

    // 1. Zero-Width Joiner dummy to wake up the target window caret.
    if (useDummy) {
        FillUnicodeInput(&inputs[idx++], L'\u200D', 0);               // ZWJ down
        FillUnicodeInput(&inputs[idx++], L'\u200D', KEYEVENTF_KEYUP); // ZWJ up
    }

    // 2. Backspaces – erase the ZWJ + the old committed text.
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

} // namespace CayIME
