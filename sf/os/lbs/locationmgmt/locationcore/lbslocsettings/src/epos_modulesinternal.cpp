// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32base.h>
#include "epos_modulesinternal.h"

// ---------------------------------------------------------
// Panic
// ---------------------------------------------------------
GLDEF_C void Panic( TInt aReason )
    {
    User::Panic( KPosModulesPanicCategory, aReason );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
GLDEF_C TBool IsHexString(const TDesC& aDes)
    {
    for (TInt i = 0; i < aDes.Length(); i++)
        {
        TChar ch((aDes.Ptr())[i]);
        if (!ch.IsHexDigit())
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
GLDEF_C TInt ParseModuleUid( const TPtrC& aStr, TUint32& aId )
    {
    // UID is 8 Hex digits
    if ( ( aStr.Length() != KUidFieldLength ) ||
         ( !IsHexString( aStr ) ) )
        {
        return KErrCorrupt;
        }

    // parse the UID
    TLex token( aStr );
    TInt err = token.Val( aId, EHex );
    if ( err != KErrNone ) 
        {
        return KErrCorrupt;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
GLDEF_C void ReplaceCommasWithWhitespace( TPtr aString )
	{
	_LIT( KSpace, " " );
	TInt i = 0;
	while ( ETrue )
		{
		TInt j = aString.Mid( i ).Locate( KFieldDelimiter );
		if ( j != KErrNotFound )
			{
			aString.Replace( i + j, 1, KSpace );
			i += j;
			}
		else
			{
			return;
			}	
		}
	}
