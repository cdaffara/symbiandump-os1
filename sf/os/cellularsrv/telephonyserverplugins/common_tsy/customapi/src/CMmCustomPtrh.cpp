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



// INCLUDE FILES
#include <ctsy/rmmcustomapi.h>
#include <etelext.h>

// ======== MEMBER FUNCTIONS ========

CMmCustomPtrHolder::CMmCustomPtrHolder()
    {
    }

void CMmCustomPtrHolder::ConstructL(
    const TInt aSizeOfPtrArray,
    const TInt aSizeOfPtrCArray )
    {
    TPtr8 ptr( NULL, 0 );
    TInt i;
    TInt ret( KErrNone );
    for ( i=0; i<aSizeOfPtrArray; i++ )
        {
        ret = iPtrArray.Append( ptr );
        // In case of an error, remember to close the array!
        if ( KErrNone != ret )
            {
            iPtrArray.Close();
            User::Leave( ret );
            }
        }
    TPtrC8 ptrC( NULL,0 );
    for ( i=0; i<aSizeOfPtrCArray; i++ )
        {
        ret = iPtrCArray.Append( ptrC );
        // In case of an error, remember to close both arrays!
        if( KErrNone != ret )
            {
            iPtrArray.Close();
            iPtrCArray.Close();
            User::Leave( ret );
            }
        }
    }

CMmCustomPtrHolder* CMmCustomPtrHolder::NewL(
    const TInt aSizeOfPtrArray,
    const TInt aSizeOfPtrCArray )
    {

    CMmCustomPtrHolder* p = new ( ELeave ) CMmCustomPtrHolder();
    CleanupStack::PushL( p );
    p->ConstructL( aSizeOfPtrArray,aSizeOfPtrCArray );
    CleanupStack::Pop();
    
    return p;
    }

CMmCustomPtrHolder::~CMmCustomPtrHolder()
    {
    iPtrArray.Close();
    iPtrCArray.Close();
    }

// ---------------------------------------------------------------------------
// CMmCustomPtrHolder::Ptr
// This method returns pointer from pointer array.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TPtr8& CMmCustomPtrHolder::Ptr(
    const TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex<iPtrArray.Count(), PanicClient(
        EEtelPanicIndexOutOfRange ) );
    
    return iPtrArray[aIndex];
    }

// ---------------------------------------------------------------------------
// CMmCustomPtrHolder::PtrC
// This method returns pointer from pointer array.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TPtrC8& CMmCustomPtrHolder::PtrC(
    const TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex<iPtrCArray.Count(), PanicClient(
        EEtelPanicIndexOutOfRange ) );
    
    return iPtrCArray[aIndex];
    }

//  End of File  
