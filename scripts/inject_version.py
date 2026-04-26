"""
inject_version.py — PlatformIO pre-build script
Adds three macros to every build so the firmware can print exactly
what it is:
    FIRMWARE_GIT_REV   — short git commit hash (e.g. "fbb2d98")
                         or "unknown" if .git is absent
    FIRMWARE_BUILD_DATE — compile date "YYYY-MM-DD"
    FIRMWARE_BUILD_TIME — compile time "HH:MM"
    FIRMWARE_DIRTY      — 1 if uncommitted changes present, 0 if clean
"""
Import("env")  # noqa: F821 — PlatformIO injects this

import subprocess
import datetime


def get_git_rev():
    try:
        rev = subprocess.check_output(
            ["git", "rev-parse", "--short=7", "HEAD"],
            stderr=subprocess.DEVNULL
        ).decode().strip()
        dirty = subprocess.call(
            ["git", "diff", "--quiet", "HEAD"],
            stderr=subprocess.DEVNULL
        )  # returns 0 if clean, 1 if dirty
        return rev, dirty != 0
    except Exception:
        return "unknown", False


rev, dirty = get_git_rev()
now = datetime.datetime.now()
date_str = now.strftime("%Y-%m-%d")
time_str = now.strftime("%H:%M")

env.Append(CPPDEFINES=[  # noqa: F821
    ("FIRMWARE_GIT_REV",    '\\"' + rev + '\\"'),
    ("FIRMWARE_BUILD_DATE", '\\"' + date_str + '\\"'),
    ("FIRMWARE_BUILD_TIME", '\\"' + time_str + '\\"'),
    ("FIRMWARE_DIRTY",      1 if dirty else 0),
])

print(f"[version] {rev}{'*' if dirty else ''} built {date_str} {time_str}")
