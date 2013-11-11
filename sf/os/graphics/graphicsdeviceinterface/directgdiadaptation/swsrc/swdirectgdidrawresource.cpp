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

#include "directgdiadapter.h"
#include "swdirectgdiengine.h"
#include "swdirectgdiimagesourceimpl.h"
#include "swdirectgdidriverimpl.h"
#include <bitdrawinterfaceid.h>
#include <e32cmn.h>
#include <bitdraw.h>
#include <bmalphablend.h>
#include <graphics/directgdidrawablesource.h>
#include <pixelformats.h>
#include "pixelutil.h"

/**
Helper class to deal with the case of blending 32-bit MAP source into 16-bit target which is not 
supported by screen driver CDrawSixteenBppBitmap implementation.

@publishedPartner
@prototype
@deprecated
*/
class TDrawDeviceWrapper
	{
public:
	TDrawDeviceWrapper(CFbsDrawDevice* aDrawDevice, CGraphicsContext::TDrawMode aDrawMode);
	inline void WriteLine(TInt aX, TInt aY, TInt aLength, TUint32* aBuffer, CGraphicsContext::TDrawMode aDrawMode);
private:
	inline TUint16 ConvertTo64K(TUint32 aColor);
	inline TUint16 Blend16MapTo64K(TUint16 aDest, TUint32 aSrc);
	void BlendLine16MapTo64K(TInt aX, TInt aY, TInt aLength, TUint32* aBuffer, CGraphicsContext::TDrawMode aDrawMode);
	void OriginalWriteLine(TInt aX, TInt aY, TInt aLength, TUint32* aBuffer, CGraphicsContext::TDrawMode aDrawMode);

private:
	CFbsDrawDevice* iDrawDevice;
	TUint32* iBits;

	typedef void (TDrawDeviceWrapper::*TWriteLineFunc)(TInt,TInt,TInt,TUint32*,CGraphicsContext::TDrawMode);
	TWriteLineFunc iWriteLineFunc;
	};

TDrawDeviceWrapper::TDrawDeviceWrapper(CFbsDrawDevice* aDrawDevice, CGraphicsContext::TDrawMode aDrawMode):
	iDrawDevice(aDrawDevice)
	{
	TAny* interface = NULL;
	TInt err = iDrawDevice->GetInterface(KFastBlit2InterfaceID, interface);
	// interface is guaranted to exist for 16-bit and 32-bit draw device
	GRAPHICS_ASSERT_DEBUG(err == KErrNone, EDirectGdiPanicUnexpectedError);

	iBits = (TUint32*) reinterpret_cast<MFastBlit2*>(interface)->Bits();
	
	// setup which funtion to call here rather tha making decision inside WriteLine which is usually called within
	// a tight scanline loop
	iWriteLineFunc = iDrawDevice->DisplayMode() == EColor64K && aDrawMode == CGraphicsContext::EDrawModePEN ? 
			&TDrawDeviceWrapper::BlendLine16MapTo64K : &TDrawDeviceWrapper::OriginalWriteLine;
	}

inline void TDrawDeviceWrapper::WriteLine(TInt aX, TInt aY, TInt aLength, TUint32* aBuffer, CGraphicsContext::TDrawMode aDrawMode)
	{
	// calling member functions via pointer to member functions i.e.
	// (object.*member_fn)(arg)
	//
	(this->*iWriteLineFunc)(aX, aY, aLength, aBuffer, aDrawMode);
	}

void TDrawDeviceWrapper::OriginalWriteLine(TInt aX, TInt aY, TInt aLength, TUint32* aBuffer, CGraphicsContext::TDrawMode aDrawMode)
	{
	iDrawDevice->WriteLine(aX, aY, aLength, aBuffer, aDrawMode);
	}

void TDrawDeviceWrapper::BlendLine16MapTo64K(TInt aX, TInt aY, TInt aLength, TUint32* aBuffer, CGraphicsContext::TDrawMode)
	{
	TUint16* pixelPtr = reinterpret_cast<TUint16*>(iBits);
	pixelPtr += (aY * iDrawDevice->LongWidth()) + aX;
	const TUint32* bufferPtr = aBuffer;
	const TUint32* bufferPtrLimit = bufferPtr + aLength;

	while (bufferPtr < bufferPtrLimit)
		{
		*pixelPtr = Blend16MapTo64K(*pixelPtr, *bufferPtr);
		++bufferPtr;
		++pixelPtr;
		}
	}

inline TUint16 TDrawDeviceWrapper::ConvertTo64K(TUint32 aSrc)
	{
	TInt col = (aSrc & 0x0000f8) >> 3;
	col |= (aSrc & 0x00fc00) >> 5;
	col |= (aSrc & 0xf80000) >> 8;
	
	return col;
	}
	
inline TUint16 TDrawDeviceWrapper::Blend16MapTo64K(TUint16 aDst, TUint32 aSrc)
	{
	const TInt alpha = aSrc >> 24;

	if(alpha == 0x00)
		{
		return aDst;
		}

	if (alpha == 0xff)
		{
		return ConvertTo64K(aSrc);
		}

	// extract source components from 16MAP
	const TInt src_rb = aSrc & 0x00ff00ff;
	const TInt src_g = aSrc & 0x0000ff00;
	const TInt oneMinusAlpha = 0x0100 - alpha;

	// extract destination components from 64K format
    TInt dr = (aDst & 0xf800) >> 8;
    dr += dr >> 5;
    TInt dg = (aDst & 0x07e0) >> 3;
    dg += dg >> 6;
    TInt db = (aDst & 0x001f) << 3;
    db += db >> 5;

	// combine red and blue components into a word to combine mult in one go
	TInt dst_rb = (dr << 16) | db;
	TInt dst_g = dg << 8;

	// dst and src are in pre-multiplied format (64K can be treated both as pre or non-pre)
	// dst = src + (1-alpha) * dst
	//
	dst_rb = (src_rb + ((oneMinusAlpha * dst_rb) >> 8)) & 0x00ff00ff;
	dst_g = (src_g + ((oneMinusAlpha * dst_g) >> 8)) & 0x0000ff00;
	
	const TUint32 argb = 0xff000000 | dst_rb | dst_g;
	return ConvertTo64K(argb);
	}

//
// implements MDirectGdiEngine interfaces
//

/**
@see MDirectGdiEngine::DrawResource(const TRect&,const RDirectGdiDrawableSource&,const TDesC8&)
*/
void CSwDirectGdiEngine::DrawResource(
		const TRect& aDestRect,
		const RDirectGdiDrawableSource& aSource,
		const TDesC8& /*aParam*/)
	{
	// DirectGDI reference implementation only support pixel based resource
	// see CSwDirectGdiDriverImpl::CreateDrawableSource()
	//
	CSwDirectGdiImageSourceImpl* imgSrc = NULL;
	TSize imgSize;
	if (CheckImageSource(aSource.Handle(), imgSrc, &imgSize) != KErrNone)
		{
		return;
		}

	// drawing resource unscaled with no rotation
	//
	DrawResourceCommon(aDestRect, imgSrc, TRect(TPoint(0,0), imgSize), DirectGdi::EGraphicsRotationNone);
	}

/**
@see MDirectGdiEngine::DrawResource(const TPoint&,const RDirectGdiDrawableSource&,DirectGdi::TGraphicsRotation)
*/
void CSwDirectGdiEngine::DrawResource(const TPoint& aPos,
		const RDirectGdiDrawableSource& aSource,
		DirectGdi::TGraphicsRotation aRotation)
	{
	CSwDirectGdiImageSourceImpl* imgSrc = NULL;
	TSize imgSize;
	if (CheckImageSource(aSource.Handle(), imgSrc, &imgSize) != KErrNone)
		{
		return;
		}

	// drawing resource unscaled
	//
	// rotation will be applied before scaling, we must create destination rectangle 
	// that match the size of rotated image

	TRect destRect(aPos, imgSize);
	if (aRotation==DirectGdi::EGraphicsRotation90 || aRotation==DirectGdi::EGraphicsRotation270)
		{
		// keep the top left corner in the same position but swap width and height
		destRect.SetWidth(imgSize.iHeight);
		destRect.SetHeight(imgSize.iWidth);
		}

	DrawResourceCommon(destRect, imgSrc, TRect(TPoint(0,0), imgSize), aRotation);
	}

/**
@see MDirectGdiEngine::DrawResource(const TRect&,const RDirectGdiDrawableSource&,DirectGdi::TGraphicsRotation)
*/
void CSwDirectGdiEngine::DrawResource(const TRect& aDestRect,
		const RDirectGdiDrawableSource& aSource,
		DirectGdi::TGraphicsRotation aRotation)
	{
	// aDestRect is not empty when we reach here
	//
	CSwDirectGdiImageSourceImpl* imgSrc = NULL;
	TSize imgSize;
	if (CheckImageSource(aSource.Handle(), imgSrc, &imgSize) != KErrNone)
		{
		return;
		}

	DrawResourceCommon(aDestRect, imgSrc, TRect(TPoint(0,0), imgSize), aRotation);
	}

