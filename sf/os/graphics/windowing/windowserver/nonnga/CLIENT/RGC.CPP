// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Shells for window server graphics class
// 
//

#include <e32std.h>
#include <graphics/wsdrawresource.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"
#include "w32comm.h"
#include <graphics/gdi/gdiconsts.h>
#include <graphics/gdi/gdistructs.h>

NONSHARABLE_STRUCT(CWindowGc::CPimpl): public CBase
/** @internalComponent @released */
	{
	friend class CWindowGc;

	CPimpl();
	CFbsFont *iFont;
	};

CWindowGc::CPimpl::CPimpl() : iFont(NULL)
	{
	}

enum {EPolygonMaxHeaderSize=sizeof(TWsCmdHeader)+sizeof(TWsGcCmdSegmentedDrawPolygonData)};

EXPORT_C CWindowGc::CWindowGc(CWsScreenDevice *aDevice) : MWsClientClass(aDevice->iBuffer), iPimpl(NULL), iDevice(aDevice)
/** Constructor which creates, but does not initialise a graphics context. 

@param aDevice Any screen device owned by the same session. Its life time 
should be at least as long as the gc itself. 
@see CWsScreenDevice::CreateContext() */
	{}

EXPORT_C CWindowGc::~CWindowGc()
/** Destructor. */
	{
	if (iBuffer && iWsHandle)
		Write(EWsGcOpFree);
	delete iPimpl;
	}

void CWindowGc::WriteTextCommand(TAny *cmd, TInt len,const TDesC &aBuf,TInt opcode,TInt opcodePtr) const
	{
	if ((aBuf.Size()+len)>(TInt)(iBuffer->BufferSize()-sizeof(TWsCmdHeader)))
		{
		WriteReplyByProvidingRemoteReadAccess(cmd,len,&aBuf,opcodePtr);
		}
	else
		{
		Write(cmd,len,aBuf.Ptr(),aBuf.Size(),opcode);
		}
	}

void CWindowGc::WriteTextCommand(TAny *cmd, TInt len,const TDesC8 &aBuf,TInt opcode,TInt opcodePtr) const
	{
	if ((aBuf.Size()+len)>(TInt)(iBuffer->BufferSize()-sizeof(TWsCmdHeader)))
		{
		WriteReplyByProvidingRemoteReadAccess(cmd,len,&aBuf,opcodePtr);
		}
	else
		{
		Write(cmd,len,aBuf.Ptr(),aBuf.Size(),opcode);
		}
	}	
	
EXPORT_C TInt CWindowGc::Construct()
/** Completes construction. 

@return KErrNone if successful, otherwise a leave error. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.
This function always causes a flush of the window server buffer. */
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	iPimpl = new CPimpl;
	if (!iPimpl)
		return KErrNoMemory;
	TInt ret;
	if ((ret=iBuffer->WriteReplyWs(EWsClOpCreateGc))<0)
		return(ret);
	iWsHandle=ret;
	return(KErrNone);
	}

EXPORT_C void CWindowGc::Activate(RDrawableWindow &aDevice)
/** Activates the context for a given window and updates iDevice with the pointer to the screen device of the screen on which aDevice is found.  

When drawing is complete, the code using the context should call Deactivate(). 
Draw methods invoked after an Activate() will affect the window specified. 
A graphics context can only be active for one window at a time. A panic occurs 
if a draw function is called before calling this function, or if Activate() 
is called twice without an intervening Deactivate().

@param aWindow The window for which the graphics context is to be activated. */
	{
	TUint devicePointer = WriteReplyInt(aDevice.WsHandle(),EWsGcOpActivate);
	iDevice = (CWsScreenDevice*)devicePointer;
	}

EXPORT_C void CWindowGc::Deactivate()
/** Frees the graphics context to be used with another window. 

This method should be called when the application has completed drawing to 
the window. */
	{
	Write(EWsGcOpDeactivate);
	iPimpl->iFont=NULL;
	}

//====================Functions from GDI.H===============================

EXPORT_C CGraphicsDevice* CWindowGc::Device() const
/** Returns a pointer to the device, more specifically a CWsScreenDevice, for the screen that the WindowGc was last activated on.
If the WindowGc has not been activated at all, it then returns the device that was passed to its constructor.

The user should be careful when calling this function since it can return the screen device of any screen in the system.
Hence, the return value of this function will be useful only if the user is aware of how the WindowGc was used before this function is called.
@return A pointer to the device for the screen that the WindowGc was last activated on or the device passed at construction*/
	{
	return(iDevice);
	}

EXPORT_C void CWindowGc::SetOrigin(const TPoint &aPoint)
/** Sets the position of the co-ordinate origin. 

All subsequent drawing operations are then done relative to this origin. The 
default origin is (0,0), the top left corner of the window.

@param aPoint A point for the origin, default (0,0). 
@see CGraphicsContext::SetOrigin() */
	{
	WritePoint(aPoint,EWsGcOpSetOrigin);
	}

EXPORT_C void CWindowGc::SetClippingRect(const TRect& aRect)
/** Sets a clipping rectangle.

Graphics drawn to the window are clipped, so that only items which fall within 
the rectangle are displayed. 

Note that clipping is additive. If a clipping region has been set using SetClippingRegion() 
then clipping will be to the intersection of that region and this rectangle.

@param aRect The clipping rectangle. 
@see SetClippingRegion() */
	{
	WriteRect(aRect,EWsGcOpSetClippingRect);
	}

EXPORT_C void CWindowGc::CancelClippingRect()
/** Cancels the clipping rectangle. 

@see SetClippingRect() */
	{
	Write(EWsGcOpCancelClippingRect);
	}

EXPORT_C TInt CWindowGc::SetClippingRegion(const TRegion &aRegion)
/** Sets the clipping region.

Drawing is always clipped to the visible area of a window. The region specified 
by this function is in addition to that area.

This function always causes a flush of the window server buffer.

@param aClippingRegion New clipping region. 
@return KErrNone if successful, KErrNoMemory if there is insufficient memory 
to create the region on the server side, otherwise another error code. */
	{
	const TInt regionCount=aRegion.Count();
	TPtrC8 ptrRect(reinterpret_cast<const TUint8*>(aRegion.RectangleList()),regionCount*sizeof(TRect));
	return(WriteReplyByProvidingRemoteReadAccess(&regionCount,sizeof(regionCount),&ptrRect,EWsGcOpSetClippingRegion));
	}

EXPORT_C void CWindowGc::CancelClippingRegion()
/** Cancels the current clipping region. */
	{
	Write(EWsGcOpCancelClippingRegion);
	}

EXPORT_C void CWindowGc::SetDrawMode(TDrawMode aDrawingMode)
/** Sets the drawing mode. 

This affects the colour that is actually drawn, because it defines the way 
that the current screen colour logically combines with the current pen colour 
and brush colour. 

There are 13 drawing modes (see CGraphicsContext::TDrawMode enum), each giving 
different logical combinations of pen, brush and screen colours. Each mode 
is produced by ORing together different combinations of seven drawing mode 
components (see CGraphicsContext::TDrawModeComponents enum).

The three most important modes are TDrawMode::EDrawModePEN, TDrawMode::EDrawModeNOTSCREEN 
and TDrawMode::EDrawModeXOR. The default drawing mode is TDrawMode::EDrawModePEN.

The drawing mode is over-ridden for line and shape drawing functions when 
a wide pen line has been selected. It is forced to TDrawMode::EDrawModePEN. 
This is to prevent undesired effects at line joins (vertexes).

Notes:

TDrawMode::EDrawModeAND gives a "colour filter" effect. For example:

ANDing with white gives the original colour 

ANDing with black gives black 

TDrawMode::EDrawModeOR gives a "colour boost" effect. For example:

ORing with black gives the original colour 

ORing with white gives white 

TDrawMode::EDrawModeXOR gives an "Exclusive OR" effect. For example:

white XOR black gives white 

white XOR white gives black 

black XOR black gives black

@param aDrawingMode A drawing mode. 
@see CGraphicsContext::SetDrawMode() */
	{
	WriteInt(aDrawingMode,EWsGcOpSetDrawMode);
	}

EXPORT_C void CWindowGc::UseFont(const CFont *aFont)
/** Sets this context's font.

The font is used for text drawing. If the font is already in the font and 
bitmap server's memory the GDI will share that copy.

Note that this function must be called prior to drawing text or the calling 
thread will panic.

@param aFont A device font. 
@see CGraphicsContext::UseFont() */
	{
	if (iPimpl->iFont!=(CFbsFont *)aFont)
		{
		iPimpl->iFont=(CFbsFont *)aFont;
		WriteInt(iPimpl->iFont->Handle(),EWsGcOpUseFont);
		}
	}

EXPORT_C void CWindowGc::DiscardFont()
/** Discards a font. 

This frees up the memory used (if the font is not being shared with some other 
process).

Note that if no font is in use when this function is called, then there is no effect.

@see CGraphicsContext::DiscardFont() */
	{
	Write(EWsGcOpDiscardFont);
	iPimpl->iFont=NULL;
	}

EXPORT_C void CWindowGc::SetUnderlineStyle(TFontUnderline aUnderlineStyle)
/** Sets the underline style for all subsequently drawn text.

@param aUnderlineStyle The underline style: either on or off.
@see CGraphicsContext::SetUnderlineStyle() */
	{
	WriteInt(aUnderlineStyle,EWsGcOpSetUnderlineStyle);
	}

EXPORT_C void CWindowGc::SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle)
/** Sets the strikethrough style for all subsequently drawn text.

@param aStrikethroughStyle The strikethrough style: either on or off. 
@see CGraphicsContext::SetStrikethroughStyle() */
	{
	WriteInt(aStrikethroughStyle,EWsGcOpSetStrikethroughStyle);
	}

void CWindowGc::SetJustification(TInt aExcessWidth,TInt aNumGaps, TInt aOpcode)
	{
	TWsGcCmdSetJustification justification;

	justification.excessWidth=aExcessWidth;
	justification.numGaps=aNumGaps;
	Write(&justification,sizeof(justification),aOpcode);
	}

EXPORT_C void CWindowGc::SetWordJustification(TInt aExcessWidth,TInt aNumGaps)
/** Sets word justification.

This function is particularly useful for doing WYSIWYG underlining or strikethrough, 
as it ensures that the lines extend correctly into the gaps between words. It is not 
intended for regular use by developers.

@param aExcessWidth The excess width (in pixels) to be distributed between 
the specified number of gaps (starting immediately) 
@param aNumGaps The number of gaps between words 
@see CGraphicsContext::SetWordJustification() */
	{
	SetJustification(aExcessWidth, aNumGaps, EWsGcOpSetWordJustification);
	}

EXPORT_C void CWindowGc::SetCharJustification(TInt aExcessWidth,TInt aNumChars)
/** Sets the character justification.

This function is used primarily to get accurate WYSIWYG, and is not intended 
for regular use by developers.

The text line that is to be justified has a certain number of characters (this 
includes the spaces between the words). It also has a distance (in pixels) 
between the end of the last word and the actual end of the line (right hand 
margin, usually). These excess width pixels are distributed amongst all the 
characters, increasing the gaps between them, to achieve full justification 
of the text line.

This function is particularly useful for WYSIWYG underlining or strikethrough, 
as it ensures that the lines extend into the gaps between characters.

See CGraphicsContext::SetCharJustification() for more information.

@param aExcessWidth The excess width (in pixels) to be distributed between 
the specified number of characters. 
@param aNumChars The number of characters involved 
@see CGraphicsContext::SetCharJustification() */
	{
	SetJustification(aExcessWidth, aNumChars, EWsGcOpSetCharJustification);
	}

EXPORT_C void CWindowGc::SetPenColor(const TRgb &aColor)
/** Sets the pen colour.

The effective pen colour depends on the drawing mode (see SetDrawMode()).

The default pen colour is black.

@param aColor The RGB colour for the pen. 
@see CGraphicsContext::SetPenColor() */
	{
	WriteInt(aColor.Internal(),EWsGcOpSetPenColor);
	}

EXPORT_C void CWindowGc::SetPenStyle(TPenStyle aPenStyle)
/** Sets the line drawing style for the pen. 

The pen is used when drawing lines and for the outline of filled shapes. There 
are 6 pen styles (see CGraphicsContext::TPenStyle enum). If no pen style is 
set, the default is TPenStyle::ESolidPen.

To use a pen style, its full context must be given, e.g. for a null pen: CGraphicsContext::TPenStyle::ENullPen.

@param aPenStyle A pen style. 
@see CGraphicsContext::SetPenStyle() */
	{
	WriteInt(aPenStyle,EWsGcOpSetPenStyle);
	}

EXPORT_C void CWindowGc::SetPenSize(const TSize& aSize)
/** Sets the line drawing size for the pen.

Lines of size greater than one pixel are drawn with rounded ends that extend 
beyond the end points, (as if the line is drawn using a circular pen tip of 
the specified size). Rounded ends of lines drawn with a wide pen are always 
drawn in TDrawMode::EDrawModePEN mode, overriding whatever mode has been set 
using SetDrawMode().

@param aSize A line size, the default being 1 pixel. 
@see CGraphicsContext::SetPenSize() */
	{
	WriteSize(aSize,EWsGcOpSetPenSize);
	}

EXPORT_C void CWindowGc::SetBrushColor(const TRgb &aColor)
/** Sets the brush colour. 

The effective brush colour depends on the drawing mode (see SetDrawMode()). 
If no brush colour has been set, it defaults to white. However the default 
brush style is null, so when drawing to a window, the default appears to be 
the window's background colour.

@param aColor The RGB colour for the brush. 
@see CGraphicsContext::SetBrushColor() */
	{
	WriteInt(aColor.Internal(),EWsGcOpSetBrushColor);
	}

EXPORT_C void CWindowGc::SetBrushStyle(TBrushStyle aBrushStyle)
/** Sets the line drawing style for the brush.

The GDI provides ten brush styles, including six built-in hatching patterns 
(see CGraphicsContext::TBrushStyle).

Use TBrushStyle::ENullBrush to draw the outline of a fillable shape on its 
own, without filling.

If the TBrushStyle::EPatternedBrush style is set, but no bitmap pattern has 
been selected using UseBrushPattern(), then the brush defaults to TBrushStyle::ENullBrush.

Hatching lines are done in the current brush colour, set using SetBrushColor(). 
Hatching can be overlaid on other graphics. The hatching pattern starts at 
the brush origin, set using SetBrushOrigin().

@param aBrushStyle The brush style. 
@see CGraphicsContext::SetBrushStyle() */
	{
	WriteInt(aBrushStyle,EWsGcOpSetBrushStyle);
	}

EXPORT_C void CWindowGc::SetBrushOrigin(const TPoint &aOrigin)
/** Sets the brush pattern origin. 

This specifies the position of the pixel in the top left corner of a reference 
pattern tile, (in absolute device co-ordinates). Other copies of the pattern 
tile are then drawn around the reference one. Thus the brush origin can be 
set as the top left corner of a shape.

The brush pattern may be a built-in style (see SetBrushStyle()), or a bitmap. 
To use a bitmap, the brush must have a pattern set (see UseBrushPattern()) 
and the brush style must be set to TBrushStyle::EPatternedBrush.

Notes:

If SetBrushOrigin() is not used, then the origin defaults to (0,0).

This brush origin remains in effect for all fillable shapes drawn subsequently, 
until a new brush origin is set. Shapes can thus be considered as windows 
onto a continuous pattern field (covering the whole clipping region of a screen 
device, or the whole device area of a printer).

@param aOrigin The origin point for the brush. 
@see CGraphicsContext::SetBrushOrigin() */
	{
	WritePoint(aOrigin,EWsGcOpSetBrushOrigin);
	}

EXPORT_C void CWindowGc::UseBrushPattern(const CFbsBitmap *aDevice)
/** Sets the brush pattern to the specified bitmap. 

For the brush to actually use the bitmap, TBrushStyle::EPatternedBrush must 
be used to set the brush style (see SetBrushStyle()). When the brush pattern 
is no longer required, use DiscardBrushPattern() to free up the memory used, 
(if the bitmap is not being shared). If UseBrushPattern() is used again without 
using DiscardBrushPattern() then the previous pattern is discarded automatically.

Notes:

When loading a bitmap, the GDI checks to see if the bitmap is already in memory. 
If the bitmap is already there, then that copy is shared.

The brush does not need to have a pattern set at all. There are several built-in 
hatching patterns, which can be selected using SetBrushStyle().

@param aDevice A bitmap pattern for the brush 
@see CGraphicsContext::UseBrushPattern() */
	{
	WriteInt(aDevice->Handle(),EWsGcOpUseBrushPattern);
	AddToBitmapArray(aDevice->Handle());
	}

EXPORT_C void CWindowGc::DiscardBrushPattern()
/** Discards a non-built-in brush pattern. 

This frees up the memory used for the bitmap, if it is not being shared by 
another process.

If no brush pattern has been set when this function is called, it has no effect.

@see CGraphicsContext::DiscardBrushPattern() */
	{
	Write(EWsGcOpDiscardBrushPattern);
	}

EXPORT_C void CWindowGc::Plot(const TPoint &aPoint)
/** Draws a single point. 

The point is drawn with the current pen settings using the current drawing 
mode.

Note: if the pen size is greater than one pixel, a filled circle of the current 
pen colour is drawn, with the pen size as the diameter and the plotted point 
as the centre. If the pen size is an even number of pixels, the extra pixels 
are drawn below and to the right of the centre. See SetPenSize().

@param aPoint The point to be drawn. 
@see CGraphicsContext::Plot() */
	{
	WritePoint(aPoint,EWsGcOpPlot);
	}

EXPORT_C void CWindowGc::DrawLine(const TPoint &aPoint1,const TPoint &aPoint2)
/** Draws a straight line between two points.

@param aPoint1 The point at the start of the line. 
@param aPoint2 The point at the end of the line. 
@see CGraphicsContext::DrawLine() */
	{
	TWsGcCmdDrawLine drawLine(aPoint1,aPoint2);
	Write(&drawLine,sizeof(drawLine),EWsGcOpDrawLine);
	}

EXPORT_C void CWindowGc::MoveTo(const TPoint &aPoint)
/** Moves the internal drawing position relative to the co-ordinate origin, without 
drawing a line.

A subsequent call to DrawLineTo() or DrawLineBy() will then use the new internal 
drawing position as the start point for the line drawn.

Notes:

The operations DrawLine(), DrawLineTo(), DrawLineBy() and DrawPolyline() also 
change the internal drawing position to the last point of the drawn line(s).

The internal drawing position is set to the co-ordinate origin if no drawing 
or moving operations have yet taken place.

@param aPoint The point to move the internal drawing position to. 
@see CGraphicsContext::MoveTo()
@see CGraphicsContext::MoveBy() */
	{
	WritePoint(aPoint,EWsGcOpMoveTo);
	}

EXPORT_C void CWindowGc::MoveBy(const TPoint &aPoint)
/** Moves the internal drawing position by a vector, without drawing a line.

The internal drawing position is moved relative to its current co-ordinates.

@param aPoint The vector to move the internal drawing position by. 
@see CGraphicsContext::MoveBy()
@see CGraphicsContext::MoveTo() */
	{
	WritePoint(aPoint,EWsGcOpMoveBy);
	}

EXPORT_C void CWindowGc::DrawLineTo(const TPoint &aPoint)
/** Draws a straight line from the current internal drawing position to a point.

@param aPoint The point at the end of the line. 
@see CGraphicsContext::DrawLineTo() */
	{
	WritePoint(aPoint,EWsGcOpDrawTo);
	}

EXPORT_C void CWindowGc::DrawLineBy(const TPoint &aPoint)
/** Draws a straight line relative to the current internal drawing position, using 
a vector.

The start point of the line is the current internal drawing position. The 
vector aVector is added to the internal drawing position to give the end point 
of the line

@param aPoint The vector to add to the current internal drawing position, 
giving the end point of the line. 
@see CGraphicsContext::DrawLineBy() */
	{
	WritePoint(aPoint,EWsGcOpDrawBy);
	}

void CWindowGc::doDrawPolyLine(const CArrayFix<TPoint> *aPointArray, const TPoint* aPointList,TInt aNumPoints)
	{
	TWsGcOpcodes opcode=EWsGcOpDrawPolyLine;
	TWsGcCmdDrawPolyLine polyLine;
	TInt maxBufLen=(iBuffer->BufferSize()-sizeof(TWsCmdHeader)-sizeof(polyLine))/sizeof(TPoint);
	TInt sent=0;
	while(sent<aNumPoints)
		{
		TInt availableLen;
		const TPoint *ptr;
		if (aPointArray)
			{
			ptr=&(*aPointArray)[sent];
			availableLen=aPointArray->End(sent)-ptr;
			}
		else
			{
			ptr=aPointList+sent;
			availableLen=aNumPoints-sent;
			}
		polyLine.numPoints=Min(availableLen,maxBufLen);
		sent+=polyLine.numPoints;
		polyLine.more=(sent!=aNumPoints);
		Write(&polyLine,sizeof(polyLine),ptr,polyLine.numPoints*sizeof(TPoint),opcode);
		polyLine.last=ptr[polyLine.numPoints-1];
		opcode=EWsGcOpDrawPolyLineContinued;
		} 
	}

EXPORT_C void CWindowGc::DrawPolyLine(const TPoint* aPointList,TInt aNumPoints)
/** Draws a polyline using points in a list. 

A polyline is a series of concatenated straight lines joining a set of points.

@param aPointList Pointer to a list of points on the polyline. 
@param aNumPoints The number of points in the point list. 
@see CGraphicsContext::DrawPolyLine() */
	{
	doDrawPolyLine(NULL,aPointList,aNumPoints);
	}

EXPORT_C void CWindowGc::DrawPolyLine(const CArrayFix<TPoint> *aPointArray)
/** Draws a polyline using points in an array. 

A polyline is a series of concatenated straight lines joining a set of points.

@param aPointArray An array containing the points on the polyline. 
@see CGraphicsContext::DrawPolyLine() */
	{
	doDrawPolyLine(aPointArray,NULL,aPointArray->Count());
	}

TInt CWindowGc::doDrawPolygon(const CArrayFix<TPoint> *aPointArray,const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
	{
	if (aNumPoints<=0)
		return(KErrNone);
	TWsGcCmdSegmentedDrawPolygonData polyData;
	polyData.index=0;
	TInt maxBufLen=(iBuffer->BufferSize()-EPolygonMaxHeaderSize)/sizeof(TPoint);
	FOREVER
		{
		const TPoint *ptr;
		TInt availableLen;
		if (aPointArray)
			{
			ptr=&(*aPointArray)[polyData.index];
			availableLen=aPointArray->End(polyData.index)-ptr;
			}
		else
			{
			ptr=aPointList+polyData.index;
			availableLen=aNumPoints-polyData.index;
			}
		polyData.numPoints=Min(availableLen,maxBufLen);
		if (polyData.index==0)	// First time around
			{
			if (polyData.numPoints==aNumPoints)	// Can it be done in one go?
				{
				TWsGcCmdDrawPolygon drawPolygon;
				drawPolygon.numPoints=aNumPoints;
				drawPolygon.fillRule=aFillRule;
				Write(&drawPolygon,sizeof(drawPolygon),ptr,aNumPoints*sizeof(TPoint),EWsGcOpDrawPolygon);
				break;
				}
			TWsGcCmdStartSegmentedDrawPolygon start;
			start.totalNumPoints=aNumPoints;
			TInt err=WriteReply(&start,sizeof(start),EWsGcOpStartSegmentedDrawPolygon);
			if (err!=KErrNone)
				return(err);
			}
		Write(&polyData,sizeof(polyData),ptr,polyData.numPoints*sizeof(TPoint),EWsGcOpSegmentedDrawPolygonData);
		polyData.index+=polyData.numPoints;
		if (polyData.index==aNumPoints)
			{
			TWsGcCmdDrawSegmentedPolygon draw;
			draw.fillRule=aFillRule;
			Write(&draw,sizeof(draw),EWsGcOpDrawSegmentedPolygon);
			break;
			}
		}
	return(KErrNone);
	}

EXPORT_C TInt CWindowGc::DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule)
/** Draws and fills a polygon using points defined in a list.

The first TPoint in the list defines the start of the first side of the polygon. 
The second TPoint defines the second vertex (the end point of the first side 
and the start point of the second side) and so on. The final side of the polygon 
is drawn using the last TPoint from the array or list, and the line drawn 
to the start point of the first side.

Self-crossing polygons can be filled according to one of two rules, TFillRule::EAlternate 
(the default), or TFillRule::EWinding. To explain the difference between these 
rules, the concept of a winding number needs to be introduced. The area outside 
any of the loops of the polygon has a winding number of zero, and is never 
filled. An inside a loop which is bounded by an area with winding number 0 
has a winding number of 1. If an area is within a loop that is bounded by 
an area with winding number 1, e.g. a loop within a loop, has a winding number 
of 2, and so on.

The filling of a polygon proceeds according to this algorithm:

If aFillRule is TFillRule::EAlternate (default) and it has an odd winding 
number, then fill the surrounding area.

If aFillRule is TFillRule::EWinding and it has a winding number greater than 
zero, then fill the surrounding area.

This function always causes a flush of the window server buffer.

@param aPointList Pointer to a list of points, specifying the vertices of 
the polygon. 
@param aNumPoints The number of points in the vertex list 
@param aFillRule Either TFillRule::EAlternate (the default) or TFillRule::EWinding. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see CGraphicsContext::DrawPolygon() */
	{
	return(doDrawPolygon(NULL,aPointList,aNumPoints,aFillRule));
	}

EXPORT_C TInt CWindowGc::DrawPolygon(const CArrayFix<TPoint> *aPointArray,TFillRule aFillRule)
/** Draws and fills a polygon using points defined in an array.

The first TPoint in the array defines the start of the first side of the polygon. 
The second TPoint defines the second vertex (the end point of the first side 
and the start point of the second side) and so on. The final side of the polygon 
is drawn using the last TPoint from the array or list, and the line drawn 
to the start point of the first side.

Self-crossing polygons can be filled according to one of two rules, TFillRule::EAlternate 
(the default), or TFillRule::EWinding. To explain the difference between these 
rules, the concept of a winding number needs to be introduced. The area outside 
any of the loops of the polygon has a winding number of zero, and is never 
filled. An inside a loop which is bounded by an area with winding number 0 
has a winding number of 1. If an area is within a loop that is bounded by 
an area with winding number 1, e.g. a loop within a loop, has a winding number 
of 2, and so on.

The filling of a polygon proceeds according to this algorithm:

If aFillRule is TFillRule::EAlternate (default) and it has an odd winding 
number, then fill the surrounding area.

If aFillRule is TFillRule::EWinding and it has a winding number greater than 
zero, then fill the surrounding area.

This function always causes a flush of the window server buffer.

@param aPointArray An array of points, specifying the vertices of the polygon. 
@param aFillRule Either TFillRule::EAlternate (the default) or TFillRule::EWinding. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see CGraphicsContext::DrawPolygon() */
	{
	return(doDrawPolygon(aPointArray,NULL,aPointArray->Count(),aFillRule));
	}

void CWindowGc::DrawArcOrPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd, TInt aOpcode)
	{
	TWsGcCmdDrawArcOrPie cmd(aRect,aStart,aEnd);
	Write(&cmd,sizeof(cmd),aOpcode);
	}

EXPORT_C void CWindowGc::DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
/** Draws an arc (a portion of an ellipse).

The point aStart is used to define one end of a line from the geometric centre 
of the ellipse. The point of intersection between this line and the ellipse 
defines the start point of the arc. The point aEnd is used to define one end 
of a second line from the geometric centre of the ellipse. The point of intersection 
between this line and the ellipse defines the end point of the arc. The pixels 
at both the start point and the end point are drawn.

The arc itself is the segment of the ellipse in an anti-clockwise direction 
from the start point to the end point.

Notes

A rectangle is used in the construction of the ellipse of which the arc is 
a segment. This rectangle is passed as an argument of type TRect.

A wide line arc is drawn with the pixels distributed either side of a true 
ellipse, in such a way that the outer edge of the line would touch the edge 
of the construction rectangle. In other words, the ellipse used to construct 
it is slightly smaller than that for a single pixel line size.

If aStart or aEnd are the ellipse centre then the line that defines the start/end 
of the arc defaults to one extending vertically above the centre point.

If aStart and aEnd are the same point, or points on the same line through 
the ellipse centre then a complete unfilled ellipse is drawn.

Line drawing is subject to pen colour, width and style and draw mode

@param aRect The rectangle in which to draw the ellipse (of which the arc is 
a segment).
@param aStart A point to define the start of the arc.
@param aEnd A point to define the end of the arc.
@see CGraphicsContext::DrawArc() */
	{
	DrawArcOrPie(aRect,aStart,aEnd,EWsGcOpDrawArc);
	}

EXPORT_C void CWindowGc::DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd)
/** Draws and fills a pie-shaped slice of an ellipse.

Outlines are subject to the current pen colour, width, style and draw mode. 
Set the pen to ENullPen for no outline. The fill is subject to brush style 
(colour, hash or pattern), the origin and the current drawing mode. Set the 
brush to ENullBrush for no fill.

The point aStart is used to define one end of a line to the centre of the 
ellipse. The point of intersection between this line and the ellipse defines 
the start point of the arc bounding the pie slice. The point aEnd is used 
to define one end of a second line to the centre of the ellipse. The point 
of intersection between this line and the ellipse defines the end point of 
the arc bounding the pie slice. The pixels at the end point are not drawn.

The pie slice itself is the area bounded by: the arc of the ellipse in an 
anticlockwise direction from the start point to the end point; the straight 
line from the start point from the geometric centre of the ellipse; the 
straight line from the end point from the geometric centre of the ellipse.

The line drawn by the pen goes inside the rectangle given by the aRect argument.

Notes:

A rectangle is used in the construction of the pie slice. This rectangle is 
passed as an argument of type TRect. The curved edge of the pie slice is an 
arc of an ellipse constructed within the rectangle.

A wide line edged pie slice has the arc drawn with the pixels distributed 
either side of a true ellipse. This is done in such a way that the outer edge 
of the line would touch the edge of the construction rectangle. In other words, 
the ellipse used to construct it is slightly smaller than that for a single 
pixel line size.

If aStart or aEnd are the ellipse centre then the line that defines the start/end 
of the arc defaults to one extending vertically above the centre point.

If aStart and aEnd are the same point, or points on the same line through 
the ellipse centre then a complete filled ellipse is drawn. A line is also 
drawn from the edge to the ellipse centre.

@param aRect A rectangle in which to draw the ellipse bounding the pie slice 
@param aStart A point to define the start of the pie slice 
@param aEnd A point to define the end of the pie slice 
@see CGraphicsContext::DrawPie() */
	{
	DrawArcOrPie(aRect,aStart,aEnd,EWsGcOpDrawPie);
	}

EXPORT_C void CWindowGc::DrawEllipse(const TRect &aRect)
/** Draws and fills an ellipse.

The ellipse is drawn inside the rectangle defined by the aRect argument. Any 
TRect that has odd pixel dimensions, has the bottom right corner trimmed to 
give even pixel dimensions before the ellipse is constructed.

The column and row of pixels containing the bottom right co-ordinate of the 
aRect argument are not part of the rectangle.

Note: a wide outline ellipse is drawn with the pixels distributed either side of 
a true ellipse, in such a way that the outer edge of the line touches the 
edge of the construction rectangle. In other words, the ellipse used to construct 
it is smaller than that for a single pixel line size.

@param aRect The rectangle in which to draw the ellipse 
@see CGraphicsContext::DrawEllipse() */
	{
	WriteRect(aRect,EWsGcOpDrawEllipse);
	}

EXPORT_C void CWindowGc::DrawRect(const TRect &aRect)
/** Draws and fills a rectangle. 

The rectangle's border is drawn with the pen, and it is filled using the brush.

@param aRect The rectangle to be drawn. 
@see CGraphicsContext::DrawRect() */
	{
	WriteRect(aRect,EWsGcOpDrawRect);
	}

EXPORT_C void CWindowGc::DrawRoundRect(const TRect &aRect,const TSize &aEllipse)
/** Draws and fills a rectangle with rounded corners.

The rounded corners are each constructed as an arc of an ellipse. The dimensions 
of each corner (corner size and corner height) are given by aEllipse. See 
DrawArc() for a description of arc construction.

The line drawn by the pen (if any) goes inside the rectangle given by the 
TRect argument.

Notes:

Dotted and dashed pen styles cannot be used for the outline of a rounded rectangle.

If either corner size dimension is greater than half the corresponding rectangle 
length, the corner size dimension is reduced to half the rectangle size.

@param aRect The rectangle to be drawn. 
@param aEllipse The dimensions of each corner. 
@see CGraphicsContext::DrawRoundRect() */
	{
	TWsGcCmdDrawRoundRect drawRoundRect(aRect,aEllipse);
	Write(&drawRoundRect,sizeof(drawRoundRect),EWsGcOpDrawRoundRect);
	}

EXPORT_C void CWindowGc::DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aDevice)
/** Draws a bitmap at a specified point. 

The function does a compress/stretch based on its internally stored size in 
twips. Note that if the twips value of the bitmap is not set then nothing 
is drawn (this is the default situation).

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

Note: this member function uses the bitmap's size in twips and does a stretch/compress 
blit using a linear DDA.

@param aTopLeft The point where the top left pixel of the bitmap is to be 
drawn 
@param aDevice The source bitmap. 
@see CGraphicsContext::DrawBitmap() */
	{
	TWsGcCmdDrawBitmap drawBitmap(aTopLeft,aDevice->Handle());
	Write(&drawBitmap,sizeof(drawBitmap),EWsGcOpDrawBitmap);
	AddToBitmapArray(aDevice->Handle());
	}

EXPORT_C void CWindowGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice)
/** Draws a bitmap in a rectangle.

The bitmap is compressed/stretched to fit the specified rectangle. Note that 
if the twips value of the bitmap is not set then nothing is drawn (this is 
the default situation).

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

Notes: this member function uses the bitmap's size in pixels and does a stretch/compress 
blit using a linear DDA.

@param aDestRect The rectangle within which the bitmap is to be drawn. 
@param aDevice The source bitmap. 
@see CGraphicsContext::DrawBitmap() */
	{
	TWsGcCmdDrawBitmap2 drawBitmap(aDestRect,aDevice->Handle());
	Write(&drawBitmap,sizeof(drawBitmap),EWsGcOpDrawBitmap2);
	AddToBitmapArray(aDevice->Handle());
	}

EXPORT_C void CWindowGc::DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice, const TRect &aSourceRect)
/** Draws a specified rectangle from a bitmap into another rectangle.

The function compresses/stretches the specified rectangle from the bitmap 
to fit the destination rectangle. Note that if the twips value of the bitmap 
is not set then nothing is drawn (this is the default situation).

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap. However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

Note: this member function uses rectangle sizes in pixels and does a stretch/compress 
blit using a linear DDA.

@param aDestRect The rectangle within which the bitmap is to be drawn. 
@param aDevice A source bitmap. 
@param aSourceRect The rectangle in the source bitmap that is copied to the 
destination rectangle. 
@see CGraphicsContext::DrawBitmap() */
	{
	TWsGcCmdDrawBitmap3 drawBitmap(aDestRect,aDevice->Handle(),aSourceRect);
	Write(&drawBitmap,sizeof(drawBitmap),EWsGcOpDrawBitmap3);
	AddToBitmapArray(aDevice->Handle());
	}

/** Draws a specified rectangle from a bitmap and its mask into another rectangle.

The function compresses/stretches the specified rectangle from the bitmap 
to fit the destination rectangle. 
The mask bitmap can be used as either a positive or negative mask. Masked 
pixels are not mapped to the destination rectangle.

A black and white (binary) mask bitmap is used. With aInvertMask=EFalse, black 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle. With aInvertMask=ETrue, white 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle.

If mask bitmap's display mode is EColor256, the function does AplhaBlending
and ignores aInvertMask parameter.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

Note: this member function uses rectangle sizes in pixels and does a stretch/compress 
blit using a linear DDA.

@param aDestRect The rectangle within which the masked bitmap is to be drawn. 
@param aBitmap A source bitmap. 
@param aSourceRect The rectangle in the source bitmap that is copied to the 
destination rectangle.
@param aMaskBitmap A mask bitmap. 
@param aInvertMask If false, a source pixel that is masked by a black pixel 
is not transferred to the destination rectangle. If true, then a source pixel 
that is masked by a white pixel is not transferred to the destination rectangle. */
EXPORT_C void CWindowGc::DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask)
	{
	TWsGcCmdDrawBitmapMasked drawBitmap(aDestRect,aBitmap->Handle(),aSourceRect,aMaskBitmap->Handle(),aInvertMask);
	Write(&drawBitmap,sizeof(drawBitmap),EWsGcOpDrawBitmapMasked);
	AddToBitmapArray(aBitmap->Handle());
	AddToBitmapArray(aMaskBitmap->Handle());
	}

EXPORT_C void CWindowGc::DrawBitmapMasked(const TRect& aDestRect, const CWsBitmap* aBitmap, const TRect& aSourceRect, const CWsBitmap* aMaskBitmap, TBool aInvertMask)
/** Draws a specified rectangle from a wserv bitmap and its mask into 
another rectangle.

The function compresses/stretches the specified rectangle from the bitmap 
to fit the destination rectangle. 
The mask bitmap can be used as either a positive or negative mask. Masked 
pixels are not mapped to the destination rectangle.

A black and white (binary) mask bitmap is used. With aInvertMask=EFalse, black 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle. With aInvertMask=ETrue, white 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle.

If mask bitmap's display mode is EColor256, the function does AplhaBlending
and ignores aInvertMask parameter.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

Note: this member function uses rectangle sizes in pixels and does a stretch/compress 
blit using a linear DDA.

@param aDestRect The rectangle within which the masked bitmap is to be drawn. 
@param aBitmap A source wserv bitmap. 
@param aSourceRect The rectangle in the source bitmap that is copied to the 
destination rectangle.
@param aMaskBitmap A mask wserv bitmap. 
@param aInvertMask If false, a source pixel that is masked by a black pixel 
is not transferred to the destination rectangle. If true, then a source pixel 
that is masked by a white pixel is not transferred to the destination rectangle. */
	{
	TWsGcCmdDrawBitmapMasked drawBitmap(aDestRect,aBitmap->WsHandle(),aSourceRect,aMaskBitmap->WsHandle(),aInvertMask);
	Write(&drawBitmap,sizeof(drawBitmap),EWsGcOpWsDrawBitmapMasked);
	AddToBitmapArray(aBitmap->Handle());
	AddToBitmapArray(aMaskBitmap->Handle());
	}
EXPORT_C void CWindowGc::DrawText(const TDesC &aBuf, const TPoint &aPos)
/** Draws horizontal text with no surrounding box. 

The appearance of the text is subject to the drawing mode, the font, pen colour, 
word justification and character justification. 

A panic occurs if this function is called when there is no font: see UseFont().

@param aBuf The string to write. 
@param aPos The point specifying the position of the baseline at the left 
end of the text.
@see CGraphicsContext::DrawText() */
	{
	TWsGcCmdDrawText printText(aPos,aBuf.Length());
	WriteTextCommand(&printText,sizeof(printText),aBuf,EWsGcOpDrawText,EWsGcOpDrawTextPtr);
	}

EXPORT_C void CWindowGc::DrawText(const TDesC &aBuf,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz,TInt aLeftMrg)
/** Draws horizontal text within a cleared box.

The appearance of the text is subject to the drawing mode, the font, pen colour, 
word justification and character justification. It is also subject to the 
background brush (set brush to ENullBrush for no effect on background).

A panic occurs if this function is called when there is no font: see UseFont().

Note: the text is clipped to the box. You must ensure that the specified string 
is not too large.

@param aBuf The text to write.
@param aBox The box to draw the text in. 
@param aBaselineOffset An offset from the top of the box to the text baseline. 
Note that the baseline is the line on which letters sit, for instance below r, s, t, and 
above the tail of q, and y. 
@param aHoriz The text alignment mode (default is left, rather than centre 
or right). 
@param aLeftMrg The left margin for left-aligned text, or the right margin 
for right-aligned text (default is zero). 
@see CGraphicsContext::DrawText() */
	{
	if (aBuf.Size()<(TInt)(iBuffer->BufferSize()-sizeof(TWsCmdHeader)-sizeof(TWsGcCmdBoxTextOptimised2))) 
		{
		if (aHoriz==ELeft && aLeftMrg==0)
			{
			TWsGcCmdBoxTextOptimised1 boxTextOpt1(aBox,aBaselineOffset,aBuf.Length());
			Write(&boxTextOpt1,sizeof(boxTextOpt1),aBuf.Ptr(),aBuf.Size(),EWsGcOpDrawBoxTextOptimised1);
			}
		else
			{
			TWsGcCmdBoxTextOptimised2 boxTextOpt2(aBox,aBaselineOffset,aHoriz,aLeftMrg,aBuf.Length());
			Write(&boxTextOpt2,sizeof(boxTextOpt2),aBuf.Ptr(),aBuf.Size(),EWsGcOpDrawBoxTextOptimised2);
			}
		}
	else
		{
		TWsGcCmdBoxText boxText(aBox,aBaselineOffset,aHoriz,aLeftMrg,aBuf.Length(),iPimpl->iFont->TextWidthInPixels(aBuf));
		WriteTextCommand(&boxText,sizeof(boxText),aBuf,EWsGcOpDrawBoxText,EWsGcOpDrawBoxTextPtr);
		}
	}

EXPORT_C void CWindowGc::DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp)
/** Draws vertical text in the specified direction.

A panic occurs if this function is called when there is no font: see UseFont().

@param aText The text to be drawn. 
@param aPos Point of origin of the text baseline. 
@param aUp Direction. ETrue for up, EFalse for down. */
	{
	TWsGcCmdDrawTextVertical printText(aPos,aText.Length(),aUp);
	WriteTextCommand(&printText,sizeof(printText),aText,EWsGcOpDrawTextVertical,EWsGcOpDrawTextVerticalPtr);
	}

EXPORT_C void CWindowGc::DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert,TInt aMargin)
/** Draws text vertically in the specified direction, within a box of the specified 
size.

A panic occurs if this function is called when there is no font: see UseFont().

@param aText The text to be drawn. 
@param aBox The bounding box within which the text should be drawn, and which 
it is clipped to.
@param aBaselineOffset The height of the top of the characters from their text 
baseline. 
@param aUp The direction. ETrue for up, EFalse for down.
@param aVert The text alignment. 
@param aMargin The margin. */
	{
	TWsGcCmdBoxTextVertical boxText(aBox);
	boxText.baselineOffset=aBaselineOffset;
	boxText.up=aUp;
	boxText.vert=aVert;
	boxText.margin=aMargin;
	boxText.length=aText.Length();
	boxText.width=iPimpl->iFont->TextWidthInPixels(aText);
	WriteTextCommand(&boxText,sizeof(boxText),aText,EWsGcOpDrawBoxTextVertical,EWsGcOpDrawBoxTextVerticalPtr);
	}

//========================Extra functions============================

EXPORT_C void CWindowGc::CopyRect(const TPoint &anOffset,const TRect &aRect)
/** Copies a rectangle from any part of the screen into the window that the gc 
is active on.

The copy part of the operation applies to the whole rectangle, irrespective 
of whether or not it within the window, however the "paste" is clipped to 
the drawing area.

The rectangle is specified in window coordinates (if the top-left of the rectangle 
is (0,0) then the area of the screen it specifies has its top-left at the 
top left corner of the window, if it is (-10,-10) then it starts 10 pixels 
above and to the left of the window). 

Note: shadows in the source rectangle will be copied. None of the area drawn to 
will gain shadowing (even if the window is already in shadow).

This version of this function is only really suitable for testing.

@param anOffset The offset from the original position to the point where the 
rectangle is copied. 
@param aRect The rectangular area to be copied. This is in window co-ordinates, 
e.g. the top left corner of the window is position (0,0) with respect to the 
rectangle.
@see CBitmapContext::CopyRect() */
	{
	TWsGcCmdCopyRect copyRect(anOffset,aRect);
	Write(&copyRect,sizeof(copyRect),EWsGcOpCopyRect);
	}

EXPORT_C void CWindowGc::BitBlt(const TPoint &aPoint, const CFbsBitmap *aBitmap)
/** Performs a bitmap block transfer.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap. However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

@param aPoint The position for the top left corner of the bitmap. 
@param aBitmap A memory-resident bitmap. 
@see CBitmapContext::BitBlt() */
	{
	if (aBitmap == NULL || !aBitmap->Handle())
		return; 
	TWsGcCmdGdiBlt2 gdiBlit(aPoint,aBitmap->Handle());
	Write(&gdiBlit,sizeof(gdiBlit),EWsGcOpGdiBlt2);
	AddToBitmapArray(aBitmap->Handle());
	}

EXPORT_C void CWindowGc::BitBlt(const TPoint &aDestination,const CFbsBitmap *aBitmap,const TRect &aSource)
/** Performs a bitmap block transfer of a rectangular piece of a bitmap.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

Note: if the rectangle aSource is larger than the bitmap then the bitmap will be padded 
with white.

@param aDestination The position for the top left corner of the bitmap. 
@param aBitmap A memory-resident bitmap 
@param aSource A rectangle defining the piece of the bitmap to be drawn, with 
co-ordinates relative to the top left corner of the bitmap 
@see CBitmapContext::BitBlt() */
	{
	if (aBitmap == NULL || !aBitmap->Handle())
		return; 
	TWsGcCmdGdiBlt3 gdiBlit(aDestination,aBitmap->Handle(),aSource);
	Write(&gdiBlit,sizeof(gdiBlit),EWsGcOpGdiBlt3);
	AddToBitmapArray(aBitmap->Handle());
	}

EXPORT_C void CWindowGc::BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask)
/** Performs a masked bitmap block transfer of a memory resident source bitmap.

The mask bitmap can be used as either a positive or negative mask. Masked 
pixels are not mapped to the destination rectangle.

A black and white (binary) mask bitmap is used. With aInvertMask=EFalse, black 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle. With aInvertMask=ETrue, white 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

@param aPoint A position for the top left corner of the bitmap. 
@param aBitmap A memory-resident source bitmap. 
@param aSourceRect A rectangle defining the piece of the bitmap to be drawn, 
with co-ordinates relative to the top left corner of the bitmap 
@param aMaskBitmap A mask bitmap. 
@param aInvertMask If false, a source pixel that is masked by a black pixel 
is not transferred to the destination rectangle. If true, then a source pixel 
that is masked by a white pixel is not transferred to the destination rectangle. 

@see CBitmapContext::BitBltMasked() */
	{
	if (aBitmap == NULL || !aBitmap->Handle() || aMaskBitmap == NULL || !aMaskBitmap->Handle())
		return; 
	TWsGcCmdBltMasked gdiBlitMasked(aPoint,aBitmap->Handle(),aSourceRect,aMaskBitmap->Handle(),aInvertMask);
	Write(&gdiBlitMasked,sizeof(gdiBlitMasked),EWsGcOpGdiBltMasked);
	AddToBitmapArray(aBitmap->Handle());
	AddToBitmapArray(aMaskBitmap->Handle());
	}

EXPORT_C void CWindowGc::BitBlt(const TPoint &aPoint, const CWsBitmap *aBitmap)
/** Performs a bitmap block transfer on a bitmap to which the window server already 
has a handle.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

This function should be used in preference to the CFbsBitmap overload if the 
bitmap is to be used more than once, as it is a lot quicker.

@param aPoint The position for the top left corner of the bitmap. 
@param aBitmap A window server bitmap. 
@see CBitmapContext::BitBlt() */
	{
	if (aBitmap == NULL || !aBitmap->Handle())
		return; 
	TWsGcCmdGdiBlt2 gdiBlit(aPoint,aBitmap->WsHandle());
	Write(&gdiBlit,sizeof(gdiBlit),EWsGcOpGdiWsBlt2);
	AddToBitmapArray(aBitmap->Handle());
	}

EXPORT_C void CWindowGc::BitBlt(const TPoint &aDestination,const CWsBitmap  *aBitmap,const TRect &aSource)
/** Performs a bitmap block transfer of a rectangular piece of a bitmap to which 
the window server already has a handle.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

This function should be used in preference to the CFbsBitmap overload if the 
bitmap is to be used more than once, as it is a lot quicker.

Note: if the rectangle aSource is larger than the bitmap then the bitmap will be padded 
with white.
	
@param aDestination The position for the top left corner of the bitmap. 
@param aBitmap A window server bitmap. 
@param aSource A rectangle defining the piece of the bitmap to be drawn, with 
co-ordinates relative to the top left corner of the bitmap 
@see CBitmapContext::BitBlt() */
	{
	if (aBitmap == NULL || !aBitmap->Handle())
		return; 
	TWsGcCmdGdiBlt3 gdiBlit(aDestination,aBitmap->WsHandle(),aSource);
	Write(&gdiBlit,sizeof(gdiBlit),EWsGcOpGdiWsBlt3);
	AddToBitmapArray(aBitmap->Handle());
	}

EXPORT_C void CWindowGc::BitBltMasked(const TPoint& aPoint,const CWsBitmap * aBitmap,const TRect& aSourceRect,const CWsBitmap * aMaskBitmap,TBool aInvertMask)
/** Performs a masked bitmap block transfer of a window server bitmap.

The mask bitmap can be used as either a positive or negative mask. Masked 
pixels are not mapped to the destination rectangle.

A black and white (binary) mask bitmap is used. With aInvertMask=EFalse, black 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle. With aInvertMask=ETrue, white 
pixels in the mask bitmap stop corresponding pixels in the source bitmap from 
being transferred to the destination rectangle.

This function should be used in preference to the CFbsBitmap overload if the 
bitmap is to be used more than once, as it is a lot quicker.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

@param aPoint A position for the top left corner of the bitmap. 
@param aBitmap A window server bitmap. 
@param aSourceRect A rectangle defining the piece of the bitmap to be drawn, 
with co-ordinates relative to the top left corner of the bitmap. 
@param aMaskBitmap A window server mask bitmap. 
@param aInvertMask If false, a source pixel that is masked by a black pixel 
is not transferred to the destination rectangle. If true, then a source pixel 
that is masked by a white pixel is not transferred to the destination rectangle. 

@see CBitmapContext::BitBltMasked() */
	{
	if (aBitmap == NULL || !aBitmap->Handle() || aMaskBitmap == NULL || !aMaskBitmap->Handle())
		return;
	TWsGcCmdBltMasked gdiBlitMasked(aPoint,aBitmap->WsHandle(),aSourceRect,aMaskBitmap->WsHandle(),aInvertMask);
	Write(&gdiBlitMasked,sizeof(gdiBlitMasked),EWsGcOpGdiWsBltMasked);
	AddToBitmapArray(aBitmap->Handle());
	AddToBitmapArray(aMaskBitmap->Handle()); 
	}

EXPORT_C void CWindowGc::MapColors(const TRect &aRect,const TRgb *aColors,TInt aNumPairs,TBool aMapForwards)
/** Re-maps pixel colours within a rectangle.

aColors is an array of paired colour values. The rectangle is scanned and 
the colour value of each pixel is looked up in the array, and if found is 
replaced by the value with which it is paired. The arguments specify the rectangle, 
the array of pairs, the number of pairs in the array, and the 'direction' 
of lookup. If no entry is found for the colour value of a given pixel it remains 
unchanged.

This function was designed for cursor display. A cursor is normally displayed 
as the XOR of the background against which it appears. However in some graphics 
modes, certain colours XOR to very similar shades. MapColors() allows more 
pleasing maps to be specified.

The effect of this function when applied to transparent pixels when the opaque
flag is set is undefined. See SetOpaque().
@param aRect Rectangle to be mapped. 
@param aColors Array of colour value pairs, one value in a pair is the lookup 
value and the other is the replacement value. 
@param aNumPairs Number of pairs in the array. aColors must have twice this 
number of elements. By default the aColors array is assumed to specify 2 colours 
(i.e. it specifies a single mapping).
@param aMapForwards Direction of lookup. ETrue to map forwards, i.e. treat 
the first value in the pair as the lookup value and the second as the replacement 
value; EFalse does the opposite. */
	{
	if (aNumPairs>16)
		Panic(EW32PanicMapColorsTooManyPairs);
	TWsGcCmdMapColors mapColors(aRect,aNumPairs,aMapForwards);
	Write(&mapColors,sizeof(mapColors),aColors,aNumPairs*sizeof(TRgb)*2,EWsGcOpMapColors);
	}

EXPORT_C void CWindowGc::Clear(const TRect &aRect)
/** Clears a rectangular area of a window.

The cleared area is filled with the current brush colour.

@param aRect The rectangle to clear. 
@see CBitmapContext::Clear() */
	{
	WriteRect(aRect,EWsGcOpClearRect);
	}

EXPORT_C void CWindowGc::Clear()
/** Clears the whole window.

The cleared area is filled with the current brush colour.

@see CBitmapContext::Clear() */
	{
	Write(EWsGcOpClear);
	}

EXPORT_C void CWindowGc::Reset()
/** Resets the graphics context to its default settings.

The drawing mode is set to TDrawMode::EDrawModePen (pen and brush colours used as 
they are); there is no clipping rectangle; the pen settings are black, 
solid, single pixel size; the brush style is null; no text font is selected.

@see CGraphicsContext::Reset() */
	{
	Write(EWsGcOpReset);
	iPimpl->iFont=NULL;
	}

EXPORT_C void CWindowGc::SetDitherOrigin(const TPoint& aPoint)
/** Sets the origin used for colour dithering.

@param aPoint New dither origin. */
	{
	Write(&aPoint,sizeof(aPoint),EWsGcOpSetDitherOrigin);
	}

EXPORT_C void CWindowGc::SetFaded(TBool aFaded)
/** Sets whether the graphics context is faded.

Fading is used to make a window appear less colourful so that other windows 
stand out. For example, a window would be faded when a dialogue is displayed 
in front of it.

@param aFaded ETrue to fade the graphics context, EFalse to unfade it. */
	{
	WriteInt(aFaded,EWsGcOpSetFaded);
	}

EXPORT_C void CWindowGc::SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)
/** Sets the fading parameters.

This function allows you to override the map used when drawing with a faded 
graphics context. However if you draw to a faded window with a faded graphics 
context, then fading on the graphics context is ignored and it will use the 
fading of the window.

Fading is used to make a window appear less colourful so that other windows stand 
out. For example, a window would be faded when a dialogue is displayed 
in front of it. 

You can either make a faded window closer to white or closer to black. 
The fading map allows you to over-ride the default fading parameters set in 
RWsSession::SetDefaultFadingParameters(). 

Fading re-maps colours to fall between the specified black and white map values. 
If aBlackMap=0 and aWhiteMap=255 then the colours are mapped unchanged. As the 
values converge, the colours are mapped to a smaller range, so the differences 
between colours in the faded graphics context decrease. If the values are reversed 
then the colours are inverted (i.e. where the gc would be black, it is now white). 

@param aBlackMap Black map fading parameter. Unfaded this is 0. 
@param aWhiteMap White map fading parameter. Unfaded this is 255. 
@see RWsSession::SetDefaultFadingParameters()
@see RWindowTreeNode::SetFaded() */
	{
	WriteInt(WservEncoding::Encode8BitValues(aBlackMap,aWhiteMap),EWsGcOpSetFadeParams);
	}

EXPORT_C TInt CWindowGc::AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect,const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt)
/**
Performs an alpha blending of the source data, aSrcBmp, with the window, using
the data from aAlphaBmp as an alpha blending factor.
The formula used is:
(S * A + W * (255 - A)) / 255, where:
- S - a pixel from aSrcBmp;
- W - a pixel from the window;
- A - a pixel from aAlphaBmp;
The contents of source and alpha bitmap are preserved.
The calculated alpha blended pixels are written to the destination - the window image.
The effect of this function when applied to transparent pixels when the opaque
flag is set is undefined. See SetOpaque().

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap.  However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

This method is supported from version 8.1
@param aDestPt Position in the target the result should be drawn to.
@param aSrcBmp A pointer to the source bitmap.
@param aSrcRect The part of the source bitmap that should be used.
@param aAlphaBmp A pointer to the bitmap used as an alpha blending factor.
@param aAlphaPt Position of the first pixel in the alpha bitmap that should be used as a source 
                for the alpha blending. The size of the area is the same as the 
                source bitmap area - aSrcRect parameter.
@see CFbsBitGc::AlphaBlendBitmaps()
*/
	{
	TWsGcCmdAlphaBlendBitmaps alphaBlend(aDestPt, aSrcBmp->Handle(), aSrcRect, aAlphaBmp->Handle(), aAlphaPt);
	Write(&alphaBlend,sizeof(alphaBlend),EWsGcOpGdiAlphaBlendBitmaps);
	AddToBitmapArray(aSrcBmp->Handle());
	AddToBitmapArray(aAlphaBmp->Handle());
	return KErrNone;
	}

EXPORT_C TInt CWindowGc::AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp, const TRect& aSrcRect,const CWsBitmap* aAlphaBmp, const TPoint& aAlphaPt)
/**
The method performs an alpha blending of the source data, aSrcBmp, with the window, using
the data from aAlphaBmp as an alpha blending factor.
For information on how this function works, see the other overload.

Windows that store their redraw commands will only store drawing position and a handle to bitmaps 
that are drawn in it. The bitmap handle is just a pointer to the bitmap in the FBSERV heap. 
At some point later WSERV may need to draw that window again and it will just replay the 
stored commands including the draw bitmap. However, if the client has changed the content of the bitmap,
WSERV will effectively draw a different bitmap when it replays the commands.

This method is supported from version 8.1
@param aDestPt Position in the target the result should be drawn to.
@param aSrcBmp A pointer to the source bitmap.
@param aSrcRect The part of the source bitmap that should be used.
@param aAlphaBmp A pointer to the bitmap used as an alpha blending factor.
@param aAlphaPt Position of the first pixel in the alpha bitmap that should be used as a source 
                for the alpha blending. The size of the area is the same as the 
                source bitmap area - aSrcRect parameter.
@see CFbsBitGc::AlphaBlendBitmaps()
*/
	{
	TWsGcCmdAlphaBlendBitmaps alphaBlend(aDestPt, aSrcBmp->WsHandle(), aSrcRect, aAlphaBmp->WsHandle(), aAlphaPt);
	Write(&alphaBlend,sizeof(alphaBlend),EWsGcOpGdiWsAlphaBlendBitmaps);
	AddToBitmapArray(aSrcBmp->Handle());
	AddToBitmapArray(aAlphaBmp->Handle());
	return KErrNone;
	}

EXPORT_C void CWindowGc::SetOpaque(TBool aDrawOpaque)
/**
Sets and unsets an opaque flag on the window.  When drawing to a transparent window with the
opaque flag set, drawing commands will not appear transparent.  When drawing to a non-transparent
window this command has no effect.
The result of non-opaque drawing performed over the top of an area of the window to which opaque
drawing has already been done is undefined.
The result of opaque alpha blending or color mapping over the top of an area of the window to
which opaque drawing has not already been done is undefined.
This method is supported from version 8.1
@param aDrawOpaque If true, opaque mode is switched on. If false, normal drawing will be used.
*/
	{
	WriteInt(aDrawOpaque, EWsGcOpSetOpaque);
	}

/** APIExtension can contain as many additional methods as is required by 
CGraphicsContext after its original conception. It takes 3 parameters.
Function is exported due to constrains of retaining BC with earlier versions.
This is not used directly by external methods, instead it is called by a named 
method in CGraphicsContext which passes the relivant arguements including an 
unique identifier for the required action.
@param aUid The unique identifier for the method that is required. Selected 
internally by a series of "if" statements. 
@see Valid Uid identifiers are listed in header gdi.h
@see CGraphicsContext
@param aOutput is a TAny pointer to a reference. Used to output data as the structure
does not need to be instantiated before the function call this adds greater 
flexibility.
@param aInput is a TAny pointer used to input data.
*/	
EXPORT_C TInt CWindowGc::APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput)
	{
	if (aUid == KGetUnderlineMetrics)
		{		
		APIExGetUnderlineMetrics(aOutput);
		return KErrNone;	
		}
	else if (aUid == KSetShadowColor)
		{
		return APIExSetShadowColor(aInput);
		}
	/* Future cases may be placed here later.*/
	else
		return CBitmapContext::APIExtension(aUid, aOutput, aInput);
	}

//The methods listed above in APIExtension follow here with the prefix APIEx.
TInt CWindowGc::APIExGetUnderlineMetrics(TAny*& aOutput)
	{
	const TInt width = Max(iPimpl->iFont->HeightInPixels() / 10,1);
	TTwoTInt* ptr = (TTwoTInt*)aOutput;
	ptr->iTop = 1 + width / 2;
	ptr->iBottom = (ptr->iTop) + width;
	return KErrNone;
	}

TInt CWindowGc::APIExSetShadowColor(TAny* aShadowColor)
	{
	const TRgb shadowColor = *(reinterpret_cast<TRgb*> (aShadowColor));
	WriteInt(shadowColor.Internal(), EWsGcOpSetShadowColor);
	return KErrNone;
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CGraphicsContext_2()
	{
	CBitmapContext::Reserved_CGraphicsContext_2();
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CBitmapContext_1()
	{
	CBitmapContext::Reserved_CBitmapContext_1();
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CBitmapContext_2()
	{
	CBitmapContext::Reserved_CBitmapContext_2();
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CBitmapContext_3()
	{
	CBitmapContext::Reserved_CBitmapContext_3();
	}
	
// was Reserved_CWindowGc_1
EXPORT_C void CWindowGc::DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect)
/** Draws an abstract artwork.
It does nothing if aDestRect values fall outside the window area.

@param aId the identifier for the artwork
@param aDestRect the destination rect within the active window for this artwork

@since 9.2
@released
*/
	{
	TWsGcCmdDrawWsGraphic drawWsGraphic(aId,aDestRect);
	Write(&drawWsGraphic,sizeof(drawWsGraphic),EWsGcOpDrawWsGraphic);
	}

// Reserved_CWindowGc_2
EXPORT_C void CWindowGc::DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect,const TDesC8& aData)
/** Draws an abstract artwork.
It does nothing if aDestRect values fall outside the window area.

@param aId the identifier for the artwork
@param aDestRect the destination rect within the active window for this artwork
@param aData opaque datagram to associate with this occasion of drawing.  The format is dependent upon the artwork

@since 9.2
@released
*/
	{
	TWsGcCmdDrawWsGraphic drawWsGraphic(aId,aDestRect);
	drawWsGraphic.iDataLen = aData.Size();
	WriteTextCommand(&drawWsGraphic, sizeof(drawWsGraphic),	aData, EWsGcOpDrawWsGraphic, EWsGcOpDrawWsGraphicPtr);	
	}

/** @internalComponent */
EXPORT_C TAny* CWindowGc::Interface(TUid /*aInterfaceId*/)
	{
	return NULL; //currently no interface supported
	}

/** @internalComponent */
EXPORT_C const TAny* CWindowGc::Interface(TUid /*aInterfaceId*/) const
	{
	return NULL; //currently no interface supported
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CWindowGc_3()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CWindowGc_4()
	{
	}

//Default implementation of reserved virtual
EXPORT_C void CWindowGc::Reserved_CWindowGc_5()
	{
	}

//The class is designed to be used for resource drawing; however this functionality is only supported in NGA architecture. 
//We have to provide an empty implementation here to avoid compilation errors.
EXPORT_C RWsDrawableSource::RWsDrawableSource()
	{
	}

EXPORT_C RWsDrawableSource::RWsDrawableSource(RWsSession&)
	{
	}

EXPORT_C TInt RWsDrawableSource::Create(const RSgDrawable&)
	{
	return KErrNotSupported;
	}

EXPORT_C TInt RWsDrawableSource::Create(const RSgDrawable&, TInt)
	{
	return KErrNotSupported;
	}

EXPORT_C const TSgDrawableId& RWsDrawableSource::DrawableId() const
	{
	return KSgNullDrawableId;
	}

EXPORT_C TInt RWsDrawableSource::ScreenNumber() const
	{
	return KSgScreenIdMain;
	}

EXPORT_C void RWsDrawableSource::Close()
	{
	}

