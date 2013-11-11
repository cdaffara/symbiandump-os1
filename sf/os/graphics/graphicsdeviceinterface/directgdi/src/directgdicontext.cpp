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

#include "directgdicontext.h"
#include "directgdipaniccodes.h"
#include "directgdifont.h"
#include "directgdidriver.h"
#include <graphics/directgdidrawablesource.h>
#include <graphics/directgdiengine.h>
#include <e32cmn.h>
#include <s32mem.h>
#include <shapeinfo.h>
#include <fbs.h>
#include <fntstore.h>

using namespace DirectGdi;

/**
CDirectGdiContext InternalizeL/ExternalizeL - version numbers.
Add new version numbers here. A reason of adding new version numbers may be adding new 
CDirectGdiContext data members, which may have to be externalized/internalized. 
When that happens:
1.Put a new enum item (like EDirectGDIContext_Ver01) with a version number, which is greater than
the last version number, that was used. 
2.Document the new enum item.
3.Update KDirectGDIContext_VerNo value to be the new enum item value.
4.Update InternalizeL/ExternalizeL methods after adding the new version number.
For example: If a new member is added to the class - TInt iSmth, when InternalizeL
is called to operate on older archive, iSmth member won't be in the archive!
So, in InternalizeL, there should be a check, something like:
TUint16 archiveVerNo = 0;
	aReadStream >> archiveVerNo;
    if(archiveVerNo < EDirectGDIContext_Ver02)   //EDirectGDIContext_Ver02 has been added, when iSmth has been added
		{
      	//Do nothing - iSmth is not in the archive
      	//Initialize it with some default value
      	iSmth = KDefVal;
		}
	else
		{
		aReadStream >> iSmth;
		}
*/
enum
	{
	EDirectGDIContext_Ver01 = 1 //Base version number, when InternalizeL/ExternalizeL were added
	};

LOCAL_D const TUint16 KDirectGDIContext_VerNo = EDirectGDIContext_Ver01;

/**
Static two-phase factory constuctor. Creates an object of this class.

@param	aDirectGdiDriver	The driver object which provides an abstract factory for creating a concrete drawing engine.

@pre	CDirectGdiDriver object has been initialised from the calling thread.
@post	Instances of CDirectGdiContext and MDirectGdiEngine have been created.

@leave	Leaves if CDirectGdiDriver has not been initialised from the calling thread, or other errors occur during object construction.
@return	Reference to constructed CDirectGdiContext.
*/
EXPORT_C CDirectGdiContext* CDirectGdiContext::NewL(CDirectGdiDriver& aDirectGdiDriver)
	{
	CDirectGdiContext* result = new(ELeave) CDirectGdiContext(aDirectGdiDriver);
	CleanupStack::PushL(result);

	// Cache a reference to the driver locally, so that we do not need to use
	// the singleton accessor all the time. The singleton accessor utilises TLS
	// and hence incurs a performance penalty.
	//
	result->ConstructL();

	CleanupStack::Pop();
	return result;
	}


/**
Installs a rendering engine instance from the supplied driver, records
a reference to the driver (for use by the destructor), and initialises it.

@pre	Invoked by NewL().
@post	Instance of MDirectGdiEngine has been created.

@leave	If CDirectGdiDriver::CreateEngine() returns an error code.
*/
void CDirectGdiContext::ConstructL()
	{
	User::LeaveIfError(iDriver.CreateEngine(iEngine));
	Reset();	// Initialise context and engine to default values.
	}


/**
Object constructor. Declared private for better encapsulation. A factory method is 
provided to instantiate this class. This class is not intended for derivation. Binary
compatibility needs to be maintained.

@param	aDirectGdiDriver The driver that coordinates management of DirectGDI resources.

@pre	The driver must be initialised.
@post	None.
*/
EXPORT_C CDirectGdiContext::CDirectGdiContext(CDirectGdiDriver& aDirectGdiDriver) :
	iDriver(aDirectGdiDriver)	
	{	
	}


/**
Unbind current target from drawing context and mark drawing engine for deletion.

@pre	None.
@post	Rendering engine has had targets unbound and is marked for deletion.
*/
EXPORT_C CDirectGdiContext::~CDirectGdiContext()
	{
	// If an engine has been bound, destroy it through the driver.
	if (iEngine)
		{
		iDriver.DestroyEngine(iEngine);
		}

	CleanUpBrushPattern();
	iClippingRegion.Close();
	}


/**
Binds a rendering target to this drawing context.

Subsequent rendering operations after calling this method will apply to the new rendering
target. The clipping region will be reset to none, in other words drawing will be clipped to the 
full area of the new target by default, other context states such as pen or brush colour,
font, etc. will remain unchanged.

This operation could fail, DirectGDI clients should always check the return value when 
calling this method. The error state is not modified by this function.

@param	aTarget	DirectGDI rendering target.

@pre	Rendering target has been fully constructed.
@post	The drawing context is bound to the new rendering target.

@return	KErrNone if successful, KErrNotSupported if target is incompatible with the drawing context,
		otherwise one of the system-wide error codes.
*/
EXPORT_C TInt CDirectGdiContext::Activate(RDirectGdiImageTarget& aTarget)
	{
	GRAPHICS_TRACE("CDirectGdiContext::Activate");	
	TInt err = iEngine->Activate(aTarget);
	
	iActivated = EFalse;
	if (err == KErrNone)
		{
		iActivated = ETrue;
		}
	return err;
	}

/**
Disables AutoUpdateJustification state. 

@see CDirectGdiContext::SetJustifyAutoUpdate
*/
EXPORT_C void CDirectGdiContext::NoJustifyAutoUpdate()
	{
	GRAPHICS_TRACE("CDirectGdiContext::NoJustifyAutoUpdate");
	iAutoUpdateJustification = EFalse;
	}

/**
Enables AutoUpdateJustification state. 
During the text drawing, some text parameters which impact on positioning the text on the screen will be updated and applied 
*/
EXPORT_C void CDirectGdiContext::SetJustifyAutoUpdate()
	{
	GRAPHICS_TRACE("CDirectGdiContext::SetJustifyAutoUpdate");
	iAutoUpdateJustification = ETrue;
	}

/**
Draws the whole of a CFbsBitmap. Calls BitBlt() with a rectangle set to the extents of aBitmap.

No scaling or stretching is involved. The current clipping region and drawing mode apply. The
source bitmap can reside in system memory or ROM. Bitmaps in system memory may be compressed
using RLE or Palette compression, and can be in any supported display mode. Please refer to
CFbsBitmap documentation for more details.

If the client modifies the content of the bitmap after issuing a BitBlt() command, the method
does not guarantee whether the old bitmap content or the new one will be drawn. Clients must call
Finish() on the driver before modifying the bitmap content if they want a guaranteed result that 
the previously issued BitBlt() will draw the old bitmap content.

In the event of a failure, the error state is set to one of the system-wide error codes.

@see	void CDirectGdiContext::BitBlt(const TPoint& aPoint, const CFbsBitmap* aBitmap, const TRect& aSourceRect);
@param	aDestPos		The position to draw the top left corner of the bitmap.
@param	aSourceBitmap	The source bitmap. 

@pre	The rendering target has been activated.
@post	Request to draw the bitmap content has been accepted. There is no guarantee that the request
		has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::BitBlt(const TPoint& aDestPos, const CFbsBitmap& aSourceBitmap)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::BitBlt(%d,%d)", aDestPos.iX, aDestPos.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	
	if (ValidateBitmap (aSourceBitmap))
		{
		iEngine->BitBlt(aDestPos, aSourceBitmap, TRect(aSourceBitmap.SizeInPixels()));
		}
	}


/** 
Draws a particular rectangle from a CFbsBitmap via bitmap block image transfer. 
The bitmap content is specified by the given source rectangle. The bitmap content is drawn 
into the rendering target starting from the given destination position. To draw the content
of the entire bitmap, a source rectangle TRect(TPoint(0,0), aBitmap.SizeInPixels()) is used. 

The source rectangle is intersected with the source bitmap’s full extent, and the intersection
will become the effective value of the source rectangle.

No scaling or stretching is involved. The current clipping region and drawing mode apply. The
source bitmap can reside in system memory or ROM. Bitmaps in system memory may be compressed
using RLE or Palette compression, and can be in any supported display mode. Please refer to
CFbsBitmap documentation for more details.

The bitmap is not guaranteed to continue to exist outside the implementation of this method, for example
DirectGDI clients may delete the bitmap immediately after issuing a BitBlt() command. The adaptation
must maintain its own copy by duplicating the bitmap if access to the bitmap data is required
later on, for example outside this method implementation. Duplicating the bitmap will increase its
reference counter and will prevent the object from being destroyed by Fbserv. The adaptation
must make sure the bitmap copy is destroyed when it is no longer needed.

If the client modifies the content of the bitmap after issuing BitBlt() command, the method
does not guarantee whether the old bitmap content or the new one will be drawn. Clients must call
Finish() on the driver before modifying the bitmap content if they want a guaranteed result that 
the previously issued BitBlt() will draw the old bitmap content.

In the event of a failure, the error state is set to one of the system-wide error codes.

@param	aDestPos		The position to draw the top left corner of the bitmap (destination position).
@param	aSourceBitmap	The source bitmap.
@param	aSourceRect		A rectangle defining the piece of the source to be drawn. No image data 
						is transferred if no intersection exists with the bitmap.  
						NOTE: The top and left hand edges are inclusive, the bottom and 
						right hand edge are exclusive.

@pre	The rendering target has been activated.
@post	Request to draw the bitmap content has been accepted. There is no guarantee that the request
		has been processed when the method returns.

@panic 	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::BitBlt(
		const TPoint& aDestPos, 
		const CFbsBitmap& aSourceBitmap, 
		const TRect& aSourceRect)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::BitBlt(%d,%d)", aDestPos.iX, aDestPos.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	
	if (ValidateBitmap(aSourceBitmap))
		{
		TRect sourceRect = IntersectBitmapWithRect(aSourceBitmap, aSourceRect);		
		if (!sourceRect.IsEmpty())
			{
			iEngine->BitBlt(aDestPos, aSourceBitmap, sourceRect);
			}
		}
	}

/**
Validates a specified bitmap image. 
The bitmap is deemed invalid if its associated handle equals KNullHandle or it 
has a width or height of zero.

The following errors are set in iDirectGdiDriver if the associated conditions are met:
	- KErrBadHandle if the handle associated with the bitmap equals KNullHandle.
	- KErrArgument is the bitmaps width or height is zero.

@param aBitmap The bitmap to validate.

@return ETrue if the specified bitmap is valid, otherwise EFalse.
*/
TBool CDirectGdiContext::ValidateBitmap(const CFbsBitmap& aBitmap)
	{
	TInt errorCode = KErrNone;
	TBool result = ETrue;

	if (aBitmap.Handle() == KNullHandle)
		{
		errorCode = KErrBadHandle;
		}
	else
		{
		// Check that mask and source bitmap have width and height.
		const TSize bitmapSize = aBitmap.SizeInPixels();
		if (!bitmapSize.iWidth || !bitmapSize.iHeight)
			{
			errorCode = KErrArgument;
			}
		}

	if (errorCode != KErrNone)
		{
		iDriver.SetError(errorCode);	
		result = EFalse;
		}

	return result;
	}

/*
Returns a TRect that intersects both the CFbsBitmap and the passed-in TRect.

@param aBitmap The bitmap to intersect with.
@param aRect The TRect object that is overlapping the bitmap.
@return A TRect that represents the intersection of the two. If the two do not intersect,
        it will be an empty TRect object.
*/
TRect CDirectGdiContext::IntersectBitmapWithRect(const CFbsBitmap& aBitmap, const TRect& aRect) const
	{
	TRect result = TRect(aBitmap.SizeInPixels());
	
	if (aRect == result)
		return result;
	
	if (result.Intersects(aRect))
		{
		result.Intersection(aRect);
		return result;
		}
	
	return TRect(0,0,0,0);
	}

/**

Validates the specified source bitmap and mask pair.
First validates the source and mask bitmaps individually, and then checks for valid
bitmap pairings. The only pairing not supported is: EGray256 masks and sources with 
an alpha-channel when using EDrawModeWriteAlpha.
 
@see CDirectGdiContext::ValidateBitmap()

The following errors are set in iDirectGdiDriver if the associated conditions are met:
	- KErrBadHandle if the handle associated with either bitmap equals KNullHandle.
	- KErrArgument if either bitmaps width or height is zero.
	- KErrArgument if the mask format is EGray256 and the source contains an alpha-channel and draw mode is EDrawModeWriteAlpha.
 
@param aSourceBitmap 	The source bitmap to validate.
@param aMaskBitmap 		The mask to validate.
 
@return ETrue if the specified bitmaps is valid, otherwise EFalse.	
*/
TBool CDirectGdiContext::ValidateSourceAndMaskBitmaps(const CFbsBitmap& aSourceBitmap, const CFbsBitmap& aMaskBitmap)
	{
	TInt errorCode = KErrNone;
	TBool result = ETrue;
	
	if (ValidateBitmap (aSourceBitmap) && ValidateBitmap (aMaskBitmap))
		{
		TDisplayMode sourceBitmapDisplayMode = aSourceBitmap.DisplayMode();
	
		// We do not currently support EGray256 masks and sources with an alpha-channel
		// with EDrawModeWriteAlpha.
		if ((iDrawMode == DirectGdi::EDrawModeWriteAlpha) &&
			(aMaskBitmap.DisplayMode() == EGray256) &&
			((sourceBitmapDisplayMode == EColor16MA) || (sourceBitmapDisplayMode == EColor16MAP)))
			{
			errorCode = KErrArgument;
			}
		}
	else
		{
		result = EFalse;
		}
	
	if (errorCode != KErrNone)
		{
		iDriver.SetError(errorCode);	
		result = EFalse;
		}
	
	return result;
	}

/**
Helper method for performing BitBltMasked.
Note that aInvertMask is ignored if aMaskPos is not at 0,0.

@see	CDirectGdiContext::BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap, const TRect& aSourceRect);
@see	CDirectGdiContext::BitBltMasked(const TPoint&, const CFbsBitmap&,const TRect&, const CFbsBitmap&, const TPoint&);

@param	aDestPos		The destination for the top left corner of the transferred bitmap. 
						It is relative to the top left corner of the destination bitmap, which may be the screen. 
@param	aSourceBitmap	A memory-resident source bitmap.
@param	aSourceRect		A rectangle defining the piece of the bitmap to be drawn, 
						with co-ordinates relative to the top left corner of the bitmap. 
@param	aMaskBitmap		Mask bitmap.
@param	aInvertMask		If EFalse, a source pixel that is masked by a black pixel is not transferred to 
						the destination rectangle. If ETrue, then a source pixel that is masked by a 
						white pixel is not transferred to the destination rectangle. If alpha blending
						is used instead of masking, this flag is ignored and no inversion takes place.
						Note that this parameter is ignored if aMaskPos does not equal TPoint(0,0).
@param	aMaskPos		The point on the mask bitmap to use as the top left corner 
*/
void CDirectGdiContext::DoBitBltMasked(
		const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap,
		TBool aInvertMask,
		const TPoint& aMaskPos)
	{
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	
	if (ValidateSourceAndMaskBitmaps(aSourceBitmap, aMaskBitmap))
		{
		// If the source rectangle does not intersect aBitmap, do nothing.
		TRect sourceRect = IntersectBitmapWithRect(aSourceBitmap, aSourceRect);
		if (!sourceRect.IsEmpty())
			{
			if (aMaskPos == TPoint(0, 0))
				{
				iEngine->BitBltMasked(aDestPos, aSourceBitmap, sourceRect, aMaskBitmap, aInvertMask);
				}
			else
				{
				TSize maskSize = aMaskBitmap.SizeInPixels();
				// Convert negative or large mask offsets into sensible positive ones for tiling.
				TPoint maskOffset(aMaskPos.iX % maskSize.iWidth, aMaskPos.iY % maskSize.iHeight);
				if (maskOffset.iX < 0) 
					maskOffset.iX += maskSize.iWidth;
				if (maskOffset.iY < 0) 
					maskOffset.iY += maskSize.iHeight;

				iEngine->BitBltMasked(aDestPos, aSourceBitmap, sourceRect, aMaskBitmap, maskOffset);
				}
			}
		}
	}


