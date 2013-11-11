// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sgutils.h"


// Helper function used to transfer a scanline in 64K to the various formats of the destination.
LOCAL_C void TransferScanLine64K(TUint8* aDest, const TUint16* aSrc, TUidPixelFormat aPixelFormatDest, TInt aPixelLength)
	{
	switch (aPixelFormatDest)
		{
	case EUidPixelFormatRGB_565:
		{
		Mem::Copy(aDest, aSrc, aPixelLength << 1);
		break;
		}
	case EUidPixelFormatARGB_8888_PRE:
		{
		TUint32* dest = reinterpret_cast<TUint32*>(aDest);
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = TRgb::Color64K(*aSrc++).Color16MAP();
			}
		break;
		}
	case EUidPixelFormatARGB_8888:
		{
		TUint32* dest = reinterpret_cast<TUint32*>(aDest);
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = TRgb::Color64K(*aSrc++).Color16MA();
			}
		break;
		}
	case EUidPixelFormatXRGB_8888:
		{
		TUint32* dest = reinterpret_cast<TUint32*>(aDest);
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = (TRgb::Color64K(*aSrc++).Color16MU() | 0xff000000);
			}
		break;
		}
	case EUidPixelFormatRGB_888:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			TInt col = TRgb::Color64K(*aSrc++).Color16M();
			*aDest++ = col;
			*aDest++ = col >> 8;
			*aDest++ = col >> 16;
			}
		break;
		}
	case EUidPixelFormatXRGB_4444:
		{
		TUint16* dest = reinterpret_cast<TUint16*>(aDest);
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = TRgb::Color64K(*aSrc++).Color4K();
			}
		break;
		}
	case EUidPixelFormatP_8:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest++ = TRgb::Color64K(*aSrc++).Color256();
			}
		break;
		}
	case EUidPixelFormatL_8:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest++ = TRgb::Color64K(*aSrc++).Gray256();
			}
		break;
		}
	case EUidPixelFormatL_4:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index+= 4)
				{
				TInt col = TRgb::Color64K(*aSrc++).Gray16();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	case EUidPixelFormatP_4:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index+= 4)
				{
				TInt col = TRgb::Color64K(*aSrc++).Color16();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	case EUidPixelFormatL_2:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index+= 2)
				{
				TInt col = TRgb::Color64K(*aSrc++).Gray4();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	case EUidPixelFormatL_1:
		{
		const TUint16* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index++)
				{
				TInt col = TRgb::Color64K(*aSrc++).Gray2();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	default:
		break;
		}
	}

// Helper function used to transfer a scanline in 16MAP to the various formats of the destination.
LOCAL_C void TransferScanLine16MAP(TUint8* aDest, const TUint32* aSrc, TUidPixelFormat aPixelFormatDest, TInt aPixelLength)
	{
	switch (aPixelFormatDest)
		{
	case EUidPixelFormatARGB_8888_PRE:
		{
		Mem::Copy(aDest, aSrc, aPixelLength << 2);
		break;
		}
	case EUidPixelFormatARGB_8888:
		{
		TUint32* dest = reinterpret_cast<TUint32*>(aDest);
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = TRgb::Color16MAP(*aSrc++).Color16MA();
			}
		break;
		}
	case EUidPixelFormatXRGB_8888:
		{
		TUint32* dest = reinterpret_cast<TUint32*>(aDest);
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = (TRgb::Color16MAP(*aSrc++).Color16MU() | 0xff000000);
			}
		break;
		}
	case EUidPixelFormatRGB_888:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			TInt col = TRgb::Color16MAP(*aSrc++).Color16M();
			*aDest++ = col;
			*aDest++ = col >> 8;
			*aDest++ = col >> 16;
			}
		break;
		}
	case EUidPixelFormatRGB_565:
		{
		TUint16* dest = reinterpret_cast<TUint16*>(aDest);
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = TRgb::Color16MAP(*aSrc++).Color64K();
			}
		break;
		}
	case EUidPixelFormatXRGB_4444:
		{
		TUint16* dest = reinterpret_cast<TUint16*>(aDest);
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*dest++ = TRgb::Color16MAP(*aSrc++).Color4K();
			}
		break;
		}
	case EUidPixelFormatP_8:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest++ = TRgb::Color16MAP(*aSrc++).Color256();
			}
		break;
		}
	case EUidPixelFormatL_8:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest++ = TRgb::Color16MAP(*aSrc++).Gray256();
			}
		break;
		}
	case EUidPixelFormatL_4:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index+= 4)
				{
				TInt col = TRgb::Color16MAP(*aSrc++).Gray16();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	case EUidPixelFormatP_4:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index+= 4)
				{
				TInt col = TRgb::Color16MAP(*aSrc++).Color16();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	case EUidPixelFormatL_2:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index+= 2)
				{
				TInt col = TRgb::Color16MAP(*aSrc++).Gray4();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		break;
		}
	case EUidPixelFormatL_1:
		{
		const TUint32* srcEnd = aSrc + aPixelLength;
		while (aSrc < srcEnd)
			{
			*aDest = 0;
			for (TInt index = 0; (index < 8) && (aSrc < srcEnd); index++)
				{
				TInt col = TRgb::Color16MAP(*aSrc++).Gray2();
				col <<= index;
				*aDest |= col;
				}
			aDest++;
			}
		}
	default:
		break;
		}
	}

// Helper function used to copy a buffer in 64K to the various formats of the destination.
LOCAL_C TInt TransferPixels64K(TAny* aDataAddressDest, TInt aDataStrideDest, TUidPixelFormat aPixelFormatDest,
                               const TUint16* aDataAddressSrc, TInt aDataStrideSrc, const TRect& aRectSource)
	{
	TInt err = KErrNone;
	const TInt width = aRectSource.Width();
	const TUint16* dataAddressSrc = PtrAdd(aDataAddressSrc + aRectSource.iTl.iX, aRectSource.iTl.iY * aDataStrideSrc);
	const TUint16* dataAddressSrcEnd = PtrAdd(dataAddressSrc, aDataStrideSrc * aRectSource.Height());

	switch (aPixelFormatDest)
		{
	case EUidPixelFormatRGB_565:
		{
		TUint8* dataAddressDest = static_cast<TUint8*>(aDataAddressDest);
		const TInt dataLength = width << 1;
		while (dataAddressSrcEnd > dataAddressSrc)
			{
			Mem::Copy(dataAddressDest, dataAddressSrc, dataLength);
			dataAddressSrc = PtrAdd(dataAddressSrc, aDataStrideSrc);
			dataAddressDest += aDataStrideDest;
			}
		break;
		}
	case EUidPixelFormatARGB_8888_PRE:
	case EUidPixelFormatARGB_8888:
	case EUidPixelFormatXRGB_8888:
	case EUidPixelFormatXRGB_4444:
	case EUidPixelFormatL_1:
	case EUidPixelFormatL_2:
	case EUidPixelFormatL_4:
	case EUidPixelFormatL_8:
	case EUidPixelFormatP_4:
	case EUidPixelFormatP_8:
	case EUidPixelFormatRGB_888:
		{
		TUint8* dataAddressDest = static_cast<TUint8*>(aDataAddressDest);
		while (dataAddressSrcEnd > dataAddressSrc)
			{
			TransferScanLine64K(dataAddressDest, dataAddressSrc, aPixelFormatDest, width);
			dataAddressSrc = PtrAdd(dataAddressSrc, aDataStrideSrc);
			dataAddressDest += aDataStrideDest;
			}
		break;
		}
	default:
		err = KErrNotSupported;
		break;
		}
	return err;
	}

// Helper function used to copy a buffer in 16MAP to the various formats of the destination.
LOCAL_C TInt TransferPixels16MAP(TAny* aDataAddressDest, TInt aDataStrideDest, TUidPixelFormat aPixelFormatDest,
                                 const TUint32* aDataAddressSrc, TInt aDataStrideSrc, const TRect& aRectSource)
	{
	TInt err = KErrNone;
	const TInt width = aRectSource.Width();
	const TUint32* dataAddressSrc = PtrAdd(aDataAddressSrc + aRectSource.iTl.iX, aRectSource.iTl.iY * aDataStrideSrc);
	const TUint32* dataAddressSrcEnd = PtrAdd(dataAddressSrc, aRectSource.Height() * aDataStrideSrc);

	switch (aPixelFormatDest)
		{
	case EUidPixelFormatARGB_8888_PRE:
			{
			TUint8* dataAddressDest = static_cast<TUint8*>(aDataAddressDest);
			const TInt dataLength = width << 2;
			while (dataAddressSrcEnd > dataAddressSrc)
				{
				Mem::Copy(dataAddressDest, dataAddressSrc, dataLength);
				dataAddressSrc = PtrAdd(dataAddressSrc, aDataStrideSrc);
				dataAddressDest += aDataStrideDest;
				}
			break;
			}
	case EUidPixelFormatXRGB_8888:
	case EUidPixelFormatARGB_8888:
	case EUidPixelFormatRGB_565:
	case EUidPixelFormatXRGB_4444:
	case EUidPixelFormatL_1:
	case EUidPixelFormatL_2:
	case EUidPixelFormatL_4:
	case EUidPixelFormatL_8:
	case EUidPixelFormatP_4:
	case EUidPixelFormatP_8:
	case EUidPixelFormatRGB_888:
		{
		TUint8* dataAddressDest = static_cast<TUint8*> (aDataAddressDest);
		while (dataAddressSrcEnd > dataAddressSrc)
			{
			TransferScanLine16MAP(dataAddressDest, dataAddressSrc, aPixelFormatDest, width);
			dataAddressSrc = PtrAdd(dataAddressSrc, aDataStrideSrc);
			dataAddressDest += aDataStrideDest;
			}
		break;
		}
	default:
		err = KErrNotSupported;
		break;
		}
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Convert from display mode to pixel format.

@param aDisplayMode Display mode to convert.

@return Pixel format corresponding to aDisplayMode, or EUidPixelFormatUnknown if aDisplayMode does not match any existing pixel format.
*/
EXPORT_C TUidPixelFormat SgUtils::DisplayModeToPixelFormat(TDisplayMode aDisplayMode)
	{
	switch (aDisplayMode)
		{
	case EGray2:
		return EUidPixelFormatL_1;
	case EGray4:
		return EUidPixelFormatL_2;
	case EGray16:
		return EUidPixelFormatL_4;
	case EGray256:
		return EUidPixelFormatL_8;
	case EColor16:
		return EUidPixelFormatP_4;
	case EColor256:
		return EUidPixelFormatP_8;
	case EColor4K:
		return EUidPixelFormatXRGB_4444;
	case EColor64K:
		return EUidPixelFormatRGB_565;
	case EColor16M:
		return EUidPixelFormatRGB_888;
	case EColor16MU:
		return EUidPixelFormatXRGB_8888;
	case ERgb:
	case EColor16MA:
		return EUidPixelFormatARGB_8888;
	case EColor16MAP:
		return EUidPixelFormatARGB_8888_PRE;
	default:
		break;
		}
	return EUidPixelFormatUnknown;
	}


/**
@publishedPartner
@prototype
@deprecated

Convert from pixel format to display mode.

@param aPixelFormat Pixel format to convert.

@return Display mode corresponding to aPixelFormat, or ENone if aPixelFormat does not match any existing display mode.
*/
EXPORT_C TDisplayMode SgUtils::PixelFormatToDisplayMode(TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)
		{
	case EUidPixelFormatL_1:
		return EGray2;
	case EUidPixelFormatL_2:
		return EGray4;
	case EUidPixelFormatL_4:
		return EGray16;
	case EUidPixelFormatL_8:
		return EGray256;
	case EUidPixelFormatP_4:
		return EColor16;
	case EUidPixelFormatP_8:
		return EColor256;
	case EUidPixelFormatXRGB_4444:
		return EColor4K;
	case EUidPixelFormatRGB_565:
		return EColor64K;
	case EUidPixelFormatRGB_888:
		return EColor16M;
	case EUidPixelFormatXRGB_8888:
		return EColor16MU;
	case EUidPixelFormatARGB_8888:
		return EColor16MA;
	case EUidPixelFormatARGB_8888_PRE:
		return EColor16MAP;
	default:
		break;
		}
	return ENone;
	}


