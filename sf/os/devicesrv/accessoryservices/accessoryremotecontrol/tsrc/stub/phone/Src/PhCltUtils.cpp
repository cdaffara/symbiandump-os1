/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Utils
*
*/


// INCLUDE FILES
#include    "PhCltUtils.h"


// CONSTANTS
_LIT( KPhCltValidChars, "+0123456789*#pwPW" );
//Prefix for Sip.
_LIT( KSipPrefix, "sip:" );
//Prefix for Tel.
_LIT( KTelPrefix, "tel:" );
//Prefix for Sos.
_LIT( KSosPrefix, "sos:" );
// @ char
_LIT( KAt, "@" );
// Prefix length
const TInt KPrefixLength = 4;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhCltUtils::IsValidChar
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool PhCltUtils::IsValidChar( TChar aChar )
    {
    TPtrC valid( KPhCltValidChars );

    return valid.Locate( aChar ) != KErrNotFound;
    }


// -----------------------------------------------------------------------------
// PhCltUtils::RemoveInvalidChars
//
// -----------------------------------------------------------------------------
//
EXPORT_C void PhCltUtils::RemoveInvalidChars( TDes& aString )
    {
    TInt index = aString.Length() - 1;

    for ( ; index >= 0; index-- )
        {
        if ( !IsValidChar( aString[ index ] ) )
            {
            aString.Delete( index, 1 ); // one character
            }
        }
    }


// -----------------------------------------------------------------------------
// PhCltUtils::RemoveURIPrefixdAndDomainChars
//
// -----------------------------------------------------------------------------
//
EXPORT_C void PhCltUtils::RemoveURIPrefixdAndDomainChars( TDes& aString )
    {
	  // Does number contains prefix. If yes then remove it

    if ( 0 == aString.FindC( KSipPrefix ) ||
         0 == aString.FindC( KTelPrefix ) ||
	       0 == aString.FindC( KSosPrefix ) )
        {
        aString.Delete( 0, KPrefixLength );
	      }
	  // Does Number contains domain part. If yes remove it at beging of @ character.
    TInt atIndex = aString.Find( KAt );
    TInt telNumberLength = aString.Length();
	  if ( 0 < atIndex && 
	       atIndex < telNumberLength )
        {
		    aString.Delete( atIndex, telNumberLength - atIndex );
		    }
    }
//  End of File  