/**
Performs a masked bitmap block transfer. Source rectangle operates in a similar way to BitBlt().

This function uses either a black and white (binary) mask bitmap, or if the mask's display mode is 
EGray256, alpha blending is used. The result is undefined if the mask pixel value is neither black nor 
white and the mask display mode is other than EGray256.

The mask is aligned with the source bitmap by aligning the first pixel of the mask and source bitmaps within
the source rectangle. Tiling in both directions applies if the mask size is smaller than the source rectangle.
Note that the mask is applied before the piece of the bitmap is defined - the mask is tiled relative to the 
top left of the original source bitmap rather than the top left of the bitmap piece. If the mask has zero
width or height, the error state is set to KErrArgument and no drawing is performed.

The mask bitmap can be used as either a positive or negative mask. Masked pixels are not mapped to the 
destination rectangle.

If the client modifies the contents of the bitmap or the mask after issuing the BitBltMasked() command, the
method does not guarantee whether the old bitmap or mask contents, or the new ones will be used. 
Clients must call Finish() on the driver before modifying the bitmap or mask contents if they want a 
guaranteed  result that the previously issued BitBltMasked() will be using the old bitmap or mask contents.

In the event of a failure, the error state is set to one of the system-wide error codes.

@see	CDirectGdiContext::BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap, const TRect& aSourceRect);
@see	CDirectGdiContext::BitBltMasked(const TPoint&, const CFbsBitmap&,const TRect&, const CFbsBitmap&, const TPoint&);

@param	aDestPos		The destination for the top left corner of the transferred bitmap. 
						It is relative to the top left corner of the destination bitmap, which may be the screen. 
@param	aSourceBitmap	A memory-resident source bitmap.
@param	aSourceRect		A rectangle defining the piece of the bitmap to be drawn, 
						with co-ordinates relative to the top left corner of the bitmap. 
@param	aMaskBitmap		Mask bitmap.
@param	aInvertMask		If EFalse, a source pixel that is masked by a black pixel is not transferred to 
						the destination rectangle. If ETrue, then a source pixel that is masked by a 
						white pixel is not transferred to the destination rectangle. If alpha blending
						is used instead of masking, this flag is ignored and no inversion takes place.

@pre	The rendering target has been activated. aBitmap and aMask hold valid handles.
@post	Request to draw the masked bitmap content has been accepted. 
		There is no guarantee that the request has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::BitBltMasked(
		const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap,
		TBool aInvertMask)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::BitBltMasked(%d,%d)", aDestPos.iX, aDestPos.iY);
	DoBitBltMasked (aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask, TPoint(0, 0));
	}


/**
Performs a masked bitmap block transfer. Source rectangle operates in a similar way to BitBlt().

This function uses either a black and white (binary) mask bitmap, or if aMaskBitmap's display mode is 
EGray256, alpha blending is used. The result is undefined if the mask pixel value is neither black nor 
white and the mask display mode is other than EGray256.

The source rectangle is intersected with the source bitmap’s full extent and the intersection
will become the effective value. The mask bitmap is aligned with the source bitmap by aligning
its pixel at the position specified in aMaskPt with the first pixel of the source bitmap
within the source rectangle. The mask bitmap will be tiled if it is smaller than the source
rectangle. If the mask has zero width or height, the error state is set to KErrArgument and 
no drawing is performed.

If the client modifies the contents of the bitmap or the mask after issuing the BitBltMasked() command, the
method does not guarantee whether the old bitmap or mask contents, or the new ones will be used. 
Clients must call Finish() on the driver before modifying the bitmap or mask contents if they want a 
guaranteed  result that the previously issued BitBltMasked() will be using the old bitmap or mask contents.

In the event of a failure, the error state is set to one of the system-wide error codes.

@see	CDirectGdiContext::BitBlt(const TPoint& aPoint, const CFbsBitmap& aBitmap, const TRect& aSourceRect);
@see	CDirectGdiContext::BitBltMasked(const TPoint&, const CFbsBitmap&,const TRect&, const CFbsBitmap&, TBool);

@param	aDestPos		The destination for the top left corner of the transferred bitmap. 
						It is relative to the top left corner of the destination bitmap, which may be the screen. 
@param	aSourceBitmap	A memory-resident source bitmap.
@param	aSourceRect		A rectangle defining the piece of the bitmap to be drawn, 
						with co-ordinates relative to the top left corner of the bitmap. 
@param	aMaskBitmap		Mask bitmap.
@param	aMaskPos		The point on the mask bitmap to use as the top left corner 

@pre	The rendering target has been activated. aBitmap and aMask hold valid handles.
@post	Request to draw the masked bitmap content has been accepted. 
		There is no guarantee that the request has been processed when the method returns.

@panic	DGDI 7, if no context is activated
*/
EXPORT_C void CDirectGdiContext::BitBltMasked(
		const TPoint& aDestPos,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap,
		const TPoint& aMaskPos)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::BitBltMasked(%d,%d)", aDestPos.iX, aDestPos.iY);
	DoBitBltMasked (aDestPos, aSourceBitmap, aSourceRect, aMaskBitmap, EFalse, aMaskPos);		
	}


/**
Resets the current clipping region to none.

@see    CDirectGdiContext::SetClippingRegion(const TRegion&)

@pre	The rendering target has been activated.
@post	Clipping region is reset to none. Subsequent rendering operations on current target will be clipped to the
		full area of the target.

@panic 	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::ResetClippingRegion()
	{
	GRAPHICS_TRACE("CDirectGdiContext::ResetClippingRegion");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	iClippingRegion.Clear();
	iEngine->ResetClippingRegion();
	}


/**
Clears the entire target area with the current brush colour. The area is filled 
as if ESolidBrush is used. Current clipping region and drawing mode apply.

@see	CDirectGdiContext::Clear(const TRect&)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
@see	CDirectGdiContext::SetClippingRegion(const TRegion& aRegion)

@pre	The rendering target has been activated.
@post	Request to clear given rectangular area (clipped to current clipping region) 
		has been accepted. There is no guarantee that the request has been processed 
		when the method returns.

@panic 	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::Clear()
	{
	GRAPHICS_TRACE("CDirectGdiContext::Clear");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	iEngine->Clear();
	}


/**
Clears the given rectangular area with current brush colour. The area is filled 
as if ESolidBrush is used. Current clipping region and drawing mode apply.

@see	CDirectGdiContext::Clear()
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
@see	CDirectGdiContext::SetClippingRegion(const TRegion&)

@param	aRect Area to be cleared.

@pre	The rendering target has been activated.
@post	Request to clear given rectangular area (clipped to current clipping region) has been accepted. 
		There is no guarantee that the request has been processed when the method returns.

@panic 	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::Clear(const TRect& aRect)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::Clear(%d,%d)", aRect.Width(), aRect.Height());
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if(aRect.Width() <= 0 || aRect.Height() <= 0)
		{
		return;
		}
	iEngine->Clear(aRect);
	}


/**
Resets the bitmap brush pattern to none. If the current brush style is EPatternedBrush, the brush
style will be set to ENullBrush.

@see	CDirectGdiContext::SetBrushPattern(const CFbsBitmap&)

@pre	None.
@post	The bitmap brush pattern is no longer used.
*/
EXPORT_C void CDirectGdiContext::ResetBrushPattern()
	{
	GRAPHICS_TRACE("CDirectGdiContext::ResetBrushPattern");
	CleanUpBrushPattern();
	if (iBrushStyle == DirectGdi::EPatternedBrush)
		{
		iBrushStyle = DirectGdi::ENullBrush;
		iEngine->SetBrushStyle(iBrushStyle);
		}
	iEngine->ResetBrushPattern();
	}


/**
Releases the selected font for this context.

@pre	None.
@post	Internal resources previously allocated during SetFont() are released.

@see	CDirectGdiContext::SetFont()
@see	CGraphicsContext::DiscardFont()
*/
EXPORT_C void CDirectGdiContext::ResetFont()
	{
	GRAPHICS_TRACE("CDirectGdiContext::ResetFont");
	iEngine->ResetFont();
	iFont.Reset();
	}


/**
Draws an arc. An arc is a segment of an ellipse which is defined by a given rectangle. The arc is drawn 
anti-clockwise from the arc start point to the arc end point. The arc start point is the intersection  
between vectors from the centre of the ellipse to the given start position and the ellipse. 
Arc end point is defined in the same way.

@param	aRect	The rectangle which defines where to draw the ellipse.
@param	aStart	Position to be used in defining the arc start point.
@param	aEnd	Position to be used in defining the arc end point.

@pre	The rendering target has been activated.
@post	Request to draw an arc has been accepted. There is no guarantee that the request 
		has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::DrawPie(const TRect&, const TPoint&, const TPoint&)
*/
EXPORT_C void CDirectGdiContext::DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawArc");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if (aRect.IsEmpty() || iPenStyle == DirectGdi::ENullPen || (iPenSize.iWidth == 0) || (iPenSize.iHeight == 0))
		{
		return;
		}
	iEngine->DrawArc(aRect, aStart, aEnd);
	}


/**
Draws and fills a pie. A pie is a shape defined by an arc from the ellipse and straight lines 
from the centre of the ellipse to the arc start and end position. 

@param	aRect	The rectangle which defines where to draw the ellipse
@param	aStart	Position to be used in defining the arc start point
@param	aEnd	Position to be used in defining the arc end point

@pre	The rendering target has been activated.
@post	Request to draw a pie has been accepted. There is no guarantee that the request 
		has been processed when the method returns.

@panic 	DGDI 7, if the rendering context has not been activated.
@panic  DGDI 9, if the brush style is EPatternedBrush but no pattern has been set.

@see	CDirectGdiContext::DrawArc(const TRect&, const TPoint&, const TPoint&)
*/
EXPORT_C void CDirectGdiContext::DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawPie");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iBrushStyle != DirectGdi::EPatternedBrush || iBrushPatternUsed, EDirectGdiPanicBrushPatternNotSet);
	
	if (aRect.IsEmpty())
		{
		return;
		}
	iEngine->DrawPie(aRect, aStart, aEnd);
	}

/**
Draws the bitmap contents into the destination rectangle. Scaling applies when 
the extents of the source bitmap and the destination rectangle do not match.
If the source rectangle is not completely contained within the source 
bitmap's extents, no drawing will take place.

If the client modifies the content of the bitmap after issuing a DrawBitmap() command,
the method does not guarantee that the old bitmap content or the new one
will be drawn. Clients must call Finish() on the driver before modifying the bitmap 
content if they want a guarantee that the previously issued DrawBitmap() will draw the
old bitmap content.

In the event of a failure, the error state is set to one of the system-wide error codes.

@see CDirectGdiContext::DrawBitmap(const TRect&, const CFbsBitmap&, const TRect&)

@param	aDestRect Destination rectangle.
@param	aSourceBitmap Source bitmap.

@pre	The rendering target has been activated.
@post	Request to draw the bitmap content has been accepted.
		There is no guarantee that the request has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::DrawBitmap(%d,%d)", aDestRect.iTl.iX, aDestRect.iTl.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if (ValidateBitmap (aSourceBitmap))
		{
		DrawBitmap(aDestRect, aSourceBitmap,TRect(TPoint(0, 0), aSourceBitmap.SizeInPixels()));		
		}
	}

/**
Draws the bitmap contents into the destination rectangle. Scaling applies when 
the destination and source rectangles do not match. The source bitmap may be 
compressed. The destination rectangle will be clipped with the current clipping 
region. If the source rectangle is not completely contained within the source 
bitmap's extents, no drawing will take place.

If the client modifies the content of the bitmap after issuing a DrawBitmap() command,
the method does not guarantee that the old bitmap content or the new one
will be drawn. Clients must call Finish() on the driver before modifying the bitmap 
content if they want a guarantee that the previously issued DrawBitmap() will draw the
old bitmap content.

In the event of a failure, the error state is set to one of the system-wide error codes.

@see CDirectGdiContext::DrawBitmap(const TRect&, const CFbsBitmap&)

@param	aDestRect		Destination rectangle.
@param	aSourceBitmap	Source bitmap.
@param	aSourceRect		Rectangle specifying the area of the source bitmap to be drawn.

@pre	The rendering target has been activated.
@post	Request to draw the bitmap content has been accepted.
		There is no guarantee that the request has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawBitmap(const TRect& aDestRect, const CFbsBitmap& aSourceBitmap, const TRect& aSourceRect)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::DrawBitmap(%d,%d)", aDestRect.iTl.iX, aDestRect.iTl.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);

	if (ValidateBitmap(aSourceBitmap))
		{
		TSize sourceSize = aSourceBitmap.SizeInPixels();
		// If source rectangle is not fully contained by the extents of the source bitmap,
		// or the size of the source bitmap is zero, do nothing.
		if (aSourceRect.iTl.iX >= 0 &&
			aSourceRect.iTl.iY >= 0 &&
			aSourceRect.iBr.iX <= sourceSize.iWidth &&
			aSourceRect.iBr.iY <= sourceSize.iHeight &&
			!aDestRect.IsEmpty() && !aSourceRect.IsEmpty())
			{
			iEngine->DrawBitmap(aDestRect, aSourceBitmap, aSourceRect);
			}
		}
	}


/**
Draws bitmap content with masking. Scaling applies to both the source bitmap and the mask. 
Both the source and the mask bitmap may be compressed. The destination and source rectangles
operate in a similar way to DrawBitmap().

This function uses either a black and white (binary) mask bitmap, or if the mask's display mode is 
EGray256, alpha blending is used. The result is undefined if the mask pixel value is neither black nor 
white and the mask display mode is other than EGray256.

The mask is aligned with the source bitmap by aligning their first pixels within the source 
rectangle. If the mask size is greater than or equal to the source bitmap size, it will be 
scaled to fit the destination rectangle in the same way the source bitmap is scaled. 
If the mask has zero width or height, the error state is set to KErrArgument and no drawing is performed.

If the mask is smaller than the source bitmap, it will be tiled to fit the source bitmap size, 
and then scaled to fit the destination rectangle.

If the client modifies the content of the bitmap or mask after issuing a DrawBitmapMasked() command,
the method does not guarantee whether the old bitmap or mask contents, or the new ones
will be used. Clients must call Finish() on the driver before modifying the bitmap or mask contents 
if they want a guaranteed result that the previously issued DrawBitmapMasked() will be using the old 
bitmap or mask contents.

In the event of a failure, the error state is set to one of the system-wide error codes.

@param	aDestRect		Destination rectangle.
@param	aSourceBitmap	Source bitmap.
@param	aSourceRect		Rectangle specifying the area of the source bitmap to be drawn.
@param	aMaskBitmap		Mask bitmap.
@param	aInvertMask		If EFalse, a source pixel that is masked by a black pixel is not transferred to 
						the destination rectangle. If ETrue, then a source pixel that is masked by a 
						white pixel is not transferred to the destination rectangle. If alpha blending
						is used instead of masking, this flag is ignored and no inversion takes place.

@pre	The rendering target has been activated.
@post	Request to draw the bitmap content with masking has been accepted.
		There is no guarantee that the request has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawBitmapMasked(
		const TRect& aDestRect,
		const CFbsBitmap& aSourceBitmap,
		const TRect& aSourceRect,
		const CFbsBitmap& aMaskBitmap,
		TBool aInvertMask)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::DrawBitmapMasked(%d,%d)", aDestRect.iTl.iX, aDestRect.iTl.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);

	if (ValidateSourceAndMaskBitmaps(aSourceBitmap, aMaskBitmap))
		{
		TSize sourceSize = aSourceBitmap.SizeInPixels();
		// Ensure source rect is fully within bounds of bitmap extents and
		// dest and source rect are not empty. 
		if (aSourceRect.iTl.iX >= 0 &&
			aSourceRect.iTl.iY >= 0 &&
			aSourceRect.iBr.iX <= sourceSize.iWidth &&
			aSourceRect.iBr.iY <= sourceSize.iHeight &&
			!aDestRect.IsEmpty() && !aSourceRect.IsEmpty())
			{
			iEngine->DrawBitmapMasked(aDestRect, aSourceBitmap, aSourceRect, aMaskBitmap, aInvertMask);
			}
		}
	}

/**
Draws and fills a rectangle with rounded corners. The corner is constructed as an arc of 
an ellipse. The outline is drawn in the current pen colour, size and style if the pen 
colour is not ENullPen. The area inside the rectangle is filled according to the current 
brush colour and style.

If the corner size has zero width or height, a square-cornered rectangle is drawn. If the corner 
size is greater than half the extents of the rectangle, an ellipse is drawn.

@param	aRect The rectangle.
@param	aCornerSize The corner size.

@see	CDirectGdiContext::SetPenSize()
@see	CDirectGdiContext::SetPenStyle()
@see	CDirectGdiContext::SetPenColor()
@see	CDirectGdiContext::SetBrushColor()
@see	CDirectGdiContext::SetBrushStyle()
@see	CDirectGdiContext::SetBrushPattern()

@pre	The rendering target has been activated.
@post	Request to draw a rectangle with rounded corners has been accepted. There is no guarantee 
		that the request has been processed when the method returns.
		
@panic 	DGDI 7, if the rendering context has not been activated.
@panic  DGDI 9, if the brush style is EPatternedBrush but no pattern has been set.
*/
EXPORT_C void CDirectGdiContext::DrawRoundRect(const TRect& aRect, const TSize& aCornerSize)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawRoundRect");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iBrushStyle != DirectGdi::EPatternedBrush || iBrushPatternUsed, EDirectGdiPanicBrushPatternNotSet);
			
	TSize ellsize(aCornerSize);
	ellsize.iWidth <<= 1;
	ellsize.iHeight <<= 1;

	if (aRect.Width() > 0 && aRect.Height() > 0)
		{
		if (ellsize.iWidth < 3 || ellsize.iHeight < 3)
			{
			DrawRect(aRect);
			return;
			}	
		if (aRect.Width() < ellsize.iWidth && aRect.Height() < ellsize.iHeight)
			{
			DrawEllipse(aRect);
			return;
			}
		iEngine->DrawRoundRect(aRect, aCornerSize);
		}
	}

/**
Draws a polyline using the points in an array. A polyline is a series of concatenated straight 
lines joining a set of points. Current pen settings and drawing mode applies. If @c aPointList 
has one element, a plot is performed.

@param	aPointList Array of points specifying points on the polyline.

@pre	The rendering target has been activated.
@post	Request to draw a polyline has been accepted.
		There is no guarantee that the request has been processed when the method returns.
		The internal drawing position is set to the last point in the array.

@panic	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::DrawPolyLineNoEndPoint(const TArray<TPoint>&)
@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
*/
EXPORT_C void CDirectGdiContext::DrawPolyLine(const TArray<TPoint>& aPointList)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawPolyLine");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if ((aPointList.Count() < 1) || iPenStyle == DirectGdi::ENullPen || (iPenSize.iWidth == 0) || (iPenSize.iHeight == 0))
		{
		return;
		}

	if (aPointList.Count() == 1)
		{
		Plot(aPointList[0]);
		}
	else
		{
		iEngine->DrawPolyLine(aPointList);
		}
	}


/**
Draws a polyline using the points in an array. A polyline is a series of concatenated straight 
lines joining a set of points. Current pen settings and drawing mode applies. If @c aPointList 
has less than two elements, no drawing is performed. If @c aPointList has exactly two elements
then a DrawLine is performed.

@param	aPointList Array of points specifying points on the polyline.

@pre	The rendering target has been activated.
@post	Request to draw a polyline has been accepted.
		There is no guarantee that the request has been processed when the method returns.
		The internal drawing position is set to the last point in the array.

@panic	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::DrawPolyLine(const TArray<TPoint>&)
@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
*/
EXPORT_C void CDirectGdiContext::DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawPolyLineNoEndPoint");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);

	const TInt points = aPointList.Count();

	if (points == 1)
		{
		Plot(aPointList[0]);
		}
	else if (points == 2)
		{
		DrawLine(aPointList[0], aPointList[1]);
		}
	else if (points > 2 && !(iPenStyle == DirectGdi::ENullPen || (iPenSize.iWidth == 0) || (iPenSize.iHeight == 0)))
		{
		iEngine->DrawPolyLineNoEndPoint(aPointList);
		}
	}

/**
Draws and fills a polygon defined using an array of points. The first point in the array defines the 
start of the first side of the polygon. The final side of the polygon is drawn using the last point 
from the array, and the line is drawn to the start point of the first side. The outline of the polygon
is drawn using the current pen settings and the area is filled with the current brush settings. 

Self-crossing polygons are filled according to the specified fill rule.

If @c aPointList is empty, no drawing is performed. If it has one element, the result is the same as Plot(). 
If it has two elements, the result is the same as DrawLine().

The error state is set to KErrArgument if aFillRule is an invalid fill rule.

@param	aPointList Array of points specifying the vertices of the polygon.
@param	aFillRule Polygon filling rule.

@pre	The rendering target has been activated.
@post	Request to draw a polygon has been accepted. There is no guarantee that the request has been processed
		when the method returns. The internal drawing position is set to the last point in the array.
		
@panic 	DGDI 7, if the rendering context has not been activated.
@panic  DGDI 9, if the brush style is EPatternedBrush but no pattern has been set.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetBrushStyle(DirectGdi::TBrushStyle)
@see	CDirectGdiContext::SetBrushPattern(const CFbsBitmap&)
@see	CDirectGdiContext::SetBrushOrigin(TPoint)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
@see	DirectGdi::TFillRule
*/
EXPORT_C void CDirectGdiContext::DrawPolygon(const TArray<TPoint>& aPointList, DirectGdi::TFillRule aFillRule)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawPolygon");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iBrushStyle != DirectGdi::EPatternedBrush || iBrushPatternUsed, EDirectGdiPanicBrushPatternNotSet);
	
	if (aFillRule != DirectGdi::EAlternate && aFillRule != DirectGdi::EWinding)
		{
		iDriver.SetError(KErrArgument);
		return;
		}
	
	if (aPointList.Count() == 0)
		{
		return;
		}
	
	iEngine->DrawPolygon(aPointList, aFillRule);
	}


/**
Draws and fills an ellipse inside the given rectangle. Current pen and brush settings apply.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetBrushStyle(DirectGdi::TBrushStyle)
@see	CDirectGdiContext::SetBrushPattern(const CFbsBitmap&)

@param	aRect The rectangle in which to draw the ellipse.

@pre	The rendering target has been activated.
@post	Request to draw an ellipse has been accepted. 
		There is no guarantee that the request has been processed when the method returns.
		
@panic 	DGDI 7, if the rendering context has not been activated.
@panic  DGDI 9, if the brush style is EPatternedBrush but no pattern has been set.
*/
EXPORT_C void CDirectGdiContext::DrawEllipse(const TRect& aRect)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawEllipse");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iBrushStyle != DirectGdi::EPatternedBrush || iBrushPatternUsed, EDirectGdiPanicBrushPatternNotSet);
	
	if (aRect.IsEmpty())
		{
		return;
		}
	iEngine->DrawEllipse(aRect);
	}


/**
Draws a straight line from the start to the end position using current pen size, colour and style. 

@param	aStart	Start position.
@param	aEnd	End position.

@pre	The rendering target has been activated.
@post	Request to draw a straight line with the current pen colour, size and style has been accepted.
		There is no guarantee that the request has been processed when the method returns.
		The internal drawing position is set to @c aEnd.

@panic	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
*/
EXPORT_C void CDirectGdiContext::DrawLine(const TPoint& aStart, const TPoint& aEnd)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawLine");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);

	// Do not draw if start/end at the same point or pensize is 0
	if(aStart == aEnd || iPenStyle == DirectGdi::ENullPen || iPenSize.iWidth == 0 || iPenSize.iHeight == 0)
		{
		return;
		}

	iEngine->DrawLine(aStart, aEnd);
	}


/**
Draws a straight line from the current internal drawing position to a point using the current pen size, colour and style.

@param	aPoint The end-point of the line.

@pre	Rendering target has been activated.
@post	Request to draw a straight line to a specified point with the current pen colour, size and style has been accepted.
		There is no guarantee that the request has been processed when the method returns.
		Internal drawing position is set to @c aPoint.

@panic	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
@see	CDirectGdiContext::MoveTo(TPoint)
@see	CDirectGdiContext::MoveBy(TPoint)
*/
EXPORT_C void CDirectGdiContext::DrawLineTo(const TPoint& aPoint)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawLineTo");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if(iPenStyle == DirectGdi::ENullPen || iPenSize.iWidth == 0 || iPenSize.iHeight == 0)
		{
		return;
		}
	iEngine->DrawLineTo (aPoint);
	}


/**
Draws a straight line relative to the current internal drawing position, using a vector. 
The start point of the line is the current internal drawing position. The vector @c aVector 
is added to the internal drawing position to give the end point of the line.

@param	aVector	The vector to add to the current internal drawing position, giving the end point of the line.

@pre	The rendering target has been activated.
@post	The request to draw a straight line using the vector with current pen colour, size and style has been
		accepted. There is no guarantee that the request has been processed when the method returns. 
		The internal drawing position is set to the end of the line.

@panic	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
@see	CDirectGdiContext::MoveTo(TPoint)
@see	CDirectGdiContext::MoveBy(TPoint)
*/
EXPORT_C void CDirectGdiContext::DrawLineBy(const TPoint& aVector)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawLineBy");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if(iPenStyle == DirectGdi::ENullPen || iPenSize.iWidth == 0 || iPenSize.iHeight == 0)
		{
		return;
		}

	if (aVector != TPoint(0,0))
		{
		iEngine->DrawLineBy(aVector);
		}
	}


/**
Draws and fills a rectangle. The outlines are drawn according to the current pen colour, size and style.
The area inside the rectangle is filled according to the current brush colour and style.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetBrushStyle(DirectGdi::TBrushStyle)
@see	CDirectGdiContext::SetBrushPattern(const CFbsBitmap&)

@param	aRect The rectangle.

@pre	The rendering target has been activated.
@post	Request to draw a rectangle according to the current pen and brush settings has been accepted.
		There is no guarantee that the request has been processed when the method returns.
		
@panic 	DGDI 7, if the rendering context has not been activated.
@panic  DGDI 9, if the brush style is EPatternedBrush but no pattern has been set.
*/
EXPORT_C void CDirectGdiContext::DrawRect(const TRect& aRect)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawRect");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iBrushStyle != DirectGdi::EPatternedBrush || iBrushPatternUsed, EDirectGdiPanicBrushPatternNotSet);
	
	if(aRect.IsEmpty())
		return;
	
	iEngine->DrawRect(aRect);
	}


/**
Draws text at the last print position.

@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetFont(const CFont*)

@param	aText	The text string to be drawn.
@param	aParam	Parameters used in drawing text.

@pre	The rendering target has been activated.
@post	Request to draw the text at the last text position using the current font and pen colour has been accepted.
		There is no guarantee that the request has been processed when the method returns.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush
		style is neither ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawText");
	DrawText(aText, aParam, iLastPrintPosition, DirectGdi::ELeft, CFont::EHorizontal);
	}


/**
Draws text at the specified text position.

@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetFont(const CFont*)

@param	aText		The text string to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aPosition	The position to draw at.

@pre	The rendering target has been activated.
@post	Request to draw the text at the specified position using the current font and pen colour has been accepted.
		There is no guarantee that the request has been processed when the method returns.
@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush
		style is neither ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawText");
	DrawText(aText, aParam, aPosition, DirectGdi::ELeft, CFont::EHorizontal);
	}


/**
Draws text clipped to the specified rectangle.

@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetFont(const CFont*)

@param	aText		The text string to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aClipRect	The clipping rectangle.

@pre	The rendering target has been activated.
@post	Request to draw the text at the last text position using the current font and pen colour, clipped to the specified rectangle has been accepted.
		There is no guarantee that the request has been processed when the method returns.
@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and 
		the brush style is neither ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipRect)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawText");
	DrawText(aText, aParam, iLastPrintPosition, DirectGdi::ELeft, CFont::EHorizontal, &aClipRect);
	}


/**
Draws text clipped to the specified filled rectangle using a baseline offset,
horizontal alignment and a margin.

@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetBrushColor(TRgb)
@see	CDirectGdiContext::SetFont(const CFont*)

@param	aText			The text string to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aClipFillRect	The clipping rectangle (this rect will also be filled before text is plotted).
@param	aBaselineOffset	An offset in pixels for the baseline from the normal position (bottom of the rectangle minus the descent of the font).
@param	aAlignment		Horizontal alignment option relative to the specified rectangle.
@param	aMargin			Offset to add to the position as calculated using specified rectangle.

@pre	The rendering target has been activated.
@post	Request to draw the text within the filled rectangle using the current font and pen colour, offset and clipped to the specified rectangle has been accepted.
		There is no guarantee that the request has been processed when the method returns.
@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush
		style is neither ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset, DirectGdi::TTextAlign aAlignment, TInt aMargin)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawText");
	TPoint p(aClipFillRect.iTl);
	p.iY += aBaselineOffset;
	switch (aAlignment)
		{
		case DirectGdi::ELeft:
			{
			p.iX += aMargin;
			break;
			}
		case DirectGdi::ERight:
			{
			p.iX = aClipFillRect.iBr.iX - aMargin;
			break;
			}
		case DirectGdi::ECenter:
			{
			p.iX += (aClipFillRect.Width() >> 1) + aMargin;
			break;
			}
		default:
			{
			iDriver.SetError(KErrArgument);
			return;
			}		
		}
	DrawText(aText, aParam, p, aAlignment, CFont::EHorizontal, &aClipFillRect, &aClipFillRect);
	}


/**
The private general DrawText routine that implements all the others.

@param	aText		The text to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aPosition	The origin of the text.
@param	aAlignment	Left, centred or right, around aPosition; not used if drawing vertically.
@param	aDirection	Direction: left to right, right to left, or top to bottom.
@param	aClipRect	If non-null, used as a clippingrect when the text is drawn.
@param	aFillRect	If non-null, filled before the text is drawn.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush
		style is neither ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
void CDirectGdiContext::DrawText(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, DirectGdi::TTextAlign aAlignment, 
	CFont::TTextDirection aDirection, const TRect* aClipRect, const TRect* aFillRect)
	{
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	// anything to do?
	if (aClipRect && aClipRect->IsEmpty())
		{
		iDriver.SetError(KErrArgument);
		return;
		}

	GRAPHICS_ASSERT_ALWAYS(iFont.Handle() != 0, EDirectGdiPanicNoFontSelected);
	// This next check actually covers both bitmap and open fonts
	const CBitmapFont* bitmap_font = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmap_font != 0, EDirectGdiPanicNoFontSelected);

	// measure the text
	CFont::TMeasureTextInput measure_text_input;
	measure_text_input.iCharJustNum = iCharJustNum;
	measure_text_input.iCharJustExcess = iCharJustExcess;
	measure_text_input.iWordJustNum = iWordJustNum;
	measure_text_input.iWordJustExcess = iWordJustExcess;
	measure_text_input.iFlags |= CFont::TMeasureTextInput::EFVisualOrder;
	if (aParam)
		{
		GRAPHICS_ASSERT_ALWAYS(aParam->iStart < aParam->iEnd ,EDirectGdiPanicBadParameter);
		measure_text_input.iStartInputChar = aParam->iStart;
		measure_text_input.iEndInputChar = Min(aText.Length(),aParam->iEnd);
		}
	CFont::TMeasureTextOutput measure_text_output;
	const TInt advance = iFont.MeasureText(aText, &measure_text_input, &measure_text_output);
	TRect text_bounds = measure_text_output.iBounds;

	//for linked fonts need an adjustment to the underline postion
	TInt underlineStrikeoutOffset = BaselineCorrection();

	if (iUnderline == DirectGdi::EUnderlineOn)
		{
		TInt underline_top = 0;
		TInt underline_bottom = 0;
		GetUnderlineMetrics(underline_top, underline_bottom);
		underline_top+=underlineStrikeoutOffset;
		underline_bottom+=underlineStrikeoutOffset;
		text_bounds.iTl.iY = Min(text_bounds.iTl.iY, underline_top);
		text_bounds.iBr.iY = Max(text_bounds.iBr.iY, underline_bottom);
		}
	if (iStrikethrough == DirectGdi::EStrikethroughOn)
		{
		TInt strike_top = 0;
		TInt strike_bottom = 0;
		GetStrikethroughMetrics(strike_top, strike_bottom);
		strike_top+=underlineStrikeoutOffset;
		strike_bottom+=underlineStrikeoutOffset;
		text_bounds.iTl.iY = Min(text_bounds.iTl.iY, strike_top);
		text_bounds.iBr.iY = Max(text_bounds.iBr.iY, strike_bottom);
		}
	if (iUnderline == DirectGdi::EUnderlineOn || iStrikethrough == DirectGdi::EStrikethroughOn)
		{
		if (aDirection == CFont::EHorizontal)
			{
			text_bounds.iTl.iX = Min(text_bounds.iTl.iX, 0);
			text_bounds.iBr.iX = Max(text_bounds.iBr.iX, advance);
			}
		else
			{
			text_bounds.iTl.iY = Min(text_bounds.iTl.iY, 0);
			text_bounds.iBr.iY = Max(text_bounds.iBr.iY, advance);
			}
		}

	// work out the text origin and new drawing position
	TPoint text_origin = aPosition;
	if (aDirection != CFont::EVertical)
		{
		const TInt leftSideBearing = Min(text_bounds.iTl.iX, 0);
		const TInt rightSideBearing = Max(text_bounds.iBr.iX, advance);
		switch (aAlignment)
			{
			// We are forbidding side-bearings to leak over the sides here,
			// but still keeping the start and end pen positions within bounds.
			case DirectGdi::ELeft:
				text_origin.iX -= leftSideBearing;
				break;
			case DirectGdi::ERight:
				text_origin.iX -= rightSideBearing;
				break;
			case DirectGdi::ECenter:
				// Centre is the average of left and right
				text_origin.iX -= (leftSideBearing + rightSideBearing) >> 1;
				break;
			default:
				iDriver.SetError(KErrArgument);
				return;
			}
		}
	iLastPrintPosition = text_origin;
	if (aDirection == CFont::EHorizontal)
		{
		iLastPrintPosition.iX += advance;
		}
	else
		{
		iLastPrintPosition.iY += advance;
		}
	text_origin.iY += bitmap_font->iAlgStyle.iBaselineOffsetInPixels;
	text_bounds.Move(text_origin);
	text_origin += iOrigin;

	// determine clipping rectangle
	TRect clipRect = aClipRect ? *aClipRect : text_bounds;

	// fill the box if necessary
	if (aFillRect && (iBrushStyle != DirectGdi::ENullBrush))
		{
		TRect fillBox = *aFillRect;
		if (fillBox.Intersects(clipRect))
			{
			fillBox.Intersection(clipRect);
			iEngine->SetPenStyle(DirectGdi::ENullPen);	// Fill box, don't outline it
			iEngine->DrawRect(fillBox);
			iEngine->SetPenStyle(iPenStyle);		// Put the pen style back
			}
		}
	if (!aText.Length())
		{		
		return;
		}

	clipRect.Move(iOrigin);

	// decide which drawing routine to call

	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	const TInt maxwidth = metrics.MaxWidth();
	// extext will be TRUE, if font is underline/strikethrough/anti-aliased or it has shadow/outline effects ON.
	// Depending on these properties it will call the proper draw routine.
	TBool extext = EFalse;
	TBool normaltext = EFalse;
	const TBool antiAliased = (bitmap_font->GlyphBitmapType() == EAntiAliasedGlyphBitmap);
	const TBool outlineAndShadow = (bitmap_font->GlyphBitmapType() == EFourColourBlendGlyphBitmap);
	if (antiAliased || outlineAndShadow )
		{
		if ((outlineAndShadow) && !((iBrushStyle == DirectGdi::ENullBrush) || (iBrushStyle == DirectGdi::ESolidBrush)))
			{
			//For future compatibility it is better if brush style of ENullBrush or ESolidBrush is used 
			//when drawing outline and shadow fonts.
			GRAPHICS_PANIC_ALWAYS(EDirectGdiPanicInvalidBrushStyle);
			}
		extext = ETrue;
		}
	else if ((iUnderline == DirectGdi::EUnderlineOn) || (iStrikethrough == DirectGdi::EStrikethroughOn) || (iCharJustNum > 0) || (iWordJustNum > 0))
		extext = ETrue;
	else
		normaltext = ETrue;

	const TInt charjustexcess = iCharJustExcess;
	const TInt charjustnum = iCharJustNum;
	const TInt wordjustexcess = iWordJustExcess;
	const TInt wordjustnum = iWordJustNum;

	// Set up the parameter block for character positioning.
	CFont::TPositionParam param;
	param.iDirection = static_cast<TInt16>(aDirection);
	param.iText.Set(aText);
	TInt endDraw = aText.Length();
	if (aParam)
		{
		param.iPosInText = aParam->iStart;
		endDraw = Min(aText.Length(),aParam->iEnd);
		}
	else
		{
		param.iPosInText = 0;
		}
	param.iPen = text_origin;

	// Draw the text.
	if (normaltext)
		{
		DoDrawText(param, endDraw, clipRect);
		}
	else if (extext)
		{
		DoDrawTextEx(param, endDraw, clipRect,underlineStrikeoutOffset);
		}

	// Reset the justification parameters to their original values.
	// These will be updated as required later in code.
	iCharJustExcess = charjustexcess;
	iCharJustNum = charjustnum;
	iWordJustExcess = wordjustexcess;
	iWordJustNum = wordjustnum;

	if (iAutoUpdateJustification)
		UpdateJustification(aText, aParam);
	}

/**
Overridden  function which draws monochrome text within the given clip rectangle. No rotation applied.
@param	aParam	Defines glyph code, ligature creation and diacritic placement.
@param	aEnd	The end position within the text descriptor to draw.
@param	aClipRect If not-empty, used as a clippingrect when the text is drawn.
*/
void CDirectGdiContext::DoDrawText(CFont::TPositionParam& aParam, const TInt aEnd, const TRect& aClipRect)
	{
	iEngine->BeginDrawGlyph();
	RShapeInfo shapeInfo;
	while (aParam.iPosInText < aEnd)
		{
		if (iFont.GetCharacterPosition2(aParam, shapeInfo))
			{
			const CFont::TPositionParam::TOutput* output = aParam.iOutput;
			for (TInt i = 0; i < aParam.iOutputGlyphs; ++i, ++output)
				iEngine->DrawGlyph(output->iBounds.iTl, output->iCode, output->iBitmap, EMonochromeGlyphBitmap, output->iBitmapSize, aClipRect); // All other parameters are default
			}
		}
	iEngine->EndDrawGlyph();
	
	if (shapeInfo.IsOpen())
		shapeInfo.Close();
	}


/**
Overridden  function which draws monochrome text within the given clip rectangle. 
The current rotation and font style (strikethrough, underline) are applied.

@param	aParam	Defines glyph code, ligature creation and diacritic placement.
@param	aEnd	The end position within the text descriptor to draw.
@param	aClipRect If not-empty, used as a clipping rect when the text is drawn.
@param  aUnderlineStrikethroughOffset the offset for the underline, passed to save calculating this value again
*/
void CDirectGdiContext::DoDrawTextEx(CFont::TPositionParam& aParam, const TInt aEnd, const TRect& aClipRect, const TInt aUnderlineStrikethroughOffset)
	{
	TPoint startPen = aParam.iPen;
	const CBitmapFont* bitmap_font = iFont.Address();
	TInt underlineTop = 0;
	TInt underlineBottom = 0;
	if (iUnderline == DirectGdi::EUnderlineOn)
		{
		GetUnderlineMetrics(underlineTop, underlineBottom);
		underlineTop+=aUnderlineStrikethroughOffset;
		underlineBottom+=aUnderlineStrikethroughOffset;
		}
	TInt strikeTop = 0;
	TInt strikeBottom = 0;
	if (iStrikethrough == DirectGdi::EStrikethroughOn)
		{
		GetStrikethroughMetrics(strikeTop, strikeBottom);
		strikeTop+=aUnderlineStrikethroughOffset;
		strikeBottom+=aUnderlineStrikethroughOffset;
		}

	iEngine->BeginDrawGlyph();
	RShapeInfo shapeInfo;
	while (aParam.iPosInText < aEnd)
		{
		if (!iFont.GetCharacterPosition2(aParam, shapeInfo))
			{
			continue;
			}

		TInt adjustment = 0;
		if ((iCharJustExcess > 0) && (iCharJustNum > 0)) // character clipping/justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iCharJustExcess, iCharJustNum);
			}

		const CFont::TPositionParam::TOutput* output = aParam.iOutput;
		for (TInt i = 0; i < aParam.iOutputGlyphs; ++i, ++output)
			{
			//get the character metrics for the glyph type
			TOpenFontCharMetrics characterParams;
			const TUint8* bitmap;
			TSize size;
			//note may now be using a glyph code, and not a character
			iFont.GetCharacterData(aParam.iOutput[i].iCode,characterParams,bitmap,size);
			TGlyphBitmapType glyphType = characterParams.GlyphType();
			
			switch (glyphType)
				{
				case EAntiAliasedGlyphBitmap:
				case EFourColourBlendGlyphBitmap:
				case EDefaultGlyphBitmap:
					iEngine->DrawGlyph(output->iBounds.iTl, output->iCode, output->iBitmap, glyphType, output->iBitmapSize, aClipRect);
					break;
					
				default:
					//if the outline or shadow is not specified for the character, then use the font setting for the glyph bitmap type
					iEngine->DrawGlyph(output->iBounds.iTl, output->iCode, output->iBitmap, 
									   bitmap_font->GlyphBitmapType(), output->iBitmapSize, aClipRect);		 
					break;
				}
			}

		if (adjustment)
			{
			aParam.iPen.iX += adjustment;
			}
		if ((iWordJustExcess > 0) && (iWordJustNum > 0) && (aParam.iOutput[0].iCode == 0x0020)) // word justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iWordJustExcess, iWordJustNum);
			aParam.iPen.iX += adjustment;
			}
		}
	iEngine->EndDrawGlyph();
	if (shapeInfo.IsOpen())
		shapeInfo.Close();

	if (iUnderline == DirectGdi::EUnderlineOn)
		{
		TRect underlineRect(startPen.iX, startPen.iY + underlineTop, aParam.iPen.iX, startPen.iY + underlineBottom);
		FillRect(underlineRect, iPenColor, aClipRect);
		}

	if (iStrikethrough == DirectGdi::EStrikethroughOn)
		{
		TRect strikethroughRect(startPen.iX, startPen.iY + strikeTop, aParam.iPen.iX, startPen.iY + strikeBottom);
		FillRect(strikethroughRect, iPenColor, aClipRect);
		}
	}


/**
Fills the given rectangle with the specified colour (subject to the clip rect).
This function is internal and used by the text drawing routines.

@param	aRect	The rectangle to fill.
@param	aColor	The colour to fill it with.
@param	aClipRect	The clipping rect.
*/
void CDirectGdiContext::FillRect(const TRect& aRect, const TRgb& aColor, const TRect& aClipRect)
	{
	TRect fillRect = aRect;
	if (fillRect.Intersects(aClipRect))
		{
		fillRect.Intersection(aClipRect);
		// Override the current settings temporarily
		iEngine->SetBrushColor(aColor);
		iEngine->SetBrushStyle(DirectGdi::ESolidBrush);
		iEngine->SetPenStyle(DirectGdi::ENullPen);	// Fill box, don't outline it
		fillRect.Move(-iOrigin);
		iEngine->DrawRect(fillRect);
		// Put things back
		iEngine->SetPenStyle(iPenStyle);
		iEngine->SetBrushStyle(iBrushStyle);
		iEngine->SetBrushColor(iBrushColor);
		}
	}


/**
Draws text at the last print position and then rotates it into a vertical position.

@param	aText	The text string to be drawn.
@param	aParam	Parameters used in drawing text.
@param	aUp		If ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush
		style is neither ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawTextVertical");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	// This next check covers both bitmap and open fonts
	const CBitmapFont* bitmapFont = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmapFont != 0, EDirectGdiPanicNoFontSelected);
	TRect clipRect2(0, 0, 0, 0);
	TInt baselineOffset = 0;
	TInt margin = 0;
	CalculateClipRect2PlusBaselineOffsetAndMargin(aText, aParam, iLastPrintPosition, aUp, clipRect2, baselineOffset, margin);
	DrawTextVertical(aText, aParam, NULL, &clipRect2, NULL, baselineOffset, -1, aUp, DirectGdi::ELeft, margin); //-1 signifies that text will not be clipped
	}


/**
Draws text vertically from the specified position.

@param	aText		The text string to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aPosition	A point specifying the position of the left end of the text.
@param	aUp			If ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush style is neither 
		ENullBrush nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawTextVertical");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	// This next check covers both bitmap and open fonts
	const CBitmapFont* bitmapFont = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmapFont != 0, EDirectGdiPanicNoFontSelected);
	TRect clipRect2(0, 0, 0, 0);
	TInt baselineOffset = 0;
	TInt margin = 0;
	CalculateClipRect2PlusBaselineOffsetAndMargin(aText, aParam, aPosition, aUp, clipRect2, baselineOffset, margin);
	DrawTextVertical(aText, aParam, NULL, &clipRect2, NULL, baselineOffset, -1, aUp, DirectGdi::ELeft, margin);//-1 signifies that text will not be clipped
	}


/**
Draws text clipped to the specified rectangle and then rotates it into a vertical position.

@param	aText		The text string to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aClipRect	The clipping rectangle.
@param	aUp			ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush style is neither ENullBrush 
		nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipRect, TBool aUp)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawTextVertical");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);	
	// This next check covers both bitmap and open fonts
	const CBitmapFont* bitmapFont = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmapFont != 0, EDirectGdiPanicNoFontSelected);
	TRect clipRect2(0, 0, 0, 0);
	TInt baselineOffset = 0;
	TInt margin = 0;
	CalculateClipRect2PlusBaselineOffsetAndMargin(aText, aParam, iLastPrintPosition, aUp, clipRect2, baselineOffset, margin);
	DrawTextVertical(aText, aParam, &aClipRect, &clipRect2, NULL, baselineOffset, -1, aUp, DirectGdi::ELeft, margin);
	}


/**
Private internal function for calculating several parameters needed by these routines.

@param	aText		The text string to be drawn.
@param	aParam		Parameters used in drawing text.
@param	aPosition	A point specifying the position of the left end of the text.
@param	aUp			ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.
@param	aClipRect2		On return, contains clipping rectangle.
@param	aBaselineOffset	On return, contains baseline offset.
@param	aMargin			On return, contains margin.
*/
void CDirectGdiContext::CalculateClipRect2PlusBaselineOffsetAndMargin(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TPoint& aPosition, TBool aUp, TRect& aClipRect2, TInt& aBaselineOffset, TInt& aMargin)
	{
	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	aBaselineOffset = metrics.MaxHeight();
	TInt height = aBaselineOffset + metrics.MaxDepth();
	// The next few lines do much the same as TextWidthInPixels but pass
	// the text in visual order instead of logical order and also take
	// full account of left and right side bearings on the text
	CFont::TMeasureTextOutput output;
	CFont::TMeasureTextInput input;
	input.iFlags = CFont::TMeasureTextInput::EFVisualOrder;
	if (aParam)
		{
		GRAPHICS_ASSERT_ALWAYS(aParam->iStart < aParam->iEnd ,EDirectGdiPanicBadParameter);
		input.iStartInputChar = aParam->iStart;
		input.iEndInputChar = Min(aText.Length(),aParam->iEnd);
		}
	TInt advance = iFont.MeasureText(aText, &input, &output);
	TInt leftBearing = output.iBounds.iTl.iX;
	TInt rightBearing = advance - output.iBounds.iBr.iX;
	aMargin = 0;
	if (aUp)
		{
		aClipRect2.iTl.iX = aPosition.iX - aBaselineOffset;
		aClipRect2.iTl.iY = aPosition.iY - advance;
		aClipRect2.iBr.iX = aPosition.iX + height - aBaselineOffset + 1;
		aClipRect2.iBr.iY = aPosition.iY;
		if (leftBearing < 0)
			{
			aClipRect2.iBr.iY -= leftBearing;
			aMargin = -leftBearing;
			}
		if (rightBearing < 0)
			{
			aClipRect2.iTl.iY += rightBearing;
			}
		}
	else
		{
		aClipRect2.iTl.iX = aPosition.iX + aBaselineOffset- height;
		aClipRect2.iTl.iY = aPosition.iY;
		aClipRect2.iBr.iX = aPosition.iX + aBaselineOffset + 1;
		aClipRect2.iBr.iY = aPosition.iY + advance;
		if (leftBearing < 0)
			{
			aClipRect2.iTl.iY += leftBearing;
			aMargin = -leftBearing;
			}
		if (rightBearing < 0)
			{
			aClipRect2.iBr.iY -= rightBearing;
			}
		}
	}


