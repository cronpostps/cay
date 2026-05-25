#pragma once
#include <windows.h>
#include <string>
#include <unordered_set>

class ExcludeManager {
public:
    static ExcludeManager& GetInstance();
    bool IsExcluded(const std::wstring& exeName);
    void Add(const std::wstring& exeName);
    void Remove(const std::wstring& exeName);
    std::unordered_set<std::wstring>& GetList() { return m_excluded; }
    void SaveToFile();

private:
    ExcludeManager();
    void LoadFromFile();
    std::unordered_set<std::wstring> m_excluded;
    std::wstring m_filePath;
};