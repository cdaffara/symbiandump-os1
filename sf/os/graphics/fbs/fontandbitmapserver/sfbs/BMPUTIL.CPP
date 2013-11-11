// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <fbs.h>
#include <graphics/lookuptable.h>
#include "fbsdefs.h"
#include "UTILS.H"

GLREF_C void Panic(TFbsPanic aPanic);

/* Macros used in support of EColor16M mode bitmaps in TBitmapUtil.
PTR2UINT, UINT2PTR defined to ease byte pointer arithmatic using
the existing iWordPos word sized pointer.
i MACROS redefine the meaning of members in the class for 16M use.
All in the name of Binary Compatibility - adding 16M support but
without wasting memory or increasing the size of this class's objects. */
#define PTR2UINT(_ptr)      reinterpret_cast<TUint32>(_ptr)
#define UINT2PTR(_expr)     reinterpret_cast<TUint32*>(_expr)
#define iNUM_WASTE_BYTES    iMask
#define iSTART_OF_SCAN_LINE iWordPos
#define iPIXEL_BYTE_OFFSET  iPixelShift
#define iPIXEL_ADDRESS      UINT2PTR(PTR2UINT(iWordPos)+iPixelShift)


/** Constructs a TBitmapUtil object for the specified bitmap. 

@note The use of extended or compressed bitmaps is not supported by TBitmapUtil.

@param aBitmap The bitmap on which this TBitmapUtil will operate. */
EXPORT_C TBitmapUtil::TBitmapUtil(CFbsBitmap* aBitmap):
    iFbsBitmap(aBitmap),	// Address of bitmap to navigate and modify.
	iWordPos(NULL),			// Non16M: Address of the word containing the bits
							//         of the current pixel.
							// 16M: Redefined to iSTART_OF_SCAN_LINE - word address
							//      of the scan line containing the current pixel.
	iMinWordPos(NULL),		// Member holds the word address of the top left pixel.
	iMaxWordPos(NULL),		// Member holds address of the word following the last
							// word in the last scan line of the bitmap.
	iBpp(0),				// Number of bits needed for each pixel's data.
	iPixelShift(0),			// Non16M: Right shift factor to convert pixel index
	                        //         to word index into scan line.
							// 16M: Redefined to iPIXEL_BYTE_OFFSET - index to first
							//      byte in pixel from the start of the scan line.
	iBitShift(0),			// Non16M: Changing index of right shift require to
	                        //         move pixel bits into LSB of word.
							// 16M: Not used, always 0
	iMask(0),               // Non16M: Mask of LSB required to hold
	                        //         pixel data eg. 16cols 0x0f
							// 16M: Redefined to iNUM_WASTE_BYTES - number of
							//      padding bytes at end of scan line 0,3,6,9 as 16M
							//      scanlines are always multiples of 12 bytes.
	iScanlineWordLength(0), // Number of 4-byte words in bitmap scan line
	iWritten(EFalse)
	{
	ASSERT(iFbsBitmap);
	__ASSERT_DEBUG(iFbsBitmap->Header().iCompression == ENoBitmapCompression, Panic(EFbsPanicInvalidBitmapType));
	}

/** Sets the current pixel position to the specified position and prepares
the bitmap for access to its pixel data.

This function must be called before using any of the other functions provided
by this class, otherwise damage to data may occur. It must also be matched
by a subsequent call to End().

@pre The bitmap which was passed on class construction has been created and holds 
	a valid handle.
@param aPosition The position to which the current pixel position should be
set. */
EXPORT_C void TBitmapUtil::Begin(const TPoint& aPosition)
	{
	iFbsBitmap->BeginDataAccess();
	TSize bmpsize(iFbsBitmap->SizeInPixels());
	ASSERT(bmpsize.iWidth > 0 && bmpsize.iHeight > 0);

	iWritten = EFalse;

	TDisplayMode dm=iFbsBitmap->DisplayMode();
	TInt numBytesInScanline = CFbsBitmap::ScanLineLength(bmpsize.iWidth,dm);
	switch(dm)
		{
	case EGray2:
		iMask=1;
		iPixelShift=5;
		iBpp=1;
		break;
	case EGray4:
		iMask=3;
		iPixelShift=4;
		iBpp=2;
		break;
	case EGray16:
	case EColor16:
		iMask=0xf;
		iPixelShift=3;
		iBpp=4;
		break;
	case EGray256:
	case EColor256:
		iMask=0xff;
		iPixelShift=2;
		iBpp=8;
		break;
	case EColor4K:
	case EColor64K:
		iMask=0xffff;
		iPixelShift=1;
		iBpp=16;
		break;
	/* Bit twiddling not used in the EColor16M, members redefined for
     different use.	 */
	case EColor16M:
		iNUM_WASTE_BYTES = numBytesInScanline-(bmpsize.iWidth*3);
		iPIXEL_BYTE_OFFSET=0;
		iBpp=24; // Indicates 16M display mode, flag for member functions
		         // to switch functionality.
		break;
	case EColor16MU:
	case EColor16MA:
	case EColor16MAP:
		iMask=0xffffffff;
		iPixelShift=0;
		iBpp=32;
		break;
	default: //shouldn't reach this point
		Panic(EFbsBitmapInvalidMode);
		}

	iMinWordPos=(TUint32*)iFbsBitmap->DataAddress();
	iScanlineWordLength = numBytesInScanline / 4;
	iMaxWordPos = iMinWordPos + (bmpsize.iHeight * iScanlineWordLength);

	TPoint correct(aPosition);
	if(correct.iX<0) correct.iX=0;
	if(correct.iY<0) correct.iY=0;
	if(correct.iX>=bmpsize.iWidth) correct.iX=bmpsize.iWidth;
	if(correct.iY>=bmpsize.iHeight) correct.iY=bmpsize.iHeight;
	SetPos(correct);
	}

/** Sets the current pixel position to the specified position and prepares
the bitmap for access to its pixel data.

This function is deprecated. Use the other overload, Begin(const TPoint&).
@deprecated

@param aPosition The position to which the current pixel position should be
set.
@param aUtil Not used.
@panic FBCLI 10 The bitmap's display mode is ERgb or ENone. */
EXPORT_C void TBitmapUtil::Begin(const TPoint& aPosition, const TBitmapUtil& /*aUtil*/)
	{
	Begin(aPosition);
	}

/** Marks the end of the access to the bitmap data.
This function must be called when finished using all the other functions
provided by this class.

There must be exactly one call to End() for each previous call to Begin()
for a given TBitmapUtil object. */
EXPORT_C void TBitmapUtil::End()
	{
	iFbsBitmap->EndDataAccess(!iWritten);
	}



/** Gets the value of the pixel at the current pixel position.

In normal usage, the value returned will immediately be used as the parameter
to a call to SetPixel(). To remove the overhead of constructing a TRgb, the
function uses a TUint32 rather than a TRgb to hold an RGB value.

@return The value of the pixel at the current pixel position. */
EXPORT_C TUint32 TBitmapUtil::GetPixel() const
	{
	if (iBpp!=24)
		// 1,2,4,8,12,16,32-bit pixels
		return((*iWordPos>>iBitShift)&iMask);
	else
		{
		// 24-bit pixels
		// Due to machine word alignment requirement on ARM, need to copy
		// pixel data at iWordPos a byte at a time if address is not aligned
		// on a 4 byte boundary i.e. 1st or 2nd least sig. bits set.
		if (iPIXEL_BYTE_OFFSET & 0x3)
			{
			TUint32 pixel = 0;
			Mem::Copy(&pixel, iPIXEL_ADDRESS, 3);
			return pixel;
			}
		else
			return (*( iPIXEL_ADDRESS )&0x00ffffff);
		}
	}

/** Sets the value of the pixel at the current pixel position.

To remove the overhead of constructing a TRgb, the function uses a TUint32
rather than a TRgb to hold an RGB value.

@param aValue The value to which the pixel at the current pixel position is
to be set. */
EXPORT_C void TBitmapUtil::SetPixel(TUint32 aValue)
	{
	iWritten = ETrue;
	if (iBpp!=24)
		{
		// 1,2,4,8,12,16,32-bit pixels
		*iWordPos&=~(iMask<<iBitShift);
		*iWordPos|=(aValue&iMask)<<iBitShift;
		if(iBpp==32 && iFbsBitmap && iFbsBitmap->DisplayMode() == EColor16MU)
				{
				*iWordPos|= 0xff000000;
				}
		}
	else
		{
		// 24-bit pixels
		// Due to machine word alignment requirement on ARM, need to copy
		// pixel data at iWordPos a byte at a time if address is not aligned
		// on a 4 byte boundary i.e. 1st or 2nd least sig. bits set.
		if (iPIXEL_BYTE_OFFSET & 0x3)
			Mem::Copy(iPIXEL_ADDRESS, &aValue, 3);
		else
			{
			*(iPIXEL_ADDRESS) &= ~0xffffff;
			*(iPIXEL_ADDRESS) |= (aValue&0x00ffffff);
			}
		}
	}

/** Sets the value of the pixel at the current pixel position to that returned
by aSource.GetPixel().

@param aSource The TBitmapUtil of the source bitmap, whose pixel value is
used to set the pixel at the current pixel position. */
EXPORT_C void TBitmapUtil::SetPixel(const TBitmapUtil& aSource)
	{
	SetPixel(aSource.GetPixel());
	}

/** Sets a new current pixel position.

@param aPosition The new current pixel position. */
EXPORT_C void TBitmapUtil::SetPos(const TPoint& aPosition)
	{
	TSize bmpsize(iFbsBitmap->SizeInPixels());
	TPoint correct(aPosition);

	correct.iX%=bmpsize.iWidth;
	correct.iY%=bmpsize.iHeight;
	if(correct.iX<0) correct.iX+=bmpsize.iWidth;
	if(correct.iY<0) correct.iY+=bmpsize.iHeight;

	iWordPos=iMinWordPos;
	iWordPos+=iScanlineWordLength*correct.iY;
	if (iBpp!=24)
		{
		// 1,2,4,8,12,16,32-bit pixels
		iWordPos+=correct.iX>>iPixelShift;
		iBitShift=(correct.iX*iBpp)%32;
		}
	else
		{
		// 24-bit pixels
		// iSTART_OF_SCAN_LINE (iWordPos) has correct value at this point
		// iBitShift not used in 16M modes
		iPIXEL_BYTE_OFFSET = correct.iX*3;	// Byte offset in iY scan line to iX pixel
		}
}

/** Decrements the x co-ordinate of the current pixel position. */
EXPORT_C void TBitmapUtil::DecXPos()
	{
	if (iBpp!=24)
		{
		// 1,2,4,8,12,16,32-bit pixels
		iBitShift-=iBpp;
		if(iBitShift<0)
			{
			iBitShift+=32;
			iWordPos--;
			if(iWordPos<iMinWordPos) iWordPos=iMaxWordPos-1;
			}
		}
	else
		{
		// 24-bit pixels
		iPIXEL_BYTE_OFFSET -= 3;

		// Scanline underflow?
		if (iPIXEL_BYTE_OFFSET < 0)
			{
			iPIXEL_BYTE_OFFSET = (iScanlineWordLength*4)-iNUM_WASTE_BYTES-3;

			// Bitmap underflow?
		    if (iSTART_OF_SCAN_LINE == iMinWordPos)
				iSTART_OF_SCAN_LINE = iMaxWordPos-iScanlineWordLength;
			else
		        iSTART_OF_SCAN_LINE -= iScanlineWordLength;
			}
		}
	}

/** Decrements the y co-ordinate of the current pixel position. */
EXPORT_C void TBitmapUtil::DecYPos()
	{
	iWordPos-=iScanlineWordLength;
	if(iWordPos<iMinWordPos) iWordPos+=iMaxWordPos-iMinWordPos;
	}

/** Increments the x co-ordinate of the current pixel position. */
EXPORT_C void TBitmapUtil::IncXPos()
	{
	if (iBpp!=24)
		{
		// 1,2,4,8,12,16,32-bit pixels
		iBitShift+=iBpp;
		if(iBitShift>=32)
			{
			iBitShift=0;
			iWordPos++;
			if(iWordPos>=iMaxWordPos) iWordPos=iMinWordPos;
			}
		}
	else
		{
		// 24-bit pixels
		iPIXEL_BYTE_OFFSET += 3;

		// Scanline overflow?
		if (iPIXEL_BYTE_OFFSET >=
				(iScanlineWordLength*4)-static_cast<TInt>(iNUM_WASTE_BYTES))
			{
			iPIXEL_BYTE_OFFSET = 0;
			iSTART_OF_SCAN_LINE += iScanlineWordLength;

			// Bitmap overflow?
			if (iSTART_OF_SCAN_LINE >= iMaxWordPos)
				iSTART_OF_SCAN_LINE = iMinWordPos;
			}
		}
	}

/** Increments the y co-ordinate of the current pixel position. */
EXPORT_C void TBitmapUtil::IncYPos()
	{
	iWordPos+=iScanlineWordLength;
	if(iWordPos>=iMaxWordPos) iWordPos-=iMaxWordPos-iMinWordPos;
	}



//
// CDitherColor256
//

class TDitherColorError
	{
public:
	inline TDitherColorError() {};
	inline TDitherColorError(TInt aRed,TInt aGreen,TInt aBlue) : iRed(aRed), iGreen(aGreen), iBlue(aBlue) {}
public:
	TInt iRed;
	TInt iGreen;
	TInt iBlue;
	};

NONSHARABLE_CLASS(CDitherColor256) : public CBase
	{
public:
	void ConstructL(TInt aLineWidth,const TColor256Util* aColorUtil = NULL);
	void Reset();
	void ConvertLine(TUint8* aDestination,TRgb* aSource);
	~CDitherColor256();
private:
	const TColor256Util* iColorUtil;
	TInt iLineWidth;
	TDitherColorError* iErrorBuffer;
	TInt iErrorBufferLength;
	};

CDitherColor256::~CDitherColor256()
	{
	delete[] iErrorBuffer;
	}

void CDitherColor256::ConstructL(TInt aLineWidth,const TColor256Util* aColorUtil)
	{
	if(iErrorBufferLength<=aLineWidth)
		{
		delete[] iErrorBuffer;
		iErrorBuffer = NULL;
		}

	if(!iErrorBuffer)
		{
		iErrorBuffer = new (ELeave) TDitherColorError[aLineWidth+1];
		iErrorBufferLength = aLineWidth+1;
		}

	iLineWidth = aLineWidth;
	if(!aColorUtil)
		aColorUtil = TColor256Util::Default();
	iColorUtil = aColorUtil;

	Reset();
	}

void CDitherColor256::Reset()
	{
	Mem::FillZ(iErrorBuffer,iErrorBufferLength*sizeof(iErrorBuffer[0]));
	}

void CDitherColor256::ConvertLine(TUint8* aDestination,TRgb* aSource)
	{
	TUint8* dstLimit = aDestination+iLineWidth;
	TDitherColorError* errorBuffer = iErrorBuffer;

	TInt r = errorBuffer->iRed;
	TInt g = errorBuffer->iGreen;
	TInt b = errorBuffer->iBlue;
	errorBuffer->iRed = 0;
	errorBuffer->iGreen = 0;
	errorBuffer->iBlue = 0;

	while(aDestination<dstLimit)
		{

		{
		TInt value = 0;

		r += aSource->Red();
		if(r>255)
			value = 255;
		else if(r>0)
			value = r;

		g += aSource->Green();
		if(g>255)
			value |= 255<<8;
		else if(g>0)
			value |= g<<8;

		b += aSource->Blue();
		if(b>255)
			value |= 255<<16;
		else if(b>0)
			value |= b<<16;

		aSource++;

		value = iColorUtil->Color256(TRgb(value));
		*(aDestination) = (TUint8)value;
		value = iColorUtil->Color256(value).Value();

		r -= value&0xFF;
		g -= (value>>8)&0xFF;
		b -= (value>>16)&0xFF;
		}

		{
		TInt x = r>>2;
		r = (r>>1)-(r>>3);
		errorBuffer->iRed += r;
		r += errorBuffer[1].iRed;
		errorBuffer[1].iRed = x;

		x = g>>2;
		g = (g>>1)-(g>>3);
		errorBuffer->iGreen += g;
		g += errorBuffer[1].iGreen;
		errorBuffer[1].iGreen = x;

		x = b>>2;
		b = (b>>1)-(b>>3);
		errorBuffer->iBlue += b;
		b += errorBuffer[1].iBlue;
		errorBuffer[1].iBlue = x;

		errorBuffer++;
		}

		aDestination++;
		}
	}

//
// CFbsColor256BitmapUtil
//

inline CFbsColor256BitmapUtil::CFbsColor256BitmapUtil()
	{ }

/** Allocates and constructs a new 256 colour bitmap utility object, optionally
specifying a palette.

@param aPalette The palette of colours to use for the destination 256 colour
bitmap. If there are more than 256 colours in the palette, only the first
256 are used. May be NULL, in which case the system default 256 colour palette
is used.
@return The newly constructed object. */
EXPORT_C CFbsColor256BitmapUtil* CFbsColor256BitmapUtil::NewL(const CPalette* aPalette)
	{
	CFbsColor256BitmapUtil* self = new (ELeave) CFbsColor256BitmapUtil;
	CleanupStack::PushL(self);
	if(aPalette)
		{
		self->iColor256Util = new (ELeave) TColor256Util;
		self->iColor256Util->Construct(*aPalette);
		}
	// coverity[leave_without_push : FALSE]
	// class member variables should not be pushed onto the cleanupstack 
	self->iDither = new (ELeave) CDitherColor256;
	CleanupStack::Pop();
	return self;
	}

CFbsColor256BitmapUtil::~CFbsColor256BitmapUtil()
	{
	delete iColor256Util;
	delete iDither;
	}

/** Copies the contents of a source bitmap to a destination 256 colour bitmap.
Both bitmaps must be the same size and the destination bitmap's display mode
must be EColor256.

@param aColor256Destination The destination to copy the bitmap to. Its display
mode must be EColor256.
@param aSource The source bitmap to copy from. Its display mode can be EColor16M,
EColor64K, EColor4K or EColor256.
@param aDither Specify EFloydSteinberg for Floyd-Steinberg dithering. By default,
no dithering.
@return KErrNone, if successful; otherwise, another of the system-wide error
codes. KErrNotSupported is returned if aSource's display mode is not one of
the values listed.
@panic EFbsColor256UtilError Either aColor256Destination's display mode is
not EColor256, or the two bitmaps are not the same size. */
EXPORT_C TInt CFbsColor256BitmapUtil::CopyBitmap(CFbsBitmap* aColor256Destination,CFbsBitmap* aSource,TDither aDither)
	{
	aSource->BeginDataAccess();
	aColor256Destination->BeginDataAccess();
	const TDisplayMode sourceMode = aSource->DisplayMode();

	TSize size(aSource->SizeInPixels());
	TInt width = size.iWidth;
	TInt height = size.iHeight;

	// Check we are being askes to copy a bitmap to a color256 bitmap of the same size
	if( aColor256Destination->DisplayMode()!=EColor256 || aColor256Destination->SizeInPixels()!=size )
		Panic(EFbsColor256UtilError);

	if( sourceMode!=EColor256 && sourceMode!=EColor4K && sourceMode!=EColor64K &&
		sourceMode!=EColor16M && sourceMode!=EColor16MU && sourceMode!=EColor16MA && sourceMode!=EColor16MAP)
		{
		aSource->EndDataAccess(ETrue);
		aColor256Destination->EndDataAccess(ETrue);
		return KErrNotSupported;
		}

	// get a TColor256Utils to use
	const TColor256Util* colorUtil = iColor256Util;
	if(!colorUtil)
		colorUtil = TColor256Util::Default();

	if(aDither==EFloydSteinberg)
		{
		TRAPD(err,iDither->ConstructL(width,colorUtil));
		if(err!=KErrNone)
			{
			aSource->EndDataAccess(ETrue);
			aColor256Destination->EndDataAccess(ETrue);
			return err;
			}
		}

	TRgb* pixelBuffer = new TRgb[width];
	if(!pixelBuffer)
		{
		aSource->EndDataAccess(ETrue);
		aColor256Destination->EndDataAccess(ETrue);
		return KErrNoMemory;
		}

	TUint8* src = (TUint8*)aSource->DataAddress();
	TInt srcPitch = aSource->ScanLineLength(width,sourceMode);
	TUint8* dst = (TUint8*)aColor256Destination->DataAddress();
	TInt dstPitch = aColor256Destination->ScanLineLength(width,EColor256);

	while(height--)
		{
		TInt i(0);
		switch(sourceMode)
			{
			case EColor256:
				for(i=0; i<width; i++)
					pixelBuffer[i] = TRgb::Color256(((TUint8*)src)[i]);
				break;
			case EColor4K:
				for(i=0; i<width; i++)
					pixelBuffer[i] = TRgb::_Color4K(((TUint16*)src)[i]);
				break;
			case EColor64K:
				for(i=0; i<width; i++)
					pixelBuffer[i] = TRgb::_Color64K(((TUint16*)src)[i]);
				break;
			case EColor16M:
				{
				TUint8* data = src;
				TUint8* dataLimit = data+width*3;
				i = 0;
				while(data<dataLimit)
					{
					TUint32 value = *(data++)<<16;
					value += *(data++)<<8;
					value += *(data++);
					pixelBuffer[i++] = TRgb(value);
					}
				}
				break;
			case EColor16MU:
				{
				TInt32* data = reinterpret_cast<TInt32*> (src);
				TInt32* dataLimit = data+width;
				i = 0;
				while(data<dataLimit)
					{
					pixelBuffer[i++] = TRgb::_Color16MU(*data++);
					}
				}
				break;
			case EColor16MA:
				{
				TInt32* data = reinterpret_cast<TInt32*> (src);
				TInt32* dataLimit = data+width;
				i = 0;
				while(data<dataLimit)
					{
					pixelBuffer[i++] = TRgb::_Color16MA(*data++);
					}
				}
				break;
			case EColor16MAP:
				{
				// perform division of color channels by alpha.
				TInt32* data = reinterpret_cast<TInt32*> (src);
				TInt32* dataLimit = data+width;
				while(data<dataLimit)
					{
					pixelBuffer[i++] = TRgb::_Color16MAP(*data++);
					}
				}
				break;
			default:
				break;
			}
		if(aDither==EFloydSteinberg)
			iDither->ConvertLine(dst,pixelBuffer);
		else
			colorUtil->Color256(dst,pixelBuffer,width);
		dst += dstPitch;
		src += srcPitch;
		}

	aSource->EndDataAccess(ETrue);
	aColor256Destination->EndDataAccess(EFalse);

	delete[] pixelBuffer;
	return KErrNone;
	}

/**

Required to ensure BC between NGage and 7.0S platforms.
Function is exported at ordinal corresponding to where NGage platform
has extended this library and must not be moved. */
EXPORT_C void DummyReserved1()
	{
	User::Panic(_L("Dummy Function"), 0);
	}


