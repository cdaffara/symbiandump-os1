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

#include <hal.h>
#include <e32std.h>
#include <stdlib.h>
#include <bitdraw.h>
#include "ScreenInfo.h"
#include "_WININC.H"

/**
The max allowed screen number + 1.
@internalComponent
*/
const TInt KMaxScreenNumber = 32;

/**
TheWinsWindowHandlers array is used as a container for the created RWindows objects.
Each RWindows object manages single Windows OS window, which can be shared between one
or more Symbian OS devices.
It cannot be a C++ array (some of the CArray... classes), because it is part of a shared
global memory. If it is a C++ array, then its constructor will be called every time, when
an exe loads scdv.dll, which means - the array data will be destroyed!
@internalComponent
*/
static RWindows* TheWinsWindowHandlers[KMaxScreenNumber];

/**
WindowHandler() global function is used to get a pointer to the RWindows object, responsible 
for the drawing operations on the Emulator on the screen with aScreenNo number.
@param aScreenNo Screen number
@return A pointer to the related RWindows object or NULL if there is no such object.
@panic "SCDV-13" panic if the aScreenNo is out of bounds [0..32].
@see RPointerArray
@internalComponent
*/
RWindows* WindowHandler(TInt aScreenNo)
    {
    __ASSERT_ALWAYS(aScreenNo >= 0 && aScreenNo < KMaxScreenNumber, ::Panic(EScreenDriverPanicInvalidScreenNo));
    return TheWinsWindowHandlers[aScreenNo];
    }

/**
Used in CreateScreenSemaphoreName().
@see CreateScreenSemaphoreName()
@internalComponent
*/
_LIT(KWindowInUseSemaphore,"WindowInUseSemaphore");

/**
This global function is used for the creation of the name of the semaphore, which helps
RWindows functionality to detect the moment, when the related Windows OS window can be 
destroyed safely - no more clients (Symbian OS screen devices).
@param aScreenNo Screen number
@param aScreenSemaphoreName An output parameter, where the semaphore name will be constructed
                            in the following format "WindowInUseSemaphore<ScreenNo>".
                            The length of aScreenSemaphoreName should be big enough for
                            the semaphore name.
@internalComponent
*/
void CreateScreenSemaphoreName(TInt aScreenNo, TDes& aScreenSemaphoreName)
    {
    aScreenSemaphoreName.Copy(KWindowInUseSemaphore);
    aScreenSemaphoreName.AppendNum(aScreenNo);
    }

/**
*/
RWindows::RWindows():
	iHwnd(NULL),
	iHdc(NULL),
	iHbitmap(NULL),
	iBitmapInfo(NULL),
	iBitmapBits(NULL),
	iH90bitmap(NULL),
	i90BitmapInfo(NULL),
	i90BitmapBits(NULL),
	i90ByteWidth(0),
	iByteWidth(0),
	iEpocBitmapBits(NULL),
	iOrientation(0),
    iScreenNo(-1)
	{
    }

/**
The method returns a pointer to the RWindows object, which manages the Windows OS window
for the aScreenNo Symbian OS device screen.
If no RWindows object exists for the requested screen number, it will be created.
@param aScreenNo Screen number
@param aHwnd Windows OS - window handle
@param aSize Screen size in pixels
@return A pointer to the related for that screen number RWindows object or NULL if
        GetWindow() call fails. The reason for the failure can be: no memory or some
        Windows OS related problem.
*/
RWindows* RWindows::GetWindow(TInt aScreenNo, TAny* aHwnd, const TSize& aSize)
	{
    //If TheWinsWindowHandlers[aScreenNo] is not null, then it is a valid RWindows object, 
    //return it (a pointer to it).
    if(TheWinsWindowHandlers[aScreenNo])
        {
		return TheWinsWindowHandlers[aScreenNo];
        }
    //TheWinsWindowHandlers[aScreenNo] is null - allocate a memory for it.
    //If the allocation fails - return NULL.
	TheWinsWindowHandlers[aScreenNo] = (RWindows*)GlobalAlloc(GPTR,sizeof(RWindows));
	if(!TheWinsWindowHandlers[aScreenNo])
        {
		return NULL;
        }
    //Construct TheWinsWindowHandlers[aScreenNo] object, return NULL if the construction fails.
	TRAPD(ret,TheWinsWindowHandlers[aScreenNo]->ConstructL(aHwnd,aSize));
	if (ret != KErrNone)
		{
		TheWinsWindowHandlers[aScreenNo]->Destroy();
		TheWinsWindowHandlers[aScreenNo] = NULL;
		return NULL;
		}
    //
    TheWinsWindowHandlers[aScreenNo]->iScreenNo = aScreenNo;
	return TheWinsWindowHandlers[aScreenNo];
	}

