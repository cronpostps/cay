
#include <windows.h>
#include <shellapi.h>
#include <winreg.h>
#include "ChanhData.h"
#include "ChanhEngine.h"
#include "KeyboardHookManager.h"
#include "InputInjector.h"
#include "MacroManager.h"
#include "SettingsUI.h"
#include "ExcludeManager.h"
#include <math.h>
bool g_normalizeTone = true; // Đổi thành true để mặc định bật
bool g_shortcutW = true;     // Thêm biến mới: mặc định bật w = ư
bool g_isGameFocused = false;
HWINEVENTHOOK g_winEventHook = nullptr;

#define WM_TRAYICON (WM_USER + 1)

#define IDM_TOGGLE 1001
#define IDM_AUTOSTART 1002
#define IDM_ABOUT 1003
#define IDM_EXIT 1004

HWND g_hWnd = nullptr;
NOTIFYICONDATAW g_nid = { 0 };
Chanh::TelexEngine g_engine;
ChanhIME::InputHookManager* g_hookManager = nullptr;

bool g_enabled = true;
HICON g_iconOn = nullptr;
HICON g_iconOff = nullptr;
bool g_pendingToggle = false;

HICON CreateTrayIcon(COLORREF themeColor) {
    int width = GetSystemMetrics(SM_CXSMICON);  // Chiều rộng tối đa
    int height = GetSystemMetrics(SM_CYSMICON); // Chiều cao tối đa
    
    HDC hdc = GetDC(nullptr);
    HDC hMemDC = CreateCompatibleDC(hdc);
    HDC hMaskDC = CreateCompatibleDC(hdc);
    
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP hMask = CreateBitmap(width, height, 1, 1, nullptr);
    
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    HBITMAP hOldMask = (HBITMAP)SelectObject(hMaskDC, hMask);
    
    RECT rect = { 0, 0, width, height };
    
    // 1. Nền Mask trắng (xuyên thấu), nền Color đen (Bắt buộc để trong suốt XOR hoạt động đúng)
    FillRect(hMaskDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    FillRect(hMemDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    
    int cx = width / 2;
    int cy = height / 2;
    
    // --- KHẮC PHỤC LỖI VIỀN ĐEN ---
    // Tuyệt đối không dùng PEN (viền) để đảm bảo hình Mask và Color khớp nhau 100% đến từng pixel
    SelectObject(hMaskDC, GetStockObject(NULL_PEN));
    SelectObject(hMemDC, GetStockObject(NULL_PEN));
    
    // 2. Vẽ hình tròn lên Mask (Bằng kích thước tối đa 0 -> width)
    SelectObject(hMaskDC, GetStockObject(BLACK_BRUSH));
    Ellipse(hMaskDC, 0, 0, width, height); 
    
    // 3. Chuẩn bị cọ màu
    HBRUSH hThemeBrush = CreateSolidBrush(themeColor);
    HBRUSH hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HPEN hWhitePen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    
    // 3.1. Vỏ ngoài (Xanh) - Phóng to kịch trần (padding = 0)
    SelectObject(hMemDC, hThemeBrush);
    Ellipse(hMemDC, 0, 0, width, height);
    
    // 3.2. Màng trắng (Trắng)
    SelectObject(hMemDC, hWhiteBrush);
    Ellipse(hMemDC, 1, 1, width - 1, height - 1);
    
    // 3.3. Ruột chanh (Xanh)
    SelectObject(hMemDC, hThemeBrush);
    Ellipse(hMemDC, 2, 2, width - 2, height - 2);
    
    // 3.4. Kẻ vạch chia múi (Trắng)
    SelectObject(hMemDC, hWhitePen);
    int inner = 2;
    int outer = width - 2;
    
    // Cắt dấu Cộng (+)
    MoveToEx(hMemDC, cx, inner, nullptr);
    LineTo(hMemDC, cx, outer);
    MoveToEx(hMemDC, inner, cy, nullptr);
    LineTo(hMemDC, outer, cy);
    
    // Cắt dấu Chéo (X)
    MoveToEx(hMemDC, inner + 1, inner + 1, nullptr);
    LineTo(hMemDC, outer - 1, outer - 1);
    MoveToEx(hMemDC, inner + 1, outer - 2, nullptr);
    LineTo(hMemDC, outer - 1, inner);
    
    // Lõi trắng ở tâm
    SelectObject(hMemDC, hWhiteBrush);
    SelectObject(hMemDC, GetStockObject(NULL_PEN));
    Ellipse(hMemDC, cx - 1, cy - 1, cx + 2, cy + 2);

    // Dọn dẹp
    SelectObject(hMemDC, hOldBitmap);
    SelectObject(hMaskDC, hOldMask);
    DeleteObject(hThemeBrush); DeleteObject(hWhiteBrush); DeleteObject(hWhitePen);
    DeleteDC(hMemDC); DeleteDC(hMaskDC); ReleaseDC(nullptr, hdc);
    
    // Đóng gói
    ICONINFO ii = { 0 };
    ii.fIcon = TRUE;
    ii.hbmMask = hMask;
    ii.hbmColor = hBitmap;
    HICON hIcon = CreateIconIndirect(&ii);
    
    DeleteObject(hMask); DeleteObject(hBitmap);
    return hIcon;
}

bool IsAutoStart() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD type;
        if (RegQueryValueExW(hKey, L"Chanh", nullptr, &type, nullptr, nullptr) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        RegCloseKey(hKey);
    }
    return false;
}

void ToggleAutoStart() {
    bool on = !IsAutoStart();
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        if (on) {
            wchar_t exePath[MAX_PATH];
            GetModuleFileNameW(nullptr, exePath, MAX_PATH);
            
            // --- BẢN VÁ: Bọc ngoặc kép để chống lỗi thư mục có khoảng trắng ---
            wchar_t quotedPath[MAX_PATH + 2];
            wsprintfW(quotedPath, L"\"%s\"", exePath);
            
            RegSetValueExW(hKey, L"Chanh", 0, REG_SZ, (const BYTE*)quotedPath, (DWORD)(lstrlenW(quotedPath) + 1) * sizeof(wchar_t));
        } else {
            RegDeleteValueW(hKey, L"Chanh");
        }
        RegCloseKey(hKey);
    }
}

void UpdateTrayIcon(bool isAdd = false) {
    g_nid.hIcon = g_enabled ? g_iconOn : g_iconOff;
    lstrcpyW(g_nid.szTip, g_enabled ? L"Chanh - B\u1eadt (Ctrl+Shift \u0111\u1ec3 t\u1eaft)" : L"Chanh - T\u1eaft (Ctrl+Shift \u0111\u1ec3 b\u1eadt)");
    Shell_NotifyIconW(isAdd ? NIM_ADD : NIM_MODIFY, &g_nid);
}

void Toggle() {
    g_enabled = !g_enabled;
    UpdateTrayIcon();
    g_engine.ResetFull();
    MessageBeep(MB_OK);
}

void ShowContextMenu(POINT pt) {
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING | (IsAutoStart() ? MF_CHECKED : MF_UNCHECKED), IDM_AUTOSTART, L"T\x1EF1 kh\x1EDFi \x0111\x1ED9ng");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"Thi\u1ebft l\u1eadp");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Tho\x00E1t");

    SetForegroundWindow(g_hWnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, g_hWnd, nullptr);
    DestroyMenu(hMenu);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_TRAYICON:
        if (LOWORD(lParam) == WM_LBUTTONUP) {
            Toggle();
        } else if (LOWORD(lParam) == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            ShowContextMenu(pt);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_TOGGLE: Toggle(); break;
        case IDM_AUTOSTART: ToggleAutoStart(); break;
        case IDM_ABOUT:
            ShowSettingsUI();
            break;
        case IDM_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

void OnKeyDownHook(ChanhIME::InputHookManager* sender, ChanhIME::HookKeyEventArgs& e) {
    if (e.extraInfo == ChanhIME::InputInjector::MAGIC_EXTRA_INFO) return;
    if (g_isGameFocused) return;

    // 1. Lấy trạng thái vật lý tức thời của các phím Modifier từ phần cứng
    bool isCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool isAlt  = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool isWin  = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 || (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;

    // 2. Tổ hợp bật/tắt bộ gõ (Ctrl + Shift)
    if (isCtrl && (e.keyCode == VK_LSHIFT || e.keyCode == VK_RSHIFT || e.keyCode == VK_SHIFT)) {
        g_pendingToggle = true; 
        return;
    }

    // 3. NẾU ĐANG GIỮ CTRL, ALT, WIN -> CHẮC CHẮN LÀ PHÍM TẮT
    if (isCtrl || isAlt || isWin) {
        g_engine.ResetFull(); // Dọn dẹp RAM bộ gõ để từ tiếp theo không bị dính
        return; // Trả về ngay, tuyệt đối không nuốt phím để VS Code, Word tự xử lý!
    }

    // 4. Các phím Modifier tự nó bấm xuống cũng bỏ qua
    if (e.keyCode == VK_LCONTROL || e.keyCode == VK_RCONTROL || e.keyCode == VK_CONTROL ||
        e.keyCode == VK_LMENU || e.keyCode == VK_RMENU || e.keyCode == VK_MENU ||
        e.keyCode == VK_LWIN || e.keyCode == VK_RWIN) {
        g_engine.ResetFull();
        return;
    }

    if (!g_enabled) return;

    // --- BẮT ĐẦU ĐOẠN CODE MACRO ---
    if (e.keyCode == VK_SPACE || e.keyCode == VK_RETURN || e.keyCode == VK_BACK || 
       (e.keyCode >= VK_PRIOR && e.keyCode <= VK_DOWN)) {
        MacroManager::GetInstance().ResetBuffer();
    }
    else if (e.character != 0) {
        if (MacroManager::GetInstance().ProcessChar(e.character)) {
            e.handled = true;      
            g_engine.ResetFull();  
            return;                
        }
    }
    // --- KẾT THÚC ĐOẠN CODE MACRO ---

    g_engine.OnKeyDown(nullptr, e);
}

void OnKeyUpHook(ChanhIME::InputHookManager* sender, ChanhIME::HookKeyEventArgs& e) {
    if (e.extraInfo == ChanhIME::InputInjector::MAGIC_EXTRA_INFO) return;

    // Xử lý bật/tắt bộ gõ khi nhả phím
    if (e.keyCode == VK_LCONTROL || e.keyCode == VK_RCONTROL || e.keyCode == VK_CONTROL) {
        if (g_pendingToggle) { 
            g_pendingToggle = false; 
            Toggle(); 
        } 
        return;
    }
    
    if (g_pendingToggle && (e.keyCode == VK_LSHIFT || e.keyCode == VK_RSHIFT || e.keyCode == VK_SHIFT)) {
        g_pendingToggle = false; 
        Toggle(); 
        return;
    }

    if (!g_enabled) return;
    g_engine.OnKeyUp(nullptr, e);
}

void OnMouseClickHook(ChanhIME::InputHookManager* sender) {
    g_engine.ResetFull();
}

// Hàm này tự động chạy khi người dùng chuyển sang cửa sổ mới (Alt+Tab, Click chuột...)
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        g_isGameFocused = false; // Mặc định là thức dậy
        
        if (hProc) {
            wchar_t path[MAX_PATH];
            DWORD size = MAX_PATH;
            // Lấy tên file .exe của cửa sổ đang active
            if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
                std::wstring wpath(path);
                size_t pos = wpath.find_last_of(L"\\/");
                std::wstring exeName = (pos == std::wstring::npos) ? wpath : wpath.substr(pos + 1);
                
                // Nếu tên .exe nằm trong danh sách đen -> Kích hoạt trạng thái ngủ đông
                if (ExcludeManager::GetInstance().IsExcluded(exeName)) {
                    g_isGameFocused = true;
                }
            }
            CloseHandle(hProc);
        }
    }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    // Single instance check
    wchar_t mutexName[256];
    wchar_t userName[256];
    DWORD userNameLen = 256;
    GetUserNameW(userName, &userNameLen);
    wsprintfW(mutexName, L"Global\\ChanhVN_%s", userName);
    
    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE); // Cấp quyền đọc công khai
    SECURITY_ATTRIBUTES sa = { sizeof(sa), &sd, FALSE };
    HANDLE hMutex = CreateMutexW(&sa, TRUE, mutexName);
    
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(nullptr, L"Chanh \u0111\u00e1ng ch\u1ea1y!", L"Th\u00f4ng b\x00e1o", MB_OK | MB_ICONINFORMATION);
        CloseHandle(hMutex);
        return 0;
    }

    // Elevate priority
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    // Initialize data (Removed because data is static)
    // Chanh BẬT: Chỉ một màu Xanh chanh (Lime) kết hợp với đường nét Trắng
    g_iconOn = CreateTrayIcon(RGB(50, 205, 50)); 
    
    // Chanh TẮT: Màu Xám trung tính
    g_iconOff = CreateTrayIcon(RGB(128, 128, 128));

    // Register Message-Only Window Class
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = L"ChanhMessageWindowClass";
    RegisterClassExW(&wcex);

    g_hWnd = CreateWindowExW(0, L"ChanhMessageWindowClass", L"ChanhMessageWindow", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInstance, nullptr);

    // Setup Tray
    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = g_hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    UpdateTrayIcon(true);

    // Start Hooks
    g_hookManager = new ChanhIME::InputHookManager();
    g_hookManager->KeyDown = OnKeyDownHook;
    g_hookManager->KeyUp = OnKeyUpHook;
    g_hookManager->MouseClick = OnMouseClickHook;
    g_winEventHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    // Message Loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    delete g_hookManager;
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
    DestroyWindow(g_hWnd);
    DestroyIcon(g_iconOn);
    DestroyIcon(g_iconOff);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    if (g_winEventHook) UnhookWinEvent(g_winEventHook);

    return (int)msg.wParam;
}
