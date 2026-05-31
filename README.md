# 🍋‍🟩 Chanh — Bộ gõ Telex Tiếng Việt Tối Giản & Mạnh Mẽ

[![Release](https://img.shields.io/github/v/release/cronpostps/chanh?style=flat-square&color=32CD32)](https://github.com/cronpostps/chanh/releases)
[![Size](https://img.shields.io/badge/size-~561_KB-brightgreen?style=flat-square)](https://github.com/cronpostps/chanh/releases/download/latest/ChanhEngine_latest.zip)
[![Platform](https://img.shields.io/badge/platform-Windows-0078d7?style=flat-square)](https://github.com/cronpostps/chanh/releases)
[![License](https://img.shields.io/badge/license-GPL--3.0-blue?style=flat-square)](LICENSE)

Bộ gõ Telex siêu nhẹ, "plug-and-play" dành cho anh em developer và game thủ. Không bloatware, không quảng cáo, bật lên là gõ với trải nghiệm mượt mà, được thiết kế để hạn chế tối đa việc phá hỏng code hay giật lag khi chơi game.

➡️ [**Tải xuống ChanhEngine_latest.zip (~561 KB)**](https://github.com/cronpostps/chanh/releases/download/latest/ChanhEngine_latest.zip)

---

## 🌱 Nguồn gốc dự án (Credits)

Dự án **Chanh** được xây dựng và phát triển dựa trên nền tảng mã nguồn mở tuyệt vời của dự án **[Cay](https://github.com/tctvn/cay)** (bởi tác giả [tctvn](https://github.com/tctvn)). 

Chúng tôi xin gửi lời cảm ơn chân thành đến tác giả của **Cay** vì một kiến trúc lõi C++ cực kỳ thông minh và tối ưu. **Chanh** kế thừa trọn vẹn triết lý "siêu nhẹ, zero-config" của người tiền nhiệm, đồng thời được "độ" thêm một loạt tính năng cao cấp để trở thành một bộ gõ toàn diện hơn cho nhu cầu sử dụng hàng ngày.

---

## ✨ Tính năng "ăn tiền" của Chanh

- **Siêu nhẹ & Tối ưu hóa (Zero-Bloat):** File thực thi chỉ khoảng ~265KB. Được build với cờ tối ưu hóa cấp cao nhất của MSVC (`/O2`, `/OPT:ICF`), không phụ thuộc thư viện bên thứ ba, ngốn RAM gần như bằng 0.
- **Ngủ đông thông minh (App Bypass):** Tự động phát hiện và nhường lại quyền điều khiển bàn phím khi bạn mở các ứng dụng/game trong danh sách Bypass (CS:GO, LoL, Dota 2, Valorant...). Tạm biệt nỗi lo kẹt phím khi tung skill!
- **Gõ tắt (Macro):** Hỗ trợ thiết lập các từ gõ tắt cá nhân hóa, giúp tăng tốc độ gõ văn bản và code.
- **Giao diện "Pro":** Bổ sung cửa sổ thiết lập UI hiện đại, quản lý danh sách Gõ tắt và Ngủ đông trực quan, lưu trữ dữ liệu tự động.
- **Khắc phục triệt để lỗi trình duyệt:** Sử dụng kỹ thuật tiêm phím ảo đột phá, giải quyết dứt điểm 100% lỗi nuốt chữ, nhảy nháy khi gõ trên thanh địa chỉ Chrome, Edge hay VSCode...
- **Tùy biến linh hoạt:** Hỗ trợ chuẩn hóa dấu tiếng Việt (òa -> oà), tùy chọn gõ `w` thành `ư`.
- **Auto-Start an toàn:** Tự khởi động nhanh chóng cùng Windows.

## 🛠️ Build từ Source

Yêu cầu: Có sẵn `CMake` và `MSVC` (Visual Studio C++ Build Tools).

```bash
git clone https://github.com/tctvn/cay.git
cd cay
cmake -B build
cmake --build build --config Release
```
*File build xong sẽ nằm tại build/Release/chanh.exe.*

## ⌨️ Cách dùng

1. Chạy file chanh.exe -> app sẽ chạy ngầm dưới khay hệ thống. Để Chanh hoạt động với các ứng dụng lõi của Windows (eg. MS Edge...), cần chạy nó dưới quyền quản trị (Run as Administrator).

2. Click chuột phải vào biểu tượng dưới khay hệ thống để mở bảng Thiết lập (Thêm Gõ tắt, Thêm App Game cần Bypass, Tự khởi động...).

3. Cú pháp chuẩn Telex: aa=â, oo=ô, ee=ê, dd=đ, w=ă/ư/ơ.

4. Dấu: s=sắc, f=huyền, r=hỏi, x=ngã, j=nặng, z=xoá dấu.

5. Phím tắt Bật/Tắt nhanh: Ctrl + Shift.

## 📜 License
[GPL-3.0 License](LICENSE) 
- Mã nguồn gốc © [Cay by tctvn](https://github.com/tctvn/cay)
- Mã nguồn độ © [Chanh by anhnn](https://github.com/cronpostps/chanh)