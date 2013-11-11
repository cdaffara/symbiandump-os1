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
//

/**
 @file
 @test
 @internalComponent
*/

#include "t_pseudoappcfbsbitmapanim.h"
#include "t_testsurfacerasterizer.h"

EXPORT_C CCFbsBitmapAnimation* CCFbsBitmapAnimation::NewL(TDisplayMode aMode, const TSize& aAnimSize, const TSize& aSurfaceSize,
                                                          TPtrC& aBitmapFile, TInt aHorizontalRate, TInt aVerticalRate,
                                                          TPixelFormat aPixelFormat)
	{
	RDebug::Print(_L("Creating CCFbsBitmapAnimation class\n"));
    CCFbsBitmapAnimation* self = new (ELeave) CCFbsBitmapAnimation();
    CleanupStack::PushL(self);
    self->ConstructL(aMode, aAnimSize, aSurfaceSize, aBitmapFile, aHorizontalRate, aVerticalRate, aPixelFormat);
    CleanupStack::Pop(); // self;
    return self;
	}

void CCFbsBitmapAnimation::ConstructL(TDisplayMode aMode, const TSize& aAnimSize, const TSize& aSurfaceSize,
                                      TPtrC& aBitmapFile, TInt aHorizontalRate, TInt aVerticalRate,
                                      TPixelFormat aPixelFormat)
	{
	RDebug::Print(_L("Creating full-screen bitmap\n"));

	iBitmapAnimFile.Set(aBitmapFile);
	iHorizontalRate = aHorizontalRate;
	iVerticalRate   = aVerticalRate;
	iAnimSize       = aAnimSize;
	iSurfaceSize    = aSurfaceSize;
	iPixelFormat    = aPixelFormat;
	iRotationSupported = ETrue;

	iFullSurfaceBitmap = new(ELeave) CFbsBitmap;

	if(iPixelFormat == EFormatYuv)
		{
		TSize sz(iSurfaceSize.iWidth/2, iSurfaceSize.iHeight);
		iFullSurfaceBitmap->Create(sz, static_cast<TDisplayMode>(aMode));
		}
	else
		{
		iFullSurfaceBitmap->Create(iSurfaceSize, static_cast<TDisplayMode>(aMode));
		}
	iFullSurfaceBitmapDevice = CFbsBitmapDevice::NewL(iFullSurfaceBitmap);
	iFullSurfaceBitmapGc = CFbsBitGc::NewL();
	iFullSurfaceBitmapGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
	}

CCFbsBitmapAnimation::CCFbsBitmapAnimation() : CTestAnimation()
	{
	};

EXPORT_C CCFbsBitmapAnimation::~CCFbsBitmapAnimation()
	{
	delete iFullSurfaceBitmapGc;
	delete iFullSurfaceBitmapDevice;
	delete iFullSurfaceBitmap;
	}

EXPORT_C void CCFbsBitmapAnimation::Rotate(TSize /*aScreenSize*/)
	{
	}

EXPORT_C void CCFbsBitmapAnimation::DrawL(CTestSurfaceRasterizer* aSurfaceRasterizer, SurfaceDetails& aSurfDetails)
	{
	iFullSurfaceBitmapGc->Activate(iFullSurfaceBitmapDevice);
	iFullSurfaceBitmapGc->Clear();

	RedrawSurfaceL();

	//Send the fullscreen bitmap to the test rasterizer
	iFullSurfaceBitmap->LockHeap();

	TUint32* bitmapBufferPtr  = iFullSurfaceBitmap->DataAddress();
	aSurfaceRasterizer->DrawRect(bitmapBufferPtr, aSurfDetails);

	iFullSurfaceBitmap->UnlockHeap();
	}

EXPORT_C void CCFbsBitmapAnimation::SetFrameNumber(TInt aFrameNumber)
	{
	iFrameCounter = aFrameNumber;
	}

void CCFbsBitmapAnimation::RedrawSurfaceL()
	{
	CFbsBitmap bitmap;
	User::LeaveIfError(bitmap.Load(iBitmapAnimFile, iFrameCounter));

	const TSize nativeSize(bitmap.SizeInPixels());

	const TSize fullScreenSize(iFullSurfaceBitmap->SizeInPixels());

	if(iPixelFormat == EFormatYuv)
		{
		TInt noOfPixels=fullScreenSize.iWidth * fullScreenSize.iHeight;
		TInt counter=noOfPixels;
		TUint32* address = iFullSurfaceBitmap->DataAddress();
		for(TInt i=0; i<counter; i++)
			{
			/* The only supported Yuv format is KPixelFormatYUV422Interleaved */
			/* Paint the surface white */
			address[i] = 0xFF7FFF7F;
			}
		}

	/* Potentially four screen copies will be required in the wrapping case *
	 * (1) no wrapping (2) wrap in x co-ord (3) wrap in y co-ord            *
	 * (4) wrap in both x and y co-ords                                     *
	 * There is no problem with specifying negative co-ordinates for the    *
	 * top LH corner as the offscreen section will not be displayed         */

	//Perform blits to the fullscreen bitmap
	//iAnimSize is the size of the square bitmap as it appears on the surface and iDisplayedSize is the size of the
	//rectangulara area limiting the animation

	TSize sz = iAnimSize;
	if(iPixelFormat == EFormatYuv)
		{
		sz = TSize(iAnimSize.iWidth/2, iAnimSize.iHeight);
		}

	iFullSurfaceBitmapGc->DrawBitmap(TRect(TPoint(iXPos, iYPos), sz), &bitmap, TRect(TPoint(0,0), nativeSize));

	//Wrap in x co-ord
	if( (iXPos + iAnimSize.iWidth) > iSurfaceSize.iWidth)
		{
		iFullSurfaceBitmapGc->DrawBitmap(TRect(TPoint(iXPos - iSurfaceSize.iWidth, iYPos), sz),
									       &bitmap, TRect(TPoint(0,0), nativeSize));
		}

	//Wrap in y co-ord
	if( (iYPos + iAnimSize.iHeight) > iSurfaceSize.iHeight)
		{
		iFullSurfaceBitmapGc->DrawBitmap(TRect(TPoint(iXPos, iYPos - iSurfaceSize.iHeight), sz),
									       &bitmap, TRect(TPoint(0,0), nativeSize));
		}

	//Wrap in both x and y co-ords
	if( ((iXPos + iAnimSize.iWidth) > iSurfaceSize.iWidth) && ((iYPos + iAnimSize.iHeight) > iSurfaceSize.iHeight) )
		{
		iFullSurfaceBitmapGc->DrawBitmap(TRect(TPoint(iXPos - iSurfaceSize.iWidth, iYPos - iSurfaceSize.iHeight), sz),
									       &bitmap, TRect(TPoint(0,0), nativeSize));
		}

	//Increment x and y positions and wrap if necessary
	iXPos += iSurfaceSize.iWidth/iHorizontalRate;
	iYPos += iSurfaceSize.iHeight/iVerticalRate;
	iXPos = iXPos % iSurfaceSize.iWidth;
	iYPos = iYPos % iSurfaceSize.iHeight;

	iFrameCounter++;
	}
