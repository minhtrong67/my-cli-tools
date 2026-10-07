/*
 * Hen Gio Tat May - Windows Shutdown Scheduler
 * Native Win32 application (no .NET / Electron dependency)
 * Material-inspired minimalist UI, bilingual Vietnamese/English.
 */

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commctrl.h>
#include <stdio.h>
#include <time.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

#define ID_RADIO_TIME       101
#define ID_RADIO_COUNTDOWN  102
#define ID_EDIT_TIME        103
#define ID_EDIT_HOURS       104
#define ID_EDIT_MINUTES     105
#define ID_CHECK_REPEAT     106
#define ID_CHECK_STARTUP    107
#define ID_CHECK_FORCE      108
#define ID_EDIT_NOTIFY      109
#define ID_STATIC_STATUS    110
#define ID_BTN_START        111
#define ID_BTN_CANCEL       112
#define ID_STATIC_COUNTDOWN 113
#define ID_GROUP_MODE       114
#define ID_GROUP_OPTIONS    115
#define ID_STATIC_TITLE     116
#define ID_STATIC_SUBTITLE  117
#define ID_BTN_LANG_VI      118
#define ID_BTN_LANG_EN      119

#define TIMER_TICK          1
#define WM_TRAYICON         (WM_USER + 1)
#define ID_TRAY_SHOW        2001
#define ID_TRAY_EXIT        2002

#define APP_NAME L"HenGioTatMay"
#define REG_RUN_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"

/* ---------- Material-ish color palette ---------- */
#define CLR_BG        RGB(0xF6,0xF8,0xFA)
#define CLR_CARD      RGB(0xFF,0xFF,0xFF)
#define CLR_BORDER    RGB(0xE1,0xE4,0xE8)
#define CLR_PRIMARY   RGB(0x1A,0x73,0xE8)
#define CLR_PRIMARY_D RGB(0x15,0x57,0xB0)
#define CLR_DANGER    RGB(0xD9,0x30,0x25)
#define CLR_DANGER_D  RGB(0xB1,0x27,0x1A)
#define CLR_TEXT      RGB(0x20,0x21,0x24)
#define CLR_SUBTEXT   RGB(0x5F,0x63,0x68)
#define CLR_DISABLED  RGB(0xC9,0xCC,0xD1)
#define CLR_WARN_BG   RGB(0xFC,0xE8,0xE6)

/* ================= i18n ================= */
typedef enum {
    STR_TITLE, STR_SUBTITLE, STR_GROUP_MODE, STR_RADIO_TIME, STR_RADIO_COUNTDOWN,
    STR_LABEL_HOUR, STR_LABEL_MINUTE, STR_GROUP_OPTIONS, STR_CHECK_REPEAT,
    STR_CHECK_STARTUP, STR_CHECK_FORCE, STR_LABEL_NOTIFY, STR_STATUS_IDLE,
    STR_STATUS_ARMED, STR_STATUS_CANCELLED, STR_STATUS_AUTOARMED, STR_STATUS_WARN,
    STR_COUNTDOWN_SHUTTINGDOWN, STR_BTN_START, STR_BTN_CANCEL, STR_ERR_TITLE,
    STR_ERR_TIME, STR_ERR_COUNTDOWN, STR_TRAY_SHOW, STR_TRAY_EXIT,
    STR_BALLOON_TITLE, STR_BALLOON_MSG, STR_TRAY_TOOLTIP, STR_COUNT
} StrId;

static const WCHAR *STR_VI[STR_COUNT] = {
    L"Hẹn Giờ Tắt Máy",
    L"Tự động tắt máy tính theo giờ hoặc đếm ngược",
    L"  Chế độ hẹn giờ  ",
    L"Theo giờ cụ thể (HH:MM)",
    L"Đếm ngược",
    L"Giờ:",
    L"Phút:",
    L"  Tuỳ chọn  ",
    L"Lặp lại hằng ngày (tự động hẹn lại vào hôm sau)",
    L"Tự khởi động cùng Windows (bắt buộc để lặp lại)",
    L"Buộc đóng ứng dụng đang mở (force)",
    L"Thông báo trước khi tắt (phút):",
    L"Chưa đặt lịch tắt máy",
    L"Đã đặt lịch tắt máy",
    L"Đã huỷ lịch tắt máy",
    L"Đã tự động hẹn lại lịch hằng ngày",
    L"Sắp tắt máy!",
    L"Đang tắt máy...",
    L"Bắt đầu",
    L"Huỷ lịch",
    L"Lỗi",
    L"Vui lòng nhập giờ hợp lệ theo định dạng HH:MM (VD: 23:30)",
    L"Vui lòng nhập thời gian đếm ngược lớn hơn 0.",
    L"Hiện cửa sổ",
    L"Thoát",
    L"Sắp tắt máy!",
    L"Máy tính sẽ tắt sau %d phút nữa. Mở ứng dụng và bấm Huỷ lịch nếu bạn muốn dừng lại.",
    L"Hẹn Giờ Tắt Máy",
};

static const WCHAR *STR_EN[STR_COUNT] = {
    L"Shutdown Scheduler",
    L"Automatically shut down your PC by time or countdown",
    L"  Schedule Mode  ",
    L"At a specific time (HH:MM)",
    L"Countdown",
    L"Hours:",
    L"Minutes:",
    L"  Options  ",
    L"Repeat daily (auto re-arm the next day)",
    L"Launch with Windows (required for daily repeat)",
    L"Force close open applications",
    L"Notify before shutdown (minutes):",
    L"No shutdown scheduled",
    L"Shutdown scheduled",
    L"Schedule cancelled",
    L"Daily schedule re-armed automatically",
    L"Shutting down soon!",
    L"Shutting down...",
    L"Start",
    L"Cancel",
    L"Error",
    L"Please enter a valid time in HH:MM format (e.g. 23:30)",
    L"Please enter a countdown time greater than 0.",
    L"Show window",
    L"Exit",
    L"Shutting down soon!",
    L"Your computer will shut down in %d minute(s). Open the app and click Cancel if you want to stop it.",
    L"Shutdown Scheduler",
};

int currentLang = 0; /* 0 = VI, 1 = EN */
#define T(id) (currentLang == 0 ? STR_VI[id] : STR_EN[id])

HWND hMain, hRadioTime, hRadioCountdown, hEditTime, hEditHours, hEditMinutes;
HWND hCheckRepeat, hCheckStartup, hCheckForce, hEditNotify, hStaticStatus, hStaticCountdown;
HWND hBtnStart, hBtnCancel, hGroupMode, hGroupOptions, hStaticTitle, hStaticSubtitle;
HWND hLabelHour, hLabelMinute, hLabelNotify, hBtnLangVi, hBtnLangEn;
NOTIFYICONDATAW nid;
HICON hAppIcon;
HFONT hFontNormal, hFontBold, hFontTitle, hFontCountdown, hFontSmall;
HBRUSH hBrushBg, hBrushCard;

/* Layout metrics computed once from actual font metrics, so control
   heights always have enough room for Vietnamese diacritics (which
   need more vertical space than plain Latin text) and so spacing
   stays consistent regardless of the system UI font / DPI in use. */
int gRowH = 0, gEditH = 0, gHourRowTop = 0;
const int gCardX = 24, gCardW = 452;
const int gGapXS = 6, gGapS = 10, gGapM = 16, gGapL = 24;

void LayoutHourMinuteRow(int rowTop);

BOOL armed = FALSE;
BOOL notified = FALSE;
time_t targetEpoch = 0;
BOOL repeatDaily = FALSE;
int savedHH = -1, savedMM = -1;
int notifyMinutes = 5;
BOOL forceClose = TRUE;
COLORREF countdownColor = CLR_PRIMARY;
COLORREF statusColor = CLR_SUBTEXT;
int statusKind = 0; /* 0=idle, 1=armed, 2=cancelled, 3=autoarmed, 4=warn */

WCHAR configPath[MAX_PATH];

/* ================= Config persistence (INI in %APPDATA%) ================= */
void BuildConfigPath() {
    WCHAR appdata[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appdata) == S_OK) {
        swprintf(configPath, MAX_PATH, L"%ls\\%ls.ini", appdata, APP_NAME);
    } else {
        wcscpy(configPath, L"hengiotatmay.ini");
    }
}

void SaveConfig(int hh, int mm, BOOL repeat, BOOL startup, int notifyMin, BOOL force, BOOL isArmed) {
    WCHAR buf[32];
    swprintf(buf, 32, L"%d", hh);   WritePrivateProfileStringW(L"Settings", L"Hour", buf, configPath);
    swprintf(buf, 32, L"%d", mm);   WritePrivateProfileStringW(L"Settings", L"Minute", buf, configPath);
    WritePrivateProfileStringW(L"Settings", L"Repeat", repeat ? L"1" : L"0", configPath);
    WritePrivateProfileStringW(L"Settings", L"Startup", startup ? L"1" : L"0", configPath);
    swprintf(buf, 32, L"%d", notifyMin); WritePrivateProfileStringW(L"Settings", L"NotifyMin", buf, configPath);
    WritePrivateProfileStringW(L"Settings", L"Force", force ? L"1" : L"0", configPath);
    WritePrivateProfileStringW(L"Settings", L"Armed", isArmed ? L"1" : L"0", configPath);
}

void LoadConfig(int *hh, int *mm, BOOL *repeat, BOOL *startup, int *notifyMin, BOOL *force, BOOL *wasArmed) {
    *hh = GetPrivateProfileIntW(L"Settings", L"Hour", -1, configPath);
    *mm = GetPrivateProfileIntW(L"Settings", L"Minute", -1, configPath);
    *repeat = GetPrivateProfileIntW(L"Settings", L"Repeat", 0, configPath) != 0;
    *startup = GetPrivateProfileIntW(L"Settings", L"Startup", 0, configPath) != 0;
    *notifyMin = GetPrivateProfileIntW(L"Settings", L"NotifyMin", 5, configPath);
    *force = GetPrivateProfileIntW(L"Settings", L"Force", 1, configPath) != 0;
    *wasArmed = GetPrivateProfileIntW(L"Settings", L"Armed", 0, configPath) != 0;
}

void SaveLangConfig(int lang) {
    WCHAR buf[8];
    swprintf(buf, 8, L"%d", lang);
    WritePrivateProfileStringW(L"Settings", L"Lang", buf, configPath);
}

int LoadLangConfig() {
    return GetPrivateProfileIntW(L"Settings", L"Lang", 0, configPath);
}

/* ================= Startup registry (Run key) ================= */
void SetStartup(BOOL enable) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) return;
    if (enable) {
        WCHAR exePath[MAX_PATH];
        GetModuleFileNameW(NULL, exePath, MAX_PATH);
        RegSetValueExW(hKey, APP_NAME, 0, REG_SZ, (BYTE*)exePath, (wcslen(exePath) + 1) * sizeof(WCHAR));
    } else {
        RegDeleteValueW(hKey, APP_NAME);
    }
    RegCloseKey(hKey);
}

/* ================= Shutdown commands ================= */
void RunShutdownIn(int seconds, BOOL force) {
    WCHAR full[192];
    swprintf(full, 192, L"cmd.exe /c shutdown /s /t %d%ls", seconds, force ? L" /f" : L"");
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    if (CreateProcessW(NULL, full, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void CancelShutdownCmd() {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    WCHAR full[64] = L"cmd.exe /c shutdown /a";
    if (CreateProcessW(NULL, full, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

/* ================= Tray ================= */
void ShowBalloon(const WCHAR *title, const WCHAR *msg) {
    nid.uFlags = NIF_INFO;
    wcsncpy(nid.szInfoTitle, title, 63);
    wcsncpy(nid.szInfo, msg, 255);
    nid.dwInfoFlags = NIIF_WARNING;
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

void InitTray(HWND hwnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATAW);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = hAppIcon;
    wcsncpy(nid.szTip, T(STR_TRAY_TOOLTIP), 127);
    Shell_NotifyIconW(NIM_ADD, &nid);
}

/* ================= Helpers ================= */
time_t ComputeNextEpoch(int hh, int mm) {
    time_t now = time(NULL);
    struct tm t;
    localtime_s(&t, &now);
    t.tm_hour = hh;
    t.tm_min = mm;
    t.tm_sec = 0;
    time_t target = mktime(&t);
    if (target <= now) target += 24 * 3600;
    return target;
}

void SetStatusKind(int kind, COLORREF color) {
    statusKind = kind;
    statusColor = color;
    SetWindowTextW(hStaticStatus, T(STR_STATUS_IDLE + kind));
    InvalidateRect(hStaticStatus, NULL, TRUE);
}

void ArmSchedule(int seconds, BOOL force) {
    RunShutdownIn(seconds, force);
    targetEpoch = time(NULL) + seconds;
    armed = TRUE;
    notified = FALSE;
    EnableWindow(hBtnStart, FALSE);
    EnableWindow(hBtnCancel, TRUE);
    InvalidateRect(hBtnStart, NULL, TRUE);
    InvalidateRect(hBtnCancel, NULL, TRUE);
    SetTimer(hMain, TIMER_TICK, 1000, NULL);
}

void DisarmSchedule(BOOL callCancelCmd) {
    if (callCancelCmd) CancelShutdownCmd();
    armed = FALSE;
    KillTimer(hMain, TIMER_TICK);
    EnableWindow(hBtnStart, TRUE);
    EnableWindow(hBtnCancel, FALSE);
    InvalidateRect(hBtnStart, NULL, TRUE);
    InvalidateRect(hBtnCancel, NULL, TRUE);
    SetStatusKind(2, CLR_SUBTEXT); /* cancelled */
    SetWindowTextW(hStaticCountdown, L"");
}

/* ================= Apply language to all controls ================= */
void ApplyLanguage() {
    SetWindowTextW(hMain, T(STR_TITLE));
    SetWindowTextW(hStaticTitle, T(STR_TITLE));
    SetWindowTextW(hStaticSubtitle, T(STR_SUBTITLE));
    SetWindowTextW(hGroupMode, T(STR_GROUP_MODE));
    SetWindowTextW(hRadioTime, T(STR_RADIO_TIME));
    SetWindowTextW(hRadioCountdown, T(STR_RADIO_COUNTDOWN));
    SetWindowTextW(hLabelHour, T(STR_LABEL_HOUR));
    SetWindowTextW(hLabelMinute, T(STR_LABEL_MINUTE));
    SetWindowTextW(hGroupOptions, T(STR_GROUP_OPTIONS));
    SetWindowTextW(hCheckRepeat, T(STR_CHECK_REPEAT));
    SetWindowTextW(hCheckStartup, T(STR_CHECK_STARTUP));
    SetWindowTextW(hCheckForce, T(STR_CHECK_FORCE));
    SetWindowTextW(hLabelNotify, T(STR_LABEL_NOTIFY));
    SetWindowTextW(hStaticStatus, T(STR_STATUS_IDLE + statusKind));
    LayoutHourMinuteRow(gHourRowTop);
    wcsncpy(nid.szTip, T(STR_TRAY_TOOLTIP), 127);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
    InvalidateRect(hBtnStart, NULL, TRUE);
    InvalidateRect(hBtnCancel, NULL, TRUE);
    InvalidateRect(hBtnLangVi, NULL, TRUE);
    InvalidateRect(hBtnLangEn, NULL, TRUE);
}

/* ================= Owner-draw flat buttons ================= */
void DrawFlatButton(LPDRAWITEMSTRUCT dis, COLORREF normal, COLORREF darker, const WCHAR *label) {
    BOOL disabled = (dis->itemState & ODS_DISABLED) != 0;
    BOOL pressed  = (dis->itemState & ODS_SELECTED) != 0;
    COLORREF fill = disabled ? CLR_DISABLED : (pressed ? darker : normal);

    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    HBRUSH hBrush = CreateSolidBrush(fill);
    HPEN hPen = CreatePen(PS_SOLID, 1, fill);
    HBRUSH hOldBrush = SelectObject(hdc, hBrush);
    HPEN hOldPen = SelectObject(hdc, hPen);

    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 10, 10);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0xFF,0xFF,0xFF));
    HFONT hOldFont = SelectObject(hdc, hFontBold);
    DrawTextW(hdc, label, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hOldFont);
}

void DrawLangButton(LPDRAWITEMSTRUCT dis, BOOL active, const WCHAR *label) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    COLORREF fill = active ? CLR_PRIMARY : CLR_CARD;
    COLORREF border = active ? CLR_PRIMARY : CLR_BORDER;
    COLORREF txt = active ? RGB(0xFF,0xFF,0xFF) : CLR_SUBTEXT;

    HBRUSH hBrush = CreateSolidBrush(fill);
    HPEN hPen = CreatePen(PS_SOLID, 1, border);
    HBRUSH hOldBrush = SelectObject(hdc, hBrush);
    HPEN hOldPen = SelectObject(hdc, hPen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, 8, 8);
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, txt);
    HFONT hOldFont = SelectObject(hdc, hFontSmall);
    DrawTextW(hdc, label, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, hOldFont);
}

/* Re-measures "Gio:"/"Hours:" and "Phut:"/"Minutes:" in the current
   language and repositions that whole row so nothing overlaps or
   gets clipped, whichever language is longer. */
void LayoutHourMinuteRow(int rowTop) {
    HDC hdc = GetDC(hMain);
    HFONT old = SelectObject(hdc, hFontNormal);
    SIZE szH, szM;
    const WCHAR *lblH = T(STR_LABEL_HOUR);
    const WCHAR *lblM = T(STR_LABEL_MINUTE);
    GetTextExtentPoint32W(hdc, lblH, (int)wcslen(lblH), &szH);
    GetTextExtentPoint32W(hdc, lblM, (int)wcslen(lblM), &szM);
    SelectObject(hdc, old);
    ReleaseDC(hMain, hdc);

    int x = gCardX + 16;
    int editTimeW = 110;
    int editSmallW = 46;
    int labelY = rowTop + (gEditH - gRowH) / 2;

    MoveWindow(hEditTime, x, rowTop, editTimeW, gEditH, TRUE);
    x += editTimeW + gGapM;

    int hourLabelW = szH.cx + 8;
    MoveWindow(hLabelHour, x, labelY, hourLabelW, gRowH, TRUE);
    x += hourLabelW + gGapS;

    MoveWindow(hEditHours, x, rowTop, editSmallW, gEditH, TRUE);
    x += editSmallW + gGapM;

    int minuteLabelW = szM.cx + 8;
    MoveWindow(hLabelMinute, x, labelY, minuteLabelW, gRowH, TRUE);
    x += minuteLabelW + gGapS;

    MoveWindow(hEditMinutes, x, rowTop, editSmallW, gEditH, TRUE);
}

/* ================= Main window procedure ================= */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        /* --- Fonts first: derived from the OS's own UI font (System UI
               font), so the app matches whatever Windows is actually
               using (Segoe UI Variable / Segoe UI / Tahoma / a regional
               font), and respects the user's text-size / DPI settings.
               Layout below is measured from these fonts, so nothing
               (especially Vietnamese diacritics) gets clipped. --- */
        NONCLIENTMETRICSW ncm; ncm.cbSize = sizeof(ncm);
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
        LOGFONTW lfBase = ncm.lfMessageFont;

        LOGFONTW lf;
        lf = lfBase; lf.lfWeight = FW_NORMAL;
        hFontNormal = CreateFontIndirectW(&lf);

        lf = lfBase; lf.lfWeight = FW_SEMIBOLD;
        hFontBold = CreateFontIndirectW(&lf);

        lf = lfBase; lf.lfWeight = FW_BOLD; lf.lfHeight = MulDiv(lfBase.lfHeight, 160, 100);
        hFontTitle = CreateFontIndirectW(&lf);

        lf = lfBase; lf.lfWeight = FW_BOLD; lf.lfHeight = MulDiv(lfBase.lfHeight, 215, 100);
        hFontCountdown = CreateFontIndirectW(&lf);

        lf = lfBase; lf.lfWeight = FW_SEMIBOLD; lf.lfHeight = MulDiv(lfBase.lfHeight, 85, 100);
        hFontSmall = CreateFontIndirectW(&lf);

        /* Measure real line heights (includes room for Vietnamese
           diacritics above/below the base line) and derive every
           row height and gap from them, instead of hard-coded pixels. */
        HDC mdc = GetDC(hwnd);
        TEXTMETRICW tmN, tmB, tmT, tmC;
        HFONT oldF = SelectObject(mdc, hFontNormal); GetTextMetricsW(mdc, &tmN); SelectObject(mdc, oldF);
        oldF = SelectObject(mdc, hFontBold);      GetTextMetricsW(mdc, &tmB); SelectObject(mdc, oldF);
        oldF = SelectObject(mdc, hFontTitle);     GetTextMetricsW(mdc, &tmT); SelectObject(mdc, oldF);
        oldF = SelectObject(mdc, hFontCountdown); GetTextMetricsW(mdc, &tmC); SelectObject(mdc, oldF);
        ReleaseDC(hwnd, mdc);

        int lineN = tmN.tmHeight + tmN.tmExternalLeading;
        int lineB = tmB.tmHeight + tmB.tmExternalLeading;
        int lineT = tmT.tmHeight + tmT.tmExternalLeading;
        int lineC = tmC.tmHeight + tmC.tmExternalLeading;

        gRowH  = lineN + 12;   /* label / checkbox / radio row: extra room for dau */
        gEditH = lineN + 14;   /* edit box: border + padding + dau room */

        int y = gGapL;

        /* --- Title row + language toggle --- */
        int titleRowH = lineT + 8;
        int smallBtnH = gRowH - 4;
        hStaticTitle = CreateWindowW(L"STATIC", T(STR_TITLE), WS_VISIBLE | WS_CHILD,
            gGapL, y, 300, titleRowH, hwnd, (HMENU)ID_STATIC_TITLE, NULL, NULL);
        int langY = y + (titleRowH - smallBtnH) / 2;
        hBtnLangEn = CreateWindowW(L"BUTTON", L"EN", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            gCardX + gCardW - 38, langY, 38, smallBtnH, hwnd, (HMENU)ID_BTN_LANG_EN, NULL, NULL);
        hBtnLangVi = CreateWindowW(L"BUTTON", L"VI", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            gCardX + gCardW - 38 - gGapXS - 38, langY, 38, smallBtnH, hwnd, (HMENU)ID_BTN_LANG_VI, NULL, NULL);
        y += titleRowH + gGapXS;

        /* --- Subtitle row --- */
        int subtitleRowH = lineN + 6;
        hStaticSubtitle = CreateWindowW(L"STATIC", T(STR_SUBTITLE), WS_VISIBLE | WS_CHILD,
            gGapL, y, gCardW, subtitleRowH, hwnd, (HMENU)ID_STATIC_SUBTITLE, NULL, NULL);
        y += subtitleRowH + gGapL;

        /* --- Card 1: mode --- */
        int groupModeTop = y;
        int modeTopPad = lineB + 14;
        int modeInnerY = groupModeTop + modeTopPad;

        hRadioTime = CreateWindowW(L"BUTTON", T(STR_RADIO_TIME),
            WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
            gCardX + 16, modeInnerY, 230, gRowH, hwnd, (HMENU)ID_RADIO_TIME, NULL, NULL);
        hRadioCountdown = CreateWindowW(L"BUTTON", T(STR_RADIO_COUNTDOWN),
            WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            gCardX + 16 + 234, modeInnerY, 150, gRowH, hwnd, (HMENU)ID_RADIO_COUNTDOWN, NULL, NULL);
        modeInnerY += gRowH + gGapS;

        gHourRowTop = modeInnerY;
        hEditTime = CreateWindowW(L"EDIT", L"23:30", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            gCardX + 16, modeInnerY, 110, gEditH, hwnd, (HMENU)ID_EDIT_TIME, NULL, NULL);
        hLabelHour = CreateWindowW(L"STATIC", T(STR_LABEL_HOUR), WS_VISIBLE | WS_CHILD,
            gCardX + 150, modeInnerY, 60, gRowH, hwnd, NULL, NULL, NULL);
        hEditHours = CreateWindowW(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            gCardX + 210, modeInnerY, 46, gEditH, hwnd, (HMENU)ID_EDIT_HOURS, NULL, NULL);
        hLabelMinute = CreateWindowW(L"STATIC", T(STR_LABEL_MINUTE), WS_VISIBLE | WS_CHILD,
            gCardX + 270, modeInnerY, 60, gRowH, hwnd, NULL, NULL, NULL);
        hEditMinutes = CreateWindowW(L"EDIT", L"30", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            gCardX + 330, modeInnerY, 46, gEditH, hwnd, (HMENU)ID_EDIT_MINUTES, NULL, NULL);
        modeInnerY += gEditH + gGapM;

        int groupModeH = modeInnerY - groupModeTop;
        hGroupMode = CreateWindowW(L"BUTTON", T(STR_GROUP_MODE), WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
            gCardX, groupModeTop, gCardW, groupModeH, hwnd, (HMENU)ID_GROUP_MODE, NULL, NULL);

        LayoutHourMinuteRow(gHourRowTop);
        y = groupModeTop + groupModeH + gGapL;

        /* --- Card 2: options --- */
        int groupOptionsTop = y;
        int optTopPad = lineB + 14;
        int optY = groupOptionsTop + optTopPad;

        hCheckRepeat = CreateWindowW(L"BUTTON", T(STR_CHECK_REPEAT),
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            gCardX + 16, optY, gCardW - 32, gRowH, hwnd, (HMENU)ID_CHECK_REPEAT, NULL, NULL);
        optY += gRowH + gGapS;

        hCheckStartup = CreateWindowW(L"BUTTON", T(STR_CHECK_STARTUP),
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            gCardX + 16, optY, gCardW - 32, gRowH, hwnd, (HMENU)ID_CHECK_STARTUP, NULL, NULL);
        optY += gRowH + gGapS;

        hCheckForce = CreateWindowW(L"BUTTON", T(STR_CHECK_FORCE),
            WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
            gCardX + 16, optY, gCardW - 32, gRowH, hwnd, (HMENU)ID_CHECK_FORCE, NULL, NULL);
        SendMessageW(hCheckForce, BM_SETCHECK, BST_CHECKED, 0);
        optY += gRowH + gGapS;

        int notifyEditW = 50;
        int notifyEditX = gCardX + gCardW - 16 - notifyEditW;
        int notifyLabelW = notifyEditX - gGapM - (gCardX + 16);
        hLabelNotify = CreateWindowW(L"STATIC", T(STR_LABEL_NOTIFY), WS_VISIBLE | WS_CHILD,
            gCardX + 16, optY, notifyLabelW, gRowH, hwnd, NULL, NULL, NULL);
        hEditNotify = CreateWindowW(L"EDIT", L"5", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            notifyEditX, optY, notifyEditW, gEditH, hwnd, (HMENU)ID_EDIT_NOTIFY, NULL, NULL);
        optY += gEditH + gGapM;

        int groupOptionsH = optY - groupOptionsTop;
        hGroupOptions = CreateWindowW(L"BUTTON", T(STR_GROUP_OPTIONS), WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
            gCardX, groupOptionsTop, gCardW, groupOptionsH, hwnd, (HMENU)ID_GROUP_OPTIONS, NULL, NULL);

        y = groupOptionsTop + groupOptionsH + gGapL;

        /* --- Status area --- */
        int statusRowH = lineB + 8;
        hStaticStatus = CreateWindowW(L"STATIC", T(STR_STATUS_IDLE), WS_VISIBLE | WS_CHILD | SS_CENTER,
            gCardX, y, gCardW, statusRowH, hwnd, (HMENU)ID_STATIC_STATUS, NULL, NULL);
        y += statusRowH + gGapXS;

        int countdownRowH = lineC + 10;
        hStaticCountdown = CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | SS_CENTER,
            gCardX, y, gCardW, countdownRowH, hwnd, (HMENU)ID_STATIC_COUNTDOWN, NULL, NULL);
        y += countdownRowH + gGapL;

        /* --- Buttons (owner-drawn, flat Material style) --- */
        int btnH = 44;
        int btnW = (gCardW - gGapM) / 2;
        hBtnStart = CreateWindowW(L"BUTTON", T(STR_BTN_START), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            gCardX, y, btnW, btnH, hwnd, (HMENU)ID_BTN_START, NULL, NULL);
        hBtnCancel = CreateWindowW(L"BUTTON", T(STR_BTN_CANCEL), WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
            gCardX + btnW + gGapM, y, btnW, btnH, hwnd, (HMENU)ID_BTN_CANCEL, NULL, NULL);
        EnableWindow(hBtnCancel, FALSE);
        y += btnH + gGapL;

        /* --- Resize the window so the client area matches the
               content we just measured out (keeps bottom margin
               even, and adapts to larger system fonts / DPI). --- */
        RECT wanted = { 0, 0, gCardX * 2 + gCardW, y };
        AdjustWindowRectEx(&wanted, GetWindowLongW(hwnd, GWL_STYLE), FALSE, GetWindowLongW(hwnd, GWL_EXSTYLE));
        SetWindowPos(hwnd, NULL, 0, 0, wanted.right - wanted.left, wanted.bottom - wanted.top,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

        HWND child = GetWindow(hwnd, GW_CHILD);
        while (child) {
            SendMessageW(child, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
            child = GetWindow(child, GW_HWNDNEXT);
        }
        SendMessageW(hStaticTitle, WM_SETFONT, (WPARAM)hFontTitle, TRUE);
        SendMessageW(hStaticSubtitle, WM_SETFONT, (WPARAM)hFontNormal, TRUE);
        SendMessageW(hStaticCountdown, WM_SETFONT, (WPARAM)hFontCountdown, TRUE);
        SendMessageW(hStaticStatus, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        SendMessageW(hGroupMode, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        SendMessageW(hGroupOptions, WM_SETFONT, (WPARAM)hFontBold, TRUE);

        SendMessageW(hRadioTime, BM_SETCHECK, BST_CHECKED, 0);

        InitTray(hwnd);

        /* Auto re-arm if previous session left an active daily schedule */
        int hh, mm, notifyMin; BOOL repeat, startup, force, wasArmed;
        LoadConfig(&hh, &mm, &repeat, &startup, &notifyMin, &force, &wasArmed);
        if (repeat && startup && hh >= 0 && mm >= 0) {
            SendMessageW(hCheckRepeat, BM_SETCHECK, BST_CHECKED, 0);
            SendMessageW(hCheckStartup, BM_SETCHECK, BST_CHECKED, 0);
            SendMessageW(hCheckForce, BM_SETCHECK, force ? BST_CHECKED : BST_UNCHECKED, 0);
            WCHAR buf[16]; swprintf(buf, 16, L"%02d:%02d", hh, mm);
            SetWindowTextW(hEditTime, buf);
            swprintf(buf, 16, L"%d", notifyMin);
            SetWindowTextW(hEditNotify, buf);
            repeatDaily = TRUE; savedHH = hh; savedMM = mm; notifyMinutes = notifyMin; forceClose = force;

            time_t next = ComputeNextEpoch(hh, mm);
            int seconds = (int)(next - time(NULL));
            ArmSchedule(seconds, force);
            SetStatusKind(3, CLR_PRIMARY_D); /* autoarmed */
            ShowWindow(hwnd, SW_HIDE);
        }
        break;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
        if (dis->CtlID == ID_BTN_START) {
            DrawFlatButton(dis, CLR_PRIMARY, CLR_PRIMARY_D, T(STR_BTN_START));
            return TRUE;
        } else if (dis->CtlID == ID_BTN_CANCEL) {
            DrawFlatButton(dis, CLR_DANGER, CLR_DANGER_D, T(STR_BTN_CANCEL));
            return TRUE;
        } else if (dis->CtlID == ID_BTN_LANG_VI) {
            DrawLangButton(dis, currentLang == 0, L"VI");
            return TRUE;
        } else if (dis->CtlID == ID_BTN_LANG_EN) {
            DrawLangButton(dis, currentLang == 1, L"EN");
            return TRUE;
        }
        break;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hCtl = (HWND)lParam;
        SetBkMode(hdcStatic, TRANSPARENT);
        if (hCtl == hStaticCountdown) {
            SetTextColor(hdcStatic, countdownColor);
        } else if (hCtl == hStaticStatus) {
            SetTextColor(hdcStatic, statusColor);
        } else if (hCtl == hStaticTitle) {
            SetTextColor(hdcStatic, CLR_TEXT);
        } else if (hCtl == hStaticSubtitle) {
            SetTextColor(hdcStatic, CLR_SUBTEXT);
        } else {
            SetTextColor(hdcStatic, CLR_TEXT);
        }
        return (LRESULT)hBrushBg;
    }

    case WM_CTLCOLORBTN:
        return (LRESULT)hBrushBg;

    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rc; GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, hBrushBg);
        return 1;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_BTN_LANG_VI && currentLang != 0) {
            currentLang = 0;
            SaveLangConfig(0);
            ApplyLanguage();
        } else if (id == ID_BTN_LANG_EN && currentLang != 1) {
            currentLang = 1;
            SaveLangConfig(1);
            ApplyLanguage();
        } else if (id == ID_BTN_START) {
            WCHAR buf[64];
            int seconds = -1;
            BOOL isTimeMode = (SendMessageW(hRadioTime, BM_GETCHECK, 0, 0) == BST_CHECKED);
            forceClose = (SendMessageW(hCheckForce, BM_GETCHECK, 0, 0) == BST_CHECKED);
            repeatDaily = (SendMessageW(hCheckRepeat, BM_GETCHECK, 0, 0) == BST_CHECKED);
            BOOL startupChecked = (SendMessageW(hCheckStartup, BM_GETCHECK, 0, 0) == BST_CHECKED);
            GetWindowTextW(hEditNotify, buf, 64);
            notifyMinutes = _wtoi(buf);
            if (notifyMinutes < 0) notifyMinutes = 0;

            if (isTimeMode) {
                GetWindowTextW(hEditTime, buf, 64);
                int hh = -1, mm = -1;
                if (swscanf(buf, L"%d:%d", &hh, &mm) != 2 || hh < 0 || hh > 23 || mm < 0 || mm > 59) {
                    MessageBoxW(hwnd, T(STR_ERR_TIME), T(STR_ERR_TITLE), MB_ICONWARNING);
                    break;
                }
                savedHH = hh; savedMM = mm;
                time_t next = ComputeNextEpoch(hh, mm);
                seconds = (int)(next - time(NULL));
            } else {
                WCHAR hbuf[16], mbuf[16];
                GetWindowTextW(hEditHours, hbuf, 16);
                GetWindowTextW(hEditMinutes, mbuf, 16);
                int h = _wtoi(hbuf), m = _wtoi(mbuf);
                seconds = h * 3600 + m * 60;
                if (seconds <= 0) {
                    MessageBoxW(hwnd, T(STR_ERR_COUNTDOWN), T(STR_ERR_TITLE), MB_ICONWARNING);
                    break;
                }
                if (repeatDaily) {
                    time_t next = time(NULL) + seconds;
                    struct tm t; localtime_s(&t, &next);
                    savedHH = t.tm_hour; savedMM = t.tm_min;
                }
            }

            if (repeatDaily && startupChecked) SetStartup(TRUE);
            else if (!repeatDaily) SetStartup(FALSE);

            SaveConfig(savedHH, savedMM, repeatDaily, startupChecked, notifyMinutes, forceClose, TRUE);
            ArmSchedule(seconds, forceClose);
            SetStatusKind(1, CLR_PRIMARY_D); /* armed */
            countdownColor = CLR_PRIMARY;
        } else if (id == ID_BTN_CANCEL) {
            DisarmSchedule(TRUE);
            SaveConfig(savedHH, savedMM, repeatDaily, FALSE, notifyMinutes, forceClose, FALSE);
        }
        break;
    }

    case WM_TIMER: {
        if (wParam == TIMER_TICK) {
            time_t now = time(NULL);
            long remaining = (long)(targetEpoch - now);
            if (remaining <= 0) {
                countdownColor = CLR_DANGER;
                InvalidateRect(hStaticCountdown, NULL, TRUE);
                SetWindowTextW(hStaticCountdown, T(STR_COUNTDOWN_SHUTTINGDOWN));
                KillTimer(hMain, TIMER_TICK);
                if (repeatDaily) {
                    SaveConfig(savedHH, savedMM, TRUE, TRUE, notifyMinutes, forceClose, TRUE);
                }
            } else {
                int hh = remaining / 3600;
                int mm = (remaining % 3600) / 60;
                int ss = remaining % 60;
                WCHAR buf[64];
                swprintf(buf, 64, L"%02d:%02d:%02d", hh, mm, ss);
                SetWindowTextW(hStaticCountdown, buf);

                if (!notified && notifyMinutes > 0 && remaining <= notifyMinutes * 60) {
                    WCHAR msg[200];
                    swprintf(msg, 200, T(STR_BALLOON_MSG), notifyMinutes);
                    ShowBalloon(T(STR_BALLOON_TITLE), msg);
                    notified = TRUE;
                }
                if (remaining <= 60) {
                    countdownColor = CLR_DANGER;
                    SetStatusKind(4, CLR_DANGER_D); /* warn */
                } else if (remaining <= (long)notifyMinutes * 60) {
                    countdownColor = RGB(0xE3,0x7B,0x00);
                }
                InvalidateRect(hStaticCountdown, NULL, TRUE);
            }
        }
        break;
    }

    case WM_TRAYICON:
        if (lParam == WM_LBUTTONDBLCLK) {
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
        } else if (lParam == WM_RBUTTONUP) {
            POINT pt; GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_SHOW, T(STR_TRAY_SHOW));
            AppendMenuW(hMenu, MF_STRING, ID_TRAY_EXIT, T(STR_TRAY_EXIT));
            SetForegroundWindow(hwnd);
            int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
            if (cmd == ID_TRAY_SHOW) { ShowWindow(hwnd, SW_SHOW); SetForegroundWindow(hwnd); }
            else if (cmd == ID_TRAY_EXIT) { DestroyWindow(hwnd); }
        }
        break;

    case WM_CLOSE:
        if (armed) {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        Shell_NotifyIconW(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrev, PWSTR pCmdLine, int nCmdShow) {
    BuildConfigPath();
    currentLang = LoadLangConfig();
    hAppIcon = LoadIconW(NULL, IDI_INFORMATION);
    hBrushBg = CreateSolidBrush(CLR_BG);
    hBrushCard = CreateSolidBrush(CLR_CARD);

    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = APP_NAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = hBrushBg;
    wc.hIcon = hAppIcon;
    RegisterClassW(&wc);

    hMain = CreateWindowExW(0, APP_NAME, T(STR_TITLE),
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 524, 552,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hMain, nCmdShow);
    UpdateWindow(hMain);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
