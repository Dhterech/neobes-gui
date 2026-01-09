# NeoBES
### The Definitive PaRappa the Rapper 2 Button Editing Software

NeoBES is a powerful tool designed for loading, creating, and modifying stage mods for PaRappa the Rapper 2 (PlayStation 2). Built upon ptr2besms and neobesms, NeoBES features a completely overhauled interface and more robust editing tools.

## Key Features

 - Native builds for Windows and Linux.
 - Works on the new PCSX2 build using PINE.
 - Improved logic for previewing and timing button placements.
 - Automatically fixes corrupted or missing scene job data from older tools.

## Compatibility & Requirements
### Emulator
- PCSX2: Requires a recent version with PINE support enabled on Advanced Settings.
- Note on Linux: The Flatpak version of PCSX2 is currently not supported as it sandboxes the PINE interface, preventing NeoBES from communicating with the emulator. Use the AppImage or native package to your distro instead.

### Game Versions
- NTSC-U (North America)
- NTSC-J (Japan)
- PAL (Europe)

### Save Files & Older Tools Support

NeoBES is compatible with mods created in ptr2besms or neobesms. However, those tools produced files with missing or unordered "scene job" data, which could lead to crashes.

#### Important Usage Tips for Legacy Files:
- Auto-Fixing: NeoBES repairs these files during the upload process by recreating the scene jobs and loading missing records.
- Do NOT modify scene jobs before your first successful upload/sync. Once synced and saved via NeoBES, the necessary data will be added.
- That old file type doesn't support reordering records, so don't save the project if you changed it to avoid corrupting your mod.
- A new, more robust file format is currently in development.

## How to Use
- Launch PCSX2 and ensure PINE is enabled in Advanced Settings.
- Start the game and navigate to a stage.
- Pause the game at the "Try Again" screen.
- Open NeoBES.
- Either create a new mod by downloading the current data from PCSX2 or load an existing file.
- Modify the buttons/lines and save your changes as desired.
- Select the option in the NeoBES menu to upload the mod back to PCSX2.
- Unpause the game and enjoy your mod!

## Keybindings 

NeoBES utilizes a mix of keyboard and mouse keybindings, most of the keyboard's actions are similar to older besms tools.

| Action | Key(s) |
| :--- | :--- |
| **Navigate Timeline** | `W` `A` `S` `D` |
| **Precision Navigation** | `Q` / `E` |
| **Place Button (1-6)** | `1`, `2`, `3`, `4`, `5`, `6` |
| **Delete Button** | `0` |
| **Create Line** | `Space` |
| **Delete Line** | `Backspace` |
| **Resize/Extend Line** | `,` / `.` |
| **Move Line** | `N` / `M` |
| **Clipboard** | `X` (Cut) / `C` (Copy) / `V` (Paste) |


