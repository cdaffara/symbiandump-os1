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
// wins\specific\multitouch.cpp
// 
//
#include "multitouch.h"
#include "resource.h"

#define KDefaultMaxProximity -100
#define KDefaultMaxPressure 5000
#define KDefaultPressureStep 500
#define KDefaultProximityStep 5

// Static members
DMultiMouse DMultiMouse::iMice[KMaxMice];
int DMultiMouse::iNumMice = 0;
DMultiMouse* DMultiMouse::iPrimary = 0;
int DMultiMouse::iMouseId = 0;
int DMultiTouch::iNumberOfMice = 0;
bool DMultiTouch::iMultiTouchSupported= FALSE;
bool DMultiTouch::iMultiTouchCreated= FALSE;
bool DMultiTouch::iMultiTouchTempEnabled= FALSE;

// Function pointers for raw input APIs
TYPEOF_RegisterRawInputDevices pfnRegisterRawInputDevices= NULL;
TYPEOF_GetRawInputData pfnGetRawInputData= NULL;
TYPEOF_GetRawInputDeviceList pfnGetRawInputDeviceList=NULL;

/**
 *  Initialise the proximity and pressure information if undefined by the user
 */
DMultiTouch::DMultiTouch(TInt aProximityStep, TInt aPressureStep)
	{
	iZMaxRange = KDefaultMaxProximity;
	iMaxPressure = KDefaultMaxPressure;
	iProximityStep = (aProximityStep == -1) ? KDefaultProximityStep : aProximityStep;
	iPressureStep = (aPressureStep == -1) ? KDefaultPressureStep : aPressureStep;
	}

/** 
 * Register all the mice 
*/
BOOL DMultiTouch::Register()
	{
	RAWINPUTDEVICE device;
	device.usUsagePage = 0x01;
	device.usUsage = 0x02;
	device.dwFlags = RIDEV_NOLEGACY; // adds HID mouse and also ignores legacy mouse messages
	device.hwndTarget = 0;
	ShowCursors();
	return pfnRegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));
	}

/**
 * Unregister mice devices
 */
BOOL DMultiTouch::UnRegister()
	{
	RAWINPUTDEVICE device;
	device.usUsagePage = 0x01;
	device.usUsage = 0x02;
	device.dwFlags = RIDEV_REMOVE;
	device.hwndTarget = NULL;
	HideCursors();
	return pfnRegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));
	}


/* * Handle multi-input Window messages
 */
void DMultiTouch::OnWmInput(HWND aHWnd,TUint aMessage,TUint aWParam,TUint aLParam,HWND aParentHwnd)
	{
	RAWINPUT ri;
	UINT dwSize = sizeof(ri);
	if (pfnGetRawInputData((HRAWINPUT)aLParam, RID_INPUT, &ri, &dwSize, sizeof(RAWINPUTHEADER))==(UINT)-1)
		{
		OutputDebugString(TEXT("GetRawInputData has an error !\n"));
		}
	else if (ri.header.dwType == RIM_TYPEMOUSE) 
		{
		DMultiMouse* mouse = DMultiMouse::Find(ri.header.hDevice);
		if (mouse)
			{
			if (!DMultiMouse::iPrimary)
				{
				DMultiMouse::iPrimary = mouse;
				DMultiMouse::iPrimary->iIsPrimary = TRUE;
				}
			mouse->HandleRawMouseEvent(ri.data.mouse, aParentHwnd);
			}
		}
	DefWindowProcA(aHWnd, aMessage, aWParam, aLParam);
	}

void DMultiTouch::HideCursors()
	{
	for (int ii=0; ii<DMultiMouse::iNumMice; ii++)
		{
		DMultiMouse::iMice[ii].iCursorWnd.Hide();
		}
	}

void DMultiTouch::ShowCursors()
	{
	for (int ii=0; ii<DMultiMouse::iNumMice; ii++)
		{
		DMultiMouse::iMice[ii].iCursorWnd.Show();
		}
	}

/**
 * The cursor window procedure
 */
static LRESULT CALLBACK CursorWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
	{
	if (msg==WM_PAINT)
		{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		CursorWindow* wnd = (CursorWindow*)GetWindowLong(hwnd, GWL_USERDATA);
		DrawIconEx(hdc,0,0, wnd->iCursor,0,0,0,NULL, DI_NORMAL | DI_DEFAULTSIZE);
		EndPaint(hwnd, &ps);
		return 0;
		}
	return DefWindowProc(hwnd, msg, wp, lp);
	}

CursorWindow::CursorWindow(): iHwnd(NULL),iCursor(NULL),iNumber(0)
	{
	}

HWND CursorWindow::Create(HMODULE hm, HWND hwndParent, int number)
	{
	// Create the window
	static ATOM atom = NULL;
	if (!atom) 
		{
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(wcex));
		wcex.cbSize = sizeof(WNDCLASSEX); 
		wcex.style			= CS_OWNDC;
		wcex.lpfnWndProc	= (WNDPROC)CursorWndProc;
		wcex.hInstance		= (HINSTANCE)hm;
		wcex.lpszClassName	= TEXT("CursorWndClass");
		wcex.hbrBackground = CreateSolidBrush(RGB(255,0,0));//Background color is also for the number
		atom = RegisterClassEx(&wcex);
		}
	iHwnd = CreateWindowA((LPCSTR)atom, NULL, WS_CHILD|WS_CLIPSIBLINGS, 0,0,64,64, hwndParent, NULL, hm, NULL);
	SetWindowLong(iHwnd, GWL_USERDATA, (LONG)this);
	iNumber = number;
	return iHwnd;
	}

void CursorWindow::Show()
	{
	ShowWindow(iHwnd, SW_NORMAL);
	}

void CursorWindow::Hide()
	{
	ShowWindow(iHwnd, SW_HIDE);
	}

BOOL CursorWindow::SetCursor(HCURSOR hc)
	{
	// Duplicate the cursor (because we're going to draw a number on the mask)
	if (iCursor) 
		{
		DestroyCursor(iCursor);
		iCursor = NULL;
		}
	iCursor = CopyCursor(hc);

	// Get information about the cursor, and select its mask bitmap into a temporary DC.
	ICONINFO ii;
	GetIconInfo(iCursor, &ii);
	iHotspot.x = ii.xHotspot;
	iHotspot.y = ii.yHotspot;
	HDC hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, ii.hbmMask);

	// Get the cursor's pixel size	
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biBitCount = 0;
	GetDIBits(hdc, ii.hbmMask, 0, 0, NULL, &bmi, DIB_RGB_COLORS);
	int cx = bmi.bmiHeader.biWidth;
	int cy = bmi.bmiHeader.biHeight;

	// Monochrome cursors have a double-height hbmMask. The top half contains the AND 
	// bitmap (which we do want) and the bottom half contains the XOR bitmap (which we 
	// dont want). Colour cursors have a single normal-height AND mask.
	BOOL isMonochrome = (ii.hbmColor==NULL);
	int cyy = isMonochrome ? (cy>>1) : cy; 

	// Draw the number into the mask
	char ach[4];
	int ld = iNumber/10;
	int rd = iNumber % 10;
	if (ld > 0)
		{
		wsprintf((LPTSTR)ach, (LPCTSTR)TEXT("%d%d"), ld,rd);
		}
	else 
		{
		wsprintf((LPTSTR)ach, (LPCTSTR)TEXT("%d"), rd);
		}

	HFONT hf = CreateFontA(12,0, 0,0,FW_THIN, FALSE,FALSE,FALSE, 0,DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, "Arial");
	SelectObject(hdc, hf);
	SetBkMode(hdc, TRANSPARENT);
	TextOutA(hdc, 0,cyy-12, ach, 2);
	DeleteObject(hf);

	// Get the bits of the mask (in 32-bit colour)
	HANDLE heap = GetProcessHeap();
	bmi.bmiHeader.biBitCount = 32;
	DWORD* bits = (DWORD*)HeapAlloc(heap, 0, cx*cy*4);
	GetDIBits(hdc, ii.hbmMask, 0, cy, bits, &bmi, DIB_RGB_COLORS);

	// Set the window to the size of the cursor
	SetWindowPos(iHwnd, NULL,0,0,cx,cyy, SWP_NOMOVE);

	// Create a cursor-shaped region by starting out with an empty region
	// and ORing in each zero-valued mask pixel.
	HRGN rgn = CreateRectRgn(0,0,0,0);
	for (int y=0 ; y<cyy ; y++) 
		{
		for (int x=0 ; x<cx ; x++) 
			{
			if (bits[(cy-y-1)*cx+x]==0) 
				{
				HRGN rgnPix = CreateRectRgn(x,y, x+1,y+1);
				CombineRgn(rgn, rgn, rgnPix, RGN_OR);
				DeleteObject(rgnPix);
				}
			}
		}

	// Cleanup
	HeapFree(heap, 0, bits);
	DeleteDC(hdc);

	// Set the window's clipping region to the cursor-shaped region
	SetWindowRgn(iHwnd, rgn, TRUE);
	return TRUE;
	}
	
void CursorWindow::GetPosition(POINT& pt)
	{
	pt.x = 0;
	pt.y = 0;
	MapWindowPoints(iHwnd, GetParent(iHwnd), &pt, 1);
	pt.x += iHotspot.x;
	pt.y += iHotspot.y;
	}

void CursorWindow::SetPosition(POINT& pt)
	{
	SetWindowPos(iHwnd, NULL, pt.x - iHotspot.x, pt.y - iHotspot.y, 0, 0, SWP_NOSIZE);
	}

/** 
 * Add the mouse device to the collection
 */
TInt DMultiMouse::Add(RAWINPUTDEVICELIST& aDev)
	{
	if (iNumMice < KMaxMice)
		{
		DMultiMouse& mouse = iMice[iNumMice];
		mouse.iDevice = aDev.hDevice;
		iNumMice++;
		return KErrNone;
		}
	else
		{
		return KErrOverflow;
		}
	}

DMultiMouse::DMultiMouse() :
	iX(-1), iY(-1), iZ(0), iDevice(0),iId(-1) 
	{
	}

DMultiMouse* DMultiMouse::Find(HANDLE aHandle)
	{
	for (TInt ii=0; ii<iNumMice; ii++)
		{
		DMultiMouse& mouse = iMice[ii];
		if (mouse.iDevice == aHandle)
			return &mouse;
		}
	return NULL;
	}

void DMultiMouse::HandleRawMouseEvent(RAWMOUSE& aEvent, HWND aWnd)
	{
	// give this pointer an id, if it doesn't already have one
	if (iId == -1)
		{
		iId = iMouseId++;
		}

	// Create the cursor window and set the cursor if not done yet
	if (iCursorWnd.iHwnd == NULL)
		{
		iCursorWnd.Create((HINSTANCE)0, aWnd,iId);
		iCursorWnd.SetCursor(LoadCursorA(NULL, (LPCSTR)IDC_ARROW));
		}

	CorrectSystemMouse();
			
	// recalc mouse position
	if (iX == -1)
		{
		// initial position
		iX = iPrimary->iX;
		iY = iPrimary->iY;
		}

	if (aEvent.usFlags & MOUSE_MOVE_ABSOLUTE)
		{
		// absolute position info can update all pointers
		iX = aEvent.lLastX;
		iY = aEvent.lLastY;
		}
	else if (!iIsPrimary)
			{
			// relative position info updates non-primary pointers,
			iX += aEvent.lLastX;
			iY += aEvent.lLastY;
			}

	// Show the cursor window
	ShowMousePos(aWnd);

	TInt message = WM_MOUSEMOVE;

	// get button state
	if (aEvent.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		{
		message = WM_LBUTTONDOWN;
		iZ = 0;
		}
	
	if (aEvent.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		{
		message = WM_LBUTTONUP;
		iZ = 0;
		}
	
	if (aEvent.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		{
		message = WM_RBUTTONDOWN;
		}
	
	if (aEvent.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		{
		message = WM_RBUTTONUP;
		}
	
	if (aEvent.usButtonFlags & RI_MOUSE_WHEEL)
		{
		message = WM_MOUSEWHEEL;
		if ((TInt16)(aEvent.usButtonData&0x8000)== 0) // positive number
			{
			if (iZ < TheMultiTouch->iMaxPressure)
				{
				if (iZ>=0)
					{ // pressure range 
					iZ += TheMultiTouch->iPressureStep;//Pressure step
					if (iZ > TheMultiTouch->iMaxPressure)
						{
						iZ = TheMultiTouch->iMaxPressure;
						}
					}
				else
					{ // proximity range
					iZ += TheMultiTouch->iProximityStep; //Proximity step
					}
				}
			}
		else
			{
			if (iZ > TheMultiTouch->iZMaxRange)
				{
				if (iZ <= 0)
					{// proximity range 
					iZ -= TheMultiTouch->iProximityStep;//Proximity step
					if (iZ < TheMultiTouch->iZMaxRange)
						{
						iZ = TheMultiTouch->iZMaxRange;
						}
					}
				else
					{// pressure range
					iZ -= TheMultiTouch->iPressureStep;//Pressure step
					}
				}
			}
		}
	
	MultiTouchWndPointer(message, iX, iY, iZ, iId);
	
	}

/**
 * Show the cursor window when the cursor is inside the client area
 */
void DMultiMouse::ShowMousePos(HWND aHWnd)
	{
	RECT client = {0,0,0,0};
	if(aHWnd)
		{
		GetWindowRect(aHWnd, &client); 
		POINT pt = {iX-client.left,iY-client.top};
		iCursorWnd.SetPosition(pt);
		}
	iCursorWnd.Show();
	}

void DMultiMouse::CorrectSystemMouse()
	{
	if (iIsPrimary)
		{
		POINT pos;
		if (GetCursorPos(&pos)) // if failed, pos contains garbage.
			{
			iX = pos.x;
			iY = pos.y;	
			}
		}
	else
		{
		SetCursorPos(iPrimary->iX,iPrimary->iY);
		}
	}
	
/** 
 * a static function to check how many mice are connected 
*/
bool DMultiTouch::Init()
	{
	HMODULE hModule = GetModuleHandleA("user32.dll");
	if(hModule == NULL)
		return FALSE;
	
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	GetVersionEx(&osvi);
	// Check for Win 2K or higher version
	if (osvi.dwMajorVersion < 5)
		{
		return FALSE;
		}
	
	// Not supported on Win2K
	if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0))
		{
		return FALSE;
		}
		
	pfnRegisterRawInputDevices 	= (TYPEOF_RegisterRawInputDevices)GetProcAddress(hModule, "RegisterRawInputDevices");
	pfnGetRawInputData 			= (TYPEOF_GetRawInputData)GetProcAddress(hModule, "GetRawInputData");
	pfnGetRawInputDeviceList 	= (TYPEOF_GetRawInputDeviceList)GetProcAddress(hModule, "GetRawInputDeviceList");
	
	if((pfnRegisterRawInputDevices == NULL) || (pfnGetRawInputData == NULL) || (pfnGetRawInputDeviceList == NULL))
		{
		return FALSE;
		}
		
	UINT nDevices;
	
	if (pfnGetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
		{
		return FALSE;
		}

	RAWINPUTDEVICELIST* pRawInputDeviceList = new RAWINPUTDEVICELIST[ nDevices ];
	if (!pRawInputDeviceList)
		{
		return FALSE;
		}

	pfnGetRawInputDeviceList(pRawInputDeviceList, &nDevices,
			sizeof(RAWINPUTDEVICELIST));

	
	for (UINT i=0; i<nDevices; i++)
		{
		RAWINPUTDEVICELIST& dev = pRawInputDeviceList[i];
		if (dev.dwType == RIM_TYPEMOUSE)
			{
			if (DMultiMouse::Add(dev)!=KErrNone)
				{
				//free the device list
				delete[] pRawInputDeviceList;
				return FALSE;
				}
			}
		}

	delete[] pRawInputDeviceList;

	// Multitouch is supported when more than 2 mice are connected (including the hidden RID mouse)
	return DMultiMouse::iNumMice > 2;
	}