/**
@publishedPartner
@prototype
@deprecated

Calculates the minimum number of bytes between rows of data in a bi-dimensional pixel buffer.
The returned value is not guaranteed to be aligned onto any boundary.

@param aWidth Width in pixels of the buffer.
@param aPixelFormat Pixel format of the buffer.
@return The minimum number of bytes between rows of data in a buffer with the given width and pixel format,
        or KErrNotSupported if aPixelFormat is not recognized.
*/
EXPORT_C TInt SgUtils::MinDataStride(TInt aWidth, TUidPixelFormat aPixelFormat)
	{
	switch (aPixelFormat)
		{
	case EUidPixelFormatXRGB_8888:
	case EUidPixelFormatBGRX_8888:
	case EUidPixelFormatXBGR_8888:
	case EUidPixelFormatBGRA_8888:
	case EUidPixelFormatARGB_8888:
	case EUidPixelFormatABGR_8888:
	case EUidPixelFormatARGB_8888_PRE:
	case EUidPixelFormatABGR_8888_PRE:
	case EUidPixelFormatBGRA_8888_PRE:
	case EUidPixelFormatARGB_2101010:
	case EUidPixelFormatABGR_2101010:
		return aWidth << 2;
	case EUidPixelFormatBGR_888:
	case EUidPixelFormatRGB_888:
		return aWidth * 3;
	case EUidPixelFormatRGB_565:
	case EUidPixelFormatBGR_565:
	case EUidPixelFormatARGB_1555:
	case EUidPixelFormatXRGB_1555:
	case EUidPixelFormatARGB_4444:
	case EUidPixelFormatARGB_8332:
	case EUidPixelFormatBGRX_5551:
	case EUidPixelFormatBGRA_5551:
	case EUidPixelFormatBGRA_4444:
	case EUidPixelFormatBGRX_4444:
	case EUidPixelFormatAP_88:
	case EUidPixelFormatXRGB_4444:
	case EUidPixelFormatXBGR_4444:
		return aWidth << 1;
	case EUidPixelFormatRGB_332:
	case EUidPixelFormatA_8:
	case EUidPixelFormatBGR_332:
	case EUidPixelFormatP_8:
	case EUidPixelFormatL_8:
		return aWidth;
	case EUidPixelFormatP_4:
	case EUidPixelFormatL_4:
		return (aWidth + 1) >> 1;
	case EUidPixelFormatP_2:
	case EUidPixelFormatL_2:
		return (aWidth + 3) >> 2;
	case EUidPixelFormatP_1:
	case EUidPixelFormatL_1:
		return (aWidth + 7) >> 3;
	default:
		return KErrNotSupported;
		}
	}


/**
@publishedPartner
@prototype
@deprecated

Transfer data from one bi-dimensional pixel buffer to another bi-dimensional pixel buffer with a different pixel format.
The pixel format conversion may involve some data loss. This function currently supports the following pixel formats.
	- For the source: EUidPixelFormatRGB_565, EUidPixelFormatARGB_8888_PRE.
	- For the destination: EUidPixelFormatARGB_8888_PRE, EUidPixelFormatARGB_8888, EUidPixelFormatXRGB_8888, EUidPixelFormatRGB_888,
	  EUidPixelFormatRGB_565, EUidPixelFormatXRGB_4444, EUidPixelFormatP_8, EUidPixelFormatL_8, EUidPixelFormatL_4, EUidPixelFormatP_4,
	  EUidPixelFormatL_2, EUidPixelFormatL_1.

@param aDataAddressDest The base address of the destination pixel buffer.
@param aDataStrideDest The number of bytes between rows of data in the destination pixel buffer.
@param aPixelFormatDest Pixel format of the destination buffer.
@param aDataAddressSrc The base address of the source pixel buffer.
@param aDataStrideSrc The number of bytes between rows of data in the source pixel buffer.
@param aPixelFormatSrc Pixel format of the source buffer.
@param aRectSource Rectangle of the source pixel buffer which will be transferred to the destination pixel buffer.

@return KErrNone if successful.
@return KErrArgument If arguments do not lie within expected range.
@return KErrNotSupported If source or destination format is not supported.
*/
EXPORT_C TInt SgUtils::TransferPixels(TAny* aDataAddressDest, TInt aDataStrideDest, TUidPixelFormat aPixelFormatDest,
                                      const TAny* aDataAddressSrc, TInt aDataStrideSrc, TUidPixelFormat aPixelFormatSrc,
                                      const TRect& aRectSource)
	{
	if (aRectSource.IsEmpty() || !aDataAddressDest || !aDataAddressSrc || (aDataStrideDest <= 0) || (aDataStrideSrc <= 0)
		|| (SgUtils::MinDataStride(aRectSource.iBr.iX, aPixelFormatSrc) > aDataStrideSrc)
		|| (SgUtils::MinDataStride(aRectSource.Width(), aPixelFormatDest) > aDataStrideDest))
		{
		return KErrArgument;
		}

	TInt err = KErrNotSupported;
	switch (aPixelFormatSrc)
		{
	case EUidPixelFormatRGB_565:
		{
		err = TransferPixels64K(aDataAddressDest, aDataStrideDest, aPixelFormatDest,
		                        static_cast<const TUint16*>(aDataAddressSrc), aDataStrideSrc, aRectSource);
		break;
		}
	case EUidPixelFormatARGB_8888_PRE:
		{
		err = TransferPixels16MAP(aDataAddressDest, aDataStrideDest, aPixelFormatDest,
		                          static_cast<const TUint32*>(aDataAddressSrc), aDataStrideSrc, aRectSource);
		break;
		}
	default:
		break;
		}
	return err;
	}
