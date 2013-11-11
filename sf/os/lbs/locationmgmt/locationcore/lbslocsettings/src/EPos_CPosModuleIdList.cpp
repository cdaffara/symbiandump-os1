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



#include <lbs/epos_cposmoduleidlist.h>
#include "epos_modulesinternal.h"

const TInt KPosGranularity = 8;


// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModuleIdList::CPosModuleIdList()
	: CArrayFixFlat<TUid>( KPosGranularity )
    {
    }


/**
 *  Two-phased constructor.
 */
EXPORT_C CPosModuleIdList* CPosModuleIdList::NewL()
    {
    CPosModuleIdList* self = new (ELeave) CPosModuleIdList;
    return self;
    }


/**
 * Destructor
 */
EXPORT_C CPosModuleIdList::~CPosModuleIdList()
    {
    }


/**
 * Finds the first occurence of a module id in the list
 *
 * @param aModuleId The module id to search for.
 * @return KErrNotFound if the module id is not in the list, otherwise
 *         the index of the id in the list.
 */
EXPORT_C TInt CPosModuleIdList::Find(
    TPositionModuleId aModuleId ) const
    {
    TInt index = 0;
    TInt offset = _FOFF( TPositionModuleId, iUid );
	TKeyArrayFix key( offset, ECmpTInt32 );

    TInt err = CArrayFixFlat<TPositionModuleId>::Find( aModuleId, key, index );

	if ( err )
		{
		return KErrNotFound;
		}

	return index;
    }

/**
 * Moves a module from one position in the list to another.
 * This method doesn't leave even though it is marked as leaving.
 * The method panics if the list positions are not values between 0
 * and Count() - 1.
 *
 * @param aOldPos The position of the module to move.
 * @param aNewPos The new position of the module.
 *
 * @panic LocationSettings EPosLocSetPanicParameterOutOfRange If parameters are out of range.
 */
EXPORT_C void CPosModuleIdList::MoveL( TInt aOldPos, TInt aNewPos )
    {
	__ASSERT_DEBUG( aOldPos >=0 && aOldPos < Count(), Panic( EPosLocSetPanicParameterOutOfRange ) );
	__ASSERT_DEBUG( aNewPos >=0 && aNewPos < Count(), Panic( EPosLocSetPanicParameterOutOfRange ) );

    TUid tmp = At( aOldPos );
    if ( aNewPos > aOldPos )
        {
        InsertL( aNewPos + 1, tmp );
        Delete( aOldPos );
        }
    else if ( aOldPos > aNewPos )
        {
        InsertL( aNewPos, tmp );
        Delete( aOldPos + 1 );
        }
    else
        {
        }
    }