/**
@see MDirectGdiEngine::DrawResource(const TRect&,const RDirectGdiDrawableSource&,const TRect&,DirectGdi::TGraphicsRotation)
*/
void CSwDirectGdiEngine::DrawResource(
		const TRect& aDestRect,				
		const RDirectGdiDrawableSource& aSource, 
		const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation)
	{
	// pre:
	// aDestRect and aSrcRect are not empty
	//
	CSwDirectGdiImageSourceImpl* imgSrc = NULL;
	TSize imgSize;
	if (CheckImageSource(aSource.Handle(), imgSrc, &imgSize) != KErrNone)
		{
		return;
		}

	// check source rectangle is fully contained within the image resource extent
	if (aSrcRect.iTl.iX < 0 ||
		aSrcRect.iTl.iY < 0 ||
		aSrcRect.iBr.iX > imgSize.iWidth ||
		aSrcRect.iBr.iY > imgSize.iHeight)
		{
		iDriver->SetError(KErrArgument);
		return;
		}   	
	
	DrawResourceCommon(aDestRect, imgSrc, aSrcRect, aRotation);
	}

//
// internal functions
//
/**
Checks image resource is fully constructed and registered with the driver.

@param  aHandle A valid handle to an image source.
@param  aImg On return, contains the image source.
@param  aSize If not NULL, will contain the dimensions of the image source.
@return KErrNone if successful, KErrBadHandle if aHandle is not a valid handle to an image source.
*/
TInt CSwDirectGdiEngine::CheckImageSource(TInt aHandle, CSwDirectGdiImageSourceImpl*& aImg, TSize* aSize)
	{
	// check image exist
	if (!iDriver->ValidImageSource(aHandle))
		{
		// replace KErrNotFound
		const TInt err = KErrBadHandle;
		iDriver->SetError(err);
		return err;
		}
	
	aImg = reinterpret_cast<CSwDirectGdiImageSourceImpl*>(aHandle);

	// RSgImage cannot be created with zero size, so there is no point in validating its size and
	// simply return image size if requested
	if (aSize)
		{		
		*aSize = aImg->Size();
		}

	return KErrNone;
	}

/**
Implements common DrawResource() functionality used by all DrawResource() variants.
*/
void CSwDirectGdiEngine::DrawResourceCommon(
		const TRect& aDestRect,
		const CSwDirectGdiImageSourceImpl* aImg,
		const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation)
	{
	// pre:
	// aDestRect and aSrcRect are not empty
	// aSrcRect is within the image extent
	//
	// translate relative coord to target absolute coord system
	TRect destRectAbs(aDestRect);
	destRectAbs.Move(iOrigin);

	// check whether we need to blend or write
	const TBool opaqueSource = (!PixelFormatUtil::HasAlpha(aImg->PixelFormat())) && (iDrawMode == DirectGdi::EDrawModePEN);
	if (opaqueSource)
		{
		iDrawMode = DirectGdi::EDrawModeWriteAlpha;
		}

	// repeat drawing op for each rectangle in the clipping region
	const TInt numOfRects = iDefaultRegionPtr->Count();	
	for (TInt idx = 0; idx < numOfRects; ++idx)
		{
		TRect clipRectAbs = (*iDefaultRegionPtr)[idx];
		if (!clipRectAbs.Intersects(destRectAbs))
			{
			continue;
			}

		// intersect current clip rect with dest rect to get actual draw area
		clipRectAbs.Intersection(destRectAbs);
		DoDrawResource(destRectAbs, aImg, aSrcRect, aRotation, clipRectAbs);

		iDrawDevice->UpdateRegion(clipRectAbs);
		}
	
	if (opaqueSource)
		{
		iDrawMode = DirectGdi::EDrawModePEN;
		}	
	}

/**
Rotate a given rectangle 90 degree clockwise around the specified origin.
*/
void CSwDirectGdiEngine::Rotate90(TRect& aRect, const TPoint& aOrigin)
	{
	// rotated bottom-left become top-left of rotated rect
	//
	TPoint bl(aRect.iTl.iX, aRect.iBr.iY);
	const TInt w = aRect.Width();
	const TInt h = aRect.Height();
	
	const TPoint dbl = bl - aOrigin;
	bl.iX = aOrigin.iX - dbl.iY;
	bl.iY = aOrigin.iY + dbl.iX;

	aRect = TRect(bl, TSize(h,w));
	}

/**
Rotate a given rectangle 180 degree clockwise around the specified origin.
*/
void CSwDirectGdiEngine::Rotate180(TRect& aRect, const TPoint& aOrigin)
	{
	// rotated bottom-right become top-left of rotated rect
	//
	TPoint br(aRect.iBr);
	const TSize sz = aRect.Size();

	const TPoint dbr = br - aOrigin;
	br.iX = aOrigin.iX - dbr.iX;
	br.iY = aOrigin.iY - dbr.iY;

	aRect = TRect(br, sz);
	}