/**
Draws text vertically, clipped to a specified rectangle, using a baseline offset, alignment and margin.

@param	aText			The text string to be drawn.
@param	aParam			Parameters used in drawing text.
@param	aClipFillRect	The clipping rectangle (this rect will also be filled before text is plotted).
@param	aBaselineOffset	Number of pixels to offset the baseline by.
@param	aUp				ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.
@param	aVert			Vertical alignment of the text relative to the specified rectangle.
@param	aMargin			Offset of the text from the position within the rectangle, using the specified alignment.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush style is neither ENullBrush 
		nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset,
												  TBool aUp, DirectGdi::TTextAlign aVert, TInt aMargin)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawTextVertical");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	// This next check covers both bitmap and open fonts
	const CBitmapFont* bitmapFont = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmapFont != 0, EDirectGdiPanicNoFontSelected);
	DrawTextVertical(aText, aParam, NULL, &aClipFillRect, &aClipFillRect, aBaselineOffset, -1, aUp, aVert, aMargin);
	}


/**
Draws text vertically, clipped to a specified rectangle, using a baseline offset, alignment and margin.

@param	aText			The text string to be drawn.
@param	aParam			Parameters used in drawing text.
@param	aClipFillRect	The clipping rectangle (this rect will also be filled before text is plotted).
@param	aBaselineOffset	Number of pixels to offset the baseline by.
@param	aTextWidth		Number of pixels to clip the text to.
@param	aUp				ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.
@param	aVert			Vertical alignment of the text relative to the specified rectangle.
@param	aMargin			Offset of the text from the position within the rectangle, using the specified alignment.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush style is neither ENullBrush 
		nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
*/
EXPORT_C void CDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect& aClipFillRect, TInt aBaselineOffset,
												  TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVert, TInt aMargin)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawTextVertical");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	// This next check covers both bitmap and open fonts
	const CBitmapFont* bitmapFont = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmapFont != 0, EDirectGdiPanicNoFontSelected);
	DrawTextVertical(aText, aParam, NULL, &aClipFillRect, &aClipFillRect, aBaselineOffset, aTextWidth, aUp, aVert, aMargin);
	}


/**
The private general DrawTextVertical() routine that implements all the others.
Two clipping rectangles received from different routines. The final rectangle will be calculated as intersection 
of first and second clipping rectangle. If aClipRect2 is empty, the error state is set to KErrArgument.

@param	aText			The text string to be drawn.
@param	aParam			Parameters used in drawing text.
@param	aClipRect1		Pointer to first clipping rectangle.
@param	aClipRect2		Pointer to second clipping rectangle. 
@param	aFillRect		Pointer to rectangle to be filled before text plotting.
@param	aBaselineOffset	Number of pixels to offset the baseline by.
@param	aTextWidth		Number of pixels to clip the text to. If negative, the text will not be clipped
@param	aUp				ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.
@param	aVert			Vertical alignment of the text relative to the specified rectangle.
@param	aMargin			Offset of the text from the position within the rectangle, using the specified alignment.

@panic	DGDI 10, if the active font is an outline and/or shadow font and the brush style is neither ENullBrush 
		nor ESolidBrush.
@panic	DGDI 11, if a font has not been set prior to calling DrawText().
@panic  DGDI 22, if aClipRect2 is NULL.
*/
void CDirectGdiContext::DrawTextVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, const TRect* aClipRect1, const TRect* aClipRect2, const TRect* aFillRect,
		TInt aBaselineOffset, TInt aTextWidth, TBool aUp, DirectGdi::TTextAlign aVert, TInt aMargin)
	{
	GRAPHICS_ASSERT_ALWAYS(aClipRect2, EDirectGdiPanicBadParameter);

	TRect clipRect2 = *aClipRect2;
	clipRect2.Move(iOrigin);

	TRect clipRect(clipRect2);
	if (aClipRect1 != NULL)
		{
		if(aClipRect1->IsEmpty())
			{
			iDriver.SetError(KErrArgument);
			return;
			}
		TRect clipRect1 = *aClipRect1;
		clipRect1.Move(iOrigin);
		clipRect.Intersection(clipRect1);
		}
	
	if ((aFillRect != NULL) && (iBrushStyle != ENullBrush))
		{
		// fill the box if necessary
		TRect fillBox = *aFillRect;
		fillBox.Move(iOrigin);
		if (fillBox.Intersects(clipRect))
			{
			fillBox.Intersection(clipRect);
			iEngine->SetPenStyle(DirectGdi::ENullPen);	// Fill box, don't outline it
			iEngine->DrawRect(*aFillRect);
			iEngine->SetPenStyle(iPenStyle);		// Put the pen style back
			}
		}
	if (!aText.Length())
		{		
		return;
		}
	if (aClipRect2->IsEmpty())
		{
		iDriver.SetError(KErrArgument);
		return;
		}
	
	const CBitmapFont* bitmapFont = iFont.Address();
	GRAPHICS_ASSERT_ALWAYS(bitmapFont != 0, EDirectGdiPanicNoFontSelected);

	CFont::TMeasureTextInput input;
	//CFont::TMeasureTextOutput
	if (aParam)
		{
		GRAPHICS_ASSERT_ALWAYS(aParam->iStart < aParam->iEnd ,EDirectGdiPanicBadParameter);
		input.iStartInputChar = aParam->iStart;
		input.iEndInputChar = Min(aText.Length(),aParam->iEnd);
		}
	TInt width = iFont.MeasureText(aText,&input);
	TOpenFontMetrics metrics;
	iFont.GetFontMetrics(metrics);
	
	if (aTextWidth < 0)
		{
		aTextWidth = width;
		}
	TPoint coords;
	coords.iX = clipRect2.iTl.iX;
	TInt directionalMultiplier = aUp ? -1 : 1;
	coords.iY = aUp ? clipRect2.iBr.iY - 1 : clipRect2.iTl.iY;
	//
	// iX calculation, for example: ascent(a)=18 descent(d)=2 size=boxwidth=fontheight(h)=20 baseline=ascent
	// pre: iX = 0
	//
	// hhhhhhhhhhhhhhhhhhhh
	// 01234567890123456789
	// aaaaaaaaaaaaaaaaaadd	aUp=ETrue
	//                   ^
	//                   iX = 18 (baseline)
	//
	// ddaaaaaaaaaaaaaaaaaa aUp=EFalse
	//  ^
	//  iX = 1 (instead of 2 ie 20-18-1 which is boxwidth-baseline-1)
	//
	coords.iX += aUp ? aBaselineOffset : clipRect2.Width() - aBaselineOffset - 1;
	switch (aVert)
		{
	case DirectGdi::ELeft:
		coords.iY += aMargin * directionalMultiplier;
		break;
	case DirectGdi::ECenter:
		coords.iY += (((clipRect2.iBr.iY - clipRect2.iTl.iY - aTextWidth) >> 1) + aMargin) * directionalMultiplier;
		break;
	case DirectGdi::ERight:
		coords.iY += (clipRect2.iBr.iY - clipRect2.iTl.iY - aTextWidth - aMargin) * directionalMultiplier;
		break;
	default:
		iDriver.SetError(KErrArgument);
		return;
		}
	iLastPrintPosition = coords;
	coords.iX += bitmapFont->iAlgStyle.iBaselineOffsetInPixels * directionalMultiplier;
	TInt prewidth = width + iCharJustExcess + iWordJustExcess;
	iLastPrintPosition.iY -= aUp ? prewidth - 1 : -prewidth;
	if (clipRect.IsEmpty() || !width)
		{
		if (iAutoUpdateJustification)
			{
			UpdateJustificationVertical(aText, aParam, aUp);
			}
		return;
		}

	/*
	Set up the parameter block for character positioning.
	Draw left to right, because although the text is being drawn vertically,
	it is done by rotating the baseline 90 degrees and drawing in the ordinary way, not by drawing
	the characters in their normal orientation but in a vertical column.
	*/
	CFont::TPositionParam param;
	param.iText.Set(aText);
	param.iPen = coords;
	TInt endDraw = aText.Length();
	if (aParam)
		{
		param.iPosInText = aParam->iStart;
		endDraw = Min(aText.Length(),aParam->iEnd);
		}
	else
		{
		param.iPosInText = 0;
		}

	// Draw the text.
	DoDrawTextVertical(param, aUp, endDraw, clipRect);
	if(iAutoUpdateJustification)
		{
		UpdateJustificationVertical(aText, aParam, aUp);
		}
	}


/**
Draws vertical text within the clipping area

@param	aParam		Defines glyph code, ligature creation and diacritic placement 
@param	aUp			ETrue, text is rotated 90 degrees anti-clockwise; EFalse, text is rotated 90 degrees clockwise.
@param	aEnd		The end position within the text descriptor to draw.
@param	aClipRect	The clipping rectangle.

@pre iFont is a valid CFont.

@panic	DGDI 13, if the active font is an outline and/or shadow font and the brush style is neither ENullBrush 
		nor ESolidBrush.
*/
void CDirectGdiContext::DoDrawTextVertical(CFont::TPositionParam& aParam, TBool aUp, const TInt aEnd, TRect& aClipRect)
	{
	const CBitmapFont* bitmapFont = iFont.Address();
	if ((bitmapFont->GlyphBitmapType() == EFourColourBlendGlyphBitmap) && !((iBrushStyle == DirectGdi::ENullBrush) || (iBrushStyle == DirectGdi::ESolidBrush)))
		{
		//For future compatibility it is better if brush style of ENullBrush or ESolidBrush is used 
		//when drawing outline and shadow fonts.
		GRAPHICS_PANIC_ALWAYS(EDirectGdiPanicInvalidBrushStyle);
		}

	TPoint startPen = aParam.iPen;
	TInt charClipping = aClipRect.iTl.iY;
	TInt underlineTop = 0;
	TInt underlineBottom = 0;

	//for linked fonts need an adjustment to the underline postion
	TInt underlineStrikeoutOffset = BaselineCorrection();

	if (iUnderline == DirectGdi::EUnderlineOn)
		{
		GetUnderlineMetrics(underlineTop, underlineBottom);
		underlineTop+=underlineStrikeoutOffset;
		underlineBottom+=underlineStrikeoutOffset;
		}
	TInt strikeTop = 0;
	TInt strikeBottom = 0;
	if (iStrikethrough == DirectGdi::EStrikethroughOn)
		{
		GetStrikethroughMetrics(strikeTop, strikeBottom);
		strikeTop+=underlineStrikeoutOffset;
		strikeBottom+=underlineStrikeoutOffset;
		}

	const DirectGdi::TGraphicsRotation rotation = aUp ? DirectGdi::EGraphicsRotation270 : DirectGdi::EGraphicsRotation90;
	iEngine->BeginDrawGlyph();
	RShapeInfo shapeInfo;
	while (aParam.iPosInText < aEnd)
		{
		TPoint startPen2 = aParam.iPen;
		if (!iFont.GetCharacterPosition2(aParam, shapeInfo))
			{
			continue;
			}
		Rotate(aParam.iPen, startPen2, aUp);
		TInt adjustment = 0;
		if(iCharJustExcess && (iCharJustNum > 0)) // character clipping/justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iCharJustExcess, iCharJustNum);
			if (adjustment < 0)
				{
				aClipRect.iTl.iY = aParam.iPen.iY + (aUp ? -adjustment : adjustment);
				}
			}

		CFont::TPositionParam::TOutput* output = aParam.iOutput;
		for (TInt i = 0; i < aParam.iOutputGlyphs; i++, output++)
			{
			Rotate(output->iBounds.iTl, startPen2, aUp);

			//get the character metrics for the glyph type
			TOpenFontCharMetrics characterParams;
			const TUint8* bitmap;
			TSize size;
			//note may now be using a glyph code, and not a character
			iFont.GetCharacterData(aParam.iOutput[i].iCode,characterParams,bitmap,size);
			TGlyphBitmapType glyphType = characterParams.GlyphType();
			
			switch (glyphType)
				{
				case EAntiAliasedGlyphBitmap:
				case EFourColourBlendGlyphBitmap:
				case EDefaultGlyphBitmap:
					iEngine->DrawGlyph(output->iBounds.iTl, output->iCode, output->iBitmap, glyphType, output->iBitmapSize, aClipRect, rotation);
					break;

				default:
					//if the outline or shadow is not specified for the character, then use the font setting
					iEngine->DrawGlyph(output->iBounds.iTl, output->iCode, output->iBitmap, bitmapFont->GlyphBitmapType(), output->iBitmapSize, aClipRect, rotation);	 
					break;
				}			
			}

		aClipRect.iTl.iY = charClipping;
		if (adjustment)
			{
			aParam.iPen.iY += aUp ? -adjustment : adjustment;
			}
		if ((iWordJustExcess > 0) && (iWordJustNum > 0) && (aParam.iOutput[0].iCode == 0x0020)) // word justification
			{
			adjustment = CGraphicsContext::JustificationInPixels(iWordJustExcess, iWordJustNum);
			aParam.iPen.iY += aUp ? -adjustment : adjustment;
			}
		}
	iEngine->EndDrawGlyph();
	if (shapeInfo.IsOpen())
		{
		shapeInfo.Close();
		}

	if (iUnderline == DirectGdi::EUnderlineOn)
		{
		TRect underlineRect; // underline
		if (aUp)
			{
			underlineRect.SetRect(startPen.iX + underlineTop, aParam.iPen.iY, startPen.iX + underlineBottom, startPen.iY + 1);
			underlineRect.iTl.iY = underlineRect.iBr.iY - underlineRect.Height();
			}
		else
			{
			underlineRect.SetRect(startPen.iX - underlineBottom, startPen.iY, startPen.iX - underlineTop, aParam.iPen.iY);
			underlineRect.iBr.iY = underlineRect.iTl.iY + underlineRect.Height();
			underlineRect.iTl.iX++; // adjust for rect not including last line
			underlineRect.iBr.iX++;
			}
		FillRect(underlineRect, iPenColor, aClipRect);
		}

	if (iStrikethrough == DirectGdi::EStrikethroughOn)
		{
		TRect strikethroughRect; // strikethrough
		if (aUp)
			{
			strikethroughRect.SetRect(startPen.iX + strikeTop, aParam.iPen.iY, startPen.iX + strikeBottom, startPen.iY + 1);
			strikethroughRect.iTl.iY = strikethroughRect.iBr.iY - strikethroughRect.Height();
			}
		else
			{
			strikethroughRect.SetRect(startPen.iX - strikeBottom, startPen.iY, startPen.iX - strikeTop, aParam.iPen.iY);
			strikethroughRect.iBr.iY = strikethroughRect.iTl.iY + strikethroughRect.Height();
			strikethroughRect.iTl.iX++;
			strikethroughRect.iBr.iX++;
			}
		FillRect(strikethroughRect, iPenColor, aClipRect);
		}
	}


