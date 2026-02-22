/*
  DualShellCommander
  Copyright (C) 2018, TartanSpartan

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "io_process.h"
#include "network_update.h"
#include "network_download.h"
#include "package_installer.h"
#include "archive.h"
#include "file.h"
#include "message_dialog.h"
#include "language.h"
#include "utils.h"

// NOTE: NEED TO TWEAK THIS, AND MAKE A GITHUB REPO TO UPDATE THE PROGRAM! DECIDE WHEN TO DO THIS SOON; THINGS MAY BREAK EVEN WITHOUT DOING THIS!
#define BASE_ADDRESS "https://raw.githubusercontent.com/TheOfficialFloW/DualShellCommander/master/releases/download"
#define VERSION_URL "/0.2/version.bin"
#define DUALSHELLCOMMANDER_UPDATE_FILE "ux0:DualShellCommander/internal/DualShellCommander.vpk"
#define DUALSHELLCOMMANDER_VERSION_FILE "ux0:DualShellCommander/internal/version.bin"

extern unsigned char _binary_resources_updater_eboot_bin_start;
extern unsigned char _binary_resources_updater_eboot_bin_size;
extern unsigned char _binary_resources_updater_param_bin_start;
extern unsigned char _binary_resources_updater_param_bin_size;

int network_update_thread(SceSize args, void *argp) {
  uint64_t size = 0;
  if (getDownloadFileSize(BASE_ADDRESS VERSION_URL, &size) >= 0 && size == sizeof(uint32_t)) {
    int res = downloadFile(BASE_ADDRESS VERSION_URL, DUALSHELLCOMMANDER_VERSION_FILE, NULL);
    if (res <= 0)
      goto EXIT;

    // Read version
    uint32_t version = 0;
    ReadFile(DUALSHELLCOMMANDER_VERSION_FILE, &version, sizeof(uint32_t));
    sceIoRemove(DUALSHELLCOMMANDER_VERSION_FILE);

    // Only show update question if no dialog is running
    if (getDialogStep() == DIALOG_STEP_NONE) {
      // New update available
      if (version > DUALSHELLCOMMANDER_VERSION) {
        int major = (version >> 0x18) & 0xFF;
        int minor = (version >> 0x10) & 0xFF;

        char version_string[8];
        sprintf(version_string, "%X.%02X", major, minor);
        if (version_string[3] == '0')
          version_string[3] = '\0';

        // Update question
        initMessageDialog(SCE_MSG_DIALOG_BUTTON_TYPE_YESNO, language_container[UPDATE_QUESTION], version_string);
        setDialogStep(DIALOG_STEP_UPDATE_QUESTION);

        // Wait for response
        while (getDialogStep() == DIALOG_STEP_UPDATE_QUESTION) {
          sceKernelDelayThread(10 * 1000);
        }

        // No
        if (getDialogStep() == DIALOG_STEP_NONE) {
          goto EXIT;
        }

        // Yes
        return downloadFileProcess(BASE_ADDRESS "/DualShellCommander.vpk", DUALSHELLCOMMANDER_UPDATE_FILE, DIALOG_STEP_DOWNLOADED);
      }
    }
  }

EXIT:
  return sceKernelExitDeleteThread(0);
}

void installUpdater() {
  // Recursively clean up pkg directory
  removePath(PACKAGE_DIR, NULL);
  sceIoMkdir(PACKAGE_DIR, 0777);

  // Make dir
  sceIoMkdir("ux0:data/pkg/sce_sys", 0777);

  // Write DualShellCommander updater files
  WriteFile("ux0:data/pkg/eboot.bin", (void *)&_binary_resources_updater_eboot_bin_start, (int)&_binary_resources_updater_eboot_bin_size);
  WriteFile("ux0:data/pkg/sce_sys/param.sfo", (void *)&_binary_resources_updater_param_bin_start, (int)&_binary_resources_updater_param_bin_size);

  // Make head.bin
  makeHeadBin();

  // Promote app
  promoteApp(PACKAGE_DIR);
}

int update_extract_thread(SceSize args, void *argp) {
  SceUID thid = -1;

  // Lock power timers
  powerLock();

  // Set progress to 0%
  sceMsgDialogProgressBarSetValue(SCE_MSG_DIALOG_PROGRESSBAR_TARGET_BAR_DEFAULT, 0);
  sceKernelDelayThread(DIALOG_WAIT); // Needed to see the percentage

  // Install updater
  installUpdater();

  // Recursively clean up pkg directory
  removePath(PACKAGE_DIR, NULL);
  sceIoMkdir(PACKAGE_DIR, 0777);

  // Open archive
  archiveClearPassword();
  int res = archiveOpen(DUALSHELLCOMMANDER_UPDATE_FILE);
  if (res < 0) {
    closeWaitDialog();
    errorDialog(res);
    goto EXIT;
  }

  // Src path
  char *src_path = DUALSHELLCOMMANDER_UPDATE_FILE "/";

  // Get archive path info
  uint64_t size = 0;
  uint32_t folders = 0, files = 0;
  getArchivePathInfo(src_path, &size, &folders, &files, NULL);

  // Update thread
  thid = createStartUpdateThread(size + folders * DIRECTORY_SIZE, 1);

  // Extract process
  uint64_t value = 0;

  FileProcessParam param;
  param.value = &value;
  param.max = size + folders * DIRECTORY_SIZE;
  param.SetProgress = SetProgress;
  param.cancelHandler = cancelHandler;

  res = extractArchivePath(src_path, PACKAGE_DIR "/", &param);
  if (res <= 0) {
    closeWaitDialog();
    setDialogStep(DIALOG_STEP_CANCELED);
    errorDialog(res);
    goto EXIT;
  }

  // Remove update file
  sceIoRemove(DUALSHELLCOMMANDER_UPDATE_FILE);

  // Make head.bin
  res = makeHeadBin();
  if (res < 0) {
    closeWaitDialog();
    errorDialog(res);
    goto EXIT;
  }

  // Set progress to 100%
  sceMsgDialogProgressBarSetValue(SCE_MSG_DIALOG_PROGRESSBAR_TARGET_BAR_DEFAULT, 100);
  sceKernelDelayThread(COUNTUP_WAIT);

  // Close
  sceMsgDialogClose();

  setDialogStep(DIALOG_STEP_EXTRACTED);

EXIT:
  if (thid >= 0)
    sceKernelWaitThreadEnd(thid, NULL, NULL);

  // Unlock power timers
  powerUnlock();

  return sceKernelExitDeleteThread(0);
}