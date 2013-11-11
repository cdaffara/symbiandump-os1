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
#include "scdraw.h"

/**
Constructs the CDrawScreenBitmap object.
@param aScreenNo Screen number. It will be used in HAL::Get() calls.
@param aBitmapAddress The screen memory address.
@param aSize Screen size
@return System-wide error codes, KErrNone if the construction was successfull.
*/
TInt CDrawScreenBitmap::ConstructScreen(TInt aScreenNo, TAny* aBitmapAddress,TSize aSize)
	{
    iScreenNo = aScreenNo;
	TInt ret = SCREEN_LAYOUT_BASE_BITMAP::Construct(aSize);
	if (ret != KErrNone)
		return ret;

	//fixup for emulating smaller screen
	TInt offset = 0;
	// get the offset in bytes
	ret = HAL::Get(iScreenNo, HALData::EDisplayOffsetBetweenLines, offset);

	TInt bitsPerPixel = BitsPerPixel(iDispMode);
 	TInt bytesPerPixel  = (bitsPerPixel/8 + (bitsPerPixel % 8 ? 1 : 0));

	if (ret == KErrNone )
		{
		// Find iLongwidth (in pixels) from the offset (in bytes).
		if (bitsPerPixel >= 8)
			{
			// a pixel fits in 1 or more bytes
		    iLongWidth =  offset / bytesPerPixel;
			}
			else
			{
			// 1 or more pixels per byte
			TInt pixelsPerByte  = 8 / bitsPerPixel;
		    iLongWidth =  offset * pixelsPerByte;
			}
		iScanLineWords = offset / 4;
		
		// correct the size of the ScanLineBuffer
		User::Heap().Free(iScanLineBuffer);
		iScanLineBuffer = (TUint32*)(User::Heap().Alloc(offset));
		if(iScanLineBuffer == NULL)
			return KErrNoMemory;
		}
	else
		{
		return ret;
		}
	//fixup for emulating smaller screen

	offset = 0; // Pass in display mode
	ret = HAL::Get(iScreenNo, HALData::EDisplayOffsetToFirstPixel,offset);
	iBits = (TUint32*)aBitmapAddress;
	iBits += offset / sizeof(TUint32);
	return ret;
	}

void CDrawScreenBitmap::SetDisplayMode(CFbsDrawDevice* aDrawDevice)
	{
	CopyOldSettings(aDrawDevice);
	}

void CDrawScreenBitmap::OrientationsAvailable(TBool aOrientation[4])
	{
	aOrientation[EOrientationNormal] = ETrue;
	aOrientation[EOrientationRotated90] = ETrue;
	aOrientation[EOrientationRotated180] = ETrue;
	aOrientation[EOrientationRotated270] = ETrue;
	}

TBool CDrawScreenBitmap::SetOrientation(TOrientation aOrientation)
	{
	iOrientation = aOrientation;
	return ETrue;
	}

TInt CDrawScreenBitmap::HorzTwipsPerThousandPixels() const
	{
	if (iSize.iWidth == 0)
		return 0;

	TInt twips = 0;
	HAL::Get(iScreenNo, HALData::EDisplayXTwips,twips);

	return twips * 1000 / iSize.iWidth; // iSize gets iWidth and iHeight swapped by changing the orientation so always use iWidth
	}

TInt CDrawScreenBitmap::VertTwipsPerThousandPixels() const
	{
	if (iSize.iHeight == 0)
		return 0;

	TInt twips = 0;
	HAL::Get(iScreenNo, HALData::EDisplayYTwips,twips);

	return twips * 1000 / iSize.iHeight;
	}

