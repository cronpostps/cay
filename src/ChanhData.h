#pragma once
#include <windows.h>

#define MAX_BUFFER 64

namespace Chanh { // <- Đổi tên namespace

class ChanhData { // <- Đổi tên class
public:
    static bool IsValidInitial(const wchar_t* s, int len);
    static bool IsValidNucleus(const wchar_t* s, int len);
    static int  GetToneIndex(wchar_t key);
    static wchar_t GetToneMark(wchar_t base, int toneIndex);
    static bool HasVietnameseMark(wchar_t ch);
    static bool HasVietnameseMark(const wchar_t* buf, int len);
    static wchar_t StripTone(wchar_t ch);
    static wchar_t StripAccent(wchar_t ch);
    static bool IsVowel(wchar_t ch);
    static wchar_t GetHookRule(wchar_t c);
};

}