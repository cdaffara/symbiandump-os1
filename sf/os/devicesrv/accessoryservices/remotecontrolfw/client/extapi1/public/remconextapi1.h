// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef REMCONEXTAPI1_H
#define REMCONEXTAPI1_H

#include <e32base.h>

// 'ExtApi1' constants apply to both interfaces within the ExtApi1 DLL, i.e. 
// both the TrackInfo and AbsVol APIs.

/** Offset in the operation-specific data of the 'result' (this field exists 
in both commands and responses, but logically only applies to responses). */
const TUint KRemConExtApi1ResultDataOffset = 0;

/** Length of the results data/field in bytes. */
const TUint KRemConExtApi1ResultDataLength = 4;

/** Used for checking operation-specific data. The operation-specific data of 
an ExtApi1 message must always contain at least a results field, so the length 
of the results field is the minimum length of this 'packet'. */
const TUint KRemConExtApi1MinimumDataLength = KRemConExtApi1ResultDataLength;

/**
The UID identifying the Track Info outer-layer RemCon interface.
*/
const TInt KRemConTrackInfoApiUid = 0x10206863;

/**
Operation ids belonging to the Track Info interface.
These are public so bearers/converters can access them.
*/
enum TRemConTrackInfoOperationId
	{
	/** Get track name. */
	ERemConGetTrackName			= 0x00,
	/** Set track name. */
	ERemConSetTrackName			= 0x01,
	/** Get artist. */
	ERemConGetArtist 			= 0x02,
	/** Get track duration. */
	ERemConGetTrackDuration		= 0x03,
	};

/**
The UID identifying the Abs Vol outer-layer RemCon interface.
*/
const TInt KRemConAbsVolApiUid = 0x1020831D;

/**
Operation ids belonging to the Abs Vol interface.
These are public so bearers/converters can access them.
*/
enum TRemConAbsVolOperationId
	{
	/** Get absolute volume. */
	ERemConGetAbsoluteVolume 	= 0x00,
	/** Set absolute volume. */
	ERemConSetAbsoluteVolume 	= 0x01,
	};

#endif // REMCONEXTAPI1_H
