/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * X11 Monitor Handling
 *
 * Copyright 2011 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <winpr/crt.h>

#ifdef WITH_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#include "xf_monitor.h"

/* See MSDN Section on Multiple Display Monitors: http://msdn.microsoft.com/en-us/library/dd145071 */

BOOL xf_detect_monitors(xfInfo* xfi, rdpSettings* settings)
{
	int i;
	int nmonitors;
	int primaryMonitor;
	int maxWidth, maxHeight;
	VIRTUAL_SCREEN* vscreen;

#ifdef WITH_XINERAMA
	int ignored, ignored2;
	XineramaScreenInfo* screen_info = NULL;
#endif

	vscreen = &xfi->vscreen;

#ifdef WITH_XINERAMA
	if (XineramaQueryExtension(xfi->display, &ignored, &ignored2))
	{
		if (XineramaIsActive(xfi->display))
		{
			screen_info = XineramaQueryScreens(xfi->display, &vscreen->nmonitors);

			if (vscreen->nmonitors > 16)
				vscreen->nmonitors = 0;

			vscreen->monitors = malloc(sizeof(MONITOR_INFO) * vscreen->nmonitors);
			ZeroMemory(vscreen->monitors, sizeof(MONITOR_INFO) * vscreen->nmonitors);

			if (vscreen->nmonitors)
			{
				for (i = 0; i < vscreen->nmonitors; i++)
				{
					vscreen->monitors[i].area.left = screen_info[i].x_org;
					vscreen->monitors[i].area.top = screen_info[i].y_org;
					vscreen->monitors[i].area.right = screen_info[i].x_org + screen_info[i].width - 1;
					vscreen->monitors[i].area.bottom = screen_info[i].y_org + screen_info[i].height - 1;

					if ((screen_info[i].x_org == 0) && (screen_info[i].y_org == 0))
						vscreen->monitors[i].primary = TRUE;
				}
			}

			XFree(screen_info);
		}
	}
#endif

	if (!xf_GetWorkArea(xfi))
	{
		xfi->workArea.x = 0;
		xfi->workArea.y = 0;
		xfi->workArea.width = WidthOfScreen(xfi->screen);
		xfi->workArea.height = HeightOfScreen(xfi->screen);
	}

	if (settings->Fullscreen)
	{
		settings->DesktopWidth = WidthOfScreen(xfi->screen);
		settings->DesktopHeight = HeightOfScreen(xfi->screen);
		maxWidth = settings->DesktopWidth;
		maxHeight = settings->DesktopHeight;
	}
	else if (settings->Workarea)
	{
		settings->DesktopWidth = xfi->workArea.width;
		settings->DesktopHeight = xfi->workArea.height;
		maxWidth = settings->DesktopWidth;
		maxHeight = settings->DesktopHeight;
	}
	else if (settings->PercentScreen)
	{
		settings->DesktopWidth = (xfi->workArea.width * settings->PercentScreen) / 100;
		settings->DesktopHeight = (xfi->workArea.height * settings->PercentScreen) / 100;
		maxWidth = settings->DesktopWidth;
		maxHeight = settings->DesktopHeight;
	}
	else
	{
		maxWidth = WidthOfScreen(xfi->screen);
		maxHeight = HeightOfScreen(xfi->screen);
	}

	if (!settings->Fullscreen && !settings->Workarea && !settings->UseMultimon)
		return TRUE;

	nmonitors = 0;
	primaryMonitor = 0;

	vscreen->area.left = settings->DesktopPosX;
	vscreen->area.right = MIN(settings->DesktopPosX + settings->DesktopWidth - 1, maxWidth);
	vscreen->area.top = settings->DesktopPosY;
	vscreen->area.bottom = MIN(settings->DesktopPosY + settings->DesktopHeight - 1, maxHeight);

	for (i = 0; i < vscreen->nmonitors; i++)
	{
		if ((vscreen->monitors[i].area.left > vscreen->area.right) || (vscreen->monitors[i].area.right < vscreen->area.left) ||
			(vscreen->monitors[i].area.top > vscreen->area.bottom) || (vscreen->monitors[i].area.bottom < vscreen->area.top))
				continue;

		settings->MonitorDefArray[nmonitors].x = vscreen->monitors[i].area.left - settings->DesktopPosX;
		settings->MonitorDefArray[nmonitors].y = vscreen->monitors[i].area.top - settings->DesktopPosY;
		settings->MonitorDefArray[nmonitors].width = MIN(vscreen->monitors[i].area.right - vscreen->monitors[i].area.left + 1, settings->DesktopWidth);
		settings->MonitorDefArray[nmonitors].height = MIN(vscreen->monitors[i].area.bottom - vscreen->monitors[i].area.top + 1, settings->DesktopHeight);
		settings->MonitorDefArray[nmonitors].is_primary = vscreen->monitors[i].primary;

		primaryMonitor |= vscreen->monitors[i].primary;
		nmonitors++;
	}

	settings->MonitorCount = nmonitors;

	if (nmonitors && !primaryMonitor)
		settings->MonitorDefArray[0].is_primary = TRUE;
	
	if (settings->MonitorCount)
	{
		settings->DesktopWidth = vscreen->area.right - vscreen->area.left + 1;
		settings->DesktopHeight = vscreen->area.bottom - vscreen->area.top + 1;
	}

	return TRUE;
}
