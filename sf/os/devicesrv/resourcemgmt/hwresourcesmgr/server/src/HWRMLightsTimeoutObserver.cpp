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
#include <centralrepository.h>
#include "cenrepobserver.h"
#include "HWRMtrace.h"
#include "HWRMLightDomainCRKeys.h"
#include "HWRMLightsTimeoutObserver.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMLightsTimeoutObserver::CHWRMLightsTimeoutObserver
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMLightsTimeoutObserver::CHWRMLightsTimeoutObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMLightsTimeoutObserver::CHWRMLightsTimeoutObserver()" ) );
    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMLightsTimeoutObserver::CHWRMLightsTimeoutObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightsTimeoutObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMLightsTimeoutObserver::ConstructL(MHWRMLightsTimeoutObserver*  aCallback)
    {    
    COMPONENT_TRACE2(_L( "HWRM Common - CHWRMLightsTimeoutObserver::ConstructL(0x%x)" ), aCallback );

    // To get access to cenrep
    iRepository = CRepository::NewL(KCRUidLightSettings);

    // set up notify handler
    iNotifyHandler = CCenRepListener::NewL(*this, 
                                                *iRepository, 
                                                KDisplayLightsTimeout);
    iNotifyHandler->StartListeningL();

    iCallback = aCallback;

    // Get initial value of vibra enabled
    TInt value(0);
    TInt err = iRepository->Get(KDisplayLightsTimeout, value);

    if ( err == KErrNone )
        {      
        iCallback->LightsTimeoutChange(value);
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Common - CHWRMLightsTimeoutObserver::ConstructL - Unable to read KDisplayLightsTimeout from Cenrep" ) );
        User::Leave(err);
        }

    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMLightsTimeoutObserver::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightsTimeoutObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMLightsTimeoutObserver* CHWRMLightsTimeoutObserver::NewL(MHWRMLightsTimeoutObserver*  aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM Common - CHWRMLightsTimeoutObserver::NewL(0x%x)" ), aCallback );

    // There must be a callback instance
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

    CHWRMLightsTimeoutObserver* self = new( ELeave ) CHWRMLightsTimeoutObserver();
    
    CleanupStack::PushL( self );
    self->ConstructL(aCallback);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Common - CHWRMLightsTimeoutObserver::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
EXPORT_C CHWRMLightsTimeoutObserver::~CHWRMLightsTimeoutObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMLightsTimeoutObserver::~CHWRMLightsTimeoutObserver()" ) );

    // Stop listening to cenrep
    if(iNotifyHandler)
        {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        }

    delete iRepository;
    
    iCallback = NULL; // PCLint demands    

    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMLightsTimeoutObserver::~CHWRMLightsTimeoutObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightsTimeoutObserver::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CHWRMLightsTimeoutObserver::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    COMPONENT_TRACE3(_L("HWRM Common - CHWRMLightsTimeoutObserver::HandleNotifyInt(0x%x, 0x%x)"), aId, aNewValue); 

    if (aId == KDisplayLightsTimeout)
        {
        if (iCallback)
            {
            // Inform user about change of the user profile vibra setting
            iCallback->LightsTimeoutChange(aNewValue);
            }
        }    

    COMPONENT_TRACE1(_L("HWRM Common - CHWRMLightsTimeoutObserver::HandleNotifyInt - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMLightsTimeoutObserver::HandleNotifyError
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
void CHWRMLightsTimeoutObserver::HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler)
#else
void CHWRMLightsTimeoutObserver::HandleNotifyError(TUint32 /*aId*/, TInt /*error*/, CCenRepListener* aHandler)
#endif
    {
    COMPONENT_TRACE4(_L("HWRM Common - CHWRMLightsTimeoutObserver::HandleNotifyError(0x%x, 0x%x, 0x%x)"), aId, error, aHandler); 
    
    // retry notification 
    if ( aHandler )
        {
        TRAPD(err,aHandler->StartListeningL());
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L("HWRM Common - CHWRMLightsTimeoutObserver::HandleNotifyError - error: %d "), err ); 
            }
        }

    COMPONENT_TRACE1(_L("HWRM Common - CHWRMLightsTimeoutObserver::HandleNotifyError - return") ); 
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
