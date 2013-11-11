/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTCCenrepObserver class implementation.
*
*/


// INCLUDE FILES
#include <centralrepository.h>
#include <cenrepnotifyhandler.h>
#include "tccenrepobserver.h"
#include "tcprivatecrkeys.h"
#include "trace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTCCenrepObserver::CTCCenrepObserver
// C++ constructor
// -----------------------------------------------------------------------------
//
CTCCenrepObserver::CTCCenrepObserver( MTCCenrepObserver& aCallback ) :
    iCallback( aCallback )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CTCCenrepObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTCCenrepObserver* CTCCenrepObserver::NewL( MTCCenrepObserver& aCallback )
    {
    FUNC_LOG;

    CTCCenrepObserver* self = 
        new( ELeave ) CTCCenrepObserver( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
   
// -----------------------------------------------------------------------------
// CTCCenrepObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTCCenrepObserver::ConstructL()
    {    
    FUNC_LOG;

    iRepository = CRepository::NewL( KCRUidTiltCompensationParameters );
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, 
                                                 *iRepository );
    iNotifyHandler->StartListeningL();
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CTCCenrepObserver::~CTCCenrepObserver()
    {
    FUNC_LOG;

    if( iNotifyHandler )
        {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        }

    delete iRepository;    
    }

// -----------------------------------------------------------------------------
// CTCCenrepObserver::GetCenrepHandle
// -----------------------------------------------------------------------------
//
CRepository& CTCCenrepObserver::GetCenrepHandle()
    {
    return *iRepository;
    }

// -----------------------------------------------------------------------------
// CTCCenrepObserver::HandleNotifyGeneric
// -----------------------------------------------------------------------------
//
void CTCCenrepObserver::HandleNotifyGeneric( TUint32 aKey )
    {
    FUNC_LOG;

    iCallback.CenrepValueChanged( aKey, *iRepository  );
    }

// -----------------------------------------------------------------------------
// CTCCenrepObserver::HandleNotifyError
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void CTCCenrepObserver::HandleNotifyError(
    TUint32 /*aKey*/, TInt aError, CCenRepNotifyHandler* aHandler)
#else
void CTCCenrepObserver::HandleNotifyError(
    TUint32 /*aKey*/, TInt /*aError*/, CCenRepNotifyHandler* aHandler)
#endif
    {
    ERROR_1( aError, "CTCCenrepObserver::HandleNotifyError - aError: %d ", aError ); 
    
    // Retry notification 
    TRAPD( err, aHandler->StartListeningL() );
    if ( err != KErrNone )
        {
        ERROR_1( err, "CTCCenrepObserver::HandleNotifyError - err: %d ", err ); 
        }
    }

//  End of File  