/**
Rotate a given rectangle 270 degree clockwise around the specified origin.
*/
void CSwDirectGdiEngine::Rotate270(TRect& aRect, const TPoint& aOrigin)
	{
	// rotated top-right become top-left of rotated rect
	//
	TPoint tr(aRect.iBr.iX, aRect.iTl.iY);
	const TInt w = aRect.Width();
	const TInt h = aRect.Height();
	
	const TPoint dtr = tr - aOrigin;
	tr.iX = aOrigin.iX + dtr.iY;
	tr.iY = aOrigin.iY - dtr.iX;

	aRect = TRect(tr, TSize(h,w));
	}

/**
Tests that the size of a given rotated rectangle match the image source extent.
*/
TBool CSwDirectGdiEngine::RotatedSizeMatch(const TRect& aDst, const TRect& aSrc, DirectGdi::TGraphicsRotation aRot)
	{
	if (aRot==DirectGdi::EGraphicsRotationNone || aRot==DirectGdi::EGraphicsRotation180)
		{
		return aDst.Size()==aSrc.Size();
		}
	else
		{
		return aDst.Width()==aSrc.Height() && aDst.Height()==aSrc.Width();
		}
	}

/**
Draws a rectangular area of resource and apply rotation and/or scaling if requested.
*/
void CSwDirectGdiEngine::DoDrawResource(
		const TRect& aDestRectAbs,
		const CSwDirectGdiImageSourceImpl* aImg,
		const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation,
		const TRect& aClipRectAbs)
	{
	// pre:
	// SetClippingRegion() already check that clipping region is always contained
	// within device full extent.

	// check src rect match the size of rotated dest rect
	if (RotatedSizeMatch(aDestRectAbs, aSrcRect, aRotation))
		{
		// aClipRect is the effective drawing clipped area, it has been intersected with dest rect by the
		// calling function/DoDrawResourceCommon() and it is not empty when we reach here

		// image size has been checked at the top level DrawResource() no need to check it again

		// use aClipRect to determine how much area should be read from the image and transform
		// the effective read area into source rect depending on rotation parameter
		
		// start with aClipRect
		TRect xformSrcRect(aClipRectAbs);
		switch (aRotation)
			{
			case DirectGdi::EGraphicsRotationNone:
				// align top-left corner of dest rect with top-left corner of src rect
				xformSrcRect.Move(aSrcRect.iTl - aDestRectAbs.iTl);
				break;
			case DirectGdi::EGraphicsRotation90:
				{
				// align top-right corner of dest rect with top-left corner of src rect
				xformSrcRect.Move(aSrcRect.iTl - TPoint(aDestRectAbs.iBr.iX, aDestRectAbs.iTl.iY));
				// rotate 270 (-90) degree using top-left corner of src rect as pivot point
				Rotate270(xformSrcRect, aSrcRect.iTl);
				}
				break;
			case DirectGdi::EGraphicsRotation180:
				{
				// align bottom-right corner of dest rect with top-left corner of src rect
				xformSrcRect.Move(aSrcRect.iTl - aDestRectAbs.iBr);
				// rotate 180 (-180) degree using top-left corner of src rect as pivot point
				Rotate180(xformSrcRect, aSrcRect.iTl);
				}
				break;
			case DirectGdi::EGraphicsRotation270:
				{
				// align bottom-left corner of dest rect with top-left corner of src rect
				xformSrcRect.Move(aSrcRect.iTl - TPoint(aDestRectAbs.iTl.iX, aDestRectAbs.iBr.iY));
				// rotate 90 (-270) degree using top-left corner of src rect as pivot point
				Rotate90(xformSrcRect, aSrcRect.iTl);
				}
				break;

			// no need for extra check, aRotation has been checked at generic layer
			}

		DoBlitResource(aClipRectAbs.iTl, aImg, xformSrcRect, aRotation);
		return;
		}

	DoScaledBlitResource(aDestRectAbs, aImg, aSrcRect, aRotation, aClipRectAbs);
	}

