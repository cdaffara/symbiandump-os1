// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

TInt CDrawThirtyTwoBppScreenBitmapAlpha::InitScreen()
	{
	TRect drawRect;
	GetDrawRect(drawRect);
	RWindows* window = ::WindowHandler(iScreenNo);
	window->iDisplayMode = DisplayMode();
	window->iEpocBitmapSize = drawRect.Size();
	window->iEpocBitmapLinePitch = iScanLineWords * 4;

	return KErrNone;
	}

TBool CDrawThirtyTwoBppScreenBitmapAlpha::SetOrientation(TOrientation aOrientation)
	{
	if (aOrientation == iEmulatorOrientation)
		return ETrue;

	SetScreenOrientation(aOrientation);
	iEmulatorOrientation = aOrientation;
	iOrientation = aOrientation ;
	return ETrue;
	}

void CDrawThirtyTwoBppScreenBitmapAlpha::UpdateRect(const TRect& aRect) const
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
	TInt scanLineLen=iLongWidth*4;
	TInt srcPixelStep=4;
	TPoint srcStart(aRect.iTl);
	switch(iOrientation)
		{
	case CFbsDrawDevice::EOrientationRotated90:
		srcPixelStep=scanLineLen;
		scanLineLen=-4;
		srcStart.iX=iSize.iWidth-1-aRect.iTl.iY;
		srcStart.iY=aRect.iTl.iX;
		break;
	case CFbsDrawDevice::EOrientationRotated180:
		srcPixelStep=-4;
		scanLineLen=-scanLineLen;
		srcStart.iX=iSize.iWidth-1-aRect.iTl.iX;
		srcStart.iY=iSize.iHeight-1-aRect.iTl.iY;
		break;
	case CFbsDrawDevice::EOrientationRotated270:
		srcPixelStep=-scanLineLen;
		scanLineLen=4;
		srcStart.iX=aRect.iTl.iY;
		srcStart.iY=iSize.iHeight-1-aRect.iTl.iX;
		break;
		}
	TUint8* srcePtr = (TUint8*)PixelAddress(srcStart.iX,srcStart.iY);
	TUint8* srcePtrLimit = srcePtr + aRect.Width()*srcPixelStep;
	TInt rowMax = aRect.iTl.iY+aRect.Height();
	srcPixelStep-=3;	// as we increment while reading it
	for(TInt row = aRect.iTl.iY; row < rowMax; row++)
		{
		TUint8* tempSrcePtr = srcePtr;
		TUint8* destPixel = WinPixelAddress(aRect.iTl.iX,row);

		while (tempSrcePtr != srcePtrLimit)
 			{
			*destPixel++=*tempSrcePtr++;
			*destPixel++=*tempSrcePtr++;
			*destPixel++=*tempSrcePtr++;
			tempSrcePtr += srcPixelStep;
 			}
 		srcePtr+=scanLineLen;
 		srcePtrLimit+=scanLineLen;
		}
	}