/**
The method releases the access the RWindows object, which manages aScreenNo screen.
If there are no more clients (Symbian OS devices) for that window, it will be destroyed.
@param aScreenNo Screen number
*/
void RWindows::ReleaseWindow(TInt aScreenNo)
	{
	if(TheWinsWindowHandlers[aScreenNo] == NULL)
        {//The related TheWinsWindowHandlers[aScreenNo] is NULL, which means that the 
         //related RWindows object has been destroyed. Do nothing.
		return;
        }
	RSemaphore windowInUse;
    TBuf<32> screenSemaphoreName;
    ::CreateScreenSemaphoreName(aScreenNo, screenSemaphoreName);
	TInt ret = windowInUse.OpenGlobal(screenSemaphoreName,EOwnerThread);
    //The related semaphore does not exist, which means - all the clients, which have a shared
    //access to aScreenNo screen, do not use it anymore. It is safe to destroy the related
    //Windows OS window object.
	if (ret == KErrNotFound)
		{
		TheWinsWindowHandlers[aScreenNo]->Destroy();
		TheWinsWindowHandlers[aScreenNo] = NULL;
		}
	windowInUse.Close();
	}

TUint8* RWindows::PixelAddress(TInt aX,TInt aY)
	{
	if (iOrientation & 1)
		{
		TUint8* pixelPtr = i90BitmapBits;
		pixelPtr += i90ByteWidth * (i90BitmapInfo->bmiHeader.biHeight - aY - 1);
		pixelPtr += aX * 3;
		return pixelPtr;
		}

	TUint8* pixelPtr = iBitmapBits;
	pixelPtr += iByteWidth * (iBitmapInfo->bmiHeader.biHeight - aY - 1);
	pixelPtr += aX * 3;
	return pixelPtr;
	}


	
void RWindows::Destroy()
	{
	GdiFlush();
	if (iHbitmap) DeleteObject(iHbitmap);
	if (iH90bitmap) DeleteObject(iH90bitmap);
	GlobalFree(iBitmapInfo);
	GlobalFree(i90BitmapInfo);
	if (iHwnd && iHdc) ReleaseDC(iHwnd,iHdc);
	GlobalFree(iEpocBitmapBits);
	GlobalFree(this);
	}

void RWindows::ConstructL(TAny* aHwnd,const TSize& aSize)
	{
	iHwnd = (HWND)aHwnd;
	iHdc = GetDC(iHwnd);
	User::LeaveIfNull(iHdc);
	SetMapMode(iHdc,MM_TEXT);
	SetROP2(iHdc,R2_COPYPEN);

	iByteWidth = aSize.iWidth*3;
	i90ByteWidth = aSize.iHeight*3;
	TInt byteSize = aSize.iWidth * aSize.iHeight * 3;

	iBitmapInfo = (BITMAPINFO*)User::LeaveIfNull(GlobalAlloc(GPTR,sizeof(BITMAPINFO)));
	iBitmapInfo->bmiHeader.biBitCount = 24;
	iBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	iBitmapInfo->bmiHeader.biWidth = aSize.iWidth;
	iBitmapInfo->bmiHeader.biHeight = aSize.iHeight;
	iBitmapInfo->bmiHeader.biPlanes = 1;
	iBitmapInfo->bmiHeader.biCompression = BI_RGB;

	i90BitmapInfo = (BITMAPINFO*)User::LeaveIfNull(GlobalAlloc(GPTR,sizeof(BITMAPINFO)));
	i90BitmapInfo->bmiHeader.biBitCount = 24;
	i90BitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	i90BitmapInfo->bmiHeader.biWidth = aSize.iHeight;
	i90BitmapInfo->bmiHeader.biHeight = aSize.iWidth;
	i90BitmapInfo->bmiHeader.biPlanes = 1;
	i90BitmapInfo->bmiHeader.biCompression = BI_RGB;

	iHbitmap = CreateDIBSection(iHdc,iBitmapInfo,DIB_RGB_COLORS,(TAny**)&iBitmapBits,NULL,0);
	User::LeaveIfNull(iHbitmap);
	Mem::Fill(iBitmapBits,byteSize,0xff);
	iH90bitmap = CreateDIBSection(iHdc,i90BitmapInfo,DIB_RGB_COLORS,(TAny**)&i90BitmapBits,NULL,0);
	User::LeaveIfNull(iH90bitmap);
	Mem::Fill(i90BitmapBits,byteSize,0xff);

	//We have to allocate iEpocBitmapBits to be large enough to hold all color bytes (32 bpp)
	//for the rectangle with aSize.iWidth & aSize.iHeight dimensions.
	iEpocBitmapBits = (TUint32*)User::LeaveIfNull(GlobalAlloc(GMEM_FIXED,aSize.iWidth * aSize.iHeight * 4));
	Mem::Fill(iEpocBitmapBits,aSize.iWidth * aSize.iHeight * 4,0xff);
	}