/**
Transform a vector, defined by a point relative to an origin, from left-to-right to up or down.

@param aPoint A point relative to the origin aOrigin.
@param aOrigin The origin to use when transforming the point aPoint.
@param aUp If ETrue, then transform the point from left-right to up, otherwise transform from 
left-right to down.
*/
void CDirectGdiContext::Rotate(TPoint& aPoint, const TPoint& aOrigin, TBool aUp)
	{
	TInt dx = aPoint.iX - aOrigin.iX;
	TInt dy = aPoint.iY - aOrigin.iY;
	if (aUp)
		{
		aPoint.iX = aOrigin.iX + dy;
		aPoint.iY = aOrigin.iY - dx;
		}
	else
		{
		aPoint.iX = aOrigin.iX - dy;
		aPoint.iY = aOrigin.iY + dx;
		}
	}


/**
Can be used to find out the top and bottom of an underline for the active font.
This allows correct calculation of the area required in which to draw text with underline.

@param	aTop The top of the underline position.
@param	aBottom The bottom of the underline position.
*/
void CDirectGdiContext::GetUnderlineMetrics(TInt& aTop, TInt& aBottom)
	{
	const TInt width = Max((iFont.HeightInPixels() / 10), 1);
	aTop = 1 + (width >> 1);
	aBottom = aTop + width;
	}


/**
Get the top and bottom of a strikethrough line for the current font, relative to the baseline.

@param	aTop The top of the strikethrough position.
@param	aBottom The bottom of the strikethrough position.
*/
void CDirectGdiContext::GetStrikethroughMetrics(TInt& aTop, TInt& aBottom)
	{
	aTop = -(iFont.AscentInPixels() * 5/12) - 1;
	aBottom = aTop + Max((iFont.HeightInPixels() / 10), 1);
	}


/**
Moves the internal drawing position relative to the co-ordinate origin, without drawing a line.
A subsequent call to DrawLineTo() or DrawLineBy() will then use the new internal drawing position
as the start point for the line drawn. 

The operations DrawLine(), DrawLineTo(), DrawLineBy() and DrawPolyline() also change the internal drawing 
position to the last point of the drawn line(s). The internal drawing position is set to the co-ordinate
origin if no drawing or moving operations have yet taken place.

@see	CDirectGdiContext::MoveBy(const TPoint&)

@param	aPoint The point to move the internal drawing position to.

@pre	The rendering target has been activated.
@post	Request to move the internal drawing position to the specified point has been accepted. 
		There is no guarantee that the request has been processed when the method returns.

@panic 	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::MoveTo(const TPoint& aPoint)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::MoveTo(%d,%d)", aPoint.iX, aPoint.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	iEngine->MoveTo(aPoint);
	}


/**
Moves the internal drawing position by a vector, relative to the current position, without drawing a line.
A subsequent call to DrawLineTo() or DrawLineBy() will then use the new internal drawing position
as the start point for the line drawn.

The operations DrawLine(), DrawLineTo(), DrawLineBy() and DrawPolyline() also change the internal drawing 
position to the last point of the drawn line(s). The internal drawing position is set to the co-ordinate
origin if no drawing or moving operations have yet taken place.

@see	CDirectGdiContext::MoveTo(const TPoint&)

@param	aVector	The vector to move the internal position by.

@pre	The rendering target has been activated.
@post	Request to move the internal drawing position by a vector has been accepted.
		There is no guarantee that the request has been processed when the method returns.
@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::MoveBy(const TPoint& aVector)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::MoveBy(%d,%d)", aVector.iX, aVector.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if (aVector != TPoint(0,0))
		{
		iEngine->MoveBy(aVector);
		}
	}


/**
Draws a point at given location using current pen colour and size. 
If the pen size is greater than 1x1 pixel, a filled circle/ellipse with current pen 
colour should be drawn with the given position as the centre.

@param	aPoint The position to plot.

@pre	The rendering target has been activated.
@post	Request to draw a point or filled circle/ellipse has been accepted.
		There is no guarantee that the request has been processed when the method returns.

@panic 	DGDI 7, if the rendering context has not been activated.

@see	CDirectGdiContext::SetPenSize(const TSize&)
@see	CDirectGdiContext::SetPenColor(TRgb)
@see	CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode)
*/
EXPORT_C void CDirectGdiContext::Plot(const TPoint& aPoint)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::Plot(%d,%d)", aPoint.iX, aPoint.iY);
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	if (iPenStyle == DirectGdi::ENullPen || iPenSize.iWidth == 0 || iPenSize.iHeight == 0)
		{
		return;
		}	
	iEngine->Plot(aPoint);
	}


/**
Resets drawing state to its default settings. This operation does not unbind the current target. 
@pre	None.
@post	The drawing state is reset to default values. Subsequent drawing will use the default settings until they are changed to different values.
*/
EXPORT_C void CDirectGdiContext::Reset()
	{
	GRAPHICS_TRACE("CDirectGdiContext::Reset");
	iEngine->Reset();

	// Explicit calls are made to the engine to apply the defaults. 
	// Set() methods should generally not be used as they perform unnecessary checks, and may also 
	// block the call to the engine if the value being set is the same as the constructor's default.	
	// Clipping regions.
	iClippingRegion.Clear();
	iEngine->ResetClippingRegion();
	// Origin.
	iOrigin.SetXY(0,0);
	iEngine->SetOrigin(iOrigin);
	// Font.
	iEngine->ResetFont();
	iFont.Reset();
	// Text.
	iLastPrintPosition.SetXY(0,0);
	iAutoUpdateJustification = ETrue;
	SetCharJustification(0, 0);
	SetWordJustification(0, 0);
	SetStrikethroughStyle(DirectGdi::EStrikethroughOff);
	SetUnderlineStyle(DirectGdi::EUnderlineOff);
	// Pen colour.
	iPenColor = KRgbBlack;
	iEngine->SetPenColor(iPenColor);
	// Pen size.
	iPenSize = TSize(1,1);
	iEngine->SetPenSize(iPenSize);
	// Pen style.
	iPenStyle = DirectGdi::ESolidPen;
	iEngine->SetPenStyle(iPenStyle);
	// Draw mode.
	iDrawMode = DirectGdi::EDrawModePEN;
	iEngine->SetDrawMode(iDrawMode);
	// Text shadow colour.
	iTextShadowColor = KRgbGray;
	iEngine->SetTextShadowColor(iTextShadowColor);
	// Brush colour.
	iBrushColor = KRgbWhite;
	iEngine->SetBrushColor(iBrushColor);
	// Brush style.
	iBrushStyle = DirectGdi::ENullBrush;
	iEngine->SetBrushStyle(iBrushStyle);
	// Brush pattern.
	CleanUpBrushPattern();
	iEngine->ResetBrushPattern();
	iBrushPatternUsed = EFalse;
	// Brush origin.
	iBrushOrigin.SetXY(0,0);
	iEngine->SetBrushOrigin(iBrushOrigin);
	// Internal drawing position.
	iEngine->MoveTo(TPoint(0,0));
	}


/**
Sets the colour for clearing, filling the area of shapes and the background of text boxes.

The default brush colour is white. However, the default brush style is ENullBrush, so when drawing 
to a target the default appears to be the target's background colour.

@see	CDirectGdiContext::Clear()
@see	CDirectGdiContext::Clear(const TRect&)
@see	CDirectGdiContext::DrawRect()
@see	CDirectGdiContext::DrawRoundRect()
@see	CDirectGdiContext::DrawPolygon()
@see	CDirectGdiContext::DrawPie()

@param	aColor Brush colour.

@pre	None.
@post	The new brush colour will be used on subsequent drawing operations if a brush style making 
		use of the brush colour is used. The new brush colour remains in effect until another SetBrushColor() 
		with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetBrushColor(const TRgb& aColor)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetBrushColor(%d)", aColor.Internal());
	if (aColor != iBrushColor)
		{	
		iBrushColor = aColor;
		iEngine->SetBrushColor(iBrushColor);
		}
	}


/**
Sets the brush pattern origin which specifies the start of a pattern tile. 
Shapes can be considered as a view port into a continuous pattern tile covering the entire 
area of rendering target. The default origin is TPoint(0,0).

@see	CDirectGdiContext::SetBrushPattern()

@param	aOrigin	An origin point for the brush. The coordinates are relative 
		to the rectangle to fill, i.e. specify TPoint(0,0) to align the pattern flush with 
		the top and left hand sides of the rectangle.

@pre	None.
@post	New brush origin will be used when filling an area with a pattern is used on 
		subsequent drawing operations. It remains in effect until another SetBrushOrigin()
		with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetBrushOrigin(const TPoint& aOrigin)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::SetBrushOrigin(%d,%d)", aOrigin.iX, aOrigin.iY);
	if (aOrigin != iBrushOrigin)
		{
		iBrushOrigin = aOrigin;
		iEngine->SetBrushOrigin(iBrushOrigin);
		}
	}


/**
Sets the brush style used when filling the area of shapes and the background of text boxes.
Use ENullBrush to draw the outline of a fillable shape on its own, without filling.

The error state is set to KErrArgument if aBrushStyle is an invalid brush style.

@see    DirectGdi::TBrushStyle

@param	aBrushStyle The brush style to set.

@pre	If aBrushStyle is EPatternedBrush, a pattern must have been set first using SetBrushPattern().
@post	New brush style will be used for subsequent drawing operations, and remains in effect
		until another SetBrushStyle() with a different parameter is called.
@panic  DGDI 9, if aBrushStyle is EPatternedBrush but no brush pattern has successfully been set.
*/
EXPORT_C void CDirectGdiContext::SetBrushStyle(DirectGdi::TBrushStyle aBrushStyle)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetBrushStyle(%d)", aBrushStyle);
	if (aBrushStyle < DirectGdi::ENullBrush || aBrushStyle > DirectGdi::EDiamondCrossHatchBrush)
		{
		iDriver.SetError(KErrArgument);
		return;
		}
	
	if (aBrushStyle != iBrushStyle)
		{
		GRAPHICS_ASSERT_ALWAYS(aBrushStyle != DirectGdi::EPatternedBrush || iBrushPatternUsed, EDirectGdiPanicBrushPatternNotSet);
		iBrushStyle = aBrushStyle;
		iEngine->SetBrushStyle(iBrushStyle);
		}
	}


/**
Sets a clipping region which will be used to clip subsequent rendering operations on the current target.
This operation is non-additive, any previous clipping region setting is replaced by the new one. A clipping
region can contain one or more rectangles and is specified in absolute values in the target coordinate system.
By default (when a target is activated for the first time) no clipping region is set and any drawing 
operations will be clipped automatically to the full area of the rendering target.

In the event of a failure, the error state is set to KErrArgument if the given region is invalid or not 
fully contained within the area of target, otherwise one of the system-wide error codes.

@see	CDirectGdiContext::ResetClippingRegion()

@param	aRegion	The new clipping region.

@pre	Region is not empty and is fully contained within the full area of the target.
@post	Subsequent rendering operations will be clipped to the given region if there is no error 
		while performing the operation, otherwise previous clipping region settings will be retained.
*/
EXPORT_C void CDirectGdiContext::SetClippingRegion(const TRegion& aRegion)
	{
	GRAPHICS_TRACE("CDirectGdiContext::SetClippingRegion");
	if (aRegion.CheckError())
		{
		iDriver.SetError(KErrArgument);
		return;
		}
	iClippingRegion.Copy(aRegion);
	if (iClippingRegion.CheckError())
		{
		iDriver.SetError(KErrNoMemory);
		return;
		}
	iEngine->SetClippingRegion(iClippingRegion);
	}


/**
Sets the drawing mode which will affect the way pen and brush colour are used in rendering operations.
The default drawing mode is EDrawModePEN.

The error state is set to KErrArgument if aDrawMode is an invalid draw mode.

@see	DirectGdi::TDrawMode

@param	aDrawMode The drawing mode.

@pre	None.
@post	The new drawing mode will be applied to subsequent rendering operations, and remains in effect
		until another SetDrawMode() with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetDrawMode(DirectGdi::TDrawMode aDrawMode)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetDrawMode(%d)", aDrawMode);
	if (aDrawMode != DirectGdi::EDrawModePEN && aDrawMode != DirectGdi::EDrawModeWriteAlpha)
		{
		iDriver.SetError(KErrArgument);
		return;
		}

	if (iDrawMode != aDrawMode)
		{
		iDrawMode = aDrawMode;
		iEngine->SetDrawMode(iDrawMode);
		}
	}


/**
Sets the origin of the drawing engine coordinate system. By default this is TPoint(0,0) and
coincides with the origin of the target coordinate system which is at the top-left corner of
the full area of target. The X value increases from left to right, and Y value increases from
top to bottom. Integer values are used to represent the position within the coordinate system. 

All drawing operations are done relative to the engine’s origin. However, the clipping region
is always specified in absolute coordinate values (using the target coordinate system) and is
not affected by changes to the drawing engine’s coordinate system origin.

@param	aPoint The new origin for the drawing engine’s coordinate system.

@pre	None.
@post	The origin of the drawing engine’s coordinate system is moved to the given position.
		All subsequent drawing operations will be done relative to the new origin. The new origin remains
		in effect until SetOrigin() is called again with a different parameter.
*/
EXPORT_C void CDirectGdiContext::SetOrigin(const TPoint& aPoint)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::SetOrigin(%d,%d)", aPoint.iX, aPoint.iY);
	if (aPoint != iOrigin)
		{	
		iOrigin = aPoint;
		iEngine->SetOrigin(iOrigin);
		}
	}


/**
Sets the colour that will be used for drawing lines, outlines of filled shapes and text. The 
default pen colour is black. For outline and shadow fonts the alpha value of the pen colour will be 
used for blending the font to the destination.

@see	CDirectGdiContext::Plot()
@see	CDirectGdiContext::DrawLine()
@see	CDirectGdiContext::DrawRoundRect()
@see	CDirectGdiContext::DrawRect()
@see	CDirectGdiContext::DrawPolyLine()
@see	CDirectGdiContext::DrawPolyLineNoEndPoint()
@see	CDirectGdiContext::DrawPolygon()
@see	CDirectGdiContext::DrawPie()
@see	CDirectGdiContext::DrawArc()
@see	CDirectGdiContext::DrawText()

@param	aColor The pen colour.

@pre	None.
@post	The new pen colour will be used for subsequent drawing of lines, outlines of filled shapes and text. 
		The new pen colour remains in effect until another SetPenColor() with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetPenColor(const TRgb& aColor)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetPenColor(%d)", aColor.Internal());
	if (aColor != iPenColor)
		{
		iPenColor = aColor;
		iEngine->SetPenColor(iPenColor);
		}
	}


/**
Sets the pen or line drawing style.

The pen style is used to draw lines and outlines of shapes. ENullPen can be used if border or 
outlines are not required (when drawing a filled shape). The default pen style is ESolidPen.

The error state is set to KErrArgument if aPenStyle is an invalid pen style.

@see	CDirectGdiContext::Plot()
@see	CDirectGdiContext::DrawLine()
@see	CDirectGdiContext::DrawRoundRect()
@see	CDirectGdiContext::DrawRect()
@see	CDirectGdiContext::DrawPolyLine()
@see	CDirectGdiContext::DrawPolyLineNoEndPoint()
@see	CDirectGdiContext::DrawPolygon()
@see	CDirectGdiContext::DrawPie()
@see	CDirectGdiContext::DrawArc()
@see	DirectGdi::TPenStyle

@param	aPenStyle The pen style.

@pre	None.
@post	The new pen style will be applied for subsequent drawing lines and outlines of filled shapes.
		The new pen style remains in effect until another SetPenStyle() with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetPenStyle(DirectGdi::TPenStyle aPenStyle)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetPenStyle(%d)", aPenStyle);
	if (aPenStyle < DirectGdi::ENullPen || aPenStyle > DirectGdi::EDotDotDashPen)
		{
		iDriver.SetError(KErrArgument);
		return;
		}

	if (aPenStyle != iPenStyle)
		{
		iPenStyle = aPenStyle;
		iEngine->SetPenStyle(iPenStyle);
		}
	}


/**
Sets the pen size for drawing lines or the outlines of a filled shape. The default pen size is 1.
Lines with pen size greater than 1 are drawn with rounded ends that extend beyond the end points
and are always drawn using EDrawModePEN for compatibility reasons.

The error state is set to KErrArgument if the specified width or height is negative.

@see	CDirectGdiContext::Plot()
@see	CDirectGdiContext::DrawLine()
@see	CDirectGdiContext::DrawRoundRect()
@see	CDirectGdiContext::DrawRect()
@see	CDirectGdiContext::DrawPolyLine()
@see	CDirectGdiContext::DrawPolyLineNoEndPoint()
@see	CDirectGdiContext::DrawPolygon()
@see	CDirectGdiContext::DrawPie()
@see	CDirectGdiContext::DrawArc()

@param	aSize The pen size.

@pre	None.
@post	The new pen size is used for subsequent drawing lines and outlines of filled shapes. The new 
		pen size remains in effect until another SetPenSize() with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetPenSize(const TSize& aSize)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::SetPenSize(%d,%d)", aSize.iWidth, aSize.iHeight);
	if ((aSize.iWidth < 0) || (aSize.iHeight < 0))
		{
		iDriver.SetError(KErrArgument);
		return;
		}
	
	if (aSize != iPenSize)
		{
		iPenSize = aSize;
		iEngine->SetPenSize(iPenSize);
		}
	}


/**
Sets the colour that will be used for drawing the shadow for shadowed text.

@param	aColor	The shadow colour.

@pre	None.
@post	The new colour will be used for subsequent drawing of text which has a type EFourColourBlendGlyphBitmap. 
		The shadow component of the text will be filled with this colour.
		The new pen colour remains in effect until another SetTextShadowColor() with a different parameter is called.
*/
EXPORT_C void CDirectGdiContext::SetTextShadowColor(const TRgb& aColor)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetTextShadowColor(%d)", aColor.Internal());
	if (aColor != iTextShadowColor)
		{
		iTextShadowColor = aColor;
		iEngine->SetTextShadowColor(aColor);
		}
	}


/**
Sets the character justification.
The function provides a concrete implementation of the pure virtual function CGraphicsContext::SetCharJustification().
The function behaviour is the same as documented (in detail) in that class.

@param	aExcessWidth	The excess width (in pixels) to be distributed between the specified number of characters. 
@param	aNumChars		The number of characters involved.

@see	CGraphicsContext::SetCharJustification()
*/
EXPORT_C void CDirectGdiContext::SetCharJustification(TInt aExcessWidth, TInt aNumChars)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::SetCharJustification(%d,%d)", aExcessWidth, aNumChars);
	if (aExcessWidth == 0 || aNumChars <= 0)
		{
		iCharJustExcess = 0;
		iCharJustNum = 0;
		}
	else
		{
		iCharJustExcess = aExcessWidth;
		iCharJustNum = aNumChars;
		}
	}


/**
Sets the word justification.
The function provides a concrete implementation of the pure virtual function CGraphicsContext::SetWordJustification().
The function behaviour is the same as documented (in detail) in that class.

@param	aExcessWidth	The width (in pixels) to be distributed between the specified number of spaces.
						It may be positive, in which case the text is stretched, or negative, in which case it is shrunk. 
@param	aNumGaps		The number of word spaces (characters with the code U+0020) over which the change in width is distributed.

@see CGraphicsContext::SetWordJustification()
*/
EXPORT_C void CDirectGdiContext::SetWordJustification(TInt aExcessWidth, TInt aNumGaps)
	{
	GRAPHICS_TRACE2("CDirectGdiContext::SetWordJustification(%d,%d)", aExcessWidth, aNumGaps);
	if (aExcessWidth <= 0 || aNumGaps <= 0)
		{
		iWordJustExcess = 0;
		iWordJustNum = 0;
		}
	else
		{
		iWordJustExcess = aExcessWidth;
		iWordJustNum = aNumGaps;
		}
	}


/**
Sets the underline style for all subsequently drawn text.
The function provides a concrete implementation of the pure virtual function CGraphicsContext::SetUnderlineStyle().
The function behaviour is the same as documented in that class.

@param	aUnderlineStyle	The underline style to be used.

@see	CGraphicsContext::SetUnderlineStyle()
*/
EXPORT_C void CDirectGdiContext::SetUnderlineStyle(DirectGdi::TFontUnderline aUnderlineStyle)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetWordJustification(%d)", aUnderlineStyle);
	iUnderline = aUnderlineStyle;
	}


/**
Sets the strikethrough style for all subsequently drawn text.
The function provides a concrete implementation of the pure virtual function CGraphicsContext::SetStrikethroughStyle().
The function behaviour is the same as documented in that class.

@param	aStrikethroughStyle	The strikethrough style to be used.

@see	CGraphicsContext::SetStrikethroughStyle()
*/
EXPORT_C void CDirectGdiContext::SetStrikethroughStyle(DirectGdi::TFontStrikethrough aStrikethroughStyle)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetStrikethroughStyle(%d)", aStrikethroughStyle);
	iStrikethrough = aStrikethroughStyle;
	}


/**
Sets the bitmap to be used as the brush pattern when EPatternedBrush is selected.
The DirectGDI generic layer owns the bitmap and will keep the bitmap until ResetBrushPattern() is called.

The client may modify the content of the bitmap used as the brush pattern. If this is done after 
issuing drawing commands there is no guarantee which bitmap content will be used as brush pattern.
Clients must call Finish() on the driver before modifying the bitmap content if they want a guaranteed 
result that the previously issued drawing commands will be drawn using the old bitmap brush pattern.

In the event of a failure, the error state is set to KErrCouldNotConnect if no connection to the font 
and bitmap server could be made, KErrBadHandle if the handle of the bitmap is null, KErrUnknown if 
no bitmap could be found with the specified handle number, otherwise one of the system-wide error codes.

@see	CDirectGdiContext::SetBrushStyle()
@see	CDirectGdiContext::SetBrushOrigin()
@see	CDirectGdiContext::ResetBrushPattern()

@param	aBitmap	Bitmap that will be used as the brush pattern.

@pre	Bitmap is fully constructed.
@post	Bitmap will be used as the brush pattern for subsequent drawing operations when EPatternedBrush
		is selected. It remains in effect until ResetBrushPattern() is called.
*/
EXPORT_C void CDirectGdiContext::SetBrushPattern(const CFbsBitmap& aBitmap)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetBrushPattern(%d)", aBitmap.Handle());
	SetBrushPattern(aBitmap.Handle());
	}


/**
Sets the bitmap to be used as the brush pattern when EPatternedBrush is selected.
The DirectGDI generic layer owns the bitmap and will keep the bitmap until ResetBrushPattern() is called.
If the client modifies the content of the bitmap used as the brush pattern after issuing any drawing 
commands that uses that brush pattern, the method does not guarantee whether the old bitmap 
content or the new one will be used as brush pattern. Clients must call Finish() on the driver 
before modifying the bitmap content if they want a guaranteed result that the previously issued 
drawing commands will be drawn using the old bitmap brush pattern.

In the event of a failure, the error state is set to KErrCouldNotConnect if no connection to the font 
and bitmap server could be made, KErrBadHandle if the handle is null, KErrUnknown if no bitmap could 
be found with the specified handle number, otherwise one of the system-wide error codes.

@param	aFbsBitmapHandle Bitmap handle that will be used as the brush pattern.

@pre 	Bitmap belonging to the handle is fully constructed.
@post 	Bitmap will be used as the brush pattern for subsequent drawing operations when EPatternedBrush
		is selected. It remains in effect until ResetBrushPattern() is called.
@panic	DGDI 8, if aFbsBitmapHandle is 0.
*/
EXPORT_C void CDirectGdiContext::SetBrushPattern(TInt aFbsBitmapHandle)
	{
	GRAPHICS_TRACE1("CDirectGdiContext::SetBrushPattern(%d)", aFbsBitmapHandle);
	if (aFbsBitmapHandle == KNullHandle)
		{
		iDriver.SetError(KErrBadHandle);
		return;
		}

	// Check we're not already using the passed brush pattern
	if (iBrushPattern.Handle() == aFbsBitmapHandle)
		{
		return;
		}

	// Delete any previously saved brush pattern
	CleanUpBrushPattern();

	TInt result = iBrushPattern.Duplicate(aFbsBitmapHandle);
	if (result == KErrNone)
		{
		result = iEngine->SetBrushPattern(iBrushPattern);
		}
	
	if (result == KErrNone)
		{
		iBrushPatternUsed = ETrue;
		}
	else
		{
		iDriver.SetError(result);
		}

	return;
	}


/**
Selects the font to be used for text drawing.
Notes:
When the font is no longer required, use ResetFont() to free up the memory used. 
If SetFont() is used again without using ResetFont() then the previous font is reset 
automatically. If no font has been selected, and an attempt is made to draw text with 
DrawText(), then a panic occurs.

@see	CDirectGdiContext::ResetFont()
@see	CDirectGdiContext::DrawText()

@param	aFont The font to be used.

@panic	DGDI 12, if aFont has an invalid handle or is not a CFbsFont, or the font cannot be duplicated.
*/
EXPORT_C void CDirectGdiContext::SetFont(const CFont* aFont)
	{
	GRAPHICS_TRACE("CDirectGdiContext::SetFont");
	// Note: We pass a ptr in, rather than a reference, because otherwise the caller would almost always have to do complex casting 
	GRAPHICS_ASSERT_ALWAYS(aFont->TypeUid() == KCFbsFontUid, EDirectGdiPanicInvalidFont);
	const CDirectGdiFont* font = reinterpret_cast<const CDirectGdiFont*>(aFont);
	GRAPHICS_ASSERT_ALWAYS(font->Handle(), EDirectGdiPanicInvalidFont);

	if (iFont.Handle() == font->Handle())
		{
		return;
		}
	ResetFont();
	TInt err = iFont.Duplicate(font->Handle());
	GRAPHICS_ASSERT_ALWAYS(err == KErrNone, EDirectGdiPanicInvalidFont); // This may seem extreme but it is what BitGdi did
	iEngine->SetFont(iFont.Address()->UniqueFontId());
	}


/**
Copies the content of a rectangular area on the target to another location.
The source rect will be intersected with the target’s full extent.

@param	aOffset	Offset from the top left corner of the rectangle to be copied to the top left corner of the copy.
@param	aRect Area to be copied.

@pre	The rendering target has been activated.
@post	Request to copy an area has been accepted. There is no guarantee that the
		request has been processed when this method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::CopyRect(const TPoint& aOffset, const TRect& aRect)
	{
	GRAPHICS_TRACE("CDirectGdiContext::CopyRect");
	if (aRect.IsEmpty() || aOffset == TPoint(0,0))
		{
		return;
		}
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	iEngine->CopyRect(aOffset, aRect);
	}

/** 
Copies all settings from the specified DirectGDI context.

@param aGc The DirectGDI context whose settings are to be copied. 
*/
EXPORT_C void CDirectGdiContext::CopySettings(const CDirectGdiContext& aGc)
	{
	GRAPHICS_TRACE("CDirectGdiContext::CopySettings");
	SetOrigin(aGc.iOrigin);
	SetFont(&(aGc.iFont));
	SetCharJustification(aGc.iCharJustExcess, aGc.iCharJustNum);
	SetWordJustification(aGc.iWordJustExcess, aGc.iWordJustNum);
	iLastPrintPosition = aGc.iLastPrintPosition;
	SetStrikethroughStyle(aGc.iStrikethrough);
	SetUnderlineStyle(aGc.iUnderline);
	SetPenColor(aGc.iPenColor);
	SetPenSize(aGc.iPenSize);
	SetPenStyle(aGc.iPenStyle);
	SetDrawMode(aGc.iDrawMode);
	SetTextShadowColor(aGc.iTextShadowColor);
	SetBrushColor(aGc.iBrushColor);
	SetBrushStyle(aGc.iBrushStyle);
	if(aGc.iBrushPattern.Handle())
		{
		SetBrushPattern(aGc.iBrushPattern.Handle());
		}
	iBrushPatternUsed = aGc.iBrushPatternUsed;
	SetBrushOrigin(aGc.iBrushOrigin);
	}

/**
Updates the justification settings.
This function assumes that NoJustifyAutoUpdate() has not been used.

@param	aText	The text for which justification is to be adjusted.
@param	aParam	Parameters used in drawing text.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 13, if NoJustifyAutoUpdate() had been called prior to this.
*/
EXPORT_C void CDirectGdiContext::UpdateJustification(const TDesC& aText, const DirectGdi::TTextParameters* aParam)
	{
	GRAPHICS_TRACE("CDirectGdiContext::UpdateJustification");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iAutoUpdateJustification, EDirectGdiPanicAutoUpdateJustificationUsed);
	if (((iCharJustNum < 1) || (iCharJustExcess == 0)) && ((iWordJustNum < 1) || (iWordJustExcess < 1)))
		{
		return;
		}

	TInt length = aText.Length();
	CFont::TPositionParam param;
	param.iText.Set(aText);	// Set the start of the string
	if (aParam)
		{
		length = aParam->iEnd;
		param.iPosInText = aParam->iStart;
		}
	TInt excess = 0;
	TInt glyphs = 0;
	RShapeInfo shapeInfo;
	for (TInt count = 0; count < length; count++)
		{
		if ((iCharJustNum > 0) && (iCharJustExcess != 0))
			{
			excess += CGraphicsContext::JustificationInPixels(iCharJustExcess, iCharJustNum);
			}
		if ((iWordJustNum > 0) && (iWordJustExcess > 0) && (aText[count] == ' '))
			{
			excess += CGraphicsContext::JustificationInPixels(iWordJustExcess, iWordJustNum);
			}
		if (iCharJustNum < (glyphs + length - count)) // there's at least 1 combined glyph to come
			{
			// otherwise we can skip this slow bit and just increment
			if (iFont.GetCharacterPosition2(param, shapeInfo))
				{
				count = param.iPosInText - 1;   // -1 'cos it gets incremented anyway
				}
			}
		glyphs++;
		}
	if (shapeInfo.IsOpen())
		{
		shapeInfo.Close();
		}
	iLastPrintPosition.iX += excess;
	}


