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



//  INCLUDE FILES
#include "cmmpblist.h"
#include "cmmphonetsy.h"
#include "cmmphonebookstoretsy.h"

// ======== MEMBER FUNCTIONS ========

CMmPBList* CMmPBList::NewL(
    CMmPhoneTsy* aMmPhone )
    {
    CMmPBList* aPBList = NULL;
    
    if( NULL != aMmPhone )
        {
        aPBList = new (ELeave) CMmPBList();  
        CleanupStack::PushL( aPBList );
        aPBList->iMmPhone = aMmPhone;
        aPBList->ConstructL();
        CleanupStack::Pop();
        }

    return aPBList;
    }

CMmPBList::CMmPBList()
    {
    }

void CMmPBList::ConstructL()
    {
    //Create phonebook container
    CreateContainerL();
    }

CMmPBList::~CMmPBList()
    {
    //delete object container
    DeleteContainer();
    }

// ---------------------------------------------------------------------------
// CMmPBList::GetMmPBByIndex
// Returns mm PB by index. The indexing begins from 0
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneBookStoreTsy* CMmPBList::GetMmPBByIndex(
    TInt aIndex )
    {
    if( iObjectContainer )
        {
        if( aIndex >= 0 && aIndex < iObjectContainer->Count() )
            {
            return (CMmPhoneBookStoreTsy*) (iObjectContainer->At( aIndex ));
            }
        }

    return NULL;
    }

//  End of File 



