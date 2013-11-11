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



#include <lbscommon.h>
#include <centralrepository.h>

#include "epos_modulesinternal.h"
#include <lbs/epos_cposmoduleidlist.h>
#include "epos_cpospostinstalledlist.h"

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPostInstalledList::CPosPostInstalledList()
	{
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPostInstalledList* CPosPostInstalledList::NewL( const TDesC& aKey )
    {
    CPosPostInstalledList* self = new (ELeave) CPosPostInstalledList();
    CleanupStack::PushL( self );
    self->ConstructL( aKey );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPostInstalledList::ConstructL( const TDesC& aKey )
	{
    ParseIdListL( aKey );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPostInstalledList::Remove( TPositionModuleId aModuleId )
	{
	TInt index = Find( aModuleId );
	if ( index != KErrNotFound )
		{
		Delete( index );
		}
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HBufC* CPosPostInstalledList::FormatLC() const
	{
    HBufC* keyBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr key = keyBuf->Des();

    TBuf<KAttributedKeyMaxItemLength> item;

    for ( TInt i = 0; i < Count(); i++ )
        {
        item.Format( KIdListKeyItemFormat, (*this)[i] );
        if ( key.Length() + item.Length() + KDelimiterLength < key.MaxLength() ) 
            {
            if ( i > 0 )
                {
                key.Append( KFieldDelimiter ); // no comma before first item
                }
            key.Append( item );
            }
        else
            {
            User::Leave( KErrOverflow );
            }
        }
	return keyBuf;
	}

// ========================= INTERNAL FUNCTIONS ==============================

// ---------------------------------------------------------------------------
//  Fails only on OOM case
// ---------------------------------------------------------------------------
//
void CPosPostInstalledList::ParseIdListL( const TDesC& aString )
	{
    HBufC* string = aString.AllocLC();
    ReplaceCommasWithWhitespace( string->Des() );

    TLex key( *string );
    while ( !key.Eos() )
        {
        TPtrC idToken = key.NextToken();
        if ( idToken.Length() == 0 )
            {
            break;  // no more items found
            }

        TUint32 id = 0;
        if ( ParseModuleUid( idToken, id ) == KErrNone &&
	         Find( TPositionModuleId( TUid::Uid( id ) ) ) == KErrNotFound )
            {
	        // Add module list item to the list
	        AppendL( TUid::Uid( id ) );
            }
        }

	CleanupStack::PopAndDestroy( string );
	}
