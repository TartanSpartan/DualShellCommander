DualShellCommander
================================================================================

DualShellCommander is a fork of VitaShell (as of VS version 1.9?). VitaShell is an alternative replacement of the PS Vita's LiveArea. It offers you a file manager, package installer, built-in
FTP and much more. This homebrew was an entry of the Revitalize PS Vita homebrew competition and won the first prize. HENkaku's molecularShell is also based on VitaShell. DualShellCommander
is inspired by the cross-platform dual pane file manager, DoubleCommander, and it's main purpose is to depict two directories in a dual pane format, with which the user can perform file
operations between  these directories. Other file browsing and UI enhancements are incorporated.

// NOTE: NEED TO CHANGE THIS MASSIVELY TO REFLECT MY OWN WORK!!!

Donation
--------
All my work is voluntary and nonprofit, however you can support my work by making a small donation - no matter how small, I'd be very thankful!
[Donation Link](https://www.paypal.me/PSVitaTartanSpartan)

How to use an USB flash drive as Memory Card on a PS TV
-------------------------------------------------------
- Format your USB flash drive as exFAT or FAT32.
- Launch DualShellCommander and press `/\` in the `home` section.
- Select `Mount uma0:` and attach your USB flash drive. You can now copy stuff from/to your USB stick.
- Once `uma0:` is listed under the partitions, press `/\` again and choose `Mount USB ux0:`. This will copy important apps like DualShellCommander, molecularShell, and other files.
- Your USB flash drive is now acting as a Memory Card.
- To sync all your apps on your USB flash drive, press `/\` and choose `Refresh livearea`. This will NOT refresh PSP games.
- If you wish to revert the patch, press `/\` and select `Umount USB ux0:`.
- Note that this patch is only temporary and you need to redo the procedure everytime you launch your PS TV.

Customization
-------------
You can customize those files:

| File                   | Note                        |
| ---------------------- | --------------------------- |
| colors.txt             | All colors adjustable       |
| archive_icon.png       | Archive icon                |
| audio_icon.png         | Audio icon                  |
| battery.png            | Battery border icon         |
| battery_bar_charge.png | Charging battery bar        |
| battery_bar_green.png  | Green battery bar           |
| battery_bar_red.png    | Red battery bar             |
| bg_audioplayer.png     | Background for audio player |
| bg_browser.png         | Background for file browser |
| bg_hexeditor.png       | Background for hex editor   |
| bg_photoviewer.png     | Background for photo viewer |
| bg_texteditor.png      | Background for text editor  |
| context.png            | Context menu image (Can be any size. Suggestion: It will look great if you add alpha channel to your image)  |
| context_more.png       | Context menu more image (Can be any size. Suggestion: It will look great if you add alpha channel to your image)  |
| cover.png              | Default album cover         |
| dialog.png             | Dialog menu image (Can be any size. This image file will be stretched by DualShellCommander to fit the dialog box. Suggestion: Don't use motives, as it will not look good with wrong proportion)  |
| fastforward.png        | Fastforward icon            |
| fastrewind.png         | Fastrewind icon             |
| file_icon.png          | File icon                   |
| folder_icon.png        | Folder icon                 |
| ftp.png                | FTP icon                    |
| image_icon.png         | Image icon                  |
| pause.png              | Pause icon                  |
| play.png               | Play icon                   |
| settings.png           | Settings icon               |
| sfo_icon.png           | SFO icon                    |
| text_icon.png          | Text icon                   |
| wallpaper.png          | Wallpaper                   |

**Theme setting:** DualShellCommander will load the theme that is set in `ux0:DualShellCommander/theme/theme.txt` (`THEME_NAME = "YOUR_THEME_NAME"`)

**General info:** You don't need to have all these files in your custom theme, if one of them is missing, the default image file will be loaded instead.

**Dialog and context image:** If these files are not available, the colors `DIALOG_BG_COLOR` and `CONTEXT_MENU_COLOR` from `colors.txt` will be used instead.

Multi-language
--------------
Put your language file at `ux0:DualShellCommander/language/x.txt`, where the file must be UTF-8 encoded and `x` is one of the language listed below:

- japanese
- english_us
- french
- spanish
- german
- italian
- dutch
- portuguese
- russian
- korean
- chinese_t
- chinese_s
- finnish
- swedish
- danish
- norwegian
- polish
- portuguese_br
- turkish

DualShellCommander does automatically load the language that matches to the current system language.
If your system language is for example french, it will load from `ux0:DualShellCommander/language/french.txt`.

Languages files are available in the `l10n` folder of this repository.

For developers
--------------
1. Install [vitasdk](https://github.com/vitasdk)
2. Clone this repository and compile the kernel module first at `DualShellCommander/modules/kernel` then the user module at `DualShellCommander/modules/user`. Both times compile it using `mkdir build && cd build && cmake .. && make install`
3. After these modules have been successfully compiled, return to the main directory and compile with `mkdir build && cd build && cmake .. && make`

Credits
-------
* TheOfficialFloW for his amazing work on the original VitaShell
* jonnius for helping with cursor indexing errors
* Team Molecule for HENkaku
* xerpi for ftpvitalib and vita2dlib
* wololo for the Revitalize contest
* sakya for Lightmp3
* Everybody who contributed on vitasdk

Changelog
---------

### Changelog 0.30
- Updated codebase to match VitaShell's changes up to version 1.91
- Implemented trial "double print" of file directory to experiment with two different actual directories

### Changelog 0.20
- Shifted QR code scanning functionality from L & R to Select (with settings option reflecting this) for all hardware except PSTV.
- Enabled "page skip" index jumping of cursor with L (up the directory) & R (down it).
- Printed battery percentage on Vita (not on PSTV) much as how it is shown in system Home Screen.

### Changelog 0.10
- Initial release.
- Amended, in all combinations of lower and upper-case, references to "VitaShell" to "DualShellCommander"
- Amended author to TartanSpartan, version number, URLs
- Redesigned /pkg graphics with "Retrowave" colors: icon0.png, bg.png and startup.png.

### For TheOfficialFloW's changelog of VitaShell from version 0.2 to 1.91, please go to https://github.com/TheOfficialFloW/VitaShell/blob/master/README.md
