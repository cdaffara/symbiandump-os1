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
// text cursor header file
// 
//

#ifndef TEXTCURSOR_H
#define TEXTCURSOR_H

struct TTextCursor
/** Text cursor parameter information.

A text cursor is a rectangular area which can be set to flash. It is normally
only visible when the window group which owns it has the keyboard focus. This
structure is used to define the parameters of a text cursor, which can be
selected for use through the window group using RWindowGroup::SetTextCursor().

Custom (non-rectangular) text cursors are supported from v7.0s. They are added
to the window server using RWsSession::SetCustomTextCursor(), after which
they can be selected for use through the window group, in the same way as
standard rectangular cursors. They have a unique identifier, stored in the
iType member, which must be greater than ETypeLastBasic. Custom text cursors
are clipped to their iHeight and iWidth values. Their iColour member is not
used.

@publishedAll
@released */
    {
	// The different normal text cursors
	// A Text cursor can either be a predefined one
	// or a general sprite. Cursors between ETypeFirst
	// and ETypeLastBasic are predefined ones (even though
	// the ones above ETypeLast do not exist and are
	// for future use)
	/** Cursor type values.

	The values between ETypeFirst and ETypeLastBasic are reserved for predefined
	cursors. Custom text cursors must have a value greater than ETypeLastBasic.

	The values not listed below are for internal use only.
	
    @publishedAll */
	enum ENamedCursors
		{
		ETypeNone,
		/** Standard filled rectangular cursor. */
		ETypeRectangle,
		/** Standard hollow rectangular cursor. */
		ETypeHollowRectangle,
        /** The first predefined cursor type.

        Equivalent to ETypeRectangle. This can be used for iterating through the predefined
        cursor types.
        
        WARNING: Enum for internal use ONLY. Compatibility is not guaranteed in future releases. */
        ETypeFirst=ETypeRectangle,
        /** The last predefined cursor type.

        Equivalent to ETypeHollowRectangle. This can be used for iterating through
        the predefined cursor types.
        
        WARNING: Enum for internal use ONLY. Compatibility is not guaranteed in future releases. */        
        ETypeLast=ETypeHollowRectangle,
        /** Reserved for future use.
		
		/** All custom text cursors must have a type value greater than this. */
		ETypeLastBasic = 1000,	
		};
	/** Added for compatibility with previous releases. */
	typedef TInt EType;
	/** Cursor flash and clipping flags.

	The flags not listed below are for internal use only. */
	enum EFlags
		{
		/** The text cursor should not flash. */
		EFlagNoFlash		= 0x00000001,
		/** The bitmap of each sprite member is clipped left of its x-origin and right of
		its x-origin plus iWidth.

		Used for custom text cursors only. */
		EFlagClipHorizontal	= 0x00000002,
		/** The bitmap of each sprite member is clipped vertically.

		How it is clipped depends on the text cursor's vertical alignment, see the
		TCustomTextCursorAlignment enum in class RWsSession.

		Used for custom text cursors only. */
		EFlagClipVertical	= 0x00000004,	
		};
	/** The cursor type.

	For possible values, see the ENamedCursors enum. */
	TInt iType;
	/** The height of the text cursor.

	This value is also used for clipping custom text cursors, if the EFlagClipVertical
	flag is set. */
    TInt iHeight;
	/** The ascent of the text cursor (the distance between the origin and the top). */
    TInt iAscent;
	/** The width of the text cursor.

	Custom text cursors are clipped to this value, if the EFlagClipHorizontal
	flag is set. */
    TInt iWidth;
	/** Cursor flash and clipping flags.

	For possible values, see the EFlags enum. */
    TUint iFlags;
	/** The colour of the text cursor.

	If the cursor is flashing, this colour is XOR'd with the screen.

	This value is not used for custom text cursors. */
	TRgb iColor;
    };

#endif // TEXTCURSOR_H
