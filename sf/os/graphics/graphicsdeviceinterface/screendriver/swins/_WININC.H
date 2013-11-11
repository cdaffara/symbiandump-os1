// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ___WININC_H__
#define ___WININC_H__

#define UNICODE
#pragma warning( disable : 4201 )
#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#include <windows.h>
#pragma warning( default : 4201 )
#include <TCHAR.H>

#include <gdi.h>

/**
RWindows describes an object, which creates and manages a Windows OS window,
used for the Symbian OS drawing operations on the Emulator.
A single window object can be shared between one or more RWindows clients.
@internalComponent
*/
class RWindows
	{
public:
	static RWindows* GetWindow(TInt aScreenNo, TAny* aHwnd,const TSize& aSize);
	static void ReleaseWindow(TInt aScreenNo);
	void Update(const TRegion& aRgn,const TSize& aSize);
	TUint8* PixelAddress(TInt aX,TInt aY);
	TUint32* EpocBitmapBits() const {return iEpocBitmapBits;}
	void SetOrientation(TInt aOrientation);
	TInt Orientation() const;
	TSize GetOrientedSize();
	TRect GetOrientedRect(const TRect &aSize);
private:
	RWindows();
	void ConstructL(TAny* aHwnd,const TSize& aSize);
	void Destroy();
	void UpdateRect(const TRect& aRect,const TSize& aSize);
public:
	TDisplayMode iDisplayMode;
	TSize iEpocBitmapSize;
	TInt iEpocBitmapLinePitch;
	TRgb* iPalette;
private:
	HWND iHwnd;
	HDC iHdc;
	HBITMAP iHbitmap;
	BITMAPINFO* iBitmapInfo;
	TUint8* iBitmapBits;
	HBITMAP iH90bitmap;
	BITMAPINFO* i90BitmapInfo;
	TUint8* i90BitmapBits;
	TInt iByteWidth;
	TInt i90ByteWidth;
	TUint32* iEpocBitmapBits;
	TInt iOrientation;
    TInt iScreenNo;
	};

RWindows* WindowHandler(TInt aScreenNo);
void CreateScreenSemaphoreName(TInt aScreenNo, TDes& aScreenSemaphoreName);

#endif
