#include "SettingsUI.h"
#include "MacroManager.h"
#include "ExcludeManager.h"
#include <windows.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

extern bool g_normalizeTone;
extern bool g_shortcutW;
extern HICON g_iconOn; // Lấy icon từ main.cpp sang

static HFONT g_uiFont = NULL;

LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hListMac, hTrig, hRes, hAddMac, hDelMac;
    static HWND hListExe, hExeInput, hAddExe, hDelExe;
    static HWND hChkTone, hChkW;

    switch(msg) {
        case WM_CREATE: {
            INITCOMMONCONTROLSEX icex; icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_LISTVIEW_CLASSES; InitCommonControlsEx(&icex);

            // ============ CỘT TRÁI: GÕ TẮT ============
            CreateWindowExW(0, L"BUTTON", L"G\u00f5 t\u1eaft (Macro)", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 15, 10, 380, 260, hWnd, NULL, NULL, NULL);
            hListMac = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER, 30, 35, 350, 150, hWnd, (HMENU)1, NULL, NULL);
            ListView_SetExtendedListViewStyle(hListMac, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
            LVCOLUMNW lvc = {0}; lvc.mask = LVCF_TEXT | LVCF_WIDTH;
            lvc.cx = 100; lvc.pszText = (LPWSTR)L"Ph\u00edm g\u00e1n"; ListView_InsertColumn(hListMac, 0, &lvc);
            lvc.cx = 245; lvc.pszText = (LPWSTR)L"K\u1ebft qu\u1ea3"; ListView_InsertColumn(hListMac, 1, &lvc);
            
            CreateWindowExW(0, L"STATIC", L"Ph\u00edm g\u00e1n:", WS_CHILD | WS_VISIBLE, 30, 200, 80, 20, hWnd, NULL, NULL, NULL);
            hTrig = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 30, 225, 80, 25, hWnd, NULL, NULL, NULL);
            CreateWindowExW(0, L"STATIC", L"K\u1ebft qu\u1ea3:", WS_CHILD | WS_VISIBLE, 125, 200, 80, 20, hWnd, NULL, NULL, NULL);
            hRes = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 125, 225, 160, 25, hWnd, NULL, NULL, NULL);
            
            hAddMac = CreateWindowExW(0, L"BUTTON", L"Th\u00eam", WS_CHILD | WS_VISIBLE, 300, 195, 80, 25, hWnd, (HMENU)2, NULL, NULL);
            hDelMac = CreateWindowExW(0, L"BUTTON", L"X\u00f3a", WS_CHILD | WS_VISIBLE, 300, 225, 80, 25, hWnd, (HMENU)3, NULL, NULL);

            // ============ CỘT PHẢI: NGỦ ĐÔNG ============
            CreateWindowExW(0, L"BUTTON", L"Danh s\u00e1ch ng\u1ee7 \u0111\u00f4ng (App Bypass)", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 410, 10, 380, 260, hWnd, NULL, NULL, NULL);
            hListExe = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER, 425, 35, 350, 150, hWnd, (HMENU)4, NULL, NULL);
            ListView_SetExtendedListViewStyle(hListExe, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
            LVCOLUMNW lvcEx = {0}; lvcEx.mask = LVCF_TEXT | LVCF_WIDTH;
            lvcEx.cx = 325; lvcEx.pszText = (LPWSTR)L"T\u00ean ti\u1ebfn tr\u00ecnh (.exe)"; ListView_InsertColumn(hListExe, 0, &lvcEx);
            
            CreateWindowExW(0, L"STATIC", L"T\u00ean file Game/App (VD: csgo.exe):", WS_CHILD | WS_VISIBLE, 425, 200, 250, 20, hWnd, NULL, NULL, NULL);
            hExeInput = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 425, 225, 260, 25, hWnd, NULL, NULL, NULL);
            
            hAddExe = CreateWindowExW(0, L"BUTTON", L"Th\u00eam", WS_CHILD | WS_VISIBLE, 695, 195, 80, 25, hWnd, (HMENU)5, NULL, NULL);
            hDelExe = CreateWindowExW(0, L"BUTTON", L"X\u00f3a", WS_CHILD | WS_VISIBLE, 695, 225, 80, 25, hWnd, (HMENU)6, NULL, NULL);

            // ============ ĐƯỜNG PHÂN CÁCH (SEPARATOR) ============
            CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ, 15, 285, 775, 2, hWnd, NULL, NULL, NULL);

            // ============ PHẦN DƯỚI CÙNG (FOOTER) ============
            hChkTone = CreateWindowExW(0, L"BUTTON", L"Chu\u1ea9n h\u00f3a d\u1ea5u (òa -> o\u00e0)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 295, 250, 20, hWnd, (HMENU)7, NULL, NULL);
            SendMessage(hChkTone, BM_SETCHECK, g_normalizeTone ? BST_CHECKED : BST_UNCHECKED, 0);

            hChkW = CreateWindowExW(0, L"BUTTON", L"G\u00f5 w \u0111\u1ec3 \u0111\u01b0\u1ee3c \u01b0 (w = \u01b0)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 30, 320, 250, 20, hWnd, (HMENU)8, NULL, NULL);
            SendMessage(hChkW, BM_SETCHECK, g_shortcutW ? BST_CHECKED : BST_UNCHECKED, 0);

            // Label thông tin tác giả (Góc phải dưới, căn lề phải)
            CreateWindowExW(0, L"STATIC", L"Chanh v1.0 : B\u1ed9 g\u00f5 Telex Unicode\nby anhnn@dgd.vn", 
                            WS_CHILD | WS_VISIBLE | SS_RIGHT, 400, 300, 375, 40, hWnd, NULL, NULL, NULL);

            // Áp dụng Font Segoe UI cho toàn bộ các control con
            EnumChildWindows(hWnd, [](HWND hwnd, LPARAM lParam) -> BOOL {
                SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
                return TRUE;
            }, (LPARAM)g_uiFont);

            // Load dữ liệu
            auto& macros = MacroManager::GetInstance().GetMacros();
            int i = 0;
            for (auto& pair : macros) {
                LVITEMW lvi = {0}; lvi.mask = LVIF_TEXT; lvi.iItem = i++; lvi.pszText = (LPWSTR)pair.first.c_str();
                ListView_InsertItem(hListMac, &lvi); ListView_SetItemText(hListMac, i-1, 1, (LPWSTR)pair.second.c_str());
            }
            
            auto& exes = ExcludeManager::GetInstance().GetList();
            int j = 0;
            for (auto& app : exes) {
                LVITEMW lvi = {0}; lvi.mask = LVIF_TEXT; lvi.iItem = j++; lvi.pszText = (LPWSTR)app.c_str();
                ListView_InsertItem(hListExe, &lvi);
            }
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 2) { // Thêm Macro
                wchar_t trig[32], res[64];
                GetWindowTextW(hTrig, trig, 32); GetWindowTextW(hRes, res, 64);
                if (lstrlenW(trig) > 0 && lstrlenW(res) > 0) {
                    MacroManager::GetInstance().GetMacros()[trig] = res; MacroManager::GetInstance().SaveToFile();
                    int count = ListView_GetItemCount(hListMac);
                    LVITEMW lvi = {0}; lvi.mask = LVIF_TEXT; lvi.iItem = count; lvi.pszText = trig;
                    ListView_InsertItem(hListMac, &lvi); ListView_SetItemText(hListMac, count, 1, res);
                    SetWindowTextW(hTrig, L""); SetWindowTextW(hRes, L"");
                }
            } else if (LOWORD(wParam) == 3) { // Xóa Macro
                int sel = ListView_GetNextItem(hListMac, -1, LVNI_SELECTED);
                if (sel != -1) {
                    wchar_t trig[32]; ListView_GetItemText(hListMac, sel, 0, trig, 32);
                    MacroManager::GetInstance().GetMacros().erase(trig); MacroManager::GetInstance().SaveToFile();
                    ListView_DeleteItem(hListMac, sel);
                }
            } else if (LOWORD(wParam) == 5) { // Thêm Exclude
                wchar_t exe[64];
                GetWindowTextW(hExeInput, exe, 64);
                if (lstrlenW(exe) > 0) {
                    ExcludeManager::GetInstance().Add(exe);
                    int count = ListView_GetItemCount(hListExe);
                    LVITEMW lvi = {0}; lvi.mask = LVIF_TEXT; lvi.iItem = count;
                    std::wstring lowerExe = exe; for (auto& c : lowerExe) c = towlower(c);
                    lvi.pszText = (LPWSTR)lowerExe.c_str();
                    ListView_InsertItem(hListExe, &lvi);
                    SetWindowTextW(hExeInput, L"");
                }
            } else if (LOWORD(wParam) == 6) { // Xóa Exclude
                int sel = ListView_GetNextItem(hListExe, -1, LVNI_SELECTED);
                if (sel != -1) {
                    wchar_t exe[64]; ListView_GetItemText(hListExe, sel, 0, exe, 64);
                    ExcludeManager::GetInstance().Remove(exe);
                    ListView_DeleteItem(hListExe, sel);
                }
            } else if (LOWORD(wParam) == 7) { g_normalizeTone = (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED);
            } else if (LOWORD(wParam) == 8) { g_shortcutW = (SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED);
            }
            break;
        }
        case WM_DESTROY:
            break;
        case WM_CLOSE: DestroyWindow(hWnd); break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void ShowSettingsUI() {
    static bool registered = false;
    if (!registered) {
        WNDCLASSEXW wc = {sizeof(WNDCLASSEXW), 0, SettingsWndProc, 0, 0, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW), NULL, L"ChanhSettingsUI", NULL};
        wc.hIcon = g_iconOn;   
        wc.hIconSm = g_iconOn; 
        RegisterClassExW(&wc);
        g_uiFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        registered = true;
    }
    
    // Khóa luôn chức năng kéo giãn/thu nhỏ cửa sổ để bảo vệ Grid layout
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
    CreateWindowExW(0, L"ChanhSettingsUI", L"Chanh - Thi\u1ebft l\u1eadp b\u1ed9 g\u00f5", style, 200, 200, 820, 390, NULL, NULL, GetModuleHandle(NULL), NULL);
}