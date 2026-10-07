# -*- coding: utf-8 -*-
"""
Random Rename Tool
-------------------
Renames files/folders with a random value.

Two ways to run this script:

  --quick    <path> [path...]   Rename instantly using saved settings,
                                  no window. This is what the right-click
                                  context menu entry runs.
  --settings                     Open a settings window to configure how
                                  Random Rename should behave (character
                                  type, length, prefix/suffix, ...). Click
                                  "Save" to store the configuration - it is
                                  then used automatically by "--quick".
                                  This is what configure_settings.bat runs.

Default behaviour (nothing configured yet): random mix of letters + numbers,
8 characters long, original file extension kept.
"""

import os
import sys
import json
import random
import string
import tkinter as tk
from tkinter import messagebox

DEFAULT_SETTINGS = {
    "mode": "alphanumeric",     # numbers | letters | alphanumeric | custom
    "length": 8,
    "letter_case": "lower",     # lower | upper | mixed
    "allow_leading_zero": True,
    "custom_charset": "",
    "prefix": "",
    "suffix": "",
    "keep_ext": True,
    "same_value_for_all": False,
}

MODE_LABELS = {
    "numbers": "🔢  Numbers",
    "letters": "🔤  Letters",
    "alphanumeric": "🔀  Letters + Numbers",
    "custom": "✨  Custom",
}

CASE_LABELS = {
    "lower": "lowercase",
    "upper": "UPPERCASE",
    "mixed": "MiXeD",
}


# ======================================================================
# Settings persistence
# ======================================================================
def settings_path():
    root = os.environ.get("APPDATA") or os.path.expanduser("~")
    folder = os.path.join(root, "RandomRenameTool")
    os.makedirs(folder, exist_ok=True)
    return os.path.join(folder, "settings.json")


def load_settings():
    path = settings_path()
    settings = dict(DEFAULT_SETTINGS)
    try:
        if os.path.exists(path):
            with open(path, "r", encoding="utf-8") as f:
                data = json.load(f)
            # migrate old key names from earlier versions
            if "digits" in data and "length" not in data:
                data["length"] = data["digits"]
            if "same_number" in data and "same_value_for_all" not in data:
                data["same_value_for_all"] = data["same_number"]
            if "mode" not in data and "digits" in data:
                data["mode"] = "numbers"
            for k in DEFAULT_SETTINGS:
                if k in data:
                    settings[k] = data[k]
    except Exception:
        pass
    return settings


def save_settings(settings):
    try:
        with open(settings_path(), "w", encoding="utf-8") as f:
            json.dump(settings, f, indent=2)
        return True
    except Exception:
        return False


# ======================================================================
# Random value generation
# ======================================================================
def get_charset(settings):
    mode = settings.get("mode", "alphanumeric")
    case = settings.get("letter_case", "lower")

    def letters():
        if case == "upper":
            return string.ascii_uppercase
        if case == "mixed":
            return string.ascii_letters
        return string.ascii_lowercase

    if mode == "numbers":
        return string.digits
    if mode == "letters":
        return letters()
    if mode == "custom":
        charset = "".join(dict.fromkeys(settings.get("custom_charset", "")))  # dedupe, keep order
        return charset if charset else (string.ascii_lowercase + string.digits)
    # alphanumeric (default)
    return letters() + string.digits


def gen_random_value(settings):
    length = max(1, int(settings.get("length", 8)))
    charset = get_charset(settings) or (string.ascii_lowercase + string.digits)
    mode = settings.get("mode", "alphanumeric")

    if mode == "numbers" and not settings.get("allow_leading_zero", True) and length > 1:
        nonzero = charset.replace("0", "") or charset
        first = random.choice(nonzero)
        rest = "".join(random.choice(charset) for _ in range(length - 1))
        return first + rest

    return "".join(random.choice(charset) for _ in range(length))


# ======================================================================
# File item helpers
# ======================================================================
def build_items(paths):
    items = []
    for p in paths:
        if not os.path.exists(p):
            continue
        d = os.path.dirname(p)
        is_dir = os.path.isdir(p)
        base = os.path.basename(p)
        if is_dir:
            name, ext = base, ""
        else:
            name, ext = os.path.splitext(base)
        items.append({
            "old_path": p, "dir": d, "is_dir": is_dir,
            "old_base": base, "name": name, "ext": ext,
            "new_name": base,
        })
    return items


def build_sample_items():
    """Fake items used only to preview a naming pattern in the settings window."""
    return [
        {"old_path": "", "dir": "", "is_dir": False, "old_base": "vacation_photo.jpg",
         "name": "vacation_photo", "ext": ".jpg", "new_name": "vacation_photo.jpg"},
        {"old_path": "", "dir": "", "is_dir": False, "old_base": "invoice_report.pdf",
         "name": "invoice_report", "ext": ".pdf", "new_name": "invoice_report.pdf"},
        {"old_path": "", "dir": "", "is_dir": True, "old_base": "Project Files",
         "name": "Project Files", "ext": "", "new_name": "Project Files"},
    ]


def build_candidate_name(item, settings, value):
    prefix = settings.get("prefix", "")
    suffix = settings.get("suffix", "")
    ext = "" if (item["is_dir"] or not settings.get("keep_ext", True)) else item["ext"]
    return f"{prefix}{value}{suffix}{ext}"


def unique_new_name(item, settings, reserved, shared_value=None, check_disk=True):
    new_name = item["old_base"]
    for _ in range(300):
        value = shared_value if shared_value is not None else gen_random_value(settings)
        new_name = build_candidate_name(item, settings, value)
        target = os.path.join(item["dir"], new_name)
        exists_on_disk = check_disk and os.path.exists(target)
        if new_name == item["old_base"]:
            if shared_value is None:
                continue
        elif target not in reserved and not exists_on_disk:
            return new_name
        if shared_value is not None:
            for i in range(1, 300):
                if item["is_dir"] or not settings.get("keep_ext", True):
                    alt = f"{new_name}_{i}"
                else:
                    stem, ext = os.path.splitext(new_name)
                    alt = f"{stem}_{i}{ext}"
                alt_target = os.path.join(item["dir"], alt)
                alt_exists = check_disk and os.path.exists(alt_target)
                if alt != item["old_base"] and alt_target not in reserved and not alt_exists:
                    return alt
    return new_name


def assign_new_names(items, settings, check_disk=True):
    reserved = set()
    shared_value = gen_random_value(settings) if settings.get("same_value_for_all") else None
    for item in items:
        new_name = unique_new_name(item, settings, reserved, shared_value, check_disk=check_disk)
        item["new_name"] = new_name
        reserved.add(os.path.join(item["dir"], new_name))
    return items


def do_rename(items):
    errors = []
    done = 0
    for item in items:
        new_name = (item.get("new_name") or "").strip()
        if not new_name:
            errors.append(f"{item['old_base']}: new name is empty")
            continue
        new_path = os.path.join(item["dir"], new_name)
        if new_path == item["old_path"]:
            done += 1
            continue
        if os.path.exists(new_path):
            errors.append(f"{item['old_base']}: '{new_name}' already exists")
            continue
        try:
            os.rename(item["old_path"], new_path)
            done += 1
        except Exception as e:
            errors.append(f"{item['old_base']}: {e}")
    return done, errors


# ======================================================================
# Quick mode (no window) - runs from the right-click context menu.
# Uses plain tkinter only, so it never depends on customtkinter being
# installed - the fast path always works.
# ======================================================================
def run_quick(paths):
    items = build_items(paths)
    if not items:
        return
    settings = load_settings()
    assign_new_names(items, settings)
    done, errors = do_rename(items)
    if errors:
        root = tk.Tk()
        root.withdraw()
        messagebox.showwarning(
            "Random Rename",
            f"Renamed: {done}/{len(items)}\n\nIssues:\n" + "\n".join(errors)
        )
        root.destroy()


