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
// Interface for Render Stage Text Cursors
// 
//

/**
 @publishedPartner
 @prototype
*/

#ifndef WSCURSOR_H
#define WSCURSOR_H

#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>

/** Wserv access to render stage Text Cursor implementation.

This interface allows delegation of Text Cursor drawing to the Render
Stage.  It must not to be confused with Custom Text Cursor drawing;
@see RWsSession::SetCustomTextCursor

Historically, Text cursors have often been implemented using XOR
drawing, because it is a cheap way to deliver a high-contrast cursor
in certain system architectures where Operating System access to
Graphics Device Memory is possible.

XOR drawing can be problematic for some Render Stages.  This is because
the XOR operation requires a read back followed by a write.  This would
stall a graphics accelerator hardware implementation.

In a render stage setting, the Text Cursor may not be the top-most
window as a transition effect can slide a semi-transparent window over
the top of the Text Cursor window.  This means the cursor cannot be XOR
drawn last after all other windows; the read-back cost cannot be hidden
at the end of drawing each frame.

The Display Render Stage, responsible for interacting with hardware,
can offer XOR drawing in some cases, for example Software Bit GDI,
and not in others, for example OpenVG back-end hardware.

This interface allows a render stage to implement Text Cursors
appropriately given the Display Render Stage which is present.

This interface uses the supplied Text Cursor Color as a hint only.
TextCursor colors are better regarded as constrast level indicators
where KRgbWhite means a high contrast text cursor, and KRgbBlack
means an invisible text cursor.

Suggested strategies:
<ol>
<li>
XOR drawing is available.  Just XOR draw the cursor each time in
the supplied color.  This corresponds to legacy non-Render Stage
behaviour.
</li>
<li>
No XOR drawing is available.  Draw a black box (solid or hollow
as appropriate).  Ensure that the UI Toolkit which provides the
platform's default text entry control has a compatible theme so
that black is an appropriate high-contrast color.
</li>
</ol>

@publishedPartner
@prototype
*/
class MWsTextCursor : public MWsObjectProvider
	{
public:

	struct TTextCursorInfo
		{
		const TRect& iCursorRect;	/** Rectangular area of the cursor in <b>window</b> co-ordinates */
		const TRegion& iRegion; 	/** Clipping region */
		TInt iTextCursorType; 		/** Cursor type; either TTextCursor::ETypeRectangle or TTextCursor::ETypeHollowRectangle */
		MWsWindow* iWindow; 		/** Window owning the cursor */
		TRgb iTextCursorColor;		/** Text Cursor Color used as a hint */

		TTextCursorInfo(const TRect& aCursorRect, const TRegion& aRegion,
				TInt aTextCursorType, MWsWindow* aWindow, TRgb aTextCursorColor)
		: iCursorRect(aCursorRect),
		iRegion(aRegion),
		iTextCursorType(aTextCursorType),
		iWindow(aWindow),
		iTextCursorColor(aTextCursorColor)
			{
			}
		};
public:
	DECLARE_WS_TYPE_ID(KMWsTextCursor)

public:
	/** Draw the Text Cursor in its Flash ON state
	 * 
	 * Draw the text cursor with the supplied attributes.  Note, the Flash OFF
	 * state does not result in a call to this function; instead a window
	 * redraw is done for the area where the cursor would reside.
	 * 
	 * @pre     The supplied Text Cursor type is either TextCursor::ETypeRectangle or
	 * 			TTextCursor::ETypeHollowRectangle
	 * 
	 * @param	aTextCursorInfo  Data structure to describe the attributes of the Text
	 * 			Cursor to Draw
	 */
	virtual void DrawTextCursor(const TTextCursorInfo& aTextCursorInfo) = 0;
	};

#endif // WSCURSOR_H
