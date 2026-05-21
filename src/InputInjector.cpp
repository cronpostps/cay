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
    if (backspaceCount <= 0 && newTextLen <= 0) return;

    // We MUST use the dummy to break RichText DOM bounds, which adds 1 to backspaces.
    bool useDummy = (backspaceCount > 0);
    int totalBs = backspaceCount + (useDummy ? 1 : 0);

    // --- BATCH 1: DELETION (Dummy + Backspaces) ---
    if (totalBs > 0) {
        INPUT bsInputs[130]; 
        int bsIdx = 0;
        
        if (useDummy) {
            FillUnicodeInput(&bsInputs[bsIdx++], L'\u200D', 0);
            FillUnicodeInput(&bsInputs[bsIdx++], L'\u200D', KEYEVENTF_KEYUP);
        }

        for (int i = 0; i < totalBs && bsIdx + 1 < 130; i++) {
            FillVkInput(&bsInputs[bsIdx++], VK_BACK, 0);
            FillVkInput(&bsInputs[bsIdx++], VK_BACK, KEYEVENTF_KEYUP);
        }
        SendInput((UINT)bsIdx, bsInputs, sizeof(INPUT));
    }

    // --- BATCH 2: INSERTION (New Text) ---
    if (newTextLen > 0) {
        INPUT txtInputs[128];
        int txtIdx = 0;
        
        for (int i = 0; i < newTextLen && txtIdx + 1 < 128; i++) {
            FillUnicodeInput(&txtInputs[txtIdx++], newText[i], 0);
            FillUnicodeInput(&txtInputs[txtIdx++], newText[i], KEYEVENTF_KEYUP);
        }
        SendInput((UINT)txtIdx, txtInputs, sizeof(INPUT));
    }
}

} // namespace CayIME
