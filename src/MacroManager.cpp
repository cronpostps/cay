#include "MacroManager.h"
#include "InputInjector.h"
#include <fstream>

// --- Hàm chuyển đổi chuẩn của Windows (Sửa lỗi Emoji) ---
std::wstring Utf8ToWide(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string WideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
// --------------------------------------------------------

MacroManager& MacroManager::GetInstance() {
    static MacroManager instance;
    return instance;
}

MacroManager::MacroManager() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring exePath(path);
    size_t pos = exePath.find_last_of(L"\\/");
    m_filePath = exePath.substr(0, pos) + L"\\chanh_macros.txt";

    LoadFromFile();

    if (m_macros.empty()) {
        m_macros[L"11"] = L"2";
        m_macros[L"=))"] = L"\xD83D\xDE01";
        SaveToFile();
    }
}

void MacroManager::LoadFromFile() {
    m_macros.clear();
    std::ifstream file(m_filePath, std::ios::binary); // Đọc Binary chống lỗi
    if (!file.is_open()) return;
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back(); // Xóa ký tự xuống dòng rác
        std::wstring wline = Utf8ToWide(line);
        size_t delim = wline.find(L"=");
        if (delim != std::wstring::npos) {
            std::wstring trigger = wline.substr(0, delim);
            std::wstring result = wline.substr(delim + 1);
            if (!trigger.empty() && !result.empty()) {
                m_macros[trigger] = result;
            }
        }
    }
}

void MacroManager::SaveToFile() {
    std::ofstream file(m_filePath, std::ios::binary);
    if (!file.is_open()) return;
    for (const auto& pair : m_macros) {
        std::string line = WideToUtf8(pair.first + L"=" + pair.second) + "\n";
        file.write(line.c_str(), line.size());
    }
}

void MacroManager::ResetBuffer() {
    m_recentChars.clear();
}

bool MacroManager::ProcessChar(wchar_t ch) {
    if (ch == 0) return false;

    m_recentChars += ch;
    if (m_recentChars.length() > 3) m_recentChars.erase(0, 1); 

    for (size_t len = m_recentChars.length(); len > 0; --len) {
        std::wstring trigger = m_recentChars.substr(m_recentChars.length() - len);
        auto it = m_macros.find(trigger);
        
        if (it != m_macros.end()) {
            int bsCount = (int)len - 1;
            ChanhIME::InputInjector::ReplaceText(bsCount, it->second.c_str(), (int)it->second.length());
            ResetBuffer();
            return true;
        }
    }
    return false;
}