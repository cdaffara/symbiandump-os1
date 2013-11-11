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

#include "absvolutils.h"

TInt GetAbsMaxVol(const TDesC8& aData, TUint& aAbsVol, TUint& aMaxVol)
	{
	// Decompose aData to get the absolute volume. It should be in the format 
	// '0xAAAAAAAA0xBBBBBBBB' where AAAAAAAA is the absolute volume, and 
	// BBBBBBBB is the maximum volume. If it isn't in this format, return 
	// error.
	TInt err = KErrArgument;

	// 20 is the size we need for the following formatting.
	if ( aData.Length() >= 20 )
		{
		TLex8 lex(aData.Mid(2, 8));
		err = lex.Val(aAbsVol, EHex);
		if ( err == KErrNone )
			{
			TLex8 lex(aData.Mid(12, 8));
			err = lex.Val(aMaxVol, EHex);
			}
		}

	return err;
	}
