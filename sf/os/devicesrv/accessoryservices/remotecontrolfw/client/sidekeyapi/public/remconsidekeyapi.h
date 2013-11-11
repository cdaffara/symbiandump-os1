// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONSIDEKEYAPI_H
#define REMCONSIDEKEYAPI_H

#include <e32base.h>

/** Offset in the operation-specific data of the 'result' (this field exists 
in both commands and responses, but logically only applies to responses). */
const TUint KRemConSideKeyApiResultDataOffset = 0;

/** Length of the results data/field in bytes. */
const TUint KRemConSideKeyApiResultDataLength = 4;

/** Used for checking operation-specific data. The operation-specific data of 
an Side Key API message must always contain at least a results field, so the length 
of the results field is the minimum length of this 'packet'. */
const TUint KRemConSideKeyApiMinimumDataLength = KRemConSideKeyApiResultDataLength;

/**
The UID identifying the Side Key outer-layer RemCon interface.
*/
const TInt KRemConSideKeyApiUid = 0x2002FF91;

/**
Operation ids belonging to the Side Key interface.
These are public so bearers/converters can access them.
*/
enum TRemConSideKeyOperationId
	{
	/** Side key up pressed. */
	ERemConSideKeyUp			= 0x00,
	/** Side key down pressed. */
	ERemConSideKeyDown			= 0x01,
	};

/** Length of the button action data in bytes. */
const TUint KRemConSideKeyApiButtonDataLength = 1;

/** Button actions. */
enum TRemConSideKeyApiButtonAction
	{
	/** A button has been pressed. */
	ERemConSideKeyApiButtonPress	= 0,
	/** A button has been released. */
	ERemConSideKeyApiButtonRelease 	= 1,
	/** A button has been clicked. */
	ERemConSideKeyApiButtonClick	= 2,
	};

#endif // REMCONSIDEKEYAPI_H
