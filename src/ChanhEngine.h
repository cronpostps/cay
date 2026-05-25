#pragma once
#include <windows.h>
#include "ChanhData.h" // <- Đổi include
#include "InputInjector.h"
#include "KeyboardHookManager.h"

namespace Chanh { // <- Đổi tên namespace

struct MyKey {
    wchar_t raw;       
    wchar_t output;    
};

class TelexEngine {
public:
    TelexEngine();
    void OnKeyDown(ChanhIME::InputHookManager* sender, ChanhIME::HookKeyEventArgs& e);
    void OnKeyUp(ChanhIME::InputHookManager* sender, ChanhIME::HookKeyEventArgs& e);
    void ResetFull();
    void CommitWord();

private:
    MyKey _buffer[MAX_BUFFER];
    int   _bufferCount;
    wchar_t _text[MAX_BUFFER];
    int     _textLen;
    void NormalizeTone();
    int  _toneIndex;
    wchar_t _lastOutput[MAX_BUFFER];
    int     _lastOutputLen;

    MyKey   _savedBuffer[MAX_BUFFER];
    int     _savedBufferCount;
    wchar_t _savedText[MAX_BUFFER];
    int     _savedTextLen;
    int     _savedToneIndex;
    bool    _canRestore;

    void SaveState();
    bool ApplyDoubleKeys(wchar_t key);
    bool ApplyHookKeys(wchar_t key);
    bool ApplyToneMarks(int toneIndex);
    void StripAllTones();
    int FindTonePosition() const;
    void UpdateScreen(const wchar_t* newOutput, int newOutputLen);
    void Commit(int extraBs = 0);
    void FallbackToRaw();
    bool ShouldBypassWord() const;
    void ResetState();

    static bool IsAlpha(wchar_t ch);
    static wchar_t ToLowerViet(wchar_t c);
    static wchar_t ToUpperViet(wchar_t c);
};

}