// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HWRMtrace.h"
#include "HWRMVibraSDKCRKeys.h"
#include "HWRMVibra.h"
#include "HWRMVibraProfileObserver.h"

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
// CHWRMVibraProfileObserver::CHWRMVibraProfileObserver
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMVibraProfileObserver::CHWRMVibraProfileObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::CHWRMVibraProfileObserver()" ) );
    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::CHWRMVibraProfileObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraProfileObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMVibraProfileObserver::ConstructL(MHWRMVibraProfileObserver*  aCallback)
    {    
    COMPONENT_TRACE2(_L( "HWRM Common - CHWRMVibraProfileObserver::ConstructL(0x%x)" ), aCallback );

    // To get access to cenrep
    iRepository = CRepository::NewL(KCRUidVibraCtrl);

    // set up notify handler
    iNotifyHandler = CCenRepListener::NewL(*this, 
                                                *iRepository, 
                                                KVibraCtrlProfileVibraEnabled);
    iNotifyHandler->StartListeningL();
    
    
    iFeedbackNotifyHandler = CCenRepListener::NewL(*this, 
                                                *iRepository, 
                                                KVibraCtrlProfileFeedbackEnabled);
    iFeedbackNotifyHandler->StartListeningL();

    iCallback = aCallback;

    // Get initial value of vibra enabled
    TInt value(0);
    TInt err = iRepository->Get(KVibraCtrlProfileVibraEnabled, value);

    if ( err == KErrNone )
        {      
        iCallback->VibraModeChange((value == 0 ) ? CHWRMVibra::EVibraModeOFF : CHWRMVibra::EVibraModeON);
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::ConstructL - Unable to read KVibraCtrlProfileVibraEnabled from Cenrep" ) );
        User::Leave(err);
        }
	
	// Get initial value of feedback vibration setting
    err = iRepository->Get(KVibraCtrlProfileFeedbackEnabled, value);
    if ( err == KErrNone )
        {      
        iCallback->VibraFeedbackModeChange((value == 0 ) ? CHWRMVibra::EVibraFeedbackModeOFF : CHWRMVibra::EVibraFeedbackModeON);
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::ConstructL - Unable to read KVibraCtrlProfileFeedbackEnabled from Cenrep" ));
        User::Leave(err);
        }
        

    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraProfileObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibraProfileObserver* CHWRMVibraProfileObserver::NewL(MHWRMVibraProfileObserver*  aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM Common - CHWRMVibraProfileObserver::NewL(0x%x)" ), aCallback );

    // There must be a callback instance
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

    CHWRMVibraProfileObserver* self = new( ELeave ) CHWRMVibraProfileObserver();
    
    CleanupStack::PushL( self );
    self->ConstructL(aCallback);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Common - CHWRMVibraProfileObserver::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
EXPORT_C CHWRMVibraProfileObserver::~CHWRMVibraProfileObserver()
    {
    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::~CHWRMVibraProfileObserver()" ) );

    // Stop listening to cenrep
    if(iNotifyHandler)
        {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
        }

    if(iFeedbackNotifyHandler)
        {
        iFeedbackNotifyHandler->StopListening();
        delete iFeedbackNotifyHandler;
        }

    delete iRepository;    

    COMPONENT_TRACE1(_L( "HWRM Common - CHWRMVibraProfileObserver::~CHWRMVibraProfileObserver - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraProfileObserver::GetVibraModeState
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibra::TVibraModeState CHWRMVibraProfileObserver::GetVibraModeState()
    {
    CHWRMVibra::TVibraModeState mode( CHWRMVibra::EVibraModeUnknown );
    TInt value( CHWRMVibra::EVibraModeUnknown );
    TInt err = iRepository->Get(KVibraCtrlProfileVibraEnabled, value);

    if ( err == KErrNone )
        {      
        mode = (value == 0 ) ? CHWRMVibra::EVibraModeOFF : CHWRMVibra::EVibraModeON;
        }
    else
        {      
        COMPONENT_TRACE2( _L( "HWRM Common - CHWRMVibraProfileObserver::GetVibraModeState - Unable to read KVibraCtrlProfileVibraEnabled from Cenrep, err %d" ), err );
        }
    
    return mode;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraProfileObserver::GetVibraFeedbackModeState
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibra::TVibraFeedbackModeState CHWRMVibraProfileObserver::GetVibraFeedbackModeState()
    {
    CHWRMVibra::TVibraFeedbackModeState mode( CHWRMVibra::EVibraFeedbackModeUnknown );
    TInt value( CHWRMVibra::EVibraFeedbackModeUnknown );
    TInt err = iRepository->Get(KVibraCtrlProfileFeedbackEnabled, value);

    if ( err == KErrNone )
        {      
        mode = (value == 0 ) ? CHWRMVibra::EVibraFeedbackModeOFF : CHWRMVibra::EVibraFeedbackModeON;
        }
    else
        {      
        COMPONENT_TRACE2( _L( "HWRM Common - CHWRMVibraProfileObserver::GetVibraModeState - Unable to read KVibraCtrlProfileFeedbackEnabled from Cenrep, err %d" ), err);
        }
    
    return mode;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraProfileObserver::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CHWRMVibraProfileObserver::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    COMPONENT_TRACE3(_L("HWRM Common - CHWRMVibraProfileObserver::HandleNotifyInt(0x%x, 0x%x)"), aId, aNewValue); 

    if (aId == KVibraCtrlProfileVibraEnabled)
        {
        
        CHWRMVibra::TVibraModeState setting;
        
        if (aNewValue == 0) // EOff
            {
            setting = CHWRMVibra::EVibraModeOFF;
            }
         else
            {
            setting = CHWRMVibra::EVibraModeON;
            }
            
        if (iCallback)
            {
            // Inform user about change of the user profile vibra setting
            iCallback->VibraModeChange(setting);
            }
        }
    else if (aId == KVibraCtrlProfileFeedbackEnabled)
        {
        
        CHWRMVibra::TVibraFeedbackModeState setting;
        
        if (aNewValue == 0) // EOff
            {
            setting = CHWRMVibra::EVibraFeedbackModeOFF;
            }
         else
            {
            setting = CHWRMVibra::EVibraFeedbackModeON;
            }
            
        if (iCallback)
            {
            // Inform user about change of the user profile vibra setting
            iCallback->VibraFeedbackModeChange(setting);
            }
        }    
            
    COMPONENT_TRACE1(_L("HWRM Common - CHWRMVibraProfileObserver::HandleNotifyInt - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMVibraProfileObserver::HandleNotifyError
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
void CHWRMVibraProfileObserver::HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler)
#else
void CHWRMVibraProfileObserver::HandleNotifyError(TUint32 aId, TInt /*error*/, CCenRepListener* aHandler)
#endif
    {
    COMPONENT_TRACE4(_L("HWRM Common - CHWRMVibraProfileObserver::HandleNotifyError(0x%x, 0x%x, 0x%x)"), aId, error, aHandler); 
    
    // retry notification 
    TRAPD(err,aHandler->StartListeningL());
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L("HWRM Common - CHWRMVibraProfileObserver::HandleNotifyError - error: %d "), err ); 
        }

    if (iCallback)
        {
        // Inform user about change of the user profile vibra setting
        if (aId == KVibraCtrlProfileVibraEnabled)
            {
            iCallback->VibraModeChange(CHWRMVibra::EVibraModeUnknown);
            }
        else if (aId == KVibraCtrlProfileFeedbackEnabled)
            {
            iCallback->VibraFeedbackModeChange(CHWRMVibra::EVibraFeedbackModeUnknown);
            }
        }

        
    COMPONENT_TRACE1(_L("HWRM Common - CHWRMVibraProfileObserver::HandleNotifyError - return") ); 
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
