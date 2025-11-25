Just a dedicated launcher for Bloodborne on shadPS4

___________________________________

COMMON ISSUES:

If you are getting this error message ("The procedure entry point..." errors when launching), redownload both shadPS4 and BBLauncher. Since BBLauncher no longer needs DLLs, this should no longer be an issue starting from BBLauncher version 12.00

FOR LINUX: As of version 10.02, the BBLauncher folder with the extra files (previously was next to the AppImage), is now moved to the usr/share/BBLauncher folder. You can just transfer your old files to that folder to carry over mods/settings

___________________________________

INSTALLATION AND USAGE:

Extract BBLauncher to any folder and run it, download shadPS4 through the build manager by clicking the "Manage builds" button. Set the shadPS4 install folder (named CUSA03173 or whatever the serial number of your game is).

_____________________________________

SKIPPING GUI:

GUI can be skipped after setting up the install folder using command line argument -n (or --args -n on Mac)

____________________________________

MOD INSTALLATION:

Place mods inside the BBLauncher/Mods folder. Click on the mod folder icon for easy access to this folder

It is highly recommended to install Bloodborne using the separate update folder option to make the most of the optimizations in BBLauncher's Mod Manager. Also, in the event a reinstall is needed, you would just have to reinstall the update instead of the entire game.

***Inside the Mods folder should be a folder with the name of the mod, and that folder should only be valid Bloodborne folders such as dvdroot_ps4, or one of the Bloodborne subfolders such as sfx, parts, map, etc.

____________________________________


Complete version history: https://github.com/rainmakerv3/BB_Launcher/releases

____________________________________

FEATURES:

Fixes the sound glitch when you crash at 60 fps
Creates backup saves after a configurable interval
Mod manager (similar to the Generic Mod Manager on nexus, but automatically structures folders and adds error checks)
Download mods directly from the app and make them available for use by the mod manager
Update shadPS4, remap controls, and change its settings from the launcher
Download and enable Bloodborne patches from the launcher
View save info and restore backup saves with the GUI
Trophy viewing/unlocking

Fully open source! Check out the Github page if you wanna build it yourself, contribute or just look at the source: https://github.com/rainmakerv3/BB_Launcher

Thanks to FMOD stream thread and Az for their code contributions! Thanks also to Missake and Brad for helping to test, and to Noxde and Kyoski for a lot of help figuring out how to view save files!
