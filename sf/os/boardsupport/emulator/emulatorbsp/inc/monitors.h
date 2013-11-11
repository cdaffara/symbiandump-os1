// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// wins\inc\monitors.h
// Declare stuff we need for multiple monitor support.
// (Not available in the version of windows.h we use.)
// 
//

#ifndef __MONITORS_H__
#define __MONITORS_H__

#ifndef SM_CMONITORS

#ifdef __cplusplus
extern "C" {
#endif

#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#define SM_SAMEDISPLAYFORMAT    81

DECLARE_HANDLE(HMONITOR);

typedef struct tagMONITORINFO
{
    DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} MONITORINFO, *LPMONITORINFO;

#ifdef __cplusplus
}
#endif

#endif		// ifndef SM_CMONITORS

/* CodeWarrior include files have SM_CMONITORS defined unconditionally,
 * but MONITOR_DEFAULTTONULL etc depend upon _WIN32_WINDOWS values.
 */
#ifndef MONITOR_DEFAULTTONULL

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

#endif		// ifndef MONITOR_DEFAULTTONULL

class SystemMonitors
	{
public:
	SystemMonitors(void);
	inline TInt Count(void) { return iCount; };
	TBool RectAllOnOne(RECT& rect);
	HMONITOR MonitorFromRect(const RECT& rect, UINT flags=MONITOR_DEFAULTTONULL);
	TBool GetMonitorInfo(HMONITOR monitor, LPMONITORINFO pMonInfo);
private:
	TBool iHaveMultiMonFunctions;
	TBool iCount;
	HMONITOR (WINAPI *ipMonitorFromRect)(LPCRECT lprcScreenCoords, UINT uFlags);
	BOOL (WINAPI *ipGetMonitorInfo)(HMONITOR hMonitor, LPMONITORINFO lpMonitorInfo);
	};

#endif		// ifndef __MONITORS_H__