# ======================================================================
# Settings window - runs from configure_settings.bat, built with
# CustomTkinter for a modern, rounded, native-feeling look with
# automatic light/dark mode support.
# ======================================================================
try:
    import customtkinter as ctk
    CTK_AVAILABLE = True
except ImportError:
    CTK_AVAILABLE = False

if CTK_AVAILABLE:
    ctk.set_appearance_mode("System")
    ctk.set_default_color_theme("blue")

ACCENT = "#0078D4"
ACCENT_HOVER = "#106ebe"
SUCCESS = ("#107c10", "#4caf50")
TEXT_SUB = ("gray40", "gray60")
GHOST_HOVER = ("gray92", "gray25")
GHOST_BORDER = ("gray70", "gray35")

FONT_FAMILY = "Segoe UI"
FONT_MONO = "Consolas"

LABEL_TO_MODE = {v: k for k, v in MODE_LABELS.items()}
LABEL_TO_CASE = {v: k for k, v in CASE_LABELS.items()}


class SettingsApp:
    def __init__(self, root):
        self.root = root
        self.settings = load_settings()
        self.items = build_sample_items()
        self._status_after_id = None

        self._init_vars()
        self._build_ui()
        self._refresh_mode_visibility()
        self.randomize_all()

    # ---------------- state ----------------
    def _init_vars(self):
        s = self.settings
        self.mode_var = tk.StringVar(value=s["mode"])
        self.mode_display_var = tk.StringVar(value=MODE_LABELS[s["mode"]])
        self.length_var = tk.IntVar(value=s["length"])
        self.case_var = tk.StringVar(value=s["letter_case"])
        self.case_display_var = tk.StringVar(value=CASE_LABELS[s["letter_case"]])
        self.leading_zero_var = tk.BooleanVar(value=s["allow_leading_zero"])
        self.custom_charset_var = tk.StringVar(value=s["custom_charset"])
        self.prefix_var = tk.StringVar(value=s["prefix"])
        self.suffix_var = tk.StringVar(value=s["suffix"])
        self.keep_ext_var = tk.BooleanVar(value=s["keep_ext"])
        self.same_value_var = tk.BooleanVar(value=s["same_value_for_all"])

    def _current_settings(self):
        try:
            length = int(self.length_var.get())
        except (tk.TclError, ValueError):
            length = self.settings.get("length", 8)
        return {
            "mode": self.mode_var.get(),
            "length": max(1, min(64, length)),
            "letter_case": self.case_var.get(),
            "allow_leading_zero": bool(self.leading_zero_var.get()),
            "custom_charset": self.custom_charset_var.get(),
            "prefix": self.prefix_var.get(),
            "suffix": self.suffix_var.get(),
            "keep_ext": bool(self.keep_ext_var.get()),
            "same_value_for_all": bool(self.same_value_var.get()),
        }

    # ---------------- helpers ----------------
    def _section_title(self, parent, text):
        ctk.CTkLabel(parent, text=text, font=ctk.CTkFont(family=FONT_FAMILY, size=13, weight="bold"),
                     anchor="w").pack(fill="x", padx=24, pady=(0, 8))

    def _ghost_button(self, parent, text, command):
        return ctk.CTkButton(
            parent, text=text, command=command,
            fg_color="transparent", hover_color=GHOST_HOVER,
            border_width=1, border_color=GHOST_BORDER,
            text_color=("gray10", "gray90"),
            font=ctk.CTkFont(family=FONT_FAMILY, size=12), height=34,
        )

    # ---------------- layout ----------------
    def _build_ui(self):
        self.root.title("Random Rename — Settings")
        self.root.geometry("720x780")
        self.root.minsize(600, 500)

        try:
            icon_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "assets", "rename_quick.ico")
            if os.path.exists(icon_path):
                self.root.iconbitmap(icon_path)
        except Exception:
            pass

        # ---------- Bottom action bar: built FIRST and packed with
        # side="bottom" so Save/Cancel always keep their space and are
        # never pushed off-screen by tall scrollable content above. ----------
        action_bar = ctk.CTkFrame(self.root, corner_radius=0)
        action_bar.pack(side="bottom", fill="x")

        self.status_var = tk.StringVar(value="")
        ctk.CTkLabel(action_bar, textvariable=self.status_var, text_color=SUCCESS,
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12, weight="bold"),
                     anchor="w").pack(fill="x", padx=24, pady=(12, 0))

        btn_row = ctk.CTkFrame(action_bar, fg_color="transparent")
        btn_row.pack(fill="x", padx=24, pady=(8, 18))

        self._ghost_button(btn_row, "🔄  Shuffle again", self.randomize_all).pack(side="left")
        self._ghost_button(btn_row, "Reset to defaults", self.reset_defaults).pack(side="left", padx=(8, 0))
        self._ghost_button(btn_row, "Cancel", self.root.destroy).pack(side="right", padx=(8, 0))
        ctk.CTkButton(
            btn_row, text="💾  Save", command=self.save,
            fg_color=ACCENT, hover_color=ACCENT_HOVER,
            font=ctk.CTkFont(family=FONT_FAMILY, size=12, weight="bold"), height=34,
        ).pack(side="right")

        # ---------- Scrollable body for everything else ----------
        body = ctk.CTkScrollableFrame(self.root, fg_color="transparent")
        body.pack(side="top", fill="both", expand=True)

        # ---------- Header ----------
        ctk.CTkLabel(body, text="🎲  Random Rename",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=22, weight="bold"),
                     anchor="w").pack(fill="x", padx=24, pady=(20, 0))
        ctk.CTkLabel(body, text="Configure how the right-click \"Random Rename\" action works.",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12), text_color=TEXT_SUB,
                     anchor="w").pack(fill="x", padx=24, pady=(2, 16))

        # ---------- Live sample card ----------
        sample_card = ctk.CTkFrame(body, corner_radius=14)
        sample_card.pack(fill="x", padx=24, pady=(0, 16))
        ctk.CTkLabel(sample_card, text="EXAMPLE RESULT",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=10, weight="bold"),
                     text_color=TEXT_SUB, anchor="w").pack(fill="x", padx=18, pady=(14, 0))
        self.sample_label = ctk.CTkLabel(sample_card, text="—",
                                          font=ctk.CTkFont(family=FONT_MONO, size=20, weight="bold"),
                                          text_color=ACCENT, anchor="w")
        self.sample_label.pack(fill="x", padx=18, pady=(2, 16))

        # ---------- Section: Character type ----------
        self._section_title(body, "CHARACTER TYPE")
        type_card = ctk.CTkFrame(body, corner_radius=14)
        type_card.pack(fill="x", padx=24, pady=(0, 16))

        seg = ctk.CTkSegmentedButton(
            type_card, values=list(MODE_LABELS.values()), variable=self.mode_display_var,
            command=self._on_mode_seg_change, font=ctk.CTkFont(family=FONT_FAMILY, size=12, weight="bold"),
            height=36,
        )
        seg.pack(fill="x", padx=18, pady=(18, 4))

        self.mode_options = ctk.CTkFrame(type_card, fg_color="transparent")
        self.mode_options.pack(fill="x", padx=18, pady=(10, 18))

        self.case_frame = ctk.CTkFrame(self.mode_options, fg_color="transparent")
        ctk.CTkLabel(self.case_frame, text="Letter case:", width=90, anchor="w",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(side="left")
        ctk.CTkSegmentedButton(
            self.case_frame, values=list(CASE_LABELS.values()), variable=self.case_display_var,
            command=self._on_case_seg_change, width=280, height=30,
            font=ctk.CTkFont(family=FONT_FAMILY, size=11),
        ).pack(side="left", fill="x", expand=True)

        self.leading_zero_frame = ctk.CTkFrame(self.mode_options, fg_color="transparent")
        ctk.CTkSwitch(self.leading_zero_frame, text="Allow leading zero (e.g. 0483920)",
                       variable=self.leading_zero_var, onvalue=True, offvalue=False,
                       command=self.randomize_all, width=340,
                       font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(anchor="w")

        self.custom_frame = ctk.CTkFrame(self.mode_options, fg_color="transparent")
        ctk.CTkLabel(self.custom_frame, text="Allowed characters:", anchor="w",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(fill="x")
        custom_entry = ctk.CTkEntry(self.custom_frame, textvariable=self.custom_charset_var,
                                     font=ctk.CTkFont(family=FONT_MONO, size=13), height=32)
        custom_entry.pack(fill="x", pady=(6, 4))
        self.custom_charset_var.trace_add("write", lambda *a: self.randomize_all())
        ctk.CTkLabel(self.custom_frame, text="Example: ABCDEFXYZ0123456789", anchor="w",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=10), text_color=TEXT_SUB).pack(fill="x")

        # ---------- Section: Format ----------
        self._section_title(body, "FORMAT")
        format_card = ctk.CTkFrame(body, corner_radius=14)
        format_card.pack(fill="x", padx=24, pady=(0, 16))

        len_row = ctk.CTkFrame(format_card, fg_color="transparent")
        len_row.pack(fill="x", padx=18, pady=(18, 0))
        ctk.CTkLabel(len_row, text="Length:", width=70, anchor="w",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(side="left")
        slider = ctk.CTkSlider(len_row, from_=1, to=32, number_of_steps=31, command=self._on_length_scale)
        slider.set(self.length_var.get())
        slider.pack(side="left", fill="x", expand=True, padx=(0, 12))
        self.length_display = ctk.CTkLabel(len_row, text=str(self.length_var.get()), width=28,
                                            font=ctk.CTkFont(family=FONT_FAMILY, size=12, weight="bold"))
        self.length_display.pack(side="left")

        text_row = ctk.CTkFrame(format_card, fg_color="transparent")
        text_row.pack(fill="x", padx=18, pady=(18, 0))
        ctk.CTkLabel(text_row, text="Prefix:", width=50, anchor="w",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(side="left")
        ctk.CTkEntry(text_row, textvariable=self.prefix_var, width=140, height=32).pack(side="left", padx=(0, 20))
        ctk.CTkLabel(text_row, text="Suffix:", width=50, anchor="w",
                     font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(side="left")
        ctk.CTkEntry(text_row, textvariable=self.suffix_var, width=140, height=32).pack(side="left")
        self.prefix_var.trace_add("write", lambda *a: self.randomize_all())
        self.suffix_var.trace_add("write", lambda *a: self.randomize_all())

        ctk.CTkSwitch(format_card, text="Keep file extension", variable=self.keep_ext_var,
                       onvalue=True, offvalue=False, command=self.randomize_all, width=220,
                       font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(anchor="w", padx=18, pady=(20, 0))
        ctk.CTkSwitch(format_card, text="Use one shared result for all", variable=self.same_value_var,
                       onvalue=True, offvalue=False, command=self.randomize_all, width=280,
                       font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(anchor="w", padx=18, pady=(10, 18))

        ctk.CTkLabel(
            body,
            text="ℹ  Click \"Save\" any time — the right-click \"Random Rename\" action will use whatever "
                 "was last saved. This window stays open so you can keep adjusting.",
            font=ctk.CTkFont(family=FONT_FAMILY, size=11), text_color=TEXT_SUB,
            anchor="w", justify="left", wraplength=640,
        ).pack(fill="x", padx=24, pady=(0, 16))

        # ---------- Section: Preview ----------
        self._section_title(body, "PREVIEW (examples)")
        self.preview_card = ctk.CTkFrame(body, corner_radius=14)
        self.preview_card.pack(fill="x", padx=24, pady=(0, 24))
        self.preview_rows_frame = ctk.CTkFrame(self.preview_card, fg_color="transparent")
        self.preview_rows_frame.pack(fill="x", padx=8, pady=8)

    # ---------------- interaction ----------------
    def _on_length_scale(self, value):
        v = int(round(float(value)))
        self.length_var.set(v)
        self.length_display.configure(text=str(v))
        self.randomize_all()

    def _on_mode_seg_change(self, value):
        self.mode_var.set(LABEL_TO_MODE.get(value, DEFAULT_SETTINGS["mode"]))
        self._refresh_mode_visibility()

    def _on_case_seg_change(self, value):
        self.case_var.set(LABEL_TO_CASE.get(value, DEFAULT_SETTINGS["letter_case"]))
        self.randomize_all()

    def _refresh_mode_visibility(self):
        mode = self.mode_var.get()
        for f in (self.case_frame, self.leading_zero_frame, self.custom_frame):
            f.pack_forget()
        if mode in ("letters", "alphanumeric"):
            self.case_frame.pack(fill="x")
        elif mode == "numbers":
            self.leading_zero_frame.pack(fill="x")
        elif mode == "custom":
            self.custom_frame.pack(fill="x")
        self.randomize_all()

    def randomize_all(self):
        settings = self._current_settings()
        assign_new_names(self.items, settings, check_disk=False)
        self._refresh_preview()
        if self.items:
            self.sample_label.configure(text=self.items[0]["new_name"])

    def _refresh_preview(self):
        for child in self.preview_rows_frame.winfo_children():
            child.destroy()
        for i, item in enumerate(self.items):
            row = ctk.CTkFrame(self.preview_rows_frame, fg_color="transparent")
            row.pack(fill="x", pady=3)
            ctk.CTkLabel(row, text=item["old_base"], anchor="w", width=250,
                         font=ctk.CTkFont(family=FONT_FAMILY, size=12),
                         text_color=TEXT_SUB).pack(side="left", padx=(8, 4))
            ctk.CTkLabel(row, text="→", anchor="center", width=20,
                         font=ctk.CTkFont(family=FONT_FAMILY, size=12)).pack(side="left")
            ctk.CTkLabel(row, text=item["new_name"], anchor="w",
                         font=ctk.CTkFont(family=FONT_MONO, size=12, weight="bold"),
                         text_color=ACCENT).pack(side="left", padx=(4, 8), fill="x", expand=True)

    def reset_defaults(self):
        d = DEFAULT_SETTINGS
        self.mode_var.set(d["mode"])
        self.mode_display_var.set(MODE_LABELS[d["mode"]])
        self.length_var.set(d["length"])
        self.case_var.set(d["letter_case"])
        self.case_display_var.set(CASE_LABELS[d["letter_case"]])
        self.leading_zero_var.set(d["allow_leading_zero"])
        self.custom_charset_var.set(d["custom_charset"])
        self.prefix_var.set(d["prefix"])
        self.suffix_var.set(d["suffix"])
        self.keep_ext_var.set(d["keep_ext"])
        self.same_value_var.set(d["same_value_for_all"])
        self._refresh_mode_visibility()

    def save(self):
        settings = self._current_settings()
        ok = save_settings(settings)
        if ok:
            self.status_var.set("✓  Settings saved — \"Random Rename\" will use this from now on.")
            if self._status_after_id is not None:
                self.root.after_cancel(self._status_after_id)
            self._status_after_id = self.root.after(3000, lambda: self.status_var.set(""))
        else:
            messagebox.showerror("Random Rename", "Could not save settings. Please try again.")


def main():
    args = sys.argv[1:]

    if args and args[0] == "--quick":
        run_quick(args[1:])
        return

    # --settings, or no arguments at all (e.g. double-clicked directly)
    if not CTK_AVAILABLE:
        root = tk.Tk()
        root.withdraw()
        messagebox.showerror(
            "Random Rename",
            "The 'customtkinter' package is required for the settings window but isn't installed.\n\n"
            "Please run install.bat again - it installs this automatically."
        )
        return

    root = ctk.CTk()
    SettingsApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
