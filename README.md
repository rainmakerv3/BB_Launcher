# BBLauncher

A dedicated shadPS4 launcher focused entirely on *Bloodborne*, designed to simplify setup, modding, updates, and quality-of-life features while staying fully open source.

---

## Installation & Usage

### Windows

1. Install **Visual C++ Redistributable (x64)**:
   [https://aka.ms/vs/17/release/vc_redist.x64.exe](https://aka.ms/vs/17/release/vc_redist.x64.exe)
   *(Required for shadPS4 as well.)*
2. Extract **BBLauncher** to any folder.
3. Run BBLauncher.
4. Click **Manage Builds** and download shadPS4.
5. Set the shadPS4 install folder (for example: `CUSA03173`, depending on your game serial).

---

## Skipping the GUI

Once the install folder is configured, the GUI can be skipped by launching BBLauncher with the following argument:

```bash
-n
```

On macOS, use:

```bash
--args -n
```

---

## Mod Installation

1. Place mods inside the following directory:

```
BBLauncher/Mods
```

2. Use the **mod folder icon** in the launcher for quick access.

### Recommended Setup

It is **highly recommended** to install Bloodborne using a **separate update folder**. This allows BBLauncher’s Mod Manager to apply optimizations more effectively. If a reinstall is needed, only the update needs to be reinstalled, not the entire game.

### Mod Folder Structure

Inside the `Mods` folder:

* Each mod must be inside its **own folder**.
* That folder should contain **only valid Bloodborne directories**, such as:
  * `dvdroot_ps4`
  * or individual subfolders like `sfx`, `parts`, `map`, etc.

---

## Features

* Fixes the **60 FPS crash sound glitch**
* Automatic **save backups** at configurable intervals
* Built-in **Mod Manager**
  * Inspired by Generic Mod Manager (Nexus)
  * Automatically structures folders
  * Includes error checking
* Download mods **directly from the launcher**
* Update **shadPS4** from within the app
* Remap controls and change shadPS4 settings
* Download and enable **Bloodborne patches**
* View save information and restore backups via GUI
* **Trophy viewing and unlocking**

---

## Common Issues

**"The procedure entry point…" error on launch (Windows):**
If you encounter this error, re-download **both shadPS4 and BBLauncher**. Starting from **BBLauncher v12.00**, external DLLs are no longer required, so this issue should no longer occur.

**Linux file location change:**
As of **v10.02**, the BBLauncher data folder (previously located next to the AppImage) has been moved to:

```
/usr/share/BBLauncher
```

To keep your existing mods and settings, simply copy your old files into this directory.

---

## Version History

Full release history is available here:

[https://github.com/rainmakerv3/BB_Launcher/releases](https://github.com/rainmakerv3/BB_Launcher/releases)

---

## Building

### Windows (VSCode)

[https://github.com/rainmakerv3/BB_Launcher/documents/building-windows.md](https://github.com/rainmakerv3/BB_Launcher/documents/building-windows.md)

### Docker:

[https://github.com/rainmakerv3/BB_Launcher/documents/building-docker.md](https://github.com/rainmakerv3/BB_Launcher/documents/building-docker.md)

---

## Open Source

BBLauncher is **fully open source**. You are welcome to build it yourself, contribute, or explore the source code:

[https://github.com/rainmakerv3/BB_Launcher](https://github.com/rainmakerv3/BB_Launcher)

---

## Credits

* **FMOD stream thread**,  **LucasPicoli**, and **Az** for code contributions
* **Missake** and **Brad** for testing
* **Noxde** and **Kyoski** for extensive help with save file research
* [**Par274**](https://github.com/par274) for documentation and code contributions
