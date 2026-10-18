# 🛠️ My Terminal Tool Pack

<p align="center">
  <strong>Bộ công cụ tiện ích tự động hóa, dòng lệnh (CLI) và ứng dụng tiện ích gọn nhẹ cho Windows</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Windows Platform" />
  <img src="https://img.shields.io/badge/Languages-C%20%7C%20Python%20%7C%20PowerShell%20%7C%20Batch-4B8BBE?style=for-the-badge" alt="Languages" />
  <img src="https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge" alt="Status" />
  <img src="https://img.shields.io/badge/License-Personal%20Use-orange?style=for-the-badge" alt="License" />
</p>

---

## 📖 Giới thiệu (Overview)

**My Terminal Tool Pack** là bộ sưu tập các tiện ích nhỏ gọn, hiệu quả cao được thiết kế nhằm tối ưu hóa trải nghiệm làm việc hàng ngày trên Windows. Bộ công cụ giải quyết nhanh các bài toán phổ biến của lập trình viên và người dùng máy tính: từ dọn dẹp bộ nhớ đệm dự án lập trình, đổi tên tệp hàng loạt từ menu ngữ cảnh, hẹn giờ tắt máy thông minh, đến tải nhạc chất lượng cao từ YouTube.

Tất cả các công cụ đều ưu tiên tiêu chí: **Không rườm rà - Tốc độ cao - Tiết kiệm tài nguyên - Dễ sử dụng ngay.**

---

## 📑 Mục lục (Table of Contents)

1. [Tổng quan các công cụ](#-tổng-quan-các-công-cụ-feature-matrix)
2. [Chi tiết từng công cụ](#-chi-tiết-từng-công-cụ)
   - [1. ASP.NET Core Bin Folder Cleaner](#1--aspnet-core-bin-folder-cleaner-clean-dotnet-bin-folder-tool)
   - [2. Windows Context Menu Random Rename](#2--windows-context-menu-random-rename-random-rename-tool)
   - [3. Hẹn Giờ Tắt Máy (Shutdown Scheduler)](#3--hẹn-giờ-tắt-máy-shutdown-scheduler-shutdown-scheduler-tool)
   - [4. YouTube MP3 High-Quality Downloader](#4--youtube-mp3-high-quality-downloader-yt-mp3-downloader-tool)
3. [Cấu trúc thư mục (Repository Structure)](#-cấu-trúc-thư-mục-repository-structure)
4. [Cài đặt môi trường chung (Prerequisites)](#-cài-đặt-môi-trường-chung-prerequisites)
5. [Xử lý sự cố thường gặp (Troubleshooting)](#-xử-lý-sự-cố-thường-gặp-troubleshooting)
6. [Tác giả & Bản quyền (Author & License)](#-tác-giả--bản-quyền-author--license)

---

## 📊 Tổng quan các công cụ (Feature Matrix)

| Công cụ | Thư mục | Công nghệ | Giao diện | Điểm nổi bật chính |
| :--- | :--- | :--- | :--- | :--- |
| **Bin Cleaner** | `clean-dotnet-bin-folder-tool` | PowerShell + Batch | CLI tương tác | Quét đệ quy, tính dung lượng, bỏ qua thư mục nặng (`node_modules`, `.git`), xóa chọn lọc/hàng loạt |
| **Random Rename** | `random-rename-tool` | Python + CustomTkinter | Context Menu + GUI | Tích hợp chuột phải Windows, đổi tên ngẫu nhiên tức thì, tùy chỉnh độ dài, prefix/suffix qua GUI |
| **Shutdown Scheduler** | `shutdown-scheduler-tool` | C (Win32 API thuần) | Native GUI | Siêu nhẹ (~450KB, ~0% RAM), hẹn giờ/đếm ngược, lặp lại hàng ngày, thông báo khay hệ thống, song ngữ |
| **YT MP3 Downloader** | `yt-mp3-downloader-tool` | Python (`yt-dlp` + `ffmpeg`) | CLI tương tác | Tải âm thanh tốt nhất, chuyển đổi MP3 320kbps kèm metadata, thanh tiến trình gọn gàng, hỗ trợ nhiều link |

---

## 🔍 Chi tiết từng công cụ

---

### 1. 🧹 ASP.NET Core Bin Folder Cleaner (`clean-dotnet-bin-folder-tool`)

Công cụ dòng lệnh chuyên dụng dành cho lập trình viên .NET / C# để dọn dẹp các thư mục `bin` và file build cache cồng kềnh, giúp giải phóng hàng chục GB ổ cứng và sửa lỗi xung đột build.

#### ✨ Tính năng nổi bật
- **Quét thông minh**: Tự động bỏ qua các thư mục không liên quan như `node_modules`, `.git`, `.vs`, `.vscode`, `dist`, `build`, `vendor`, `.next`, v.v., giúp tốc độ quét cực nhanh.
- **Thống kê chi tiết**: Tính toán dung lượng chính xác của từng thư mục `bin` tìm được (theo B, KB, MB, GB).
- **Lựa chọn xóa linh hoạt**:
  - Xóa theo số thứ tự mong muốn (ví dụ: `d 1` hoặc `d 1, 3, 5`).
  - Xóa toàn bộ danh sách chỉ với lệnh `all`.
  - Có bước xác nhận (`y/N`) an toàn trước khi xóa thực tế.
- **Xóa triệt để**: Sử dụng cơ chế xóa đa tầng (PowerShell `Remove-Item` kết hợp fallback sang CMD `rmdir /s /q`) để giải quyết các tệp bị khoá.

#### 🚀 Cách sử dụng
1. Mở thư mục `clean-dotnet-bin-folder-tool/`.
2. Nhấp đúp chuột vào file `run-tool.bat`.
3. Nhập đường dẫn thư mục chứa dự án cần quét (nhấn `Enter` để quét mặc định ổ `D:\`).
4. Nhập các lệnh điều khiển:
   ```text
   d <số>     Xóa theo chỉ số (ví dụ: d 1  hoặc  d 1,3,5)
   all        Xóa tất cả các folder bin trong danh sách
   q          Thoát chương trình
   ```

---

### 2. 🏷️ Windows Context Menu Random Rename (`random-rename-tool`)

Tiện ích tích hợp trực tiếp vào menu chuột phải của Windows Explorer, cho phép đổi tên ngẫu nhiên file hoặc thư mục ngay lập tức mà không cần mở cửa sổ dòng lệnh.

#### ✨ Tính năng nổi bật
- **Thao tác 1 chạm**: Chuột phải vào bất kỳ tệp hoặc thư mục nào > chọn **"Random Rename"** > tệp được đổi tên ngay lập tức trong nền.
- **Không yêu cầu quyền Administrator**: Cài đặt trực tiếp vào registry người dùng hiện tại (`HKCU`), an toàn và không ảnh hưởng hệ thống.
- **Cửa sổ cấu hình GUI hiện đại**: Được xây dựng bằng `CustomTkinter`, hỗ trợ xem trước (live preview) và lưu cấu hình:
  - Chọn kiểu ký tự: Chỉ số, Chỉ chữ cái, Chữ và Số (Alphanumeric), hoặc Bộ ký tự tùy chọn (Custom Charset).
  - Tùy chỉnh chữ hoa/thường (lowercase, UPPERCASE, MiXeD).
  - Tùy chỉnh độ dài chuỗi ngẫu nhiên bằng thanh trượt trực quan.
  - Thêm tiền tố (Prefix) hoặc hậu tố (Suffix) (ví dụ: `IMG_a8f3k2z9`).
  - Tùy chọn giữ nguyên phần mở rộng của file (`.png`, `.docx`, `.mp4`...).
  - Tùy chọn dùng chung một tên khi chọn nhiều tệp cùng lúc.

#### 🚀 Cài đặt & Sử dụng
1. Đặt thư mục `random-rename-tool/` ở một vị trí cố định (ví dụ: `C:\Tools\random-rename-tool`).
2. Chạy file `install.bat` để đăng ký vào menu chuột phải Windows.
3. Để tùy chỉnh cấu hình: Chạy `configure_settings.bat`, chỉnh thông số mong muốn và nhấn **Save**.
4. Để gỡ bỏ tiện ích khỏi chuột phải: Chạy file `uninstall.bat`.

---

### 3. ⏰ Hẹn Giờ Tắt Máy - Shutdown Scheduler (`shutdown-scheduler-tool`)

Ứng dụng hẹn giờ tắt máy nguyên bản (native Windows desktop application) viết bằng ngôn ngữ C trực tiếp trên nền Win32 API. 

#### ✨ Tính năng nổi bật
- **Siêu nhẹ & Hiệu năng cao**: File thực thi `.exe` duy nhất (~450KB), khởi động tức thì, tiêu thụ gần như 0% CPU và RAM. Không cần cài đặt .NET Runtime, Java hay Electron.
- **Hai chế độ hẹn giờ linh hoạt**:
  - Hẹn theo giờ cố định trong ngày (`HH:MM`).
  - Hẹn theo thời gian đếm ngược (giờ và phút).
- **Lặp lại hàng ngày (Daily Repeat)**: Tự động khởi động cùng Windows và kích hoạt lại lịch hẹn mỗi ngày ngay cả sau khi restart máy.
- **Cảnh báo trước khi tắt máy**: Hiển thị bong bóng thông báo (tray balloon) trước thời điểm tắt máy (có thể tùy chỉnh số phút báo trước) để người dùng kịp thời lưu tài liệu hoặc hủy lịch.
- **Tích hợp khay hệ thống (System Tray)**: Thu nhỏ xuống khay đồng hồ, nhấp đúp biểu tượng để mở lại giao diện.
- **Giao diện song ngữ**: Chuyển đổi giữa tiếng Việt và tiếng Anh chỉ với 1 click, tự động ghi nhớ tùy chọn.
- **Tương thích giao diện mượt mà**: Tự động đo đạc font chữ hệ thống (`SPI_GETNONCLIENTMETRICS`), hỗ trợ hiển thị tiếng Việt có dấu chuẩn xác và sắc nét trên mọi mức DPI (Windows 10 & 11).

#### 🚀 Cách sử dụng & Build
- **Sử dụng trực tiếp**: Chạy file `shutdown-scheduler.exe` trong thư mục `shutdown-scheduler-tool/`.
- **Cấu hình lưu trữ tại**: `%APPDATA%\HenGioTatMay.ini`.
- **Tùy chọn biên dịch (Build from Source với MinGW)**:
  ```bash
  x86_64-w64-mingw32-windres app.rc -O coff -o app_res.o
  x86_64-w64-mingw32-gcc -O2 -mwindows -municode -finput-charset=UTF-8 -fexec-charset=UTF-8 \
      -o shutdown-scheduler.exe shutdown_scheduler.c app_res.o -lcomctl32 -lshell32 -ladvapi32
  ```

---

### 4. 🎵 YouTube MP3 High-Quality Downloader (`yt-mp3-downloader-tool`)

Công cụ dòng lệnh hỗ trợ tải âm thanh từ YouTube với chất lượng cao nhất bằng lõi `yt-dlp` và tự động convert sang định dạng MP3 thông qua `ffmpeg`.

#### ✨ Tính năng nổi bật
- **Chất lượng âm thanh tối đa**: Tự động bóc tách stream âm thanh tốt nhất (`bestaudio/best`) và xuất ra định dạng MP3 với VBR/CBR tối ưu nhất (`preferredquality: 0`).
- **Ghi metadata đầy đủ**: Tự động gắn thẻ ID3 (tiêu đề bài hát, nghệ sĩ...) vào tệp MP3 hoàn chỉnh.
- **Giao diện dòng lệnh trực quan**: Thanh tiến trình cập nhật tại chỗ (single-line progress) hiển thị phần trăm tải, tốc độ mạng (MB/s) và thời gian còn lại (ETA).
- **Hỗ trợ tải nhiều liên kết**: Nhập nhiều đường dẫn YouTube cách nhau bằng dấu cách hoặc dấu phẩy để tải liên tục.
- **Tự động lưu trữ**: Mọi file sau khi hoàn tất được lưu gọn gàng tại thư mục:
  ```text
  %USERPROFILE%\Downloads\MP3_YouTube
  ```
- **Tự động thiết lập môi trường**: File `run_download_mp3.bat` tự kiểm tra Python, tự cài `yt-dlp` nếu máy chưa có và hướng dẫn cài `ffmpeg`.

#### 🚀 Cách sử dụng
1. Mở thư mục `yt-mp3-downloader-tool/`.
2. Nhấp đúp vào `run_download_mp3.bat` (hoặc chạy lệnh `python download_mp3.py`).
3. Dán liên kết video YouTube và nhấn `Enter`.
4. Nhấn `Enter` để trống khi muốn thoát chương trình.

---

## 📂 Cấu trúc thư mục (Repository Structure)

```text
my-terminal-tool-pack/
│
├── 🧹 clean-dotnet-bin-folder-tool/
│   └── run-tool.bat               # Script PowerShell & Batch quét & xóa folder bin .NET
│
├── 🏷️ random-rename-tool/
│   ├── assets/
│   │   └── rename_quick.ico       # Icon context menu chuột phải
│   ├── configure_settings.bat     # Mở giao diện cài đặt đổi tên
│   ├── install.bat                # Đăng ký context menu vào Windows Registry
│   ├── uninstall.bat              # Gỡ bỏ context menu khỏi Windows Registry
│   ├── random_rename.py           # Core logic đổi tên & GUI CustomTkinter
│   ├── requirements.txt           # Thư viện phụ thuộc (customtkinter)
│   └── README.txt                 # Hướng dẫn chi tiết cho công cụ rename
│
├── ⏰ shutdown-scheduler-tool/
│   ├── app.manifest               # Manifest kích hoạt Common Controls v6 & DPI aware
│   ├── app.rc                     # Resource script
│   ├── shutdown_scheduler.c       # Mã nguồn C (Win32 API)
│   ├── shutdown-scheduler.exe     # File thực thi binary portable
│   └── README.md                  # Tài liệu kỹ thuật chi tiết
│
├── 🎵 yt-mp3-downloader-tool/
│   ├── download_mp3.py            # Script xử lý download & convert mp3 qua yt-dlp
│   └── run_download_mp3.bat       # Launcher tự kiểm tra môi trường và chạy script
│
└── 📄 README.md                   # Tài liệu hướng dẫn tổng quan bộ công cụ
```

---

## 💻 Cài đặt môi trường chung (Prerequisites)

Để tận dụng tối đa tất cả các công cụ trong bộ pack, khuyến nghị hệ điều hành của bạn chuẩn bị sẵn các thành phần sau:

### 1. Hệ điều hành
- **Windows 10** hoặc **Windows 11** (64-bit).

### 2. Python 3.8+ (Dành cho `random-rename-tool` & `yt-mp3-downloader-tool`)
- Tải từ trang chủ: [python.org](https://www.python.org/downloads/)
- ⚠️ **Lưu ý quan trọng**: Khi cài đặt, hãy tích chọn **"Add python.exe to PATH"**.

### 3. FFmpeg (Dành cho `yt-mp3-downloader-tool`)
- Cài đặt nhanh bằng lệnh Windows Package Manager (khuyên dùng):
  ```powershell
  winget install ffmpeg
  ```
- Hoặc tải thủ công bản build từ: [gyan.dev/ffmpeg/builds](https://www.gyan.dev/ffmpeg/builds/) và thêm thư mục `bin` vào biến môi trường `PATH`.

---

## ❓ Xử lý sự cố thường gặp (Troubleshooting)

<details>
<summary><strong>1. Lỗi "'python' is not recognized as an internal or external command"</strong></summary>

- **Nguyên nhân**: Python chưa được cài đặt hoặc chưa được thêm vào biến môi trường `PATH`.
- **Cách khắc phục**: Cài lại Python từ [python.org](https://www.python.org/downloads/) và nhớ tick chọn **"Add python.exe to PATH"**, hoặc thêm thủ công đường dẫn cài đặt Python vào `System Environment Variables`.
</details>

<details>
<summary><strong>2. Menu chuột phải "Random Rename" không xuất hiện sau khi chạy install.bat</strong></summary>

- **Nguyên nhân**: Windows Explorer chưa kịp cập nhật cache registry.
- **Cách khắc phục**:
  1. Mở `Task Manager` (nhấn `Ctrl + Shift + Esc`).
  2. Tìm mục `Windows Explorer` trong danh sách Processes.
  3. Nhấp chuột phải chọn **Restart**.
</details>

<details>
<summary><strong>3. Windows SmartScreen hiển thị cảnh báo khi chạy shutdown-scheduler.exe</strong></summary>

- **Nguyên nhân**: File thực thi được biên dịch trực tiếp từ mã nguồn C cá nhân và chưa được ký số thương mại (code signing certificate).
- **Cách khắc phục**: Chọn **"More info" (Xem thêm)** > bấm **"Run anyway" (Vẫn chạy)**. Ứng dụng an toàn 100% và không chứa mã độc.
</details>

<details>
<summary><strong>4. yt-mp3-downloader-tool báo thiếu ffmpeg</strong></summary>

- **Nguyên nhân**: Chưa có công cụ `ffmpeg` để convert âm thanh sang tệp định dạng `.mp3`.
- **Cách khắc phục**: Mở PowerShell và chạy lệnh `winget install ffmpeg`, sau đó mở lại terminal mới để nhận lệnh.
</details>

---

## ⚖️ Tuyên bố miễn trừ trách nhiệm (Disclaimer)

- Công cụ `yt-mp3-downloader-tool` được tạo ra cho mục đích sử dụng cá nhân và sao lưu nội dung hợp pháp (các bài giảng, nội dung do chính bạn tạo ra hoặc âm thanh được tác giả cấp quyền công khai). Vui lòng tuân thủ điều khoản dịch vụ của nền tảng và luật sở hữu trí tuệ hiện hành.

---

## 👤 Tác giả & Giấy phép (Author & License)

- **Tác giả**: [minhtrong67](https://github.com/minhtrong67)
- **Repository**: [my-terminal-tool-pack](https://github.com/minhtrong67/my-terminal-tool-pack)
- **Giấy phép**: Dự án được chia sẻ dưới dạng mã nguồn mở cho mục đích học tập và sử dụng cá nhân.

<p align="center">
  <i>Đóng góp ý kiến và Pull Request luôn được hoan nghênh! ⭐ Hãy để lại một ngôi sao nếu bộ công cụ hữu ích với bạn!</i>
</p>
