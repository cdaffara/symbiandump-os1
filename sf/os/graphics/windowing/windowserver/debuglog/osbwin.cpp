// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

// Win32 dependent code for debugging wserv offscreenbuffer and UI surface on emulator
//
#include "osbwin.h"
#include "_windows.h"

TInt CDebugOsbWin::iId=0;
const TInt KMaxBuffer=32;
HBITMAP* TheBitmap[KMaxBuffer];
HWND* TheWindow[KMaxBuffer];

// Application window specific messages
#define WMA_RESIZE		(WM_APP + 0)

struct TWin32Info
	{
	HWND iHwnd;
	HDC iHdc;
	HBITMAP iHbitmap;
	BITMAPINFO* iBitmapInfo;
	};

EXPORT_C CDebugOsbWin* CDebugOsbWin::NewL(const TDesC& aName, TSize aSize)
	{
	CDebugOsbWin* self = new(ELeave) CDebugOsbWin(aName, aSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDebugOsbWin::CDebugOsbWin(const TDesC& aName, TSize aSize):
	iThreadCreated(EFalse), iSize(aSize), iName(aName)
	{}

EXPORT_C CDebugOsbWin::~CDebugOsbWin()
	{
	iSem.Close();
	if (iThreadCreated)
		{
		iThread.Terminate(0);
		iThread.Close();
		}
	if (iWin32)
		{
		if (iWin32->iHbitmap)
			DeleteObject(iWin32->iHbitmap);
		if (iWin32->iBitmapInfo)
			User::Free(iWin32->iBitmapInfo);
		if (iWin32->iHwnd && iWin32->iHdc)
			ReleaseDC(iWin32->iHwnd,iWin32->iHdc);
		delete iWin32;
		}
	if (iPalette)
		delete iPalette;
	--CDebugOsbWin::iId;
	}

void CDebugOsbWin::ConstructL()
	{
	if (iId>=KMaxBuffer)
		User::Leave(KErrNoMemory);
	iPalette=CPalette::NewDefaultL(EColor256);
	iWin32=new(ELeave) TWin32Info;
	// store window handle and bitmap association to global table
	TheWindow[iId]=&(iWin32->iHwnd);
	TheBitmap[iId]=&(iWin32->iHbitmap);

	_LIT(KIdFormat, " (%d)");
	iName.AppendFormat(KIdFormat, iId);

	++iId;
	iSem.CreateLocal(0);
	const TInt KHeapSize=0x4000;
	User::LeaveIfError(iThread.Create(iName,CDebugOsbWin::ThreadMain,KDefaultStackSize,KHeapSize,KHeapSize,this));
	iThread.SetPriority(EPriorityMuchLess);
	iThread.Resume();
	iThreadCreated=ETrue;
	iSem.Wait();
	// iHwnd initialized by WinMain
	iWin32->iHdc=GetDC(iWin32->iHwnd);
	SetMapMode(iWin32->iHdc,MM_TEXT);
	SetROP2(iWin32->iHdc,R2_COPYPEN);
	TInt byteSize=iSize.iWidth*iSize.iHeight*3;
	iWin32->iBitmapInfo=(BITMAPINFO*)User::Alloc(sizeof(BITMAPINFO));
	User::LeaveIfNull(iWin32->iBitmapInfo);
	iWin32->iBitmapInfo->bmiHeader.biBitCount=24;
	iWin32->iBitmapInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	iWin32->iBitmapInfo->bmiHeader.biWidth=iSize.iWidth;
	iWin32->iBitmapInfo->bmiHeader.biHeight=-iSize.iHeight;//top-down DIB
	iWin32->iBitmapInfo->bmiHeader.biPlanes=1;
	iWin32->iBitmapInfo->bmiHeader.biCompression=BI_RGB;
	iWin32->iHbitmap=CreateDIBSection(iWin32->iHdc,iWin32->iBitmapInfo,DIB_RGB_COLORS,(TAny**)&iBitmapBits,NULL,0);
	User::LeaveIfNull(iWin32->iHbitmap);
	Mem::Fill(iBitmapBits,byteSize,0xff);
	}

EXPORT_C void CDebugOsbWin::Refresh(TSize aSize, TDisplayMode aDisplayMode, const TUint32* aDataAddress)
	{
	TBool hasResized = EFalse;

	// When screen is rotated, the size can change as width and height are swapped.
	if (iSize != aSize)
		{
		iSize = aSize;
		iWin32->iBitmapInfo->bmiHeader.biWidth = aSize.iWidth;
		iWin32->iBitmapInfo->bmiHeader.biHeight = -aSize.iHeight; //top-down DIB
		hasResized = ETrue;
		}

	switch (aDisplayMode)
		{
		case EGray4:
			Copy2Bpp(aDataAddress);
			break;
		case EColor256:
			Copy8Bpp(aDataAddress);
			break;
		case EColor4K:
			Copy12Bpp(aDataAddress);
			break;
		case EColor64K:
			Copy16Bpp(aDataAddress);
			break;
		case EColor16M:
			Copy24Bpp(aDataAddress);
			break;
		case EColor16MU:
		case EColor16MA:
		case EColor16MAP:	// Should really have alpha divided out
			CopyU24Bpp(aDataAddress);
			break;
		default:
			return;
		}
	SetDIBitsToDevice(iWin32->iHdc,0,0,iSize.iWidth,iSize.iHeight,0,0,0,iSize.iHeight,iBitmapBits,
		((LPBITMAPINFO)iWin32->iBitmapInfo),DIB_RGB_COLORS);
	if (hasResized)
		{
		// This will cause a redraw so no need to invalidate.
		PostMessage(iWin32->iHwnd, WMA_RESIZE, iSize.iWidth + iHExtra, iSize.iHeight + iVExtra);
		}
	else
		{
		InvalidateRect(iWin32->iHwnd,NULL,FALSE);
		}
	}

void CDebugOsbWin::Copy2Bpp(const TUint32* aDataAddress)
	{
	const TUint8 gray[]={0,85,170,255};
	
	const TUint8* src=(const TUint8*)aDataAddress;
	TUint8* dest=iBitmapBits;
	const TInt width=iSize.iWidth>>2;
	for (TInt row=0;row<iSize.iHeight;++row)
		{
		for (TInt col=0;col<width;++col)
			{
			TUint8 p1=*src++;
			TUint8 p2=TUint8((p1>>2) & 0x03);
			TUint8 p3=TUint8((p1>>4) & 0x03);
			TUint8 p4=TUint8((p1>>6) & 0x03);
			p1&=0x03;
			dest[0]=dest[1]=dest[2]=gray[p1];
			dest[3]=dest[4]=dest[5]=gray[p2];
			dest[6]=dest[7]=dest[8]=gray[p3];
			dest[9]=dest[10]=dest[11]=gray[p4];
			dest+=12; // 1 byte source equals to 4 destination pixels
			}
		}
	}

void CDebugOsbWin::Copy8Bpp(const TUint32* aDataAddress)
	{
	if (!iPalette)
		return;
	const TUint8* src=(const TUint8*)aDataAddress;
	TUint8* dest=iBitmapBits;
	for (TInt row=0;row<iSize.iHeight;++row)
		{
		for (TInt col=0;col<iSize.iWidth;++col)
			{
			const TRgb rgb(iPalette->GetEntry(*src++));
			dest[0]=TUint8(rgb.Blue());
			dest[1]=TUint8(rgb.Green());
			dest[2]=TUint8(rgb.Red());
			dest+=3;
			}
		}
	}

void CDebugOsbWin::Copy12Bpp(const TUint32* aDataAddress)
	{

	const TUint16* src=(const TUint16*)aDataAddress;
	TUint8* dest=iBitmapBits;
	for (TInt row=0;row<iSize.iHeight;++row)
		{
		for (TInt col=0;col<iSize.iWidth;++col)
			{
			dest[0]=TUint8((*src & 0x00f));
			dest[0]|=dest[0] << 4;
			dest[1]=TUint8((*src & 0x0f0));
			dest[1]|=dest[1] >> 4;
			dest[2]=TUint8((*src & 0xf00)>>4);
			dest[2]|=dest[2] >> 4;
			++src;
			dest+=3;
			}
		}
	}

void CDebugOsbWin::Copy16Bpp(const TUint32* aDataAddress)
	{
	const TUint16* src=(const TUint16*)aDataAddress;
	TUint8* dest=iBitmapBits;
	for (TInt row=0;row<iSize.iHeight;++row)
		{
		for (TInt col=0;col<iSize.iWidth;++col)
			{
			dest[0]=TUint8((*src & 0x001f)<<3);
			dest[0]=TUint8(dest[0]+(dest[0]>>5));
			dest[1]=TUint8((*src & 0x07e0)>>3);
			dest[1]=TUint8(dest[1]+(dest[1]>>6));
			dest[2]=TUint8((*src & 0xf800)>>8);
			dest[2]=TUint8(dest[2]+(dest[2]>>5));
			++src;
			dest+=3;
			}
		}
	}

void CDebugOsbWin::CopyU24Bpp(const TUint32* aDataAddress)
	{
	const TUint8* src=(const TUint8*)aDataAddress;
	TUint8* dest=iBitmapBits;
	for (TInt row=0;row<iSize.iHeight;++row)
		{
		for (TInt col=0;col<iSize.iWidth;++col)
			{
			dest[0]=*src++;
			dest[1]=*src++;
			dest[2]=*src++;
			++src; // unpack, takes 4 bytes per pixel
			dest+=3;
			}
		}
	}

void CDebugOsbWin::Copy24Bpp(const TUint32* aDataAddress)
	{
	const TUint8* src = (const TUint8*)aDataAddress;
	Mem::Copy(iBitmapBits, src, 3 * iSize.iWidth * iSize.iHeight);
	}

HBITMAP* GetBitmap(HWND aHwnd)
	{
	TInt i;
	for (i=0;i<CDebugOsbWin::iId;++i)
		{
		if (*(TheWindow[i])==aHwnd)
			return TheBitmap[i];
		}
	return NULL;
	}

TInt32 APIENTRY WindowProc(HWND aHwnd,TUint aMsg,TUint aWparam,TInt32 aLparam)
	{
	switch (aMsg)
		{
		case WM_PAINT:
			{
			HBITMAP* hBitmap=GetBitmap(aHwnd);
			if (hBitmap)
				{
				PAINTSTRUCT p;
				BeginPaint(aHwnd,&p);
		   		HDC hdcBits;
				BITMAP bm;
	    		hdcBits=CreateCompatibleDC(p.hdc);
				GetObject(*hBitmap,sizeof(BITMAP),&bm);
	    		SelectObject(hdcBits,*hBitmap);
				RECT windowRect;
				GetClientRect(aHwnd,&windowRect);
				BitBlt(p.hdc,0,0,windowRect.right-windowRect.left,windowRect.bottom-windowRect.top,hdcBits,0,0,SRCCOPY);
				DeleteDC(hdcBits);
	 			EndPaint(aHwnd,&p);
				}
			}
			return 0;
		case WMA_RESIZE:
			{
			RECT rc;
			GetWindowRect(aHwnd, &rc);
			MoveWindow(aHwnd, rc.left, rc.top, aWparam, aLparam, TRUE);
			}
			break;
		default:
			return DefWindowProc(aHwnd,aMsg,aWparam,aLparam);
		}
	return 1;
	}

TInt CDebugOsbWin::ThreadMain(TAny* aArg)
    {
    CDebugOsbWin* self=(CDebugOsbWin*)aArg;
    if (!self || !self->iWin32)
    	return KErrArgument;
    TWin32Info* win32=self->iWin32;
    TSize size=self->iSize;
    WNDCLASS wndclass;
    const TText *szAppName=self->iName.Ptr();
    wndclass.style=CS_HREDRAW|CS_VREDRAW;
    wndclass.lpfnWndProc=WindowProc;
    wndclass.cbClsExtra=0;
    wndclass.cbWndExtra=0;
    wndclass.hInstance=NULL;
    wndclass.hIcon=LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor=LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName=NULL;
    wndclass.lpszClassName=(LPCTSTR)szAppName;
	self->iHExtra = GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	self->iVExtra = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
    RegisterClass(&wndclass);
	win32->iHwnd=CreateWindow((LPCTSTR)szAppName,
                    (LPCTSTR)szAppName,
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                    CW_USEDEFAULT,
                    (CDebugOsbWin::iId-1)*(size.iHeight+30),
                    size.iWidth+self->iHExtra,
                    size.iHeight+self->iVExtra,
                    NULL,
                    NULL,
                    NULL,
                    NULL);
    ShowWindow(win32->iHwnd, SW_SHOWNORMAL);
    UpdateWindow(win32->iHwnd);
	// Now ConstructL can continue with ready to use HWND
	self->iSem.Signal();
	MSG msg;
	// Can't use normal win32 loop. Theoritically, GetMessage with specific HWND param should do, but
	// somehow it's still messing emulator main message loop.
	// The standard win32 loop in debug log to window (deblogwn.cpp) doesn't  seem to work on 9.1
	while (1)
		{
		if (PeekMessage(&msg,win32->iHwnd,0,0,PM_REMOVE))
			DispatchMessage(&msg);
		User::After(300*1000);
		}
	return KErrNone;
    }
