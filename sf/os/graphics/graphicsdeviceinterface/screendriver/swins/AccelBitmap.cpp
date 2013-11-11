// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "GraphicsAccelerator.h"

// Windows headers...
#define UNICODE
#pragma warning( disable : 4201 )
#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#include <windows.h>
#include <windowsx.h>
#pragma warning( default : 4201 )

#include <bitdraw.h>
#include "_WININC.H"

/** Fills a structure with data which describes the basic details of this bitmap.

@param aInfo Bitmap information structure to fill with the details of this bitmap.
@return KErrNone if successful, KErrBadHandle if a value has not been assigned to iHandle. */
EXPORT_C TInt RHardwareBitmap::GetInfo(TAcceleratedBitmapInfo& aInfo) const
	{
	if(!iHandle)
		return(KErrBadHandle);

	// -1 or less refers to the screen. It is the (screen number + 1) actually with "-" sign.
	if(iHandle <= -1)
		{
        RWindows* window = ::WindowHandler(-iHandle - 1);
		aInfo.iDisplayMode = window->iDisplayMode;
		aInfo.iAddress = (TUint8*)(window->EpocBitmapBits());
		aInfo.iSize = window->iEpocBitmapSize;
		aInfo.iLinePitch = window->iEpocBitmapLinePitch;
		aInfo.iPhysicalAddress = 0;
		switch(aInfo.iDisplayMode)
			{
			case ENone:
				aInfo.iPixelShift = -1;
				break;
			case EGray2:
				aInfo.iPixelShift = 0;
				break;
			case EGray4:
				aInfo.iPixelShift = 1;
				break;
			case EGray16:
			case EColor16:
				aInfo.iPixelShift = 2;
				break;
			case EGray256:
			case EColor256:
				aInfo.iPixelShift = 3;
				break;
			case EColor4K:
			case EColor64K:
				aInfo.iPixelShift = 4;
				break;
			case EColor16M:
			case ERgb:
			case EColor16MU:
				aInfo.iPixelShift = 5;
				break;
			}
		}
	else
		aInfo = *(TAcceleratedBitmapInfo*)iHandle;	// iHandle is really a pointer to a TAcceleratedBitmapInfo

	return(KErrNone);
	}

/**
This method sets the value of iHandle data member.
When the iHandle's value is positive - the it is the bitmap address in the memory.
When the iHandle's value is negative - it is a screen number reference:
 "-1" - screen 0, "-2" - screen 1, "-3" - screen 2, ...
All that means: negative iHandle values describe the RHardwareBitmap object as a screen
bitmap, the screen number can be calculated from iHandle's value. 
Positive iHandle values describe RHardwareBitmap object as an in-memory bitmap.
By default iHandle is initialized with 0, which means - invalid handle and uninitialized
RHardwareBitmap object.
@param aScreenNo Screen number.
@return KErrNone
*/
EXPORT_C TInt RHardwareBitmap::SetAsScreenReference(TInt aScreenNo)
	{
	iHandle = aScreenNo;
	return KErrNone;
	}

EXPORT_C TInt RHardwareBitmap::Create(TDisplayMode aDisplayMode, TSize aSize, TUid /*aCreatorUid*/)
	{
	// Make line pitch different from normal bitmaps, useful for testing purposes.
	TInt hwWidth = ((aSize.iWidth + 15) / 16) * 16;

	TInt linePitch;

	switch(aDisplayMode)
		{
		case EColor256:
		case EGray256:
			linePitch = hwWidth;
			break;
		case EColor64K:
		case EColor4K:
			linePitch = hwWidth * 2;
			break;
		case EColor16M:
			linePitch = (((hwWidth * 3) + 11) / 12) * 12; // Multiples of 12 bytes!
			break;
		case ERgb:
		case EColor16MU:
			linePitch = hwWidth * 4;
			break;
		default:
			return(KErrNotSupported);
		}

	TInt memSize = sizeof(TAcceleratedBitmapInfo)+linePitch*aSize.iHeight;

	TAcceleratedBitmapInfo* bitmap = (TAcceleratedBitmapInfo*)GlobalAllocPtr(GMEM_FIXED,memSize);

	if(!bitmap)
		return(KErrNoMemory);

	bitmap->iDisplayMode = aDisplayMode;
	bitmap->iAddress = (TUint8*)(bitmap+1);
	bitmap->iSize = aSize;
	bitmap->iLinePitch = linePitch;
	switch(aDisplayMode)
		{
		case ENone:
			bitmap->iPixelShift = -1;
			break;
		case EGray2:
			bitmap->iPixelShift = 0;
			break;
		case EGray4:
			bitmap->iPixelShift = 1;
			break;
		case EGray16:
		case EColor16:
			bitmap->iPixelShift = 2;
			break;
		case EGray256:
		case EColor256:
			bitmap->iPixelShift = 3;
			break;
		case EColor4K:
		case EColor64K:
			bitmap->iPixelShift = 4;
			break;
		case EColor16M:
		case ERgb:
		case EColor16MU:
			bitmap->iPixelShift = 5;
			break;
		}
	bitmap->iPhysicalAddress = 0;

	iHandle = (TInt)bitmap;

	return(KErrNone);
	}

EXPORT_C void RHardwareBitmap::Destroy()
	{
	GlobalFreePtr((void*)iHandle);
	}

