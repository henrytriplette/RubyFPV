/*
    Ruby Licence
    Copyright (c) 2024 Petru Soroaga petrusoroaga@yahoo.com
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Copyright info and developer info must be preserved as is in the user
        interface, additions could be made to that info.
        * Neither the name of the organization nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
        * Military use is not permited.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Julien Verneuil BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "../base/base.h"
#include "../base/config.h"
#include "../base/hardware.h"
#include "../base/hw_procs.h"
#include "../common/string_utils.h"
#include "media.h"
#include "../renderer/render_engine.h"
#include "popup.h"
#include "ruby_central.h"
#include "shared_vars.h"
#include "timers.h"

#include <sys/types.h>
#include <dirent.h>
#include <string.h>


static int s_iScreenshotsCountOnDisk = 0;
static int s_iVideoCountOnDisk = 0;

static int s_iMediaBootCount = 0;
static int s_iMediaInitCount = 0;
static char s_szMediaCurrentScreenshotFileName[1024];
static char s_szMediaCurrentVideoFileInfo[1024];

void _media_remove_invalid_files()
{
   DIR *d;
   FILE* fd;
   struct dirent *dir;
   char szFile[1024];
   char szComm[1024];
   log_line("Searching and removing invalid media files...");
   d = opendir(FOLDER_MEDIA);
   if (d)
   {
      while ((dir = readdir(d)) != NULL)
      {
         if ( strlen(dir->d_name) < 4 )
            continue;

         sprintf(szFile, "%s%s", FOLDER_MEDIA, dir->d_name);
         long lSize = 0;
         fd = fopen(szFile, "rb");
         if ( NULL != fd )
         {
            fseek(fd, 0, SEEK_END);
            lSize = ftell(fd);
            fseek(fd, 0, SEEK_SET);
            fclose(fd);
         }
         else
            log_softerror_and_alarm("Failed to open file [%s] for checking it's size.", szFile);
         if ( lSize > 3 )
            continue;
         log_line("Removing invalid media file: [%s]", dir->d_name);

         // Remove invalid file and info file for it

         strcpy(szFile, dir->d_name);
         int pos = strlen(szFile);
         while (pos > 0 && szFile[pos] != '.')
            pos--;
         pos++;

         szFile[pos] = 0;
         strcat(szFile, "h264");
         snprintf(szComm, sizeof(szComm)/sizeof(szComm[0]), "rm -rf %s%s", FOLDER_MEDIA, szFile);
         hw_execute_bash_command(szComm, NULL);

         szFile[pos] = 0;
         strcat(szFile, "info");
         snprintf(szComm, sizeof(szComm)/sizeof(szComm[0]), "rm -rf %s%s", FOLDER_MEDIA, szFile);
         hw_execute_bash_command(szComm, NULL);

         szFile[pos] = 0;
         strcat(szFile, "png");
         snprintf(szComm, sizeof(szComm)/sizeof(szComm[0]), "rm -rf %s%s", FOLDER_MEDIA, szFile);
         hw_execute_bash_command(szComm, NULL);

         ruby_signal_alive();
      }
      closedir(d);
   }
   else
      log_softerror_and_alarm("Failed to open media dir to search for invalid videos.");
   log_line("Searching and removing invalid media files complete.");
}


bool media_init_and_scan()
{
   log_line("Media Storage: Init media storage...");
   ruby_pause_watchdog();

   s_iMediaBootCount = 0;
   s_szMediaCurrentScreenshotFileName[0] = 0;
   s_szMediaCurrentVideoFileInfo[0] = 0;
   s_iMediaInitCount++;

   char szFile[128];
   strcpy(szFile, FOLDER_CONFIG);
   strcat(szFile, FILE_CONFIG_BOOT_COUNT);
   FILE* fd = fopen(szFile, "r");
   if ( NULL != fd )
   {
      if ( 1 != fscanf(fd, "%d", &s_iMediaBootCount) )
         s_iMediaBootCount = 0;
      fclose(fd);
   }

   media_scan_files();

   ruby_resume_watchdog();
   log_line("Media Storage: Init media storage completed.");

   return true;
}

void media_scan_files()
{
   char szOutBuff[1024];

   _media_remove_invalid_files();
 

   // Count files in a folder:
   // ls media/ | grep picture- | wc -l
   // ls media/ | grep '.info' | wc -l

   s_iScreenshotsCountOnDisk = 0;
   szOutBuff[0] = 0;
   hw_execute_bash_command("ls media/ | grep picture- | wc -l", szOutBuff);
   if ( 0 < strlen(szOutBuff) )
      s_iScreenshotsCountOnDisk = strtol(szOutBuff, NULL, 10);

   s_iVideoCountOnDisk = 0;
   szOutBuff[0] = 0;
   hw_execute_bash_command("ls media/ | grep '.info' | wc -l", szOutBuff);
   if ( 0 < strlen(szOutBuff) )
      s_iVideoCountOnDisk = strtol(szOutBuff, NULL, 10);

   ruby_signal_alive();

   log_line("Media storage: Found %d screenshots on storage.", s_iScreenshotsCountOnDisk );
   log_line("Media storage: Found %d videos on storage.", s_iVideoCountOnDisk );
}

int media_get_screenshots_count()
{
   return s_iScreenshotsCountOnDisk;
}

int media_get_videos_count()
{
   return s_iVideoCountOnDisk;
}


char* media_get_screenshot_filename()
{
   char vehicle_name[MAX_VEHICLE_NAME_LENGTH+1];

   strcpy(vehicle_name, "none");
   if ( NULL != g_pCurrentModel )
      strcpy(vehicle_name, g_pCurrentModel->vehicle_name);
   if ( 0 == strlen(vehicle_name) || (1 == strlen(vehicle_name) && vehicle_name[0] == ' ') )
      strcpy(vehicle_name, "none");
    
   str_sanitize_filename(vehicle_name);

   sprintf(s_szMediaCurrentScreenshotFileName, FILE_FORMAT_SCREENSHOT, vehicle_name, s_iMediaBootCount, g_TimeNow/1000, g_TimeNow%1000 );
   return s_szMediaCurrentScreenshotFileName;
}

char* media_get_video_filename()
{
   char vehicle_name[MAX_VEHICLE_NAME_LENGTH+1];

   strcpy(vehicle_name, "none");
   if ( NULL != g_pCurrentModel )
      strcpy(vehicle_name, g_pCurrentModel->vehicle_name);
   if ( 0 == strlen(vehicle_name) || (1 == strlen(vehicle_name) && vehicle_name[0] == ' ') )
      strcpy(vehicle_name, "none");

   str_sanitize_filename(vehicle_name);

   sprintf(s_szMediaCurrentVideoFileInfo, FILE_FORMAT_VIDEO_INFO, vehicle_name, s_iMediaBootCount, g_TimeNow/1000, g_TimeNow%1000 );
   return s_szMediaCurrentVideoFileInfo;
}

bool media_take_screenshot(bool bIncludeOSD)
{
   if ( ! bIncludeOSD )
   {
      g_pRenderEngine->startFrame();
      g_pRenderEngine->endFrame();
      hardware_sleep_ms(20);
   }

   char szFile[1024];

   media_get_screenshot_filename();
   strcpy(szFile, FOLDER_MEDIA);
   strcat(szFile, s_szMediaCurrentScreenshotFileName);

   ruby_signal_alive();
   hw_launch_process2("./raspi2png", "-p", szFile);
   ruby_signal_alive();

   log_line("Media Storage: Took a screenshot to file: %s", szFile);
   s_iScreenshotsCountOnDisk++;

   Popup* p = new Popup("Screenshot taken", 0.1,0.72, 2);
   popups_add_topmost(p);
   return true;
}
