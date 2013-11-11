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
#include <e32svr.h>
#include <centralrepository.h>
#include "HWRMVibraImpl.h"
#include "HWRMVibraClientServer.h"
#include "HWRMtrace.h"
#include "HWRMVibraSDKCRKeys.h"
#include "HWRMVibraStatusObserver.h"
#include "HWRMPrivateCRKeys.h"
#include "hwrmuipluginhandler.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::CHWRMVibraImpl
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMVibraImpl::CHWRMVibraImpl()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::CHWRMVibraImpl()"));
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::CHWRMVibraImpl - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::ConstructL(MHWRMVibraObserver* aCallback)
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::ConstructL()" ));
    
    // NULL is acceptable value for aCallback, just means that 
    // client is not interested in callbacks.
    iCallback = aCallback;    
    iFeedbackCallback = NULL;

    // To get access to cenrep
    iProfileObserver = CHWRMVibraProfileObserver::NewL(this);

    // Get privileged SID and compare to this process SID
    CRepository* hwrmRep = CRepository::NewLC(KCRUidHWRMSettings);
    TInt sidInt(0);
    User::LeaveIfError(hwrmRep->Get(KHWRMAudioVibraSID, sidInt));
    TSecureId sid(sidInt);
    RProcess thisProcess;
    COMPONENT_TRACE3(_L( "HWRM VibraClient - CHWRMVibraImpl::ConstructL - Client SID: 0x%x, Privileged SID: 0x%x" ), thisProcess.SecureId().iId, sidInt );    
    if ( thisProcess.SecureId() == sid )
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::ConstructL - Client is privileged" ) );    
        iPrivilegedClient = ETrue;
        }
    else
        {
        COMPONENT_TRACE1( _L( "HWRM VibraClient - CHWRMVibraImpl::ConstructL - Client is not privileged" ) );    
        iPrivilegedClient = EFalse;
        }
    CleanupStack::PopAndDestroy(hwrmRep);
    thisProcess.Close();
    
    // Connect the session and order notifications
    OpenL();
    
    TInt err(KErrNone);    
   	TRAP(err, iUiPluginHandler = CHwrmUiPluginHandler::NewL( ));
   	if ( err != KErrNone )
    	{    	
    	COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Open() - No Cone Plugin Handler" ), err );
    	iUiPluginHandler = NULL;
    	}
    if(iUiPluginHandler)
    	{
    	iUiPluginHandler->AddForegroundObserverL(this);    	
    	}
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::ConstructL - return" ) );    
    }


// -----------------------------------------------------------------------------
// CHWRMVibra::SetFeedbackObserver
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::SetFeedbackObserver(MHWRMVibraFeedbackObserver* aCallback)
    {
    iFeedbackCallback = aCallback;
    iFeedbackCallback->VibraFeedbackModeChanged(iVibraFeedbackSetting);
    }
    

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMVibraImpl::~CHWRMVibraImpl()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::~CHWRMVibraImpl()" ));
    

    // close HWRM client, also deletes status observer
    Close();

    delete iProfileObserver;    
        
    // iCallback is not owned so it is not cleaned
    iCallback = NULL;
    
    // iStatusObserver is deleted in close, NULL it here so pc-lint doesn't complain
    iStatusObserver = NULL;
    
    delete iUiPluginHandler;

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::~CHWRMVibraImpl - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMVibraImpl::OpenL
// Opens session and orders notifications
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::OpenL()
    {
    COMPONENT_TRACE1( _L( "HWRM VibraClient - CHWRMVibraImpl::Open()" ));
    
    User::LeaveIfError(iClient.Connect(HWRMResourceTypeVibra));
    
    // Order status notifications
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::Open - Ordering notifications" ));
    iStatusObserver = CHWRMVibraStatusObserver::NewL(iCallback);            
    	
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::Open - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::Close
// Closes session.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::Close()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::Close()" ));
        
    // Inform session that we are cleaning up now.
    // Ignore errors, just trace them
    if (iClient.Handle())
        {
        iClient.ExecuteOperation(EHWRMCleanupVibra, TIpcArgs());
        }
    
    // Abort status notifications
    if ( iStatusObserver )
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::Close - canceling status notifications" ));
        iStatusObserver->Cancel();
        delete iStatusObserver;
        iStatusObserver = NULL;
        }
                
    iClient.Close();
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::Close - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraImpl::ReserveVibra
// Reserve vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::ReserveVibraL()
    {
    ReserveVibraL(EFalse, EFalse);
    }    

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::ReserveVibra
// Reserve vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::ReserveVibraL(TBool aRestoreState, TBool aForceNoCCoeEnv)
    {
    COMPONENT_TRACE3(_L( "HWRM VibraClient - CHWRMVibraImpl::ReserveVibraL(0x%x, 0x%x)" ), aRestoreState, aForceNoCCoeEnv);
    
    if ( !aForceNoCCoeEnv )
        {                      
 		if(iUiPluginHandler)
    		{
    		// Check that we are on foreground (i.e. we have keyboard focus.)
            // Note: There can be windows on top of our application that have explicitly disabled keyboard focus,
            // but then again the foreground/background observer will not kick in in those cases either.
            // -> We can use focus to determine foreground status here.            
    		if(iUiPluginHandler->InForeground() == EFalse)
				{
    			COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::ReserveVibra - Application does not have focus, Reserve denied." ) );
                User::Leave(KErrNotReady);                
    			}
    		} 		        	
        else
        	{
            COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::::ReserveLightL - No iUiPluginHandler available" ) );
            User::Leave(KErrBadHandle);        	
        	}       
        }
    
    // Do the actual reservation
    DoReserveVibraL(aRestoreState, aForceNoCCoeEnv);
    
    if ( !aForceNoCCoeEnv )
        {
        // set autoreserve flag
        iAutoReserving = ETrue;
        }
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::ReserveVibraL - return" ) );
    }
    

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::ReleaseVibra
// Release vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::ReleaseVibra()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::ReleaseVibra()" ) );
    
    DoReleaseVibra();

    iAutoReserving = EFalse;
    
    COMPONENT_TRACE1( _L( "HWRM VibraClient - CHWRMVibraImpl::ReleaseVibra - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMVibraImpl::DoReserveVibraL
// Reserve vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::DoReserveVibraL(TBool aRestoreState, TBool aForceNoCCoeEnv)
    {
    COMPONENT_TRACE3( _L( "HWRM VibraClient - CHWRMVibraImpl::DoReserveVibra(0x%x, 0x%x)" ), aRestoreState, aForceNoCCoeEnv);
    
    // Check if vibra is already reserved by this session. 
    // In that case do nothing.
    if ( iReserved )
        {
        return;
        }

    // Execute reservation
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMReserveVibra, TIpcArgs(aRestoreState, aForceNoCCoeEnv)));

    // Reserve successful, mark as reserved.
    iReserved = ETrue;
       
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::DoReserveVibra - return" ) );
    }
    

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::DoReleaseVibra
// Release vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::DoReleaseVibra()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::DoReleaseVibra()" ) );
    
    // Only release if reserved.
    if ( iReserved )
        {
        // Execute release
        TInt err = iClient.ExecuteOperation(EHWRMReleaseVibra, TIpcArgs());
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::DoReleaseVibra - Release execution failed: %d" ), err);
            }
        // If an error is raised on the server side before or after
        // vibra is released, attention must be paid to what the "iReserved"
        // flag should be set to
        iReserved = EFalse;
        }
        
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::DoReleaseVibra - return" ) );
    }

//------------------------------------------------------------------------------
// CHWRMVibraImpl::HandleGainingForeground     
// Inherited From MCoeForegroundObserver - called when app comes to foreground
//------------------------------------------------------------------------------
//
void CHWRMVibraImpl::HandleGainingForeground()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::HandleGainingForeground()" ));

    // Reserve if vibra is autoreserving
    if ( iAutoReserving )
        {
        TRAPD(err, DoReserveVibraL(ETrue, EFalse));
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::HandleGainingForeground - Error reserving: %d " ), err );
            }
        }

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::HandleGainingForeground - return " ) );
    }


//------------------------------------------------------------------------------
// CHWRMVibraImpl::HandleLosingForeground     
// Inherited From MCoeForegroundObserver - called when app goes to background 
//------------------------------------------------------------------------------
void CHWRMVibraImpl::HandleLosingForeground()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::HandleLosingForeground()" ) );

    if ( iAutoReserving )
        {
        DoReleaseVibra();
        }

    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::HandleLosingForeground - return " ) );
    }


// -----------------------------------------------------------------------------
// CHWRMVibraImpl::StartVibraL
// Start vibra with duration.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::StartVibraL(TInt aDuration)
    {
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL(0x%x)" ), aDuration);

    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - Invalid HWRM client." ) );    
        User::Leave(KErrBadHandle);
        }

    if ( (aDuration < 0) || (aDuration > KHWRMVibraMaxDuration) )
        {
        User::Leave(KErrArgument);
        }
	
    // make sure that vibra settings gets updated.
    VibraSettings();

    // nothing happens if vibra is not on from settings.        
    if ( iVibraSetting == EVibraModeON || iPrivilegedClient )
        {
        // Call start vibra from server with default intensity    
        User::LeaveIfError(iClient.ExecuteOperation(EHWRMStartVibraDefaultIntensity, TIpcArgs(aDuration)));
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - Vibra not available." ) );    
        User::Leave(KErrAccessDenied);
        }
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::StartVibraL
// Start vibra with duration and intensity.
// -----------------------------------------------------------------------------
//

void CHWRMVibraImpl::StartVibraL(TInt aDuration, TInt aIntensity)
    {
    COMPONENT_TRACE3(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL(0x%x, 0x%x)" ), aDuration, aIntensity);
        
    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - Invalid HWRM client." ) );    
        User::Leave(KErrBadHandle);
        }

    if ( aDuration < 0 || aDuration > KHWRMVibraMaxDuration )
        {
        User::Leave(KErrArgument);
        }

    // check that intensity is in valid range
    if(aIntensity < KHWRMVibraMinIntensity || aIntensity > KHWRMVibraMaxIntensity)
        {
        COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - Invalid intensity argument: %d" ), aIntensity );    
        User::Leave(KErrArgument);
        }

    // make sure that vibra settings gets updated.
    VibraSettings();
	
    // nothing happens if vibra is not on from settings.        
    if ( iVibraSetting == EVibraModeON || iPrivilegedClient )
        {
        // Call start vibra from server with specified intensity    
        User::LeaveIfError(iClient.ExecuteOperation(EHWRMStartVibra, TIpcArgs(aDuration, aIntensity)));
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - Vibra not available." ) );    
        User::Leave(KErrAccessDenied);
        }
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::StopVibraL
// Stop vibra.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::StopVibraL()
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StopVibraL()" ));
        
    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StartVibraL - Invalid HWRM client." ) );    
        User::Leave(KErrBadHandle);
        }

    // always try to stop vibra just in case, even if it is not available 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMStopVibra, TIpcArgs()));
    
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::StopVibraL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::VibraSettings
// Return vibra mode profile setting state
// -----------------------------------------------------------------------------
//
CHWRMVibra::TVibraModeState CHWRMVibraImpl::VibraSettings() const
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::VibraSettings()" ));

    // get the current state
	iVibraSetting = iProfileObserver->GetVibraModeState();
               
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::VibraSettings - return 0x%x" ), iVibraSetting );
    
    return iVibraSetting;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::VibraStatus
// Return vibra status
// -----------------------------------------------------------------------------
//
CHWRMVibra::TVibraStatus CHWRMVibraImpl::VibraStatus() const
    {
    COMPONENT_TRACE1(_L( "HWRM VibraClient - CHWRMVibraImpl::VibraStatus()" ));
    
    CHWRMVibra::TVibraStatus status = CHWRMVibra::EVibraStatusUnknown;
    
    if ( iStatusObserver )
        {    
        status = iStatusObserver->CurrentStatus();
        }
               
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::VibraStatus - return 0x%x" ), status );
    
    return status;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::VibraModeChange
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::VibraModeChange(CHWRMVibra::TVibraModeState aNewState)
    {
    COMPONENT_TRACE2(_L("HWRM VibraClient - CHWRMVibra::VibraModeChange(0x%x)"), aNewState); 

    iVibraSetting = aNewState;

    if ( iCallback )
        {
        iCallback->VibraModeChanged(iVibraSetting);
        }

    COMPONENT_TRACE1(_L("HWRM VibraClient - CHWRMVibra::VibraModeChange - return") ); 
    }
// -----------------------------------------------------------------------------
// CHWRMVibraImpl::PulseVibraL
// Start vibra with default intensity and duration.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::PulseVibraL()
    {
    COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL" )) );

    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Invalid HWRM client." ) ) );    
        User::Leave(KErrBadHandle);
        }

	// make sure vibra feedback state gets updated
	VibraFeedbackSettings();
	
    // nothing happens if vibra is not on from settings.        
    if ( iVibraFeedbackSetting == EVibraFeedbackModeON )
        {
        // Call start vibra from server with default intensity    
        User::LeaveIfError(iClient.ExecuteOperation(EHWRMPulseVibraDefault, TIpcArgs()));
        }
    else
        {
        COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Feedback vibration not available." ) ) );    
        User::Leave(KErrAccessDenied);
        }
    
    COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::PulseVibraL
// Start vibra with specified duration and default intensity.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::PulseVibraL(TInt aDuration)
    {
    COMPONENT_TRACE2(_L("HWRM VibraClient - CHWRMVibraImpl::PulseVibraL(0x%x)" ), aDuration);
    
    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Invalid HWRM client." ) ) );    
        User::Leave(KErrBadHandle);
        }

    if ( aDuration < 0 || aDuration > KHWRMVibraMaxDuration )
        {
        User::Leave(KErrArgument);
        }

	// make sure vibra feedback state gets updated
	VibraFeedbackSettings();
	
    // nothing happens if feedback vibra is not on from settings.        
    if ( iVibraFeedbackSetting == EVibraFeedbackModeON )
        {
        // Call start vibra from server with specified intensity    
        User::LeaveIfError(iClient.ExecuteOperation(EHWRMPulseVibraDefaultIntensity, TIpcArgs(aDuration)));
        }
    else
        {
        COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Feedback vibration not available." ) ) );    
        User::Leave(KErrAccessDenied);
        }
    
    COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::PulseVibraL
// Start vibra with specified duration and intensity.
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::PulseVibraL(TInt aDuration, TInt aIntensity)
    {
    COMPONENT_TRACE3(_L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL(0x%x, 0x%x)" ), aDuration, aIntensity);
        
    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Invalid HWRM client." ) ) );    
        User::Leave(KErrBadHandle);
        }

    if ( aDuration < 0 || aDuration > KHWRMVibraMaxDuration )
        {
        User::Leave(KErrArgument);
        }

    // check that intensity is in valid range
    if(aIntensity < KHWRMVibraMinPulseIntensity || aIntensity > KHWRMVibraMaxIntensity)
        {
        COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Invalid intensity argument: %d" ), aIntensity );    
        User::Leave(KErrArgument);
        }

	// make sure vibra feedback state gets updated
	VibraFeedbackSettings();
	
    // nothing happens if vibra is not on from settings.        
    if ( iVibraFeedbackSetting == EVibraFeedbackModeON )
        {
        // Call start vibra from server with specified intensity    
        User::LeaveIfError(iClient.ExecuteOperation(EHWRMPulseVibra, TIpcArgs(aDuration, aIntensity)));
        }
    else
        {
        COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - Feedback vibration not available." ) ) );    
        User::Leave(KErrAccessDenied);
        }
            
    
    COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::PulseVibraL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::VibraFeedbackSettings
// Return feedback vibration mode profile setting state
// -----------------------------------------------------------------------------
//
CHWRMVibra::TVibraFeedbackModeState CHWRMVibraImpl::VibraFeedbackSettings() const
    {
    COMPONENT_TRACE1( ( _L( "HWRM VibraClient - CHWRMVibraImpl::VibraFeedbackSettings()" )) );

    // get the current vibra feedback state
	iVibraFeedbackSetting = iProfileObserver->GetVibraFeedbackModeState();
               
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibraImpl::VibraFeedbackSettings - return 0x%x" ), iVibraFeedbackSetting );
    
    return iVibraFeedbackSetting;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraImpl::VibraFeedbackModeChange
// -----------------------------------------------------------------------------
//
void CHWRMVibraImpl::VibraFeedbackModeChange(CHWRMVibra::TVibraFeedbackModeState  aNewState)
    {
    COMPONENT_TRACE2(_L("HWRM VibraClient - CHWRMVibra::VibraFeedbackModeChange(0x%x)"), aNewState); 

    iVibraFeedbackSetting = aNewState;

    if ( iFeedbackCallback )
        {
        iFeedbackCallback->VibraFeedbackModeChanged(iVibraFeedbackSetting);
        }

    COMPONENT_TRACE1(_L("HWRM VibraClient - CHWRMVibra::VibraFeedbackModeChange - return") ); 
    }



// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
