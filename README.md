Just a lightweight launcher for Bloodborne on shadPS4

___________________________________

**INSTALLATION AND USAGE: For Windows**

You can either:

A) extract BBLauncher to any folder and run it, OR

B) place the Launcher exe next the shadPS4 exe. If you are using an SDL (no GUI) build for shadPS4, the other files in the BB Launcher folder have to be also transferred to the shadPS4 folder.

Please read below regarding the advantages/disadvantages of both options:

***Starting Release7.5, BBLauncher can now be put in a separate folder from shadPS4 to prevent the DLL error "The procedure entry point ??0QString@@QEAA@VQStringView@@@Z could not be located in the dynamic link library. This occurs if shadPS4 is using a different version of QT DLLs than BBLauncher

PLEASE NOTE HOWEVER: shadPS4 will always use the user folder from where it was launched, there is nothing that can be done on my end to change that. Whenever you launch shadPS4 with BBLauncher, it will look for the user folder in the BBLauncher folder, but if you run shadPS4 directly, it will look for the user folder in the shadPS4 folder. You can still put the two exes in the same folder like before to share the user folder, and to prevent the DLL error you can do the following:

***If you have BBLauncher and shadPS4 in the same folder and are getting "The procedure entry point ??0QString@@QEAA@VQStringView@@@Z could not be located in the dynamic link library", download the latest nightly version of shadPS4 https://nightly.link/shadps4-emu/shadPS4/workflows/build/main and extract, move the exe and all of the updated DLLs and folders to the installation folder. The latest builds of shad/BBLauncher should always be have the same DLLs as long I keep up the updates so this would avoid the error

_____________________________________

**INSTALLATION AND USAGE: For Linux and Mac**

You will be prompted to select the shadps4 binary on startup. If the binary does not open, from terminal change path to where BBLauncher is and run the ff command: chmod +x BB_Launcher-qt.AppImage

_____________________________________

**SKIPPING GUI:**

GUI can be skipped after setting up the install folder using command line argument -n (or --args -n on Mac)

____________________________________

**MOD INSTALLATION:**

Place mods inside the BBLauncher/Mods folder. Click on the mod folder icon for easy access to this folder

It is highly recommended to install Bloodborne using the separate update folder option to make the most of the optimizations in BBLauncher's Mod Manager. Also, in the event a reinstall is needed, you would just have to reinstall the update instead of the entire game.

***Inside the Mods folder should be a folder with the name of the mod, and that folder should only be valid Bloodborne folders such as dvdroot_ps4, or one of the Bloodborne subfolders such as sfx, parts, map, etc.

____________________________________

**FEATURES:**

- Fixes the sound glitch when you crash at 60 fps
- Creates backup saves after a configurable interval
- Mod manager (similar to the Generic Mod Manager on nexus, but automatically structures folders and adds error checks)
- Update shadPS4, remap controls, and change its settings from the launcher
- Download and enable Bloodborne patches from the launcher
- View save info and restore backup saves with the GUI
- Trophy viewing/unlocking
