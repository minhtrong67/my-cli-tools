#!/usr/bin/env python3
"""
Công cụ tải nhạc MP3 từ YouTube (chất lượng cao nhất) bằng yt-dlp.

Yêu cầu:
    - Python 3.8+
    - Thư viện yt-dlp: pip install -U yt-dlp
    - ffmpeg đã cài và có trong PATH (dùng để convert sang mp3)

Cách dùng:
    python download_mp3.py
    (chương trình sẽ hỏi link liên tục cho đến khi bạn nhấn Enter để thoát)

Lưu ý pháp lý:
    Chỉ tải các video mà bạn có quyền tải (nội dung của bạn, nội dung
    được cấp phép, hoặc video có giấy phép cho phép tải về). Việc tải
    nội dung có bản quyền mà không được phép có thể vi phạm điều khoản
    dịch vụ của YouTube và luật bản quyền tại một số quốc gia.
"""

import os
import sys
import shutil

try:
    from yt_dlp import YoutubeDL
except ImportError:
    print("Chưa cài yt-dlp. Chạy lệnh sau rồi thử lại:")
    print("    pip install -U yt-dlp")
    sys.exit(1)


LINE = "─" * 46


def check_ffmpeg():
    if shutil.which("ffmpeg") is None:
        print("⚠  Không tìm thấy ffmpeg trong PATH.")
        print("   Cài bằng lệnh: winget install ffmpeg")
        print("   Hoặc tải tại: https://www.gyan.dev/ffmpeg/builds/")
        sys.exit(1)


def get_output_dir():
    out_dir = os.path.join(os.path.expanduser("~"), "Downloads", "MP3_YouTube")
    os.makedirs(out_dir, exist_ok=True)
    return out_dir


def make_progress_hook():
    """Hiển thị 1 dòng tiến trình gọn gàng, tự ghi đè lên chính nó."""
    state = {"last_title": None}

    def hook(d):
        status = d.get("status")
        if status == "downloading":
            title = d.get("info_dict", {}).get("title", "Đang tải...")
            if state["last_title"] != title:
                state["last_title"] = title
                print(f"\n  🎵 {title}")
            percent = d.get("_percent_str", "").strip()
            speed = d.get("_speed_str", "").strip()
            eta = d.get("_eta_str", "").strip()
            line = f"     {percent}  •  {speed}  •  còn lại {eta}   "
            print(f"\r{line}", end="", flush=True)
        elif status == "finished":
            print(f"\r     Đang chuyển sang MP3...{' ' * 20}", end="", flush=True)

    return hook


def download_one(ydl, url):
    try:
        ydl.download([url])
        print("\r     ✅ Xong!" + " " * 30)
        return True
    except Exception as e:
        msg = str(e).splitlines()[0]
        print(f"\r     ❌ Lỗi: {msg}" + " " * 10)
        return False


def build_ydl(out_dir):
    ydl_opts = {
        "format": "bestaudio/best",
        "outtmpl": os.path.join(out_dir, "%(title)s.%(ext)s"),
        "postprocessors": [
            {
                "key": "FFmpegExtractAudio",
                "preferredcodec": "mp3",
                "preferredquality": "0",  # chất lượng cao nhất có thể
            },
            {"key": "FFmpegMetadata"},
        ],
        "noplaylist": True,
        "ignoreerrors": True,
        "quiet": True,           # ẩn log kỹ thuật, chỉ hiện progress hook của mình
        "no_warnings": True,
        "progress_hooks": [make_progress_hook()],
        "extractor_args": {
            "youtube": {"player_client": ["android", "ios", "web"]},
        },
    }
    return YoutubeDL(ydl_opts)


def parse_urls(raw):
    return [u.strip() for u in raw.replace(",", " ").split(" ") if u.strip()]


def print_banner(out_dir):
    os.system("cls" if os.name == "nt" else "clear")
    print(LINE)
    print("   🎧  TẢI MP3 TỪ YOUTUBE")
    print(LINE)
    print(f"   📁 Lưu tại: {out_dir}")
    print(LINE)


def main():
    check_ffmpeg()
    out_dir = get_output_dir()
    print_banner(out_dir)

    total_ok, total_fail = 0, 0

    while True:
        print()
        raw = input("🔗 Dán link YouTube (Enter để thoát): ").strip()
        if not raw:
            break

        urls = parse_urls(raw)
        ydl = build_ydl(out_dir)
        for url in urls:
            ok = download_one(ydl, url)
            total_ok += 1 if ok else 0
            total_fail += 0 if ok else 1
        ydl.__exit__(None, None, None)

    print()
    print(LINE)
    print(f"   Đã tải xong: {total_ok} bài" + (f"   •   Lỗi: {total_fail}" if total_fail else ""))
    print(f"   📁 Thư mục: {out_dir}")
    print(LINE)
    input("\nNhấn Enter để đóng...")


if __name__ == "__main__":
    main()
