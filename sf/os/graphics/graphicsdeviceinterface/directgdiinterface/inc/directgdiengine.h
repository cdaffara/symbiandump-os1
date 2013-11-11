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


#ifndef DIRECTGDIENGINE_H
#define DIRECTGDIENGINE_H

/**
@file
@publishedPartner
@prototype
*/

#include <graphics/directgditypes.h>
#include <e32std.h>

// Forward declarations.
//
class CFbsBitmap;
class RDirectGdiImageSource;
class RDirectGdiImageTarget;
class RDirectGdiDrawableSource;

/**
Pure virtual interface for licensee adaptation of GDI platform implementation.

@publishedPartner
@prototype
@deprecated
*/
class MDirectGdiEngine
	{
public:

	/**
	@see	CDirectGdiContext::Activate()
	@see	Deactivate()
	 */
	virtual TInt Activate(RDirectGdiImageTarget& aTarget) = 0;
	
	/**
	Unbinds the current rendering target (if any) from drawing engine. Drawing engine states or settings which
	are not dependent or non-target-specific remain unchanged.
	
	The DirectGDI generic layer will call this method prior to activating the drawing engine for another 
	rendering target.
	
	@see	Activate()
	
	@pre	None.
	@post	Drawing engine is no longer associated with any rendering target.
	 */
	virtual void Deactivate() = 0;
	
	/**
	@see	CDirectGdiContext::SetOrigin()
	 */
	virtual void SetOrigin(const TPoint& aOrigin) = 0;
	
	/**
	@see	CDirectGdiContext::ResetClippingRegion()
	@see	SetClippingRegion()
	 */
	virtual void SetClippingRegion(const TRegion& aRegion) = 0;
	
	/**
	@see	CDirectGdiContext::ReetClippingRegion()
	@see	SetClippingRegion()
	 */
	virtual void ResetClippingRegion() = 0;
	
	/**
	@see	CDirectGdiContext::SetDrawMode()
	 */
	virtual void SetDrawMode(DirectGdi::TDrawMode aMode) = 0;
	
	/**
	@see	CDirectGdiContext::SetPenColor()
	@see	DrawLine()
	@see	DrawLineTo()
	@see	DrawLineBy()
	@see	DrawPolyLine()
	@see	DrawPolygon()
	@see	DrawRect()
	@see	DrawRoundRect()
	 */
	virtual void SetPenColor(const TRgb& aColor) = 0;
	
	/**
	@see	CDirectGdiContext::SetPenStyle()
	@see	DrawLine()
	@see	DrawLineTo()
	@see	DrawLineBy()
	@see	DrawPolyLine()
	@see	DrawPolygon()
	@see	DrawRect()
	@see	DrawRoundRect()
	 */
	virtual void SetPenStyle(DirectGdi::TPenStyle aStyle) = 0;
	
	/**
	@see	CDirectGdiContext::SetPenSize()
	@see	DrawLine()
	@see	DrawLineTo()
	@see	DrawLineBy()
	@see	DrawPolyLine()
	@see	DrawPolygon()
	@see	DrawRect()
	@see	DrawRoundRect()
	@see	Plot()
	 */
	virtual void SetPenSize(const TSize& aSize) = 0;
	
	/**
	@see	CDirectGdiContext::SetTextShadowColor()
	 */
	virtual void SetTextShadowColor(const TRgb& aColor) = 0;
	
	/**
	@see	CDirectGdiContext::SetBrushColor()
	 */
	virtual void SetBrushColor(const TRgb& aColor) = 0;
	
	/**
	@see 	CDirectGdiContext::SetBrushStyle()
	@see	SetBrushPattern()
	 */
	virtual void SetBrushStyle(DirectGdi::TBrushStyle aStyle) = 0;
	
	/**
	@see	CDirectGdiContext::SetBrushOrigin()
	@see	SetBrushPattern()
	 */
	virtual void SetBrushOrigin(const TPoint& aOrigin) = 0;
	
	/**
	@see	CDirectGdiContext::SetBrushPattern()
	@see	ResetBrushPattern()
	
	@return KErrNone if successful, otherwise one of the system-wide error codes.
	 */
	virtual TInt SetBrushPattern(const CFbsBitmap& aPattern) = 0;
	
	/**
	@see 	CDirectGdiContext::ResetBrushPattern()
	@see	SetBrushPattern()
	 */
	virtual void ResetBrushPattern() = 0;
	
	/**
	@see 	CDirectGdiContext::SetFont()
	
	@param  aFontId The Font identifier.
	*/
	virtual void SetFont(TUint32 aFontId) = 0;
	
	/**
	@see 	CDirectGdiContext::ResetFont()
	 */
	virtual void ResetFont() = 0;
	
	/**
	Resets the engine state to default values. 
	 
	@see 	CDirectGdiContext::Reset()
	 */
	virtual void Reset() = 0;
	
	/**
	@see	CDirectGdiContext::Clear(const TRect&)
	@see	Clear()
	@see    SetBrushColor()
	 */
	virtual void Clear(const TRect& aRect) = 0;
	
	/**
	@see	CDirectGdiContext::Clear()
	@see	Clear(const TRect&)
	@see    SetBrushColor()
	 */
	virtual void Clear() = 0;
	
	/**
	@see    CDirectGdiContext::MoveTo()
	@see	MoveBy()
	 */
	virtual void MoveTo(const TPoint& aPoint) = 0;
	
	/**
	@see    CDirectGdiContext::MoveBy()
	@see	MoveTo()
	 */
	virtual void MoveBy(const TPoint& aVector) = 0;
	
	/**
	@see    CDirectGdiContext::Plot()	
	@see    SetPenSize()
	@see    SetPenColor()
	@see    SetDrawMode()
	 */
	virtual void Plot(const TPoint& aPoint) = 0;
	
	/**
	@see    CDirectGdiContext::DrawLine()	
	@see    DrawLineTo()
	@see    DrawLineBy()
	@see    SetPenSize()
	@see    SetPenStyle()
	@see    SetPenColor()
	@see    SetDrawMode()
	 */
	virtual void DrawLine(const TPoint& aStart, const TPoint& aEnd) = 0;
	
	/**
	@see    CDirectGdiContext::DrawLineTo()	
	@see    DrawLine()
	@see    DrawLineBy()
	@see    SetPenSize()
	@see    SetPenStyle()
	@see    SetPenColor()
	@see    SetDrawMode()
	@see    MoveTo()
	@see    MoveBy()
	 */
	virtual void DrawLineTo(const TPoint& aPoint) = 0;
	
	/**
	@see    CDirectGdiContext::DrawLineBy()	
	@see    DrawLine()
	@see    DrawLineTo()
	@see    SetPenSize()
	@see    SetPenStyle()
	@see    SetPenColor()
	@see    DrawMode()
	@see    MoveTo()
	@see    MoveBy()
	 */
	virtual void DrawLineBy(const TPoint& aVector) = 0;
	
	/**
	@see	CDirectGdiContext::DrawRect()
	 */
	virtual void DrawRect(const TRect& aRect) = 0;
	
	/**
	@see	CDirectGdiContext::DrawRoundRect()
	 */
	virtual void DrawRoundRect(const TRect& aRect, const TSize& aCornerSize) = 0;
	
	/**
	@see	CDirectGdiContext::DrawPolyLine()	
	@see	SetPenColor()
	@see	SetPenSize()
	@see	SetPenStyle()
	@see	SetDrawMode()
	 */
	virtual void DrawPolyLine(const TArray<TPoint>& aPointList) = 0;
	
	/**
	@see	CDirectGdiContext::DrawPolyLineNoEndPoint()	
	@see	SetPenColor()
	@see	SetPenSize()
	@see	SetPenStyle()
	@see	SetDrawMode()
	 */
	virtual void DrawPolyLineNoEndPoint(const TArray<TPoint>& aPointList) = 0;
	
	/**
	@see	CDirectGdiContext::DrawPolygon()
	 */
	virtual void DrawPolygon(const TArray<TPoint>& aPoints,	DirectGdi::TFillRule aRule) = 0;
	
	/**
	@see	CDirectGdiContext::DrawArc()
	@see	DrawPie()
	 */
	virtual void DrawArc(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd) = 0;
	
	/**
	@see	CDirectGdiContext::DrawPie()
	 */
	virtual void DrawPie(const TRect& aRect, const TPoint& aStart, const TPoint& aEnd) = 0;
	
	/**
	@see	CDirectGdiContext::DrawEllipse()
	 */
	virtual void DrawEllipse(const TRect& aRect) = 0;
	
	/** 
	@see	CDirectGdiContext::BitBlt(const TPoint&, const CFbsBitmap&, const TRect&)
	 */
	virtual void BitBlt(const TPoint& aDestPos, const CFbsBitmap& aBitmap, const TRect& aSourceRect) = 0;
	
	/**
	@see	CDirectGdiContext::BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool)
	 */
	virtual void BitBltMasked(const TPoint& aDestPos,				
							const CFbsBitmap& aBitmap,
							const TRect& aSourceRect,				
							const CFbsBitmap& aMask,
							TBool aInvertMask) = 0;
	
	/**
	@see	CDirectGdiContext::BitBltMasked(const TPoint&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, const TPoint&)
	 */
	virtual void BitBltMasked(const TPoint& aDestPos,			
							const CFbsBitmap& aBitmap,
							const TRect& aSourceRect,				
							const CFbsBitmap& aMask,
							const TPoint& aMaskPos) = 0;
	
	/**
	@see	CDirectGdiContext::DrawBitmap(const TRect&, const CFbsBitmap&, const TRect&)
	 */
	virtual void DrawBitmap(const TRect& aDestRect, 
							const CFbsBitmap& aBitmap, 
							const TRect& aSourceRect) = 0;
	
	/**
	@see	CDirectGdiContext::DrawBitmapMasked(const TRect&, const CFbsBitmap&, const TRect&, const CFbsBitmap&, TBool)
	 */
	virtual void DrawBitmapMasked(const TRect& aDestRect,				
								const CFbsBitmap& aBitmap,
								const TRect& aSourceRect,				
								const CFbsBitmap& aMask,
								TBool aInvertMask) = 0;
	
	/**
	@see	CDirectGdiContext::DrawResource(const TPoint&, const RDirectGdiImageSource&, DirectGdi::TGraphicsRotation)
	 */
	virtual void DrawResource(const TPoint& aPos, 
							const RDirectGdiDrawableSource& aSource, 
							DirectGdi::TGraphicsRotation aRotation) = 0;
	
	/**
	@see	CDirectGdiContext::DrawResource(const TRect&, const RDirectGdiImageSource&, DirectGdi::TGraphicsRotation)
	 */
	virtual void DrawResource(const TRect& aDestRect,
							const RDirectGdiDrawableSource& aSource,
							DirectGdi::TGraphicsRotation aRotation) = 0;
	
	/**
	@see	CDirectGdiContext::DrawResource(const TRect&, const RDirectGdiImageSource&, const TRect&, DirectGdi::TGraphicsRotation)
	 */
	virtual void DrawResource(const TRect& aDestRect,				
							const RDirectGdiDrawableSource& aSource,
							const TRect& aSourceRect,
							DirectGdi::TGraphicsRotation aRotation) = 0;
	
	/**
	@see	CDirectGdiContext::DrawResource(const TRect&, const RDirectGdiDrawableSource&, const TDesC8&)
	 */
	virtual void DrawResource(const TRect& aDestRect, const RDirectGdiDrawableSource& aSource, const TDesC8& aParam) = 0;
	
	/**
	Draws a glyph.
	
	@param	aPos			Position to start drawing the glyph after rotation has been applied (if necessary).
	@param	aChar			Character being drawn.
	@param	aGlyphImage		Pointer to the glyph image data.
	@param	aBitmapType		Type of bitmap format.
	@param	aGlyphImageSize	Glyph image size.
	@param	aClipRect		Clipping rect.
	@param	aRotation		Rotation specifying how the glyph will be drawn.
	
	@pre	The rendering target has been activated.
	@post	Request to draw the glyph has been accepted.
	 */
	virtual void DrawGlyph(const TPoint& aPos, 
						const TChar aChar, 
						const TUint8* aGlyphImage,
						const TGlyphBitmapType aBitmapType, 
						const TSize& aGlyphImageSize, 
						const TRect& aClipRect,
						const DirectGdi::TGraphicsRotation aRotation = DirectGdi::EGraphicsRotationNone) = 0;
	
	/**
	Copies the contents of a rectangular area on the target to another location.
	The source rectangle will be intersected with the full extent of the target.
	
	@param	aOffset Offset from the top left corner of the rectangle to be copied to the top left corner of the copy.
	@param	aRect Area to be copied.
	
	@pre 	The rendering target has been activated.
	@post 	Request to copy an area has been accepted. There is no guarantee that the
			request has been processed when this method returns.
	 */
	virtual void CopyRect(const TPoint& aOffset, const TRect& aRect) = 0;
	
	/**
	Writes the drawing engine object state to the passed write stream.
	
	@see    CDirectGdiContext::ExternalizeL()
	@see	InternalizeL()
	
	@param	aWriteStream Write stream.

	@pre	None.
	@post	The drawing engine object state has been written to the write stream.
	 */
	virtual void ExternalizeL(RWriteStream& aWriteStream) = 0;
	
	/**
	Sets the drawing engine object state from the read stream.
	
	@see    CDirectGdiContext::InternalizeL()
	@see	ExternalizeL()
	 
	@param	aReadStream Read stream.
	 
	@pre	None.
	@post	The drawing engine object state has been updated with the values from the read stream.
	 */
	virtual void InternalizeL(RReadStream& aReadStream) = 0;
	
	/**
	@see CDirectGdiContext::GetInterface()
	 */
	virtual TInt GetInterface(TUid aInterfaceId, TAny*& aInterface) = 0;
	
	/**
	Prepares the drawing engine for drawing text as multiple DrawGlyph() commands.
	This function must always have a corresponding call to EndDrawGlyph() after the multiple 
	DrawGlyph() commands have been made.
	Other than DrawGlyph(), no other rendering should be issued to the engine until EndDrawGlyph() has been called.
	
	@see EndDrawGlyph()
	@see DrawGlyph()
	
	@pre None.
	@post None.
	 */
	virtual void BeginDrawGlyph() = 0;
	
	/**
	Notifies the drawing engine that the last call to DrawGlyph() has been made for the 
	text string. This function must always be matched to a corresponding BeginDrawGlyph() call. 
	Other than DrawGlyph(), no other rendering should be issued to the engine until EndDrawGlyph() has 
	been called.
	
	@see BeginDrawGlyph()
	@see DrawGlyph()
	
	@pre BeginDrawGlyph() has been called.
	@post None.  
	 */
	virtual void EndDrawGlyph() = 0;
	
	};

const TUid KDirectGdiEngineUid = {0x10285A72};


#endif