/**
Draws a rectangular area of resource rotated with no scaling.
@panic DGDIAdapter 1009, if the pixel format of the draw device is unknown (debug only). 
*/
void CSwDirectGdiEngine::DoBlitResource(
		const TPoint& aDest,
		const CSwDirectGdiImageSourceImpl* aImg,
		const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation)
	{		
	// pre:
	// aDest is the top-left of clipped destination rectangle i.e. intersection of user destination
	// rectangle with current clipping rect
	// aSrcRect is rotated clipped read area i.e. effective read area with respect to original 
	// image orientation i.e.it no longer represents user specified source rect.

	// no need to do extra check on parameters here because:
	// aDest is guaranteed to be within device drawing area
	// aSrcRect is guaranteed to be within image source area
	
	const TUidPixelFormat devFormat = PixelFormatUtil::ConvertToPixelFormat(iDrawDevice->DisplayMode());
	GRAPHICS_ASSERT_DEBUG(devFormat != EUidPixelFormatUnknown, EDirectGdiPanicInvalidDisplayMode);

	const TUidPixelFormat imgFormat = aImg->PixelFormat();
	const TUint32* imgAddr = reinterpret_cast<TUint32*>(aImg->DataBuffer());
	const TInt imgStride = aImg->Stride();
	const TSize imgSize = aImg->Size();
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);
	
	const TInt width = aSrcRect.Width();
	const TInt height = aSrcRect.Height();
	// write scanline starting from top dest row
	TInt destY = aDest.iY;
	// setup pixel reader toolkit
	TPixelBufferReader reader(imgAddr, imgSize, imgStride, imgFormat);
	TDrawDeviceWrapper writer(iDrawDevice, GcDrawMode(iDrawMode));
	
	//
	// special case when rotation is none
	//
	if (aRotation == DirectGdi::EGraphicsRotationNone)
		{
		TAny* interface = NULL;
	
		// source and destination format match and using write alpha mode i.e. reduce blit to memcpy
		if (iDrawMode == DirectGdi::EDrawModeWriteAlpha && devFormat == imgFormat)
			{
			TInt err = iDrawDevice->GetInterface(KFastBlit2InterfaceID, interface);
			if (err == KErrNone)
				{
				GRAPHICS_ASSERT_DEBUG(interface, EDirectGdiPanicInvalidPointer);
				MFastBlit2* fblit = static_cast<MFastBlit2*>(interface);
				err = fblit->WriteBitmapBlock(aDest, imgAddr, imgStride, imgSize, aSrcRect);
				if (err == KErrNone)
					{
					return;
					}
				}
			}

		// fallback from MFastBlit2  when source and destination format match. 
		// Note that there was previously an optimization added here that used MFlastBlend if 
		// available, however it did not work correctly for some cases using DrawResource so has been 
		// removed from this code. (A source drawn with OpenGLES on to a target in XRGB_8888 format
		// actually had alpha in the unused channel which was being drawn by MFastBlend)
		if (devFormat == imgFormat)
			{
			for (TInt row=aSrcRect.iTl.iY; row<aSrcRect.iBr.iY; ++row,++destY)
				{
				const TPoint pos(aSrcRect.iTl.iX, row);
				const TUint32* slptr = reader.GetPixelAddr(pos);
				writer.WriteLine(aDest.iX, destY, width, const_cast<TUint32*>(slptr), drawMode);
				}
			
			return;
			}

		// there is one additional case that can be optimised by eleminating copy when:
		// rotation : none
		// scaling 	: none
		// dst		: opaque
		// src		: has alpha and premultiplied
		// mode 	: PEN
		}

	//
	// generic cases
	//
	// copying is necessary either to convert pixel format or to read back buffer in reverse order 
	// to achieve rotation
	//
	const TInt scanLineBytes = iDrawDevice->ScanLineBytes();
	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	TPtr8 scanLineDes(reinterpret_cast<TUint8*>(scanLineBuffer),scanLineBytes,scanLineBytes);

	// if destination is opaque e.g. RGB_565, XRGB_8888 and the source has alpha, it shall blend
	// (because iDrawMode is set to WritePEN)
	// we treat opaque destination as in pre-multiplied format (with alpha value 1), and read the 
	// source in pre-multiplied format too, to enable optimum blending computation i.e.
	// "src + (1-alpha) * dst"
	const TUidPixelFormat readFormat = !PixelFormatUtil::HasAlpha(devFormat) &&
							iDrawMode == DirectGdi::EDrawModePEN ?
							EUidPixelFormatARGB_8888_PRE : devFormat;

	if (aRotation == DirectGdi::EGraphicsRotationNone)
		{
		// general fallback from FastBlendBitmap
		//
		const TInt readLen = width;
		// normal read scanline, left to right
		//
		for (TInt row=aSrcRect.iTl.iY; row<aSrcRect.iBr.iY; ++row,++destY)
			{
			const TPoint pos(aSrcRect.iTl.iX, row);
			reader.GetScanLine(scanLineDes,pos, readLen, readFormat, TPixelBufferReader::EReadHorizontal);
			writer.WriteLine(aDest.iX, destY, readLen, scanLineBuffer, drawMode);
			}
		}
	else if (aRotation == DirectGdi::EGraphicsRotation90)
		{
		const TInt readLen = height;
		// read scanline vertically from bottom to up, and from the lef-most column
		//
		for (TInt col=aSrcRect.iTl.iX; col<aSrcRect.iBr.iX; ++col,++destY)
			{
			const TPoint pos(col, aSrcRect.iBr.iY-1);
			reader.GetScanLine(scanLineDes, pos, readLen, readFormat, TPixelBufferReader::EReadVerticalReverse);
			writer.WriteLine(aDest.iX, destY, readLen, scanLineBuffer, drawMode);
			}
		}
	else if (aRotation == DirectGdi::EGraphicsRotation180)
		{
		const TInt readLen = width;
		// read scanline from right to left, starting from the most bottom scanline
		//
		for (TInt row=aSrcRect.iBr.iY-1; row>=aSrcRect.iTl.iY; --row,++destY)
			{
			const TPoint pos(aSrcRect.iBr.iX-1, row);
			reader.GetScanLine(scanLineDes, pos, readLen, readFormat, TPixelBufferReader::EReadHorizontalReverse);
			writer.WriteLine(aDest.iX, destY, readLen, scanLineBuffer, drawMode);
			}
		}
	else if (aRotation == DirectGdi::EGraphicsRotation270)
		{
		const TInt readLen = height;
		// read scanline vertically top to bottom, and from the right-most column
		//
		for (TInt col=aSrcRect.iBr.iX-1; col>=aSrcRect.iTl.iX; --col,++destY)
			{
			const TPoint pos(col, aSrcRect.iTl.iY);
			reader.GetScanLine(scanLineDes, pos, readLen, readFormat, TPixelBufferReader::EReadVertical);
			writer.WriteLine(aDest.iX, destY, readLen, scanLineBuffer, drawMode);
			}
		}
	}