void RWindows::SetOrientation(TInt aOrientation)
	{
	iOrientation = aOrientation;
	}

TInt RWindows::Orientation() const
	{
	return iOrientation;
	}

void RWindows::Update(const TRegion& aRgn,const TSize& aSize)
	{
	if(aRgn.CheckError())
		UpdateRect(TRect(aSize),aSize);
	else
		for(TInt count=0;count<aRgn.Count();count++)
			UpdateRect(aRgn[count],aSize);
	}

void RWindows::UpdateRect(const TRect& aRect,const TSize& aSize)
	{
	if (iOrientation & 1)
		SetDIBitsToDevice(iHdc,aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height(),
		aRect.iTl.iX,aSize.iWidth-aRect.iBr.iY,0,aSize.iWidth,i90BitmapBits,
		((LPBITMAPINFO)i90BitmapInfo),DIB_RGB_COLORS);
	else
		SetDIBitsToDevice(iHdc,aRect.iTl.iX,aRect.iTl.iY,aRect.Width(),aRect.Height(),
		aRect.iTl.iX,aSize.iHeight-aRect.iBr.iY,0,aSize.iHeight,iBitmapBits,
		((LPBITMAPINFO)iBitmapInfo),DIB_RGB_COLORS);
	}

//Gets the Size dimension the RWindow is using as per orientation
TSize RWindows::GetOrientedSize()
	{
	TSize orientedSize;
	if (iOrientation&1)
	orientedSize.SetSize(iEpocBitmapSize.iHeight,iEpocBitmapSize.iWidth);
	else
	orientedSize = iEpocBitmapSize;

	return orientedSize;
	}

//Gets the actual orientation based refresh rect required from a 'physical' screen reference.
TRect RWindows::GetOrientedRect(const TRect &aScreenRect)
	{	
	TRect orientedRect;	
	
	switch(iOrientation)		
	{
		case 1:
			{
			orientedRect.SetRect(aScreenRect.iTl.iY, 		
								 iEpocBitmapSize.iWidth-aScreenRect.iBr.iX, 
								 aScreenRect.iBr.iY,
								 iEpocBitmapSize.iWidth-aScreenRect.iTl.iX);	
			break;
			}
		case 2:
			{	
			orientedRect.SetRect(iEpocBitmapSize.iWidth-aScreenRect.iBr.iX,
								 iEpocBitmapSize.iHeight-aScreenRect.iBr.iY, 							 
								 iEpocBitmapSize.iWidth-aScreenRect.iTl.iX,
								 iEpocBitmapSize.iHeight-aScreenRect.iTl.iY);
			break;
			}
		case 3:
		{
			orientedRect.SetRect(iEpocBitmapSize.iHeight-aScreenRect.iBr.iY, 		
								 aScreenRect.iTl.iX, 
							 	 iEpocBitmapSize.iHeight-aScreenRect.iTl.iY,
							 	 aScreenRect.iBr.iX);
			break;
		}		
		default:
		{
			orientedRect = aScreenRect;
			break;				
		} 
	}
	
	return orientedRect;	
	}