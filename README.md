# 🌶️ Cay — Bộ gõ tiếng Việt Native siêu nhẹ

> Bộ gõ Telex tối giản dành cho Windows, được viết lại hoàn toàn bằng C++ Native (Zero-CRT). Kích thước siêu nhỏ (~19KB), tốc độ phản hồi tính bằng micro-giây, không cần cài đặt.

[![Release](https://img.shields.io/github/v/release/tctvn/cay?style=flat-square&color=FF4500)](https://github.com/tctvn/cay/releases)
[![Size](https://img.shields.io/badge/size-18_KB-brightgreen?style=flat-square)](https://github.com/tctvn/cay/releases/download/cay/cay.exe)
[![Platform](https://img.shields.io/badge/platform-Windows-0078d7?style=flat-square)](https://github.com/tctvn/cay/releases)
[![License](https://img.shields.io/badge/license-GPL--3.0-blue?style=flat-square)](LICENSE)

---

Cay là bộ gõ tiếng Việt tối giản được thiết kế nhằm thay thế các bộ gõ cồng kềnh. Ở phiên bản mới nhất, Cay được viết lại hoàn toàn bằng C++ Native (bỏ qua C-Runtime), tối ưu dung lượng xuống mức cực hạn (~18KB) và tiêu thụ RAM gần như bằng 0.

➡️ [**Tải về cay.exe (19 KB)**](https://github.com/tctvn/cay/releases/download/cay/cay.exe)

> [!IMPORTANT]
> **Yêu cầu hệ thống:** Windows 10 / 11 (64-bit). Khởi chạy trực tiếp không cần framework đi kèm.

---

## ⚡ Triết lý thiết kế

Cay hướng tới sự tối giản tối đa trong vận hành:

- **Siêu nhẹ, siêu tốc độ (19 KB):** Không chứa C-Runtime, thư viện động hay STL. Code thuần API Win32 giúp bộ gõ phản hồi nhanh nhất có thể.
- **Không cài đặt:** 1 file thực thi duy nhất `cay.exe`. Tải về và chạy.
- **Không tiến trình ngầm dư thừa:** Dịch vụ chạy ngầm được tối ưu bằng bitwise logic, không sử dụng string processing cồng kềnh.

---

## 🚀 Tính năng chính

### 🧠 Xử lý dấu thông minh (Smart Tone Placement)
Tự động phân tích cấu trúc nguyên âm tiếng Việt (kể cả với các từ đang gõ dở) để đặt dấu thanh chuẩn xác theo ngữ pháp (Ví dụ: `nguyeenx` ➔ `nguyễn`, `hoas` ➔ `hóa`).

### ⚡ Early Rejection Bypass (Level 1 & 2)
Thuật toán loại trừ (bypass) cực nhanh ngay tại thời điểm gõ (Level 1 & 2). Tự động nhận diện từ tiếng Anh và các tổ hợp phụ âm/nguyên âm sai quy tắc tiếng Việt để nhường quyền gõ phím, không làm hỏng từ tiếng Anh.

### 🔌 Không cần cấu hình (Zero-Config)
Hoạt động ngay sau khi mở mà không cần thiết lập bảng mã hay phím kích hoạt phức tạp.

### 🤫 Hoạt động ẩn dưới System Tray
Không có giao diện UI phức tạp. Nằm gọn dưới khay hệ thống (System Tray). Tích hợp menu bật tắt khởi động cùng Windows.

---

## ⌨️ Quy tắc gõ Telex

Hỗ trợ kiểu gõ Telex chuẩn:

### Nguyên âm & Phụ âm kép

| Tổ hợp phím | Kết quả | | Tổ hợp phím | Kết quả |
| :--- | :--- | - | :--- | :--- |
| `aa` | **â** | | `ow` | **ơ** |
| `aw` | **ă** | | `uw` | **ư** |
| `ee` | **ê** | | `dd` | **đ** |
| `oo` | **ô** | | | |

### Quy tắc dấu thanh

| Phím gõ | Dấu thanh | Ví dụ | Kết quả |
| :---: | :--- | :--- | :--- |
| **`s`** | Sắc | `hoas` | **hóa** |
| **`f`** | Huyền | `chaof` | **chào** |
| **`r`** | Hỏi | `hoir` | **hỏi** |
| **`x`** | Ngã | `ngax` | **ngã** |
| **`j`** | Nặng | `nawjng` | **nặng** |
| **`z`** | Xóa dấu | `hoasz` | **hoa** |

*💡 Mẹo: Nhấn tổ hợp phím `Ctrl + Shift` bất kỳ lúc nào để bật hoặc tắt nhanh bộ gõ.*

---

## 🛠️ Kiến trúc mã nguồn (C++ Native)

Kiến trúc No-CRT với các file thành phần:

```text
src/
├── main.cpp                 # Điểm khởi chạy (wWinMain), System Tray & Registry.
├── CayEngine.h/cpp          # State machine lõi (Xử lý Telex, ghép vần, logic bỏ dấu).
├── CayData.h/cpp            # Cơ sở dữ liệu tĩnh (Bảng mã Unicode, quy tắc chính tả).
├── KeyboardHookManager.h/cpp# Windows Hook (WH_KEYBOARD_LL) đánh chặn phím cấp thấp.
├── InputInjector.h/cpp      # Giả lập nhập liệu, xử lý SendInput (Xóa dummy ZWJ, chèn từ).
└── no_crt.cpp               # Triển khai các intrinsic functions (memset, memcpy) thay thế CRT.
```

---

## 📝 Giấy phép & Liên kết

Dự án được phân phối dưới giấy phép **GNU General Public License v3.0 (GPL-3.0)**. Chi tiết xem tại file [LICENSE](LICENSE).

*   **Mã nguồn GitHub**: [https://github.com/tctvn/cay](https://github.com/tctvn/cay)
*   **Trang phát hành (Releases)**: [https://github.com/tctvn/cay/releases](https://github.com/tctvn/cay/releases)
