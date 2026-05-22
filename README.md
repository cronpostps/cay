# 🌶️ Cay — Bộ gõ Telex Minimalist

[![Release](https://img.shields.io/github/v/release/tctvn/cay?style=flat-square&color=FF4500)](https://github.com/tctvn/cay/releases)
[![Size](https://img.shields.io/badge/size-22_KB-brightgreen?style=flat-square)](https://github.com/tctvn/cay/releases/download/cay/cay.exe)
[![Platform](https://img.shields.io/badge/platform-Windows-0078d7?style=flat-square)](https://github.com/tctvn/cay/releases)
[![License](https://img.shields.io/badge/license-GPL--3.0-blue?style=flat-square)](LICENSE)

Bộ gõ Telex siêu nhẹ, "plug-and-play" dành cho anh em developer hệ tối giản. Không config rườm rà, bật lên là gõ với trải nghiệm mượt mà, được thiết kế để hạn chế tối đa việc phá hỏng code hay lỗi vặt trên IDE.

➡️ [**Download cay.exe (22 KB)**](https://github.com/tctvn/cay/releases/download/cay/cay.exe)

---

## ✨ Features (Tính năng "ăn tiền")

- **Siêu nhẹ & Sạch sẽ:** File thực thi chỉ vỏn vẹn ~22KB. Chạy độc lập, không phụ thuộc thư viện rườm rà, ngốn RAM gần như bằng 0.
- **Không lỗi vặt:** Đã fix triệt để các bệnh nan y như lỗi nuốt chữ, nhảy nháy khi gõ trên thanh địa chỉ Chrome, thanh tìm kiếm VSCode hay GitHub/CodeMirror.
- **Smart Bypass (Gõ code cực bốc):** Tự động nhận diện từ tiếng Anh hoặc các chuỗi sai quy tắc chính tả tiếng Việt để nhường lại quyền gõ phím nguyên thủy. Anh em cứ gõ code tiếng Anh vô tư mà không sợ bị bắt dấu sai lệch.
- **Zero-config:** Không cần chỉnh bảng mã, không cần chọn kiểu gõ. Tải về là chạy, bật là gõ.

## 🛠️ Build từ Source

Yêu cầu: Có sẵn `CMake` và `MSVC`.

```bash
git clone https://github.com/tctvn/cay.git
cd cay
cmake -B build
cmake --build build --config Release
```
*File build xong sẽ nằm gọn tại `build/Release/cay.exe`.*

## ⌨️ Cách dùng

- Chạy file `cay.exe` (app sẽ chạy ngầm dưới khay hệ thống - System Tray).
- Cú pháp chuẩn Telex: `aa`=â, `oo`=ô, `ee`=ê, `dd`=đ, `w`=ă/ư/ơ.
- Dấu: `s`=sắc, `f`=huyền, `r`=hỏi, `x`=ngã, `j`=nặng, `z`=xoá dấu.
- **Phím tắt Bật/Tắt:** `Ctrl + Shift`.

## 📜 License
[GPL-3.0 License](LICENSE) © [tctvn](https://github.com/tctvn/cay).
Mã nguồn: [GitHub Repository](https://github.com/tctvn/cay).
