// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsurfacehelper.h"
#include <w32std.h>

CSurfaceHelper* CSurfaceHelper::NewL()
	{
	CSurfaceHelper* helper = new(ELeave)CSurfaceHelper;
	CleanupStack::PushL(helper);
	helper->ConstructL();
	CleanupStack::Pop(helper);
	return helper;
	}

CSurfaceHelper::~CSurfaceHelper()
	{
	DestroySurfaces();
	iSurfaces.Close();
	iManager.Close();
	}

/**
 * Load logical device and open a surface manager
 */
void CSurfaceHelper::ConstructL()
	{
	TInt res = iManager.Open();
	if (res != KErrNone)
		{
		User::Leave(res);
		}
	}

/**
 * Create the surface with the given parameters.
 * The surface will be owned by the instance of this class  
 */
TSurfaceId CSurfaceHelper::CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride, TInt aBuffers)
	{
	RSurfaceManager::TSurfaceCreationAttributesBuf bf;
	RSurfaceManager::TSurfaceCreationAttributes& b = bf();
	
	b.iSize.iWidth = aSize.iWidth;
	b.iSize.iHeight = aSize.iHeight;
	b.iBuffers = aBuffers;				// number of buffers in the surface
	b.iPixelFormat = aPixelFormat;
	b.iStride = aStride;		// Number of bytes between start of one line and start of next
	b.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	b.iAlignment = 4;			// alignment, 1,2,4,8 byte aligned
	b.iContiguous = EFalse;
	b.iMappable = ETrue;

	TSurfaceId surface = TSurfaceId::CreateNullId();

	User::LeaveIfError(iManager.CreateSurface(bf, surface));
	iSurfaces.AppendL(surface);
	return surface;
	}

/**
 * Destroy all surfaces wich have been created by the instance of this class
 */
void CSurfaceHelper::DestroySurfaces()
	{
	TInt numSurfaces = iSurfaces.Count();
	for (TInt index = 0; index < numSurfaces; index++)
		{
		TSurfaceId surfaceId = iSurfaces[index];
		TInt err = iManager.CloseSurface(surfaceId);
		__ASSERT_DEBUG(err ==KErrNone, User::Panic(_L("CSurfaceHelper::DestroySurfaces"), err));
		}
	iSurfaces.Reset();
	}

/**
Fill a rectangle on the given surface.

@param aSurface		The surface to be filled.
@param aStartPos	Where to place the rectangle.
@param aSize		Size of the rectangle.
@param aColor		The colour to fill it with.
*/
void CSurfaceHelper::FillRectangleL(const TSurfaceId& aSurface, const TPoint& aStartPos, const TSize& aSize, const TRgb& aColor)
	{
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	User::LeaveIfError(iManager.SurfaceInfo(aSurface, infoBuf));
	TUint32 color = 0;

	if (info.iSize.iHeight<0 || info.iSize.iWidth<0 || info.iStride<0)
		{
		User::Leave(KErrCorrupt);
		}
	if (info.iSize.iHeight==0 || info.iSize.iWidth==0 || info.iStride==0)
		{
		User::Leave(KErrNotReady);
		}

	switch (info.iPixelFormat)
		{
		case EUidPixelFormatXRGB_8888:
			{
			color = aColor.Color16MU();
#ifdef ALPHA_FIX_24BIT
			color |= ((ALPHA_FIX_24BIT)&0xff)<<24;
#endif
			break;
			}
		case EUidPixelFormatARGB_8888:
			{
			color = aColor.Color16MA();
			break;
			}
		case EUidPixelFormatARGB_8888_PRE:
			{
			color = aColor.Color16MAP();
			break;
			}
		case EUidPixelFormatRGB_565:
			{
			color = aColor.Color64K();
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			break;
			}
		}

	RChunk chunk;
	User::LeaveIfError(iManager.MapSurface(aSurface, chunk));
	CleanupClosePushL(chunk);
	TUint8* surfacePtr = chunk.Base();
	
	// Check for out of bounds
	TBool validRect = ETrue;
	TInt surfaceWidth = info.iSize.iWidth;
	TInt surfaceHeight = info.iSize.iHeight;
	
	// Width and Height
	if ((aStartPos.iX + aSize.iWidth) > surfaceWidth)
		{
		validRect = EFalse;
		}
	
	if ((aStartPos.iY + aSize.iHeight) > surfaceHeight)
		{
		validRect = EFalse;
		}
	
	// Starting position
	if ((aStartPos.iX < 0) || (aStartPos.iY < 0))
		{
		validRect = EFalse;
		}
	
	if (!validRect)
		{
		User::Leave(KErrOverflow);
		}
		
	if (info.iPixelFormat == EUidPixelFormatRGB_565)
		{//2 bytes per pixel
		if ( info.iSize.iWidth*2>info.iStride)
			{
			User::Leave(KErrOverflow);
			}
		
		TInt offset;
		User::LeaveIfError(iManager.GetBufferOffset(aSurface, 0, offset));
		TUint16* ptr = reinterpret_cast<TUint16*>(surfacePtr + offset);
		
		// Fill the rectangle
		TInt yPos = aStartPos.iY;
		TInt xPos = aStartPos.iX;
		for (TInt yy = 0; yy < aSize.iHeight; ++yy)
			{
			ptr = reinterpret_cast<TUint16*>(surfacePtr + (yPos*info.iStride));
			for (TInt xx = 0; xx < aSize.iWidth; ++xx)
				{
				ptr[xPos] = color;
				xPos++;
				}
			xPos = aStartPos.iX;
			yPos++;
			}
		}
	else
		{
		if ( info.iSize.iWidth*4>info.iStride)
			{
			User::Leave(KErrOverflow);
			}

		TInt offset;
		User::LeaveIfError(iManager.GetBufferOffset(aSurface, 0, offset));
		TUint32* ptr = reinterpret_cast<TUint32*>(surfacePtr + offset);		
		
		// Fill the rectangle
		TInt yPos = aStartPos.iY;
		TInt xPos = aStartPos.iX;
		for (TInt yy = 0; yy < aSize.iHeight; ++yy)
			{
			ptr = reinterpret_cast<TUint32*>(surfacePtr+(yPos*info.iStride));
			for (TInt xx = 0; xx < aSize.iWidth; ++xx)
				{
				ptr[xPos] = color;
				xPos++;
				}
			xPos = aStartPos.iX;
			yPos++;
			}
		}
	CleanupStack::PopAndDestroy(&chunk);
	}
