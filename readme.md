# NeoBES <img src="res/neobes.ico" align="right" width="120" height="120" >

**The Definitive PaRappa the Rapper 2 Button Editing Software.**

NeoBES is a native tool for loading, creating, and modifying stage mods for *PaRappa the Rapper 2* (PS2) via PCSX2 and PINE. It features an overhauled interface, improved timing logic, and legacy file repair.

---

## 1. Compatibility

### System Requirements
*   **OS:** Windows or Linux, or really any other plataform that can compile QT.
*   **Emulator:** PCSX2 (Recent build).
    *   *Note:* Linux Flatpak versions are **not supported** due to PINE sandbox restrictions. Use AppImage or native packages.
*   **PINE:** Must be enabled in PCSX2 Advanced Settings.

### Supported Game Versions
*   NTSC-U (North America)
*   NTSC-J (Japan)
*   PAL (Europe)

### Legacy File Support
NeoBES supports files from `ptr2besms` and `neobesms`.
*   **Auto-Fix:** Corrupted/missing scene jobs in legacy files are repaired during upload.
*   **Warning:** Do **not** modify scene jobs or reorder records on legacy files before the first successful sync/save, or data corruption may occur.

---

## 2. Usage

1.  **Launch PCSX2**: Ensure **PINE** is enabled (`Settings > Advanced`).
2.  **Start Game**: Load any region of *PaRappa the Rapper 2* and enter the desired stage.
3.  **Pause**: Pause the game in the **"Try Again"** screen.
4.  **Open NeoBES**:
    *   **New**: Download current stage data from PCSX2.
    *   **Load**: Open an existing mod file.
5.  **Edit**: Modify lines and buttons.
6.  **Sync**: Select **Upload** in NeoBES.
7.  **Play**: Unpause the game to test changes.

---

## 3. Controls

| Context | Action | Input |
| :--- | :--- | :--- |
| **Navigation** | Timeline Scroll | `W` `A` `S` `D` |
| | Precision Scroll | `Q` / `E` |
| **Editing** | Place Input (1-6) | `1` - `6` |
| | Delete Input | `0` |
| **Lines** | Create Line | `Space` |
| | Delete Line | `Backspace` |
| | Resize Line | `,` / `.` |
| | Move Line | `N` / `M` |
| **Clipboard** | Cut / Copy / Paste | `X` / `C` / `V` |

## 4. Documentation

For technical details regarding file structures, and internal game logic, refer to the documentation located in the `docs/` directory.
