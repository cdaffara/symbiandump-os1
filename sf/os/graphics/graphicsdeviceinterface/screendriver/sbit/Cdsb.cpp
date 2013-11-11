// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cdsb.h>
#include <e32hal.h>

#ifdef SYMBIAN_GRAPHICS_GCE
#undef __WINS__
#include "../sgeneric/scdraw.h"
#elif defined(__WINS__)
#include "_WININC.H"
#endif


//
//	CGenericDirectScreenBitmap: Implementation of generic CDirectScreenBitmap class
//
NONSHARABLE_CLASS(CGenericDirectScreenBitmap) : public CDirectScreenBitmap
	{
public:
	 CGenericDirectScreenBitmap();
	 CGenericDirectScreenBitmap(TInt aScreenNo);
	~CGenericDirectScreenBitmap();

public:
	// Pure virtual from CDirectScreenBitmap
	virtual TInt Create(const TRect& aScreenRect, TSettingsFlags aSettingsFlags);
	virtual TInt BeginUpdate(TAcceleratedBitmapInfo& aBitmapInfo);
	virtual void EndUpdate(TRequestStatus& aComplete);
	virtual void EndUpdate(const TRect& aScreenRect, TRequestStatus& aComplete);
	virtual void Close();

public:
	enum TDsbPanic
		{
		EPanicAlreadyCreated = 1,
		EPanicNotCreated     = 2,
		EPanicInvalidMode    = 3,
		EPanicOutOfBounds	 = 4,
		EPanicNotWordAligned = 5,
		EPanicInvalidRect    = 6,
		EPanicIncompleteRequest = 7,
		EPanicHandleReturnedIncorrect = 8
		};
    static void Panic(TDsbPanic aPanic);

private:
	TInt DoCreate();

#ifdef __WINS__
	TRgb ExtractRgb(TUint8* aBuffer,TInt aPixelOffset) const;
	void WinsUpdate(const TRect& aScreenRect);
#endif

private:
#ifndef __WINS__
	TInt    iVideoAddress;
#endif
#ifdef SYMBIAN_GRAPHICS_GCE
    CScreenDeviceHelper iSurfaceUpdater;
    TUidPixelFormat iPixelFormat;
#endif
	TAcceleratedBitmapInfo iBitmapInfo;
	TUint32 iSettingsFlags;
	TRect   iUpdateRect;
	TInt    iDisplayOffsetLines;
	TInt    iBytesPerPixel;
	TInt	iDisplayOffsetPixel;
	TBool   iCreated;
    TInt    iScreenNo;
    RChunk 	iChunk;
    TBool 	iHasChunk;
	};

//
// Create a new instance of a DSB
// The default screen (with number 0) will be used
EXPORT_C CDirectScreenBitmap* CDirectScreenBitmap::NewL()
	{
	CGenericDirectScreenBitmap* pDsb=new(ELeave) CGenericDirectScreenBitmap;
	return(pDsb);
	}

//
// Create a new instance of a DSB
// The screen with aScreenNo number will be used
EXPORT_C CDirectScreenBitmap* CDirectScreenBitmap::NewL(TInt aScreenNo)
    {
	CGenericDirectScreenBitmap* pDsb = new (ELeave) CGenericDirectScreenBitmap(aScreenNo);
	return pDsb;
    }

CGenericDirectScreenBitmap::CGenericDirectScreenBitmap() :
#ifndef __WINS__
	iVideoAddress(NULL),
#endif
	iBitmapInfo(),
	iSettingsFlags(ENone),
	iUpdateRect(),
	iDisplayOffsetLines(0),
	iBytesPerPixel(0),
	iDisplayOffsetPixel(0),
	iHasChunk(EFalse)
	{
    }

CGenericDirectScreenBitmap::CGenericDirectScreenBitmap(TInt aScreenNo) :
    iScreenNo(aScreenNo)
    {
    }

CGenericDirectScreenBitmap::~CGenericDirectScreenBitmap()
	{
	Close();
	}

//
// Create a DSB object using a region of the screen previously
// 'claimed' via the Window Servers Direct Screen Acceass API
//
TInt CGenericDirectScreenBitmap::Create(const TRect& aScreenRect, TSettingsFlags aSettingsFlags)
	{
	__ASSERT_ALWAYS(!iCreated, Panic(EPanicAlreadyCreated));

	__ASSERT_ALWAYS((aSettingsFlags&(EDoubleBuffer|EIncrementalUpdate)) !=
					(EDoubleBuffer|EIncrementalUpdate), Panic(EPanicInvalidMode));

    //Initialize iScreenNo - its value occupies upper 5 bits of aSettingsFlags.
    //(32 - 1) is max allowed screen number
    iScreenNo = static_cast <TUint32> ((aSettingsFlags & 0xF8000000) >> 27);

	// Find the screen dimensions from the HAL and validate
	TInt screenWidth = 0;
	TInt screenHeight = 0;

	TInt r = HAL::Get(iScreenNo, HALData::EDisplayXPixels, screenWidth);
	if (r!=KErrNone)
		{
		return r;
		}

	r = HAL::Get(iScreenNo, HALData::EDisplayYPixels,screenHeight);
	if (r!=KErrNone)
		{
		return r;
		}

	__ASSERT_ALWAYS(aScreenRect.iTl.iX >= 0 &&
	                aScreenRect.iTl.iY >= 0 &&
	                ((aScreenRect.Width() <= screenWidth &&
	                aScreenRect.Height() <= screenHeight)
	                ||
	                (aScreenRect.Width() <= screenHeight &&
	                aScreenRect.Height() <= screenWidth)),
	                Panic(EPanicOutOfBounds));

	__ASSERT_ALWAYS(aScreenRect.Width() > 0 &&
					aScreenRect.Height() > 0, Panic(EPanicInvalidRect));

	iUpdateRect = aScreenRect;
	iSettingsFlags = aSettingsFlags;

	r = DoCreate();
	if(r != KErrNone)
		{
		return r;
		}

	// Check that LHS is word-aligned
	const TUint pixelStartInBytes = aScreenRect.iTl.iX*iBytesPerPixel;
	const TUint pixelEndInBytes = aScreenRect.iBr.iX*iBytesPerPixel;

	// Check the alignment
	__ASSERT_ALWAYS(!(pixelStartInBytes & 0x03), Panic(EPanicNotWordAligned));
	__ASSERT_ALWAYS(!(pixelEndInBytes & 0x03),	 Panic(EPanicNotWordAligned));

	const TUint scanlineBytes = pixelEndInBytes-pixelStartInBytes;

#ifndef __WINS__	
	iVideoAddress += pixelStartInBytes +						// X Offset
						 iUpdateRect.iTl.iY*iDisplayOffsetLines +	// Y Offset
						 iDisplayOffsetPixel;						 // Pixel (Palette) Offset
#endif 
	
	// Since this is a generic implementation, we only require one
	//  buffer for both incremental and double buffered mode
	iBitmapInfo.iAddress = (TUint8*)User::Alloc(scanlineBytes * iUpdateRect.Height());
	if(!iBitmapInfo.iAddress)
		return KErrNoMemory;

	iBitmapInfo.iSize			 = TSize(iUpdateRect.Width(), iUpdateRect.Height());
	iBitmapInfo.iLinePitch		 = scanlineBytes;
	iBitmapInfo.iPhysicalAddress = 0;

	// All done
	iCreated = ETrue;

	return KErrNone;
	}

#ifndef __WINS__
//
// Attempt to initialise using the HAL
//
TInt CGenericDirectScreenBitmap::DoCreate()
	{
    // For GCE, if multiple modes are supported, only one needs to be returned.
	// On the emulator, what is returned via EDisplayMode can be set via ColorDepth setting in epoc.ini
	// On the hardware, 24/32bpp color mode is returned via EDisplayMode as pre-defined. 
	TInt displayMode = 0;
	TInt r1 = HAL::Get(iScreenNo, HALData::EDisplayMode, displayMode);
	if (r1!=KErrNone)
		{
		return r1;
		}

	TInt bitsPerPixel = displayMode;

	r1 = HAL::Get(iScreenNo, HALData::EDisplayBitsPerPixel, bitsPerPixel);
	if (r1!=KErrNone)
		{
		return r1;
		}
	iBytesPerPixel = (bitsPerPixel+7) >> 3;

	// Store the important driver parameters to avoid HAL calls when drawing
	iDisplayOffsetPixel = displayMode;
	r1 = HAL::Get(iScreenNo, HALData::EDisplayOffsetToFirstPixel, iDisplayOffsetPixel);
	if (r1!=KErrNone)
		{
		return r1;
		}
	iDisplayOffsetLines = displayMode;
	r1 = HAL::Get(iScreenNo, HALData::EDisplayOffsetBetweenLines, iDisplayOffsetLines);
	if (r1!=KErrNone)
		{
		return r1;
		}
	
	TInt val = 0;
	r1 = HAL::Get(iScreenNo, HALData::EDisplayMemoryHandle,val);
	if (r1 == KErrNotSupported)
		{
		// EDisplayMemoryHandle is not supported, use the address instead
		r1 = HAL::Get(iScreenNo, HALData::EDisplayMemoryAddress, iVideoAddress);
		if (r1!=KErrNone)
			{
			return r1;
			}
		}
	else if (r1 == KErrNone)
		{
		// EDisplayMemoryHandle is supported, use the handle base address
		__ASSERT_ALWAYS(val != 0, Panic(EPanicHandleReturnedIncorrect));
		RChunk chunk;
		r1 = chunk.SetReturnedHandle(val);
		if (r1 != KErrNone)
			{
			return r1;
			}
		iChunk = chunk;
		r1 = iChunk.Duplicate(RThread(), EOwnerProcess);
		chunk.Close();
		if (r1 != KErrNone)
			{
			return r1;
			}
		iVideoAddress = (TInt)iChunk.Base();
		iHasChunk = ETrue;
		}
	else  
		{
		return r1;
		}

	// The HAL doesn't specify explicit dislpay modes, so we need
	// to calculate the mode using bitsPerPixel and isMono
	TBool isMono = displayMode;
	r1 = HAL::Get(iScreenNo, HALData::EDisplayIsMono, isMono);
	if (r1!=KErrNone)
		{
		return r1;   
		}
     switch(bitsPerPixel)
		{
		// EGray2, EGray4, EGray16, EColor16 not supported on the hardware
		// The generic scdv support EGray2, EGray4, EGray16, EColor16 on the emulator
		// However, the low display modes should not be used.
		case 1:
			iBitmapInfo.iPixelShift = 0;
			iBitmapInfo.iDisplayMode = EGray2;
			iBytesPerPixel = 2;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatUnknown;
#endif
			break;
		case 2:
			iBitmapInfo.iPixelShift = 1;
			iBitmapInfo.iDisplayMode = EGray4;
			iBytesPerPixel = 2;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatUnknown;
#endif
			break;
		case 4:
			iBitmapInfo.iPixelShift = 2;
			iBitmapInfo.iDisplayMode = isMono ? EGray16 : EColor16;
			iBytesPerPixel = 2;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatUnknown;
#endif
			break;	 
		case 8:
			iBitmapInfo.iPixelShift = 3;
			iBitmapInfo.iDisplayMode = isMono ? EGray256 : EColor256;
			iBytesPerPixel = 2;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatUnknown;
#endif
			break;
		case 12:
			iBitmapInfo.iPixelShift = 4;
			iBitmapInfo.iDisplayMode = EColor4K;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatXRGB_4444;
#endif
			break;
		case 16:
			iBitmapInfo.iPixelShift = 4;
			iBitmapInfo.iDisplayMode = EColor64K;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatRGB_565;
#endif
			break;
		case 24:
		case 32:
			iBitmapInfo.iPixelShift = 5;
			iBitmapInfo.iDisplayMode = EColor16MA;
#ifdef SYMBIAN_GRAPHICS_GCE
			iPixelFormat = EUidPixelFormatARGB_8888;
			iBytesPerPixel = 4;
#endif
			break;
		default:
			return KErrNotSupported;
		}

#ifdef SYMBIAN_GRAPHICS_GCE
    TInt ret = iSurfaceUpdater.Construct(iScreenNo,iPixelFormat,displayMode);
 	if (ret != KErrNone)
 		{
 		return ret;
 		}
#endif

	return KErrNone;
	}
#endif

#ifdef __WINS__
//
// Attempt to initialise for WINS
// This is intended for use if HAL initialisation fails.
//
TInt CGenericDirectScreenBitmap::DoCreate()
	{
    RWindows* window = ::WindowHandler(iScreenNo);
    if(window)
		{
		iBitmapInfo.iDisplayMode = window->iDisplayMode;
		iDisplayOffsetLines		 = window->iEpocBitmapLinePitch;
		iDisplayOffsetPixel		 = 0;

		switch(iBitmapInfo.iDisplayMode)
			{
			case EGray2:
				iBitmapInfo.iPixelShift = 0;
				iBytesPerPixel = 1;
				break;
			case EGray4:
				iBitmapInfo.iPixelShift = 1;
				iBytesPerPixel = 1;
				break;
			case EGray16:
			case EColor16:
				iBitmapInfo.iPixelShift = 2;
				iBytesPerPixel = 1;
				break;
			case EGray256:
			case EColor256:
				iBitmapInfo.iPixelShift = 3;
				iBytesPerPixel = 1;
				break;
			case EColor4K:
			case EColor64K:
				iBitmapInfo.iPixelShift = 4;
				iBytesPerPixel = 2;
				break;
			case EColor16M:
				iBitmapInfo.iPixelShift = 5;
				iBytesPerPixel = 3;
				break;
			case ERgb:
			case EColor16MU:
			case EColor16MA:
			case EColor16MAP:
				iBitmapInfo.iPixelShift = 5;
				iBytesPerPixel = 4;
				break;
			default:
				return KErrNotSupported;
			}

		return KErrNone;
		}
	else
		{
		return KErrNoMemory;
		}
	}
#endif

//
// Begin update - aBitmapinfo returns the current bitmap
//
TInt CGenericDirectScreenBitmap::BeginUpdate(TAcceleratedBitmapInfo& aBitmapInfo)
	{
	__ASSERT_ALWAYS(iCreated, Panic(EPanicNotCreated));
	aBitmapInfo = iBitmapInfo;
	return KErrNone;
	}


#ifdef __WINS__

void CGenericDirectScreenBitmap::WinsUpdate(const TRect& aScreenRect)
	{
	const TUint dS = iBitmapInfo.iLinePitch;
	const TUint offY = aScreenRect.iTl.iY - iUpdateRect.iTl.iY;
	TUint8* pS = iBitmapInfo.iAddress + (offY*dS);
	RWindows* window = ::WindowHandler(iScreenNo);
	TInt orientation=window->Orientation();
	TRect orientedRect;
	TSize orientedSize;

	orientedSize = window->GetOrientedSize();
	orientedRect = window->GetOrientedRect(aScreenRect);

	TInt BrX = iUpdateRect.iBr.iX-1;
	TInt BrY = iUpdateRect.iBr.iY-1;

	for(TInt y=aScreenRect.iTl.iY; y < aScreenRect.iBr.iY; y++)
		{

		TRgb pixelColor;

		TInt xSource = aScreenRect.iTl.iX-iUpdateRect.iTl.iX;

		for(TInt x = aScreenRect.iTl.iX; x < aScreenRect.iBr.iX; x++)
			{
			TUint8* pD;

			switch(orientation)
				{
				case 0:
					pD = window->PixelAddress(x, y);
					break;
				case 1:
					pD = window->PixelAddress(y, BrX-x);
					break;
				case 2:
					pD = window->PixelAddress(BrX-x, BrY-y);
					break;
				case 3:
					pD = window->PixelAddress(BrY-y, x);
					break;
				default:
					break;
				}

			// Convert the user colour to RGB, as required by the WINS Emulator
			pixelColor = ExtractRgb(pS, xSource++);

			pD[0] = TUint8(pixelColor.Blue());
			pD[1] = TUint8(pixelColor.Green());
			pD[2] = TUint8(pixelColor.Red());

			}
		pS += dS;
		}

	RRegion region;
	region.AddRect(orientedRect);
	window->Update(region,orientedSize);
	region.Close();
	}

#endif

//
// End Update - Commit changes to the Frame Buffer
//
void CGenericDirectScreenBitmap::EndUpdate(TRequestStatus& aComplete)
	{
	__ASSERT_ALWAYS(iCreated, Panic(EPanicNotCreated));

	TUint8* pS = iBitmapInfo.iAddress;
	const TUint dS = iBitmapInfo.iLinePitch;
	
#ifdef __WINS__

	WinsUpdate(iUpdateRect);

#else
	TUint8* pD = (TUint8*)iVideoAddress;
	const TUint dD = iDisplayOffsetLines;

	for(TInt y=iUpdateRect.Height(); y>0; y--)
		{
		Mem::Move((void *)pD, (void *)pS, dS);
		pS += dS;
		pD += dD;
		}
#endif
	
#ifdef SYMBIAN_GRAPHICS_GCE
	// update the region and complete notification on the client's TRequestStatus
	// This means the backbuffer is held up until the surface update has completed
	// notification.
	iSurfaceUpdater.UpdateRegion(iUpdateRect);
	iSurfaceUpdater.Update(aComplete);
#else
	// In the generic implementation, complete the request immediately to allow the client
	// to render to the back buffer
	TRequestStatus* pComplete=&aComplete;
	User::RequestComplete(pComplete,KErrNone);
#endif

 	}

//
// End Update - Commit changes within the given area to the Frame Buffer
//
void CGenericDirectScreenBitmap::EndUpdate(const TRect& aScreenRect, TRequestStatus& aComplete)
	{
	__ASSERT_ALWAYS(iCreated, Panic(EPanicNotCreated));

	__ASSERT_ALWAYS(aScreenRect.iTl.iX >= iUpdateRect.iTl.iX &&
					aScreenRect.iTl.iY >= iUpdateRect.iTl.iY &&
					aScreenRect.iBr.iX <= iUpdateRect.iBr.iX &&
					aScreenRect.iBr.iY <= iUpdateRect.iBr.iY, Panic(EPanicOutOfBounds));

	__ASSERT_ALWAYS(aScreenRect.Width() > 0 &&
					aScreenRect.Height() > 0, Panic(EPanicInvalidRect));

	// We can only use this functionality in Incremental Update mode
	__ASSERT_ALWAYS(iSettingsFlags == CDirectScreenBitmap::EIncrementalUpdate, Panic(EPanicInvalidMode));

	const TUint offY = aScreenRect.iTl.iY - iUpdateRect.iTl.iY;

	const TUint dS = iBitmapInfo.iLinePitch;

#ifdef __WINS__

	WinsUpdate(aScreenRect);

#else
	
	TInt xLeft  = aScreenRect.iTl.iX*iBytesPerPixel;
	TInt xRight = aScreenRect.iBr.iX*iBytesPerPixel;

	// We have already tested the limits against iUpdateRect, which is
	// word aligned, so we are safe to perform a round up/down here
	// without fear of overrunning the buffer.

	xLeft &= ~0x03;
	xRight = (xRight+0x03)&~0x03;

	const TUint offX = xLeft - iUpdateRect.iTl.iX*iBytesPerPixel;

	const TUint bytesToCopy = xRight-xLeft;

	const TUint dD = iDisplayOffsetLines;

	TUint8* pS = iBitmapInfo.iAddress + offX + (offY*dS);
	TUint8* pD = (TUint8*)iVideoAddress + offX + (offY*dD);

	for(TInt y=aScreenRect.Height(); y>0; y--)
		{
		Mem::Move((void *)pD, (void *)pS, bytesToCopy);
		pS += dS;
		pD += dD;
		}
#endif

#ifdef SYMBIAN_GRAPHICS_GCE
	// update the region and complete notification on the client's TRequestStatus
	// This means the backbuffer is held up until the surface update has completed
	// notification.
	iSurfaceUpdater.UpdateRegion(iUpdateRect);
	iSurfaceUpdater.Update(aComplete);
#else
	// In the generic implementation, complete the request immediately to allow the client
	// to render to the back buffer
	TRequestStatus* pComplete=&aComplete;
	User::RequestComplete(pComplete,KErrNone);
#endif	

	}

#ifdef __WINS__
//
// Extracts the RGB value of the specidied pixel from the buffer
//
TRgb CGenericDirectScreenBitmap::ExtractRgb(TUint8* aBuffer,TInt aPixelOffset) const
	{
	switch (iBitmapInfo.iDisplayMode)
		{
	case EGray2:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 3));
		if (byte & (1 << (aPixelOffset & 7)))
			return KRgbWhite;
		return KRgbBlack;
		}
	case EGray4:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 2));
		byte >>= ((aPixelOffset & 3) << 1);
		return TRgb::_Gray4(byte & 3);
		}
	case EGray16:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 1));
		if (aPixelOffset & 1)
			byte >>= 4;
		return TRgb::_Gray16(byte & 0xf);
		}
	case EGray256:
		return TRgb::_Gray256(*(aBuffer + aPixelOffset));
	case EColor16:
		{
		TUint8 byte = *(aBuffer + (aPixelOffset >> 1));
		if (aPixelOffset & 1)
			byte >>= 4;
		return TRgb::Color16(byte & 0xf);
		}
	case EColor256:
		return TRgb::Color256(*(aBuffer + aPixelOffset));
	case EColor4K:
		{
		TUint16 doubleByte = *(((TUint16*)aBuffer) + aPixelOffset);
		return TRgb::_Color4K(doubleByte & 0xfff);
		}
	case EColor64K:
		{
		TUint16 doubleByte = *(((TUint16*)aBuffer) + aPixelOffset);
		return TRgb::_Color64K(doubleByte);
		}
	case EColor16M:
		{
		aBuffer += aPixelOffset * 3;
		TInt value = *aBuffer++;
		value |= *aBuffer++ << 8;
		value |= *aBuffer << 16;
		return TRgb::_Color16M(value);
		}
	case ERgb:
		return *(((TRgb*)aBuffer) + aPixelOffset);
	case EColor16MU:
		{
		aBuffer += aPixelOffset * 4;
		TInt value = *aBuffer++;
		value |= *aBuffer++ << 8;
		value |= *aBuffer << 16;
		return TRgb::_Color16MU(value);
		}
	case EColor16MA:
		{
		aBuffer += aPixelOffset * 4;
		TInt value = *aBuffer++;
		value |= *aBuffer++ << 8;
		value |= *aBuffer++ << 16;
		value |= *aBuffer << 24;
		return TRgb::_Color16MA(value);
		}
	case EColor16MAP:
		{
		aBuffer += aPixelOffset * 4;
		TInt value = *aBuffer++;
		value |= *aBuffer++ << 8;
		value |= *aBuffer++ << 16;
		value |= *aBuffer << 24;
		return TRgb::_Color16MAP(value);
		}	
	default:
		break;
		};
	return KRgbBlack;
	}
#endif


//
// Close - Deallocate resources and cancel outstanding updates
//
void CGenericDirectScreenBitmap::Close()
	{
	if(iCreated)
		{
#ifdef SYMBIAN_GRAPHICS_GCE
		iSurfaceUpdater.CancelUpdateNotification();
#endif
		User::Free(iBitmapInfo.iAddress);
		iBitmapInfo.iAddress = NULL;
		iCreated = EFalse;
		}
	if (iHasChunk)
		{
		iChunk.Close();
		}
	}

//
// Panic the user if a serious error occurs
//
void CGenericDirectScreenBitmap::Panic(TDsbPanic aPanic)
	{
	_LIT( KPanicString, "CGenericDirectScreenBitmap" );
	User::Panic( KPanicString, aPanic );
	}


