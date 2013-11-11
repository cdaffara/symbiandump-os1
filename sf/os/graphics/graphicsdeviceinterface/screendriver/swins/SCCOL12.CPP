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

#include "SCDRAW.H"
#include "_WININC.H"

TInt CDrawTwelveBppScreenBitmap::InitScreen()
	{
	TRect drawRect;
	GetDrawRect(drawRect);
    RWindows* window = ::WindowHandler(iScreenNo);
    window->iDisplayMode = DisplayMode();
	window->iEpocBitmapSize = drawRect.Size();
	window->iEpocBitmapLinePitch = (iScanLineWords*4);

	return KErrNone;
	}

TBool CDrawTwelveBppScreenBitmap::SetOrientation(TOrientation aOrientation)
	{
	if (aOrientation == iEmulatorOrientation)
		return ETrue;

	SetScreenOrientation(aOrientation);
	iEmulatorOrientation = aOrientation;
	iOrientation = aOrientation ;
	return ETrue;
	}

void CDrawTwelveBppScreenBitmap::UpdateRect(const TRect& aRect) const
	{
	ASSERT(aRect.iTl.iX >= 0 && aRect.iTl.iY >= 0);
#if defined(_DEBUG)
	if (iOrientation&1)
		{
		ASSERT(aRect.iBr.iX <= iSize.iHeight);
		ASSERT(aRect.iBr.iY <= iSize.iWidth);
		}
	else
		{
		ASSERT(aRect.iBr.iX <= iSize.iWidth);
		ASSERT(aRect.iBr.iY <= iSize.iHeight);
		}
#endif
	TInt scanLineLen=iLongWidth;
	TInt srcPixelStep=1;
	TPoint srcStart(aRect.iTl);
	switch(iOrientation)
		{
	case CFbsDrawDevice::EOrientationRotated90:
		srcPixelStep=scanLineLen;
		scanLineLen=-1;
		srcStart.iX=iSize.iWidth-1-aRect.iTl.iY;
		srcStart.iY=aRect.iTl.iX;
		break;
	case CFbsDrawDevice::EOrientationRotated180:
		srcPixelStep=-1;
		scanLineLen=-scanLineLen;
		srcStart.iX=iSize.iWidth-1-aRect.iTl.iX;
		srcStart.iY=iSize.iHeight-1-aRect.iTl.iY;
		break;
	case CFbsDrawDevice::EOrientationRotated270:
		srcPixelStep=-scanLineLen;
		scanLineLen=1;
		srcStart.iX=aRect.iTl.iY;
		srcStart.iY=iSize.iHeight-1-aRect.iTl.iX;
		break;
		}
	TUint16* srcePtr = CDrawSixteenBppBitmapCommon::PixelAddress(srcStart.iX,srcStart.iY);
	TUint16* srcePtrLimit = srcePtr + aRect.Width()*srcPixelStep;
	TInt rowMax = aRect.iTl.iY+aRect.Height();
	for(TInt row = aRect.iTl.iY; row < rowMax; row++)
		{
		TUint16* tempSrcePtr = srcePtr;
		TUint8* destPixel = WinPixelAddress(aRect.iTl.iX,row);

		while (tempSrcePtr != srcePtrLimit)
			{
			destPixel[0] = TUint8((*tempSrcePtr & 0x00f));
			destPixel[0] |= destPixel[0] << 4;
			destPixel[1] = TUint8((*tempSrcePtr & 0x0f0));
			destPixel[1] |= destPixel[1] >> 4;
			destPixel[2] = TUint8((*tempSrcePtr & 0xf00)>>4);
			destPixel[2] |= destPixel[2] >> 4;	
			
			tempSrcePtr+=srcPixelStep;
		
			destPixel += 3;
			}

		srcePtr += scanLineLen;
		srcePtrLimit += scanLineLen;
		}
	}

