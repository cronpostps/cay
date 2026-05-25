#include "ExcludeManager.h"
#include <fstream>

static std::wstring Utf8ToW(const std::string& str) {
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size);
    return wstr;
}
static std::string WToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

ExcludeManager& ExcludeManager::GetInstance() {
    static ExcludeManager instance;
    return instance;
}

ExcludeManager::ExcludeManager() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring exePath(path);
    size_t pos = exePath.find_last_of(L"\\/");
    m_filePath = exePath.substr(0, pos) + L"\\chanh_exclude.txt";
    LoadFromFile();

    if (m_excluded.empty()) {
        m_excluded.insert(L"csgo.exe");
        m_excluded.insert(L"league of legends.exe");
        m_excluded.insert(L"dota2.exe");
        m_excluded.insert(L"valorant-win64-shipping.exe");
        SaveToFile();
    }
}

void ExcludeManager::LoadFromFile() {
    m_excluded.clear();
    std::ifstream file(m_filePath, std::ios::binary);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::wstring wline = Utf8ToW(line);
        for (auto& c : wline) c = towlower(c); // Đưa về chữ thường
        m_excluded.insert(wline);
    }
}

void ExcludeManager::SaveToFile() {
    std::ofstream file(m_filePath, std::ios::binary);
    if (!file.is_open()) return;
    for (const auto& app : m_excluded) {
        std::string line = WToUtf8(app) + "\n";
        file.write(line.c_str(), line.size());
    }
}

bool ExcludeManager::IsExcluded(const std::wstring& exeName) {
    std::wstring lowerName = exeName;
    for (auto& c : lowerName) c = towlower(c);
    return m_excluded.find(lowerName) != m_excluded.end();
}

void ExcludeManager::Add(const std::wstring& exeName) {
    std::wstring lowerName = exeName;
    for (auto& c : lowerName) c = towlower(c);
    m_excluded.insert(lowerName);
    SaveToFile();
}

void ExcludeManager::Remove(const std::wstring& exeName) {
    std::wstring lowerName = exeName;
    for (auto& c : lowerName) c = towlower(c);
    m_excluded.erase(lowerName);
    SaveToFile();
}