# 🛠️ my-cli-tools

<p align="center">
  <strong>Bộ công cụ dòng lệnh (CLI) và tiện ích gọn nhẹ cho Windows</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows%2010%20%7C%2011-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Windows 10 | 11" />
  <img src="https://img.shields.io/badge/Languages-C%20%7C%20Python%20%7C%20PowerShell%20%7C%20Batch-4B8BBE?style=for-the-badge" alt="C, Python, PowerShell, Batch" />
  <img src="https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge" alt="Status: Active" />
</p>

---

## Giới thiệu

**my-cli-tools** gom 5 tiện ích nhỏ cho công việc hằng ngày trên Windows: đổi tên tệp ngẫu nhiên và chuyển đổi video/ảnh từ menu chuột phải, hẹn giờ tắt máy bằng ứng dụng C thuần Win32, tải nhạc YouTube sang MP3, và mở Terminal tại thư mục đang xem bằng phím tắt.

Mỗi công cụ nằm trong một thư mục riêng, bạn có thể clone cả repo hoặc chỉ lấy công cụ mình cần.

## Mục lục

1. [Tổng quan các công cụ](#tổng-quan-các-công-cụ)
2. [Chi tiết từng công cụ](#chi-tiết-từng-công-cụ)
3. [Cấu trúc thư mục](#cấu-trúc-thư-mục)
4. [Cài đặt môi trường](#cài-đặt-môi-trường)
5. [Xử lý sự cố](#xử-lý-sự-cố)
6. [Lưu ý khi sử dụng](#lưu-ý-khi-sử-dụng)
7. [Tác giả](#tác-giả)

---

## Tổng quan các công cụ

| Công cụ | Thư mục | Công nghệ | Cách dùng | Điểm nổi bật |
| :--- | :--- | :--- | :--- | :--- |
| **Shutdown Scheduler** | [`shutdown-scheduler`](shutdown-scheduler) | C (Win32 API) | Ứng dụng desktop | File `.exe` ~450KB, hẹn giờ cố định hoặc đếm ngược, lặp lại hàng ngày, song ngữ Việt - Anh |
| **Random Rename** | [`random-rename`](random-rename) | Python + CustomTkinter | Menu chuột phải + GUI | Đổi tên ngẫu nhiên 1 chạm, tùy chỉnh độ dài, prefix/suffix, xem trước |
| **File Converter** | [`file-converter`](file-converter) | PowerShell + FFmpeg | Menu chuột phải | Video sang GIF (2 chế độ), đổi định dạng ảnh PNG/JPG/WebP/BMP/GIF/TIFF/ICO |
| **YouTube MP3 Downloader** | [`mp3-downloader`](mp3-downloader) | Python (`yt-dlp` + `ffmpeg`) | CLI tương tác | Tải âm thanh tốt nhất sang MP3, gắn thẻ ID3, tải nhiều liên kết |
| **Terminal Here** | [`open-in-terminal-shortcut`](open-in-terminal-shortcut) | Ứng dụng khay hệ thống | Phím tắt toàn cục | Mở Terminal tại thư mục Explorer đang xem, đổi phím tắt được (mặc định `Ctrl+Alt+T`) |

---

## Chi tiết từng công cụ

### 1. ⏰ Shutdown Scheduler (`shutdown-scheduler`)

Ứng dụng hẹn giờ tắt máy viết bằng C, dùng trực tiếp Win32 API.

**Tính năng**

- **Nhẹ**: một file `.exe` ~450KB, không cần .NET Runtime, Java hay Electron.
- **Hai chế độ hẹn giờ**: theo giờ cố định trong ngày (`HH:MM`) hoặc đếm ngược (giờ và phút).
- **Lặp lại hàng ngày**: tự khởi động cùng Windows và kích hoạt lại lịch hẹn.
- **Cảnh báo trước khi tắt**: hiện thông báo ở khay hệ thống trước thời điểm tắt máy (số phút báo trước tùy chỉnh được).
- **Khay hệ thống**: thu nhỏ xuống khay, nhấp đúp biểu tượng để mở lại.
- **Song ngữ**: chuyển Việt - Anh bằng 1 click, nhớ lựa chọn.
- **Hiển thị đúng trên mọi mức DPI**: đo font hệ thống bằng `SPI_GETNONCLIENTMETRICS`.

**Sử dụng**

- Chạy `shutdown-scheduler.exe` trong thư mục `shutdown-scheduler/`.
- Cấu hình được lưu tại `%APPDATA%\HenGioTatMay.ini`.

**Tự build từ mã nguồn (MinGW-w64)**

```bash
x86_64-w64-mingw32-windres app.rc -O coff -o app_res.o
x86_64-w64-mingw32-gcc -O2 -mwindows -municode -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    -o shutdown-scheduler.exe shutdown_scheduler.c app_res.o -lcomctl32 -lshell32 -ladvapi32
```

---

### 2. 🏷️ Random Rename (`random-rename`)

Tiện ích tích hợp vào menu chuột phải của Windows Explorer, đổi tên tệp hoặc thư mục ngẫu nhiên mà không cần mở cửa sổ dòng lệnh.

**Tính năng**

- **1 chạm**: chuột phải vào tệp hoặc thư mục, chọn **Random Rename**.
- **Không cần quyền Administrator**: cài vào registry người dùng hiện tại (`HKCU`).
- **Cửa sổ cấu hình** (CustomTkinter) có xem trước và lưu cấu hình:
  - Kiểu ký tự: chỉ số, chỉ chữ cái, chữ và số, hoặc bộ ký tự tùy chọn.
  - Chữ thường, chữ HOA hoặc trộn.
  - Độ dài chuỗi ngẫu nhiên bằng thanh trượt.
  - Tiền tố (prefix) và hậu tố (suffix), ví dụ `IMG_a8f3k2z9`.
  - Giữ nguyên phần mở rộng của tệp (`.png`, `.docx`, `.mp4`...).
  - Dùng chung một tên khi chọn nhiều tệp cùng lúc.

**Cài đặt & sử dụng**

1. Đặt thư mục `random-rename/` ở vị trí cố định (ví dụ `C:\Tools\random-rename`).
2. Chạy `install.bat` để đăng ký vào menu chuột phải.
3. Chạy `configure_settings.bat` để chỉnh cấu hình, bấm **Save**.
4. Chạy `uninstall.bat` để gỡ khỏi menu chuột phải.

---

### 3. 🔄 File Converter (`file-converter`)

Tiện ích menu chuột phải để chuyển video thành GIF và đổi định dạng ảnh. Cần FFmpeg cho phần video.

**Tính năng**

- **Video sang GIF** với hai chế độ: GIF dung lượng nhẹ hoặc GIF chất lượng cao (bảng màu tối ưu, Lanczos).
- **Đổi định dạng ảnh**: PNG, JPG, WebP, BMP, GIF, TIFF, ICO.
- **Thông báo ở khay hệ thống** khi chuyển đổi xong, không mở cửa sổ phiền.
- **Cài vào `HKCU`** bằng `install.bat`, gỡ bằng `uninstall.bat`.

---

### 4. 🎵 YouTube MP3 Downloader (`mp3-downloader`)

Công cụ dòng lệnh tải âm thanh từ YouTube bằng `yt-dlp` và chuyển sang MP3 bằng `ffmpeg`.

**Tính năng**

- **Âm thanh tốt nhất**: lấy stream `bestaudio/best` rồi xuất MP3.
- **Metadata**: gắn thẻ ID3 (tiêu đề, nghệ sĩ...).
- **Tiến trình một dòng**: hiển thị phần trăm, tốc độ (MB/s) và thời gian còn lại (ETA).
- **Nhiều liên kết**: nhập nhiều đường dẫn, cách nhau bằng dấu cách hoặc dấu phẩy.
- **Thư mục lưu**: `%USERPROFILE%\Downloads\MP3_YouTube`.
- **Tự kiểm tra môi trường**: `run_download_mp3.bat` kiểm tra Python, tự cài `yt-dlp` nếu thiếu và hướng dẫn cài `ffmpeg`.

**Sử dụng**

1. Mở thư mục `mp3-downloader/`.
2. Nhấp đúp `run_download_mp3.bat` (hoặc chạy `python download_mp3.py`).
3. Dán liên kết YouTube và nhấn `Enter`.
4. Nhấn `Enter` khi để trống để thoát.

---

### 5. ⌨️ Terminal Here (`open-in-terminal-shortcut`)

Ứng dụng chạy ở khay hệ thống, mở Windows Terminal, PowerShell hoặc Command Prompt tại thư mục Explorer đang xem.

**Tính năng**

- **Phím tắt toàn cục**: mặc định `Ctrl+Alt+T`, đổi được (ví dụ `Ctrl+Shift+T`, `Win+T`).
- **Lấy đúng thư mục** của cửa sổ Explorer đang active.
- **Chạy nền nhẹ** ở khay đồng hồ; chuột phải biểu tượng để đổi phím tắt.
- **Tự khởi động cùng Windows** qua thư mục Startup; `uninstall.bat` gỡ bỏ.

---

## Cấu trúc thư mục

```text
my-cli-tools/
├── file-converter/              # Video sang GIF, đổi định dạng ảnh (menu chuột phải)
├── mp3-downloader/              # Tải nhạc YouTube sang MP3 (yt-dlp + ffmpeg)
├── open-in-terminal-shortcut/   # Mở Terminal tại thư mục Explorer bằng phím tắt
├── random-rename/               # Đổi tên ngẫu nhiên (menu chuột phải + GUI)
├── shutdown-scheduler/          # Hẹn giờ tắt máy (C, Win32 API)
├── .gitignore
└── README.md
```

---

## Cài đặt môi trường

```powershell
git clone https://github.com/minhtrong67/my-cli-tools.git
cd my-cli-tools
```

| Thành phần | Cần cho | Ghi chú |
| :--- | :--- | :--- |
| Windows 10/11 (64-bit) | Tất cả | |
| Python 3.8+ | `random-rename`, `mp3-downloader` | Khi cài, tích **Add python.exe to PATH** |
| FFmpeg | `mp3-downloader`, `file-converter` | Cài nhanh bằng lệnh bên dưới |

```powershell
winget install ffmpeg
```

Hoặc tải bản build từ [gyan.dev/ffmpeg/builds](https://www.gyan.dev/ffmpeg/builds/) và thêm thư mục `bin` vào biến môi trường `PATH`.

---

## Xử lý sự cố

<details>
<summary><strong>Lỗi "'python' is not recognized as an internal or external command"</strong></summary>

- **Nguyên nhân**: Python chưa được cài hoặc chưa nằm trong `PATH`.
- **Cách khắc phục**: cài lại từ [python.org](https://www.python.org/downloads/) và tích **Add python.exe to PATH**, hoặc thêm đường dẫn Python vào biến môi trường thủ công.
</details>

<details>
<summary><strong>Menu chuột phải không xuất hiện sau khi chạy install.bat</strong></summary>

- **Nguyên nhân**: Windows Explorer chưa cập nhật cache registry.
- **Cách khắc phục**: mở Task Manager (`Ctrl + Shift + Esc`), chọn **Windows Explorer** trong danh sách Processes, bấm chuột phải rồi chọn **Restart**.
</details>

<details>
<summary><strong>Windows SmartScreen cảnh báo khi chạy shutdown-scheduler.exe</strong></summary>

- **Nguyên nhân**: file `.exe` tự biên dịch từ mã nguồn C và chưa được ký số (code signing).
- **Cách khắc phục**: mã nguồn có sẵn trong repo để bạn tự kiểm tra hoặc tự build lại. Nếu tin tưởng, chọn **More info** rồi **Run anyway**.
</details>

<details>
<summary><strong>Báo thiếu ffmpeg</strong></summary>

- **Nguyên nhân**: chưa có `ffmpeg` để chuyển âm thanh sang MP3 hoặc video sang GIF.
- **Cách khắc phục**: chạy `winget install ffmpeg` trong PowerShell, sau đó mở terminal mới.
</details>

---

## Lưu ý khi sử dụng

`mp3-downloader` dành cho mục đích cá nhân và sao lưu nội dung hợp pháp (bài giảng, nội dung do chính bạn tạo, hoặc âm thanh được tác giả cho phép tải). Hãy tuân thủ điều khoản của nền tảng và luật sở hữu trí tuệ hiện hành.

---

## Tác giả

- **Tác giả**: [minhtrong67](https://github.com/minhtrong67), với sự hỗ trợ của Claude (Anthropic) làm trợ lý AI.
- **Repository**: [my-cli-tools](https://github.com/minhtrong67/my-cli-tools)
- **Giấy phép**: mã nguồn mở cho mục đích học tập và sử dụng cá nhân. Repo hiện chưa có file `LICENSE`; nếu muốn người khác dùng lại rõ ràng, bạn nên thêm một giấy phép (ví dụ MIT).

<p align="center">
  <i>Góp ý và Pull Request luôn được hoan nghênh. Nếu bộ công cụ hữu ích, hãy để lại một ⭐ cho repo!</i>
</p>
