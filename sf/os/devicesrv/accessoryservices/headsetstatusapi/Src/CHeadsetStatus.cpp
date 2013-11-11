/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Main functionality of CHeadsetStatus
*
*
*/


// INCLUDE FILES
#include "HeadsetStatus.h"
#include "CHeadsetStatusContainer.h"
#include <e32std.h>

// UNNANMED NAMESPACE FOR LOCAL DEFINITIONS
namespace
    {
    //  CONSTANTS
    _LIT( KPanicCat, "HeadsetStatus" );
    
    //  DATA TYPES
    enum TPanicCode
        {
        EInvalidObserver
        };
    }

// ========================= MEMBER FUNCTIONS ================================
   
// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
CHeadsetStatus::CHeadsetStatus( )
    {
    }

// ---------------------------------------------------------------------------
// CHeadsetStatus::NewL
// ---------------------------------------------------------------------------
EXPORT_C CHeadsetStatus* CHeadsetStatus::NewL( )
    {
    CHeadsetStatus* self = new ( ELeave ) CHeadsetStatus();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatus::ConstructL
// 
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
void CHeadsetStatus::ConstructL()
    {
    iContainer = CHeadsetStatusContainer::NewL();
    }
     
// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
EXPORT_C CHeadsetStatus::~CHeadsetStatus()
    {    
    delete iContainer;
    }

// ---------------------------------------------------------------------------
// CHeadsetStatus::HeadsetConnectedL
// 
// Method to query the headsets connectivity status. Returns error code 
// (or KErrNone when query succeeded) and sets the query result into the 
// boolean in which the parameter aState is referring.
// ---------------------------------------------------------------------------
EXPORT_C TInt CHeadsetStatus::HeadsetConnectedL( TBool& aState )
    {
    return iContainer->QueryHeadsetStatus( aState );
    }
    
// ---------------------------------------------------------------------------
// CHeadsetStatus::SetObserver
// ---------------------------------------------------------------------------
EXPORT_C void CHeadsetStatus::SetObserver( MHeadsetStatusObserver* aObserver )
    {
    __ASSERT_ALWAYS( aObserver, User::Panic( KPanicCat, EInvalidObserver ) );
    
    iContainer->SetObserver( aObserver );
    }

// ---------------------------------------------------------------------------
// CHeadsetStatus::StartObserving
// ---------------------------------------------------------------------------
EXPORT_C void CHeadsetStatus::StartObserving()
    {
    iContainer->StartObserving();
    }

// ---------------------------------------------------------------------------
// CHeadsetStatus::StopObserving
// ---------------------------------------------------------------------------
EXPORT_C void CHeadsetStatus::StopObserving()
    {
    iContainer->StopObserving();    
    }

// ---------------------------------------------------------------------------
// CHeadsetStatus::StopObserving
// ---------------------------------------------------------------------------
EXPORT_C TBool CHeadsetStatus::IsObserving() const
    {
    return iContainer->IsObserving();
    }

// End of File
