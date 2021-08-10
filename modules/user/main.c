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

#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/io/fcntl.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <dualshellcommander_kernel.h>

int shellUserIsUx0Redirected() {
  return shellKernelIsUx0Redirected();
}

int shellUserRedirectUx0() {
  return shellKernelRedirectUx0();
}

int shellUserUnredirectUx0() {
  return shellKernelUnredirectUx0();
}

int shellUserMountById(ShellMountIdArgs *args) {
  return shellKernelMountById(args);  
}

int shellUserGetRifVitaKey(const void *license_buf, void *klicensee){
  return shellKernelGetRifVitaKey(license_buf, klicensee);  
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) {
  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp) {
  return SCE_KERNEL_STOP_SUCCESS;
}