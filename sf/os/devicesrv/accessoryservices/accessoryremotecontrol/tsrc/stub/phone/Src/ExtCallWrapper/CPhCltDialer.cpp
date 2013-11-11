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
* Description:  Dialer class.
*
*/



// INCLUDE FILES
#include    "CPhCltDialer.h"
#include    "CPhCltDialerDlg.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPhCltDialer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltDialer* CPhCltDialer::NewL()
    {
    CPhCltDialer* self = 
        new ( ELeave ) CPhCltDialer;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPhCltDialer::~CPhCltDialer()
    {
    if ( iDestroyed )
        {
        *iDestroyed = ETrue;
        iDestroyed = NULL;
        }

    delete iDialDlg;
    }


// -----------------------------------------------------------------------------
// CPhCltDialer::DialData
//
// -----------------------------------------------------------------------------
//
EXPORT_C TPhCltExtPhoneDialData& 
    CPhCltDialer::DialData()
    {
    return iDialDlg->DialData();
    }


// -----------------------------------------------------------------------------
// CPhCltDialer::ExecuteLD
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltDialer::ExecuteLD()
    {
    TBool destroyed = EFalse;
    iDestroyed = &destroyed;

    CleanupStack::PushL( this );
    CleanupStack::PushL( TCleanupItem( DoClear, this ) );

    iDialDlg->ExecuteDialLD();

    if ( !destroyed )
        {
        CleanupStack::PopAndDestroy(); // DoClear
        CleanupStack::PopAndDestroy( this );
        }
    else
        {
        // Already deleted
        CleanupStack::Pop();
        CleanupStack::Pop( this );
        }
    }


// -----------------------------------------------------------------------------
// CPhCltDialer::CPhCltDialer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltDialer::CPhCltDialer()
    {
    }


// -----------------------------------------------------------------------------
// CPhCltDialer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCltDialer::ConstructL()
    {
    iDialDlg = new ( ELeave ) CPhCltDialerDlg;
    }


// -----------------------------------------------------------------------------
// CPhCltDialer::DoClear
//
// -----------------------------------------------------------------------------
//
void CPhCltDialer::DoClear( TAny* aAny )
    {
    CPhCltDialer* dialer =
        reinterpret_cast< CPhCltDialer* >( aAny );

    dialer->iDestroyed = NULL;
    dialer->iDialDlg = NULL;
    }


//  End of File  
