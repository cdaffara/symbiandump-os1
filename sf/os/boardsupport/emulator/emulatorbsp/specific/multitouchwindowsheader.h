// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\specific\multitouchwindowsheader.h
// 
//
/** 
 * This header file includes some Windows API declarations
 */
#ifndef MULTITOUCHWINDOWSHEADER_H
#define MULTITOUCHWINDOWSHEADER_H

#pragma warning(disable:4201) // ignore the anonymous struct warning in this file
#define _ANONYMOUS_UNION
#define _ANONYMOUS_STRUCT

#undef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A

#define RID_INPUT  0x10000003
#define RID_HEADER 0x10000005
#define RIDEV_REMOVE       0x00000001
#define RIDEV_PAGEONLY     0x00000020
#define RIDEV_NOLEGACY     0x00000030
#define MOUSE_MOVE_ABSOLUTE      0x00000001
#define RI_MOUSE_LEFT_BUTTON_DOWN   0x0001
#define RI_MOUSE_LEFT_BUTTON_UP     0x0002
#define RI_MOUSE_RIGHT_BUTTON_DOWN  0x0004
#define RI_MOUSE_RIGHT_BUTTON_UP    0x0008
#define RI_MOUSE_WHEEL              0x0400
#define RIM_TYPEMOUSE    0x00000000

DECLARE_HANDLE(HRAWINPUT);
typedef struct tagRAWINPUTHEADER {
	DWORD dwType;
	DWORD dwSize;
	HANDLE hDevice;
	WPARAM wParam;
} RAWINPUTHEADER,*PRAWINPUTHEADER;

typedef struct tagRAWMOUSE {
	USHORT usFlags;
	_ANONYMOUS_UNION union {
		ULONG ulButtons;
		_ANONYMOUS_STRUCT struct {
			USHORT usButtonFlags;
			USHORT usButtonData;
		};
	};
	ULONG ulRawButtons;
	LONG lLastX;
	LONG lLastY;
	ULONG ulExtraInformation;
} RAWMOUSE,*PRAWMOUSE,*LPRAWMOUSE;

typedef struct tagRAWKEYBOARD {
	USHORT MakeCode;
	USHORT Flags;
	USHORT Reserved;
	USHORT VKey;
	UINT Message;
	ULONG ExtraInformation;
} RAWKEYBOARD,*PRAWKEYBOARD,*LPRAWKEYBOARD;

typedef struct tagRAWHID {
	DWORD dwSizeHid;
	DWORD dwCount;
	BYTE bRawData;
} RAWHID,*PRAWHID,*LPRAWHID;

typedef struct tagRAWINPUT {
	RAWINPUTHEADER header;
	union {
		RAWMOUSE    mouse;
		RAWKEYBOARD keyboard;
		RAWHID      hid;
	} data;
} RAWINPUT,*PRAWINPUT,*LPRAWINPUT;

typedef struct tagRAWINPUTDEVICE {
	USHORT usUsagePage;
	USHORT usUsage;
	DWORD dwFlags;
	HWND hwndTarget;
} RAWINPUTDEVICE,*PRAWINPUTDEVICE,*LPRAWINPUTDEVICE;

typedef const RAWINPUTDEVICE *PCRAWINPUTDEVICE;

typedef struct tagRAWINPUTDEVICELIST {
	HANDLE hDevice;
	DWORD dwType;
} RAWINPUTDEVICELIST,*PRAWINPUTDEVICELIST;

typedef WINUSERAPI BOOL (WINAPI *TYPEOF_RegisterRawInputDevices)(PCRAWINPUTDEVICE,UINT,UINT);
typedef WINUSERAPI UINT (WINAPI *TYPEOF_GetRawInputData)(HRAWINPUT,UINT,LPVOID,PUINT,UINT);
typedef WINUSERAPI UINT (WINAPI *TYPEOF_GetRawInputDeviceList)(PRAWINPUTDEVICELIST,PUINT,UINT);

#endif