/**
Draws a rectangular area of resource rotated and/or scaled up/down.
@panic DGDIAdapter 1009, if the pixel format of the draw device is unknown (debug only).
*/
void CSwDirectGdiEngine::DoScaledBlitResource(
		const TRect& aDestRectAbs,
		const CSwDirectGdiImageSourceImpl* aImg,
		const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation,
		const TRect& aClipRectAbs)
	{
	// pre:
	// aDestRectAbs is the user specified dest rect that has been translated to target coord system. It may
	// be larger/outside the target drawing area. We must not modify this rect as it will be used to
	// determine the scaling factor.
	// aSrcRect is the user specified src rect, it is within the image extent.
	// aClipRectAbs is the intersection of current clipping rect and aDestRectAbs and is within the
	// target drawing area.

	const TUidPixelFormat devFormat = PixelFormatUtil::ConvertToPixelFormat(iDrawDevice->DisplayMode());
	GRAPHICS_ASSERT_DEBUG(devFormat != EUidPixelFormatUnknown, EDirectGdiPanicInvalidDisplayMode);

	const TUint32* imgAddr = reinterpret_cast<TUint32*>(aImg->DataBuffer());
	const TInt imgStride = aImg->Stride();
	const TSize imgSize = aImg->Size();
	const TUidPixelFormat imgFormat = aImg->PixelFormat();
	const CGraphicsContext::TDrawMode drawMode = GcDrawMode(iDrawMode);

	// if destination is opaque e.g. RGB_565, XRGB_8888 and the source has alpha, it shall blend
	// (because iDrawMode is set to WritePEN)
	// we treat opaque destination as in pre-multiplied format (with alpha value 1), and read the 
	// source in pre-multiplied format too, to enable optimum blending computation i.e.
	// "src + (1-alpha) * dst"
	const TUidPixelFormat readFormat = !PixelFormatUtil::HasAlpha(devFormat) &&
							iDrawMode == DirectGdi::EDrawModePEN ?
							EUidPixelFormatARGB_8888_PRE : devFormat;

	TUint32* scanLineBuffer = iDrawDevice->ScanLineBuffer();
	const TInt scanLineBytes = iDrawDevice->ScanLineBytes();
	TPtr8 scanLineDes(reinterpret_cast<TUint8*>(scanLineBuffer), scanLineBytes, scanLineBytes);

	// setup pixel reader toolkit
	//
	TPixelBufferReader reader(imgAddr, imgSize, imgStride, imgFormat);
	TDrawDeviceWrapper writer(iDrawDevice, GcDrawMode(iDrawMode));

	if (aRotation == DirectGdi::EGraphicsRotationNone)
		{		
		// Note that there was previously an optimization added here that used MFlastBlend if 
		// available, however it did not work correctly for some cases using DrawResource so has been 
		// removed from this code. (A source drawn with OpenGLES on to a target in XRGB_8888 format
		// actually had alpha in the unused channel which was being drawn by MFastBlend)
		//
		const TInt srcLen = aSrcRect.Width();
		const TInt destLen = aDestRectAbs.Width();
		const TInt clipLen = aClipRectAbs.Width();
		const TInt clipPos = aClipRectAbs.iTl.iX - aDestRectAbs.iTl.iX;

		// setup DDA for scaling in Y direction based on src rect and dst rect size
		// scaling in X direction will be done by GetScaledScanLine()
		//
		// setup Y scaler and start from top-left of src/dest to bottom-right of src/dest
		
		TLinearDDA yScaler;
		yScaler.Construct(
				TPoint(aSrcRect.iTl.iY, aDestRectAbs.iTl.iY),
				TPoint(aSrcRect.iBr.iY, aDestRectAbs.iBr.iY),
				TLinearDDA::ELeft);

		// move to position of current clip rect top row as the start of dest Y
		TInt srcY;	
		yScaler.JumpToYCoord2(srcY, aClipRectAbs.iTl.iY);

		// yPos contains mapping between src Y (TPoint.iX) and dest Y (TPoint.iY)
		TPoint yPos(srcY, aClipRectAbs.iTl.iY);

		// write to target from top to bottom
		//
		while (yPos.iY < aClipRectAbs.iBr.iY)
			{
			reader.GetScaledScanLine(scanLineDes,
									TPoint(aSrcRect.iTl.iX, yPos.iX),	// src X and Y
									clipPos,							// clipped dest X
									clipLen,
									destLen,
									srcLen,
									readFormat,
									TPixelBufferReader::EReadHorizontal);
			
			// use dest Y here
			writer.WriteLine(aClipRectAbs.iTl.iX, yPos.iY, clipLen, scanLineBuffer, drawMode);

			// move scaler one position
			yScaler.NextStep(yPos);
			};
		}
	else if (aRotation == DirectGdi::EGraphicsRotation90)
		{
		// we're going to read source vertically from bottom to up, swap relevant bits and pieces
		// dst-width corresponds to src-height
		// dst-height corresponds to src-width
		//
		const TInt srcLen = aSrcRect.Height();
		const TInt destLen = aDestRectAbs.Width();

		// the following doesn't change, the amount of pixel read (vertically) will be based on
		// the drawing area witdh i.e. clip width
		//
		const TInt clipLen = aClipRectAbs.Width();

		// offset into read area doesn't change either, it will be translated into some Y position
		// from bottom row of source image
		const TInt clipPos = aClipRectAbs.iTl.iX - aDestRectAbs.iTl.iX;

		// setup DDA for scaling in src X direction based on src rect and dst rect size
		// scaling in src Y direction will be done by GetScaledScanLine(EReadVerticalReverse)
		//
		// scaler map dest Y to src X because of 90 degre rotation
		// start from  top row dest, left-most column src to bottom row dest, right-most column src
		//
		TLinearDDA xScaler;
		xScaler.Construct(
				TPoint(aSrcRect.iTl.iX, aDestRectAbs.iTl.iY),
				TPoint(aSrcRect.iBr.iX, aDestRectAbs.iBr.iY),
				TLinearDDA::ELeft);
		
		// move to position of current clip rect top row as the start of src X
		TInt srcX;	
		xScaler.JumpToYCoord2(srcX, aClipRectAbs.iTl.iY);

		// xPos contains mapping between src X (TPoint.iX) and dest Y (TPoint.iY)
		TPoint xPos(srcX, aClipRectAbs.iTl.iY);

		// write to target from top to bottom
		//
		while (xPos.iY < aClipRectAbs.iBr.iY)
			{
			// read pixel vertically from left column to right
			reader.GetScaledScanLine(scanLineDes,
									TPoint(xPos.iX, aSrcRect.iBr.iY - 1),	// src X, src Y
									clipPos,								// distance from bottom source
									clipLen,
									destLen,
									srcLen,
									readFormat,
									TPixelBufferReader::EReadVerticalReverse);
			
			// use dest Y here
			writer.WriteLine(aClipRectAbs.iTl.iX, xPos.iY, clipLen, scanLineBuffer, drawMode);

			// move scaler one position
			xScaler.NextStep(xPos);
			}
		}
	else if (aRotation == DirectGdi::EGraphicsRotation180)
		{
		const TInt srcLen = aSrcRect.Width();
		const TInt destLen = aDestRectAbs.Width();
		const TInt clipLen = aClipRectAbs.Width();

		// clipPos doesn't need to be inverted (using iBr.iX) because the yScaler below
		// will do that for us (by stepping backward)
		//
		const TInt clipPos = aClipRectAbs.iTl.iX - aDestRectAbs.iTl.iX;

		// setup DDA for scaling in Y direction based on src rect and dst rect size
		// scaling in X direction will be done by GetScaledScanLine()
		//
		// setup Y scaler and start from bottom-right of src/dest to top-left of src/dest
		// to do backward stepping (src Y inversion)

		TLinearDDA yScaler;
		yScaler.Construct(
				// we starting from 1st pixel from bottom of source image, we need to
				// step back 1 position as bottom-row coord is beyond the last pixel in the source rect
				TPoint(aSrcRect.iBr.iY - 1, aDestRectAbs.iTl.iY),
				TPoint(aSrcRect.iTl.iY - 1, aDestRectAbs.iBr.iY),
				TLinearDDA::ELeft);

		// move to position of current clip rect top row as the start of dest Y
		// which will calculate inverted src Y distance from bottom of source rectangle
		TInt invSrcY;	
		yScaler.JumpToYCoord2(invSrcY, aClipRectAbs.iTl.iY);

		// yPos contains mapping between inverted src Y (TPoint.iX) and dest Y (TPoint.iY)
		TPoint yPos(invSrcY, aClipRectAbs.iTl.iY);

		// write to target from top to bottom
		//
		while (yPos.iY < aClipRectAbs.iBr.iY)
			{
			// read scanline from righ to left i.e. use aSrcRect.iBr.iX-1 (last column)
			// as starting src X value
			reader.GetScaledScanLine(scanLineDes,
									TPoint(aSrcRect.iBr.iX - 1, yPos.iX),	// src X and inverted src Y
									clipPos,								// clipped dest X
									clipLen,
									destLen,
									srcLen,
									readFormat,
									TPixelBufferReader::EReadHorizontalReverse);
			
			// use dest Y here
			writer.WriteLine(aClipRectAbs.iTl.iX, yPos.iY, clipLen, scanLineBuffer, drawMode);

			// move scaler one position
			yScaler.NextStep(yPos);
			}
		}
	else if(aRotation == DirectGdi::EGraphicsRotation270)
		{
		// we're going to read source vertically from top to bottom, swap relevant bits and pieces
		// dst-width corresponds to src-height
		// dst-height corresponds to src-width
		const TInt srcLen = aSrcRect.Height();
		const TInt destLen = aDestRectAbs.Width();

		// the following doesn't change, the amount of pixel read (vertically) will be based on
		// the drawing area witdh i.e. clip width
		const TInt clipLen = aClipRectAbs.Width();

		// offset into read area doesn't change either, it will be translated into some Y position
		// from top row of source image
		const TInt clipPos = aClipRectAbs.iTl.iX - aDestRectAbs.iTl.iX;

		// setup DDA for scaling in src X direction based on src rect and dst rect size
		// scaling in src Y direction will be done by GetScaledScanLine(EReadVertical)
		//
		// scaler map dest Y to src X because of 270 degre rotation
		// start from top row dest, right-most column src to bottom row dest, left-most column src

		TLinearDDA xScaler;
		xScaler.Construct(
				// decrement 1 pixel to get into last pixel within source
				TPoint(aSrcRect.iBr.iX - 1, aDestRectAbs.iTl.iY),
				TPoint(aSrcRect.iTl.iX - 1, aDestRectAbs.iBr.iY),
				TLinearDDA::ELeft);

		// move to position of current clip rect top row as the start of src X
		TInt srcX;	
		xScaler.JumpToYCoord2(srcX, aClipRectAbs.iTl.iY);
		// xPos contains mapping between src X (TPoint.iX) and dest Y (TPoint.iY)
		TPoint xPos(srcX, aClipRectAbs.iTl.iY);

		// write to target from top to bottom
		//
		while (xPos.iY < aClipRectAbs.iBr.iY)
			{
			// read pixel vertically from left column to right
			reader.GetScaledScanLine(scanLineDes,
									TPoint(xPos.iX, aSrcRect.iTl.iY),		// src X, src Y
									clipPos,								// distance from bottom source
									clipLen,
									destLen,
									srcLen,
									readFormat,
									TPixelBufferReader::EReadVertical);
			
			// use dest Y here
			writer.WriteLine(aClipRectAbs.iTl.iX, xPos.iY, clipLen, scanLineBuffer, drawMode);

			// move scaler one position
			xScaler.NextStep(xPos);
			}
		}
	}