/**
Updates the justification for vertical text.
This function assumes that NoJustifyAutoUpdate() has not been used.

@param	aText	The text for which justification is to be adjusted.
@param	aParam	Parameters used in drawing text.
@param	aUp		ETrue, if text is to be justified upwards; EFalse, if text is to be justified downwards.

@panic	DGDI 7, if the rendering context has not been activated.
@panic	DGDI 13, if NoJustifyAutoUpdate() had been called prior to this.
*/
EXPORT_C void CDirectGdiContext::UpdateJustificationVertical(const TDesC& aText, const DirectGdi::TTextParameters* aParam, TBool aUp)
	{
	GRAPHICS_TRACE("CDirectGdiContext::UpdateJustificationVertical");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	GRAPHICS_ASSERT_ALWAYS(iAutoUpdateJustification, EDirectGdiPanicAutoUpdateJustificationUsed);

	if (((iCharJustNum < 1) || (iCharJustExcess == 0)) && ((iWordJustNum < 1) || (iWordJustExcess < 1)))
		{
		return;
		}

	TInt length = aText.Length();
	CFont::TPositionParam param;
	param.iText.Set(aText);	// Set the start of the string
	if (aParam)
		{
		length = aParam->iEnd;
		param.iPosInText = aParam->iStart;
		}
	TInt excess = 0;
	TInt glyphs = 0;
	RShapeInfo shapeInfo;
	for (TInt count = 0; count < length; count++)
		{
		if ((iCharJustNum > 0) && (iCharJustExcess != 0))
			{
			excess += CGraphicsContext::JustificationInPixels(iCharJustExcess, iCharJustNum);
			}
		if ((iWordJustNum > 0) && (iWordJustExcess > 0) && (aText[count] == ' '))
			{
			excess += CGraphicsContext::JustificationInPixels(iWordJustExcess, iWordJustNum);
			}
		if (iCharJustNum < (glyphs + length - count)) // there's at least 1 combined glyph to come
			{
			// otherwise we can skip this slow bit and just increment
			if (iFont.GetCharacterPosition2(param, shapeInfo))
				{
				count = param.iPosInText - 1;   // -1 because it gets incremented anyway
				}
			}
		glyphs++;
		}
		if (shapeInfo.IsOpen())
			{
			shapeInfo.Close();
			}

		if (aUp)
			{
			iLastPrintPosition.iY -= excess;
			}
		else
			{
			iLastPrintPosition.iY += excess;
			}
	}


/**
Selects a font for text drawing but does not take a copy. 
The original must not be destroyed until SetFont(), SetFontNoDuplicate(), ResetFont() 
or the destructor is called.

@param	aFont A pointer to the font to be used.
@panic	DGDI 12, if aFont has no handle or is not a CFbsFont.
*/
EXPORT_C void CDirectGdiContext::SetFontNoDuplicate(const CDirectGdiFont* aFont)
	{
	GRAPHICS_TRACE("CDirectGdiContext::SetFontNoDuplicate");
	// Note: We pass a ptr in, rather than a reference, because otherwise the caller would almost always have to do complex casting 
	GRAPHICS_ASSERT_ALWAYS(aFont->TypeUid() == KCFbsFontUid, EDirectGdiPanicInvalidFont);
	GRAPHICS_ASSERT_ALWAYS(aFont->Handle(), EDirectGdiPanicInvalidFont);

	if (iFont.Handle() == aFont->Handle())
		{
		return;
		}
	
	ResetFont();
	iFont = *aFont;
	iEngine->SetFont(iFont.Address()->UniqueFontId());
	}


/**
Checks to see if a brush pattern is currently set.

@return	ETrue is a brush pattern is currently set, EFalse if no brush pattern is currently set.
*/
EXPORT_C TBool CDirectGdiContext::HasBrushPattern() const
	{
	GRAPHICS_TRACE("CDirectGdiContext::HasBrushPattern");
	return iBrushPatternUsed;
	}


/**
Tests whether a font is used.

@return	ETrue, if a font is being used; EFalse, otherwise.
*/
EXPORT_C TBool CDirectGdiContext::HasFont() const
	{
	GRAPHICS_TRACE("CDirectGdiContext::HasFont");
	TBool result = EFalse;
	if (iFont.Handle() != KNullHandle)
		result = ETrue;
	return result;
	}


/**
Externalises the context and the drawing engine object to the write stream.
It is important that the font and brush bitmap of the GC is maintained between
calls to ExternalizeL() and InternalizeL().  The font handle and brush bitmap handle
is externalised, not the underlying data.  This is done for performance reasons.

@param	aWriteStream Write stream.

@pre	None.
@post	The context and drawing engine object states are written to the write stream.

@see	MDirectGdiEngine::InternalizeL
@leave	If there was an error writing to the write stream.
*/
EXPORT_C void CDirectGdiContext::ExternalizeL(RWriteStream& aWriteStream)
	{	
	GRAPHICS_TRACE("CDirectGdiContext::ExternalizeL");
	aWriteStream << KDirectGDIContext_VerNo;
	iEngine->ExternalizeL(aWriteStream);

	aWriteStream << iOrigin;
	aWriteStream.WriteInt32L(iFont.Handle());
	aWriteStream.WriteInt32L(iCharJustExcess);
	aWriteStream.WriteInt32L(iCharJustNum);
	aWriteStream.WriteInt32L(iWordJustExcess);
	aWriteStream.WriteInt32L(iWordJustNum);
	aWriteStream << iLastPrintPosition;
	aWriteStream.WriteUint8L(iStrikethrough); 
	aWriteStream.WriteUint8L(iUnderline);
	aWriteStream << iPenColor;
	aWriteStream.WriteUint32L(iPenSize.iWidth);
	aWriteStream.WriteUint32L(iPenSize.iHeight);
	aWriteStream.WriteUint8L(iPenStyle);
	aWriteStream.WriteUint8L(iDrawMode);
	aWriteStream << iTextShadowColor;
	aWriteStream << iBrushColor;
	aWriteStream.WriteInt32L(iBrushPattern.Handle());
	aWriteStream.WriteUint8L(iBrushStyle);
	aWriteStream.WriteUint8L(iBrushPatternUsed);
	aWriteStream << iBrushOrigin;
	aWriteStream.WriteUint8L(iAutoUpdateJustification);
	}


/**
Internalises the context and the drawing engine object from the read stream.
It is important that the font and brush bitmap of the GC is maintained between
calls to ExternalizeL() and InternalizeL().  The font handle and brush bitmap handle
is externalised, not the underlying data.  This is done for performance reasons.

@param	aReadStream	Read stream.

@pre	The font has not been released since the last call of CDirectGdiContext::ExternalizeL on the stream
@pre	The handle of the brush pattern bitmap has not been closed since the call to CDirectGdiContext::ExternalizeL on the stream.
@post	The context and drawing engine object states are updated with the values from the read stream.

@see	MDirectGdiEngine::ExternalizeL
@leave	If there was an error reading from the read stream.
*/
EXPORT_C void CDirectGdiContext::InternalizeL(RReadStream& aReadStream)
	{
	GRAPHICS_TRACE("CDirectGdiContext::InternalizeL");
	TUint16 archiveVerNo = 0;
	aReadStream >> archiveVerNo;
	iEngine->InternalizeL(aReadStream);
	
	TPoint origin;
	aReadStream >> origin;
	SetOrigin(origin);
	ResetFont();
	TInt fontHandle = aReadStream.ReadInt32L();
	if(fontHandle)
		{
		TInt res = iFont.Duplicate(fontHandle);
		if(res == KErrNone)
			{
			iEngine->SetFont(iFont.Address()->UniqueFontId());
			}
		else
			{
			iDriver.SetError(res);
			}
		}
	iCharJustExcess = aReadStream.ReadUint32L();
	iCharJustNum = aReadStream.ReadUint32L();
	iWordJustExcess = aReadStream.ReadUint32L();
	iWordJustNum = aReadStream.ReadUint32L();
	aReadStream >> iLastPrintPosition;
	iStrikethrough = (DirectGdi::TFontStrikethrough)aReadStream.ReadUint8L(); 
	iUnderline = (DirectGdi::TFontUnderline)aReadStream.ReadUint8L();
	TRgb penColor;
	aReadStream >> penColor;
	SetPenColor(penColor);
	TSize penSize;
	penSize.iWidth = aReadStream.ReadUint32L();
	penSize.iHeight = aReadStream.ReadUint32L();
	SetPenSize(penSize);
	DirectGdi::TPenStyle penStyle = (DirectGdi::TPenStyle)aReadStream.ReadUint8L();
	SetPenStyle(penStyle);
	DirectGdi::TDrawMode drawMode = (DirectGdi::TDrawMode)aReadStream.ReadUint8L();
	SetDrawMode(drawMode);
	TRgb textShadowColor;
	aReadStream >> textShadowColor;
	SetTextShadowColor(textShadowColor);
	TRgb brushColor;
	aReadStream >> brushColor;
	SetBrushColor(brushColor);
	TInt patternHandle = aReadStream.ReadInt32L();
	if (patternHandle)
		{
		// Brush pattern must be set before style, otherwise there'll be a panic!
		SetBrushPattern(patternHandle);
		}
	DirectGdi::TBrushStyle brushStyle;
	brushStyle = (DirectGdi::TBrushStyle)aReadStream.ReadInt8L();
	SetBrushStyle(brushStyle);
	iBrushPatternUsed = (TBool)aReadStream.ReadUint8L();
	TPoint brushOrigin;
	aReadStream >> brushOrigin;
	SetBrushOrigin(brushOrigin);
	iAutoUpdateJustification = (TBool)aReadStream.ReadUint8L();
	}


/**
Retrieves the currently set brush colour.

@return	The current brush colour.
*/
EXPORT_C TRgb CDirectGdiContext::BrushColor() const
	{	
	return iBrushColor;
	}


/**
Retrieves the currently set pen colour.

@return	The current pen colour.
*/
EXPORT_C TRgb CDirectGdiContext::PenColor() const
	{
	return iPenColor;
	}

/**
Retrieves the currently set text shadow colour.

@return	The current text shadow colour.
*/
EXPORT_C TRgb CDirectGdiContext::TextShadowColor() const
	{
	return iTextShadowColor;
	}

/**
Draws an image based resource which may be generated using non-native rendering API such as OpenGL ES 
or OpenVG. The resource will be drawn at the specified position in its original size with orientation 
according to the specified rotation parameter. The current clipping region applies. The resource can be
drawn rotated using the DirectGdi::TGraphicsRotation enum which defines possible rotation values in 
clockwise degrees.

In the event of a failure, the error state is set to one of the system-wide error codes.

@param	aPos		The position of the top-left corner of the resource.
@param	aSource		The resource to be drawn.
@param	aRotation	The rotation to be applied to the resource before it is drawn. The default value is DirectGdi::EGraphicsRotationNone.

@pre	Drawing context has been activated on a rendering target. The resource has been fully constructed.
@post	Request to draw resource has been accepted. There is no guarantee that the request has been completed 
		when this method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawResource(
		const TPoint& aPos,
		const RDirectGdiDrawableSource& aSource,
		DirectGdi::TGraphicsRotation aRotation)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawResource");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	
	if (aSource.Handle() != KNullHandle)		
		{
		iEngine->DrawResource(aPos, aSource, aRotation);
		}
	else
		{
		iDriver.SetError(KErrBadHandle);
		}
	}

/**
Draws an image based resource. The resource will be rendered to the given destination rectangle on 
rendering target in its original dimensions with orientation according to the specified rotation parameter. 
Drawing will be clipped to the given destination rectangle. The current clipping region applies. 

In the event of a failure, the error state is set to one of the system-wide error codes.

@param	aDestRect	Destination rectangle to which the resource will be rendered.
@param	aSource		The resource to be drawn.
@param	aRotation	Rotation to be applied to the resource before it is drawn. Default value is DirectGdi::EGraphicsRotationNone.

@pre	Drawing context has been activated on a rendering target. The resource has been fully constructed.
@post	Request to draw resource has been accepted. There is no guarantee that the request has been completed 
		when this method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawResource(const TRect& aDestRect,
											const RDirectGdiDrawableSource& aSource,
											DirectGdi::TGraphicsRotation aRotation)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawResource");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);

	if (aSource.Handle() != KNullHandle)		
		{	
		if ((aDestRect.Width() > 0) && (aDestRect.Height() > 0))
			{
			iEngine->DrawResource(aDestRect, aSource, aRotation);
			}
		}
	else
		{
		iDriver.SetError(KErrBadHandle);
		}
	}


/**
Draws an image based resource. The resource is rendered into the given destination rectangle.
Scaling (stretching or compression) applies if the destination rectangle is different from the
source rectangle. The resource orientation is set based on the specified rotation parameter
before scaling and drawing operations are performed.

If the user modifies the content of the resource after issuing a DrawResource() command (from the
same thread), the adaptation must make sure that the user’s operations are serialised within
that thread, for example, DrawResource() is processed before the modify operations. The adaptation
does not guarantee the result if the resource modification is performed from threads other than
the one that issued the DrawResource() command. To achieve a guaranteed result in that case, users
must perform synchronisation between any threads that operate on the resource and issue Finish()
on the driver whenever necessary. When using other renderers or mappings, synchronisation is needed 
even when this is from within the same thread.

In the event of a failure, the error state is set to one of the system-wide error codes.

@param	aDestRect	The destination rectangle to which the resource will be rendered.
@param	aSource		The resource to draw.
@param	aSrcRect	The source rectangle specifying the area/sub-area of the resource to be rendered.
@param	aRotation	Rotation to be applied to the resource before it is drawn

@pre	The rendering target has been activated. The resource has been fully constructed.
@post	Request to draw an image based resource has been accepted. There is no guarantee that the
		request has been completed when this method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawResource(
		const TRect& aDestRect,
		const RDirectGdiDrawableSource& aSource,
		const TRect& aSrcRect,
		DirectGdi::TGraphicsRotation aRotation)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawResource");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);
	
	if (aSource.Handle() != KNullHandle)		
		{
		if ((aDestRect.Width() > 0) && (aDestRect.Height() > 0)
			&& (aSrcRect.Width() > 0) && (aSrcRect.Height() > 0))
			{
			iEngine->DrawResource(aDestRect, aSource, aSrcRect, aRotation);
			}
		}
	else
		{
		iDriver.SetError(KErrBadHandle);
		}
	}


/**
Draws a non-image based resource. The resource will be rendered into the given destination rectangle.
The current clipping region applies. The adaptation is free to interpret the parameters and may define
their own rules on how to handle the rendering of a non-image based resource.

In the event of a failure, the error state is set to one of the system-wide error codes.

@param	aDestRect	The destination rectangle to which the resource will be rendered.
@param	aSource		The resource.
@param	aParam		Parameters specifying how to draw the resource. 

@pre	The rendering target has been activated. The resource has been fully constructed.
@post	Request to draw a non-image based resource has been accepted. 
		There is no guarantee that the request has been completed when this method returns.

@panic	DGDI 7, if the rendering context has not been activated.
*/
EXPORT_C void CDirectGdiContext::DrawResource(
		const TRect& aDestRect,
		const RDirectGdiDrawableSource& aSource,
		const TDesC8& aParam)
	{
	GRAPHICS_TRACE("CDirectGdiContext::DrawResource");
	GRAPHICS_ASSERT_ALWAYS(iActivated, EDirectGdiPanicContextNotActivated);	
	
	if (aSource.Handle() != KNullHandle)		
		{
		if ((aDestRect.Width() > 0) && (aDestRect.Height() > 0))
			{
			iEngine->DrawResource(aDestRect, aSource, aParam);
			}
		}
	else
		{
		iDriver.SetError(KErrBadHandle);
		}
	}

/**
Retrieves a pointer to an instance of the appropriate extension interface implementation.

@param aInterfaceId Interface identifier of the interface to be retrieved.	
@param aInterface On return, holds the specified interface, or NULL if the interface cannot be found.

@pre    None.
@post   None.

@return KErrNone If the interface is supported, KErrNotSupported otherwise.
 */
EXPORT_C TInt CDirectGdiContext::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	GRAPHICS_TRACE("CDirectGdiContext::GetInterface");
	return iEngine->GetInterface(aInterfaceId, aInterface);
	}

/**
Release the brush pattern's handle, and mark it as no longer used.
*/
void CDirectGdiContext::CleanUpBrushPattern()
	{
	iBrushPattern.Reset();
	iBrushPatternUsed = EFalse;	
	}

/** 
@internalTechnology

Returns the baseline correction associated with this font.
This value is used to alter the underline/strikethrough position applied to linked fonts.

@return The baseline correction value set by the rasterizer; or 0 if not set
*/	
TInt CDirectGdiContext::BaselineCorrection()
	{
	TOpenFontMetrics metrics;
	if (iFont.GetFontMetrics(metrics))
		return metrics.BaselineCorrection();
	else
		return 0;
	}
