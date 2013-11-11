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
#include "HWRMtrace.h"
#include "HWRMVibraSDKCRKeys.h"
#include "HWRMVibraCommonData.h"
#include "HWRMClientServer.h"
#include "HWRMPrivatePSKeys.h"
#include "HWRMPrivateCRKeys.h"
#include "HWRMConfiguration.h"
#include "HWRMVibraService.h"
#include "HWRMDomainPSKeys.h"



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
// CHWRMVibraCommonData::CHWRMVibraCommonData
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMVibraCommonData::CHWRMVibraCommonData(CHWRMVibraCommonService& aCommonService)
    : iPulseSupported(EFalse),
    iLastPublishedStatus(CHWRMVibra::EVibraStatusNotAllowed),
    iCommonService(aCommonService)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::CHWRMVibraCommonData()" ) );
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::CHWRMVibraCommonData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonData::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                      CHWRMReservationHandler* aReservationHandler)
    {    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL()" ) );

    // Get read only values from cenrep
    CRepository* repository = CRepository::NewLC(KCRUidVibraCtrl);

    TInt value(0);
    User::LeaveIfError(repository->Get(KVibraMinOffTime, value));
    iMinimumOffTime = value * 1000; // milliseconds -> microseconds
    if ( iMinimumOffTime.Int() < 0 )
        {
        iMinimumOffTime = 0;  // do not allow negative value
        }
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iMinimumOffTime: %d" ), iMinimumOffTime.Int() );
    
    User::LeaveIfError(repository->Get(KVibraCtrlMaxTime, value));
    iMaximumOnTime = value * 1000; // milliseconds -> microseconds
    if ( iMaximumOnTime.Int() < 0 )
        {
        iMaximumOnTime = 0;  // do not allow negative value
        }
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iMaximumOnTime: %d" ), iMaximumOnTime.Int() );
    
    User::LeaveIfError(repository->Get(KVibraCtrlProfileVibraEnabled, value));
    iVibraSetting = (value == 0 ) ? CHWRMVibra::EVibraModeOFF : CHWRMVibra::EVibraModeON;
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iVibraSetting: %d" ), iVibraSetting );
    
    User::LeaveIfError(repository->Get(KVibraCtrlProfileFeedbackEnabled, value));
    iVibraFeedbackSetting = (value == 0 ) ? CHWRMVibra::EVibraFeedbackModeOFF : CHWRMVibra::EVibraFeedbackModeON;
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iVibraFeedbackSetting: %d" ), iVibraFeedbackSetting );
    
    User::LeaveIfError(repository->Get(KVibraCtrlFeedbackIntensity, iFeedbackIntensity));
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iFeedbackIntensity: %d" ), iFeedbackIntensity );
    
    User::LeaveIfError(repository->Get(KVibraCtrlFeedbackDuration, iFeedbackDuration));
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iFeedbackDuration: %d" ), iFeedbackDuration );
    

    
    CleanupStack::PopAndDestroy(repository);
    
    // Get privileged SID
    CRepository* hwrmRep = CRepository::NewLC(KCRUidHWRMSettings);
    TInt sidValue(0);
    User::LeaveIfError(hwrmRep->Get(KHWRMAudioVibraSID, sidValue));
    iPrivilegedSid = new (ELeave) TSecureId(sidValue);    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iPrivilegedSid: 0x%x" ), iPrivilegedSid->iId );

    // Initiate and get vibra blocking mask
    iConfVibraBlockMask =
        KHWRMInternalSettingsChargerBlockVibra |
        KHWRMInternalSettingsChargerBlockFeedback |
        KHWRMInternalSettingsAccessoryBlockFeedback |
        KHWRMInternalSettingsCoolOffBlockFeedback;

    TInt err = hwrmRep->Get(KHWRMInternalSettings, value);
    if ( err == KErrNone )
        {
        iConfVibraBlockMask = value;
        // clear bits, which do not mean block-mask
        iConfVibraBlockMask &= ~KHWRMInternalSettingsDeviceSupportsPulse;
        // set pulse support value
        iPulseSupported = value & KHWRMInternalSettingsDeviceSupportsPulse;
        }

    CleanupStack::PopAndDestroy(hwrmRep);
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - iPrivilegedSid: 0x%x" ), iPrivilegedSid->iId );
    
    // Set up status P&S value
    User::LeaveIfError(RProperty::Define(KPSUidHWResourceNotification, KHWRMVibraStatus, RProperty::EInt, KNoCapability, KHWRMSidOnlyPolicy));
    User::LeaveIfError(iProperty.Attach(KPSUidHWResourceNotification, KHWRMVibraStatus));
    
    // Set up profile observer (Will also initialize the P&S value indirectly on callback)
    iProfileObserver = CHWRMVibraProfileObserver::NewL(this);

    // Construct vibra service last so that the common data is in order to be used
    // in service construction.    
    iVibraService = CHWRMVibraService::NewL(aPluginHandler, aReservationHandler, iCommonService, *this, EFalse);

    // Set up P&S observers for vibra blocking
    iAccessoryBlockObserver = CHWRMPSChangeObserver::NewL(*this, KPSUidHWRM, KHWRMAccessoryVibraBlocked);
    iInternalBlockObserver = CHWRMPSChangeObserver::NewL(*this, KPSUidHWRMPrivate, KHWRMInternalVibraBlocked);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMVibraCommonData* CHWRMVibraCommonData::NewL(CHWRMPluginHandler* aPluginHandler,
                                                 CHWRMReservationHandler* aReservationHandler,
                                                 CHWRMVibraCommonService& aCommonService)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::NewL()" ) );

    
    CHWRMVibraCommonData* self = new( ELeave ) CHWRMVibraCommonData(aCommonService);

    
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aReservationHandler);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMVibraCommonData::~CHWRMVibraCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::~CHWRMVibraCommonData()" ) );

    delete iProfileObserver;    
    
    iProperty.Close();

    delete iAccessoryBlockObserver;
    delete iInternalBlockObserver;
    
    delete iVibraService;
    
    delete iPrivilegedSid;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::~CHWRMVibraCommonData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraState
// -----------------------------------------------------------------------------
//
CHWRMVibra::TVibraModeState CHWRMVibraCommonData::VibraState()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::TVibraModeState()" ) );

    // If there has been a problem in the profile observer, try recreating it.
    if ( iVibraSetting == CHWRMVibra::EVibraModeUnknown )
        {
        COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibra::VibraState - Restarting profile observer") ); 
        delete iProfileObserver;
        iProfileObserver = NULL;
        
        TRAP_IGNORE(iProfileObserver = CHWRMVibraProfileObserver::NewL(this));
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::TVibraModeState - return 0x%x" ), iVibraSetting );

    return iVibraSetting;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraModeChange
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonData::VibraModeChange(CHWRMVibra::TVibraModeState aNewState)
    {
    COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::VibraModeChange(0x%x)"), aNewState); 

    iVibraSetting = aNewState;
    
    if ( iVibraSetting == CHWRMVibra::EVibraModeON )
        {
        // if just coming out of mode change, vibra can be assumed to be stopped
        // except if vibra is set on by privileged client.
        if ( !iPrivilegedVibraOn )
            {            
            PublishVibraStatus( CHWRMVibra::EVibraStatusStopped );
            }
        }
    else if ( !iPrivilegedVibraOn )
        {
        PublishVibraStatus( CHWRMVibra::EVibraStatusNotAllowed );
        
        // If vibra service is initialized, stop vibra.
        // Service will be uninitialized on first startup mode change, but 
        // vibra will not be on at that point anyway, as nobody could have
        // started it yet.
        if ( iVibraService )
            {
            iVibraService->ForceVibraStop();
            }        
        }
    else
        {
        // nothing
        }

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::VibraModeChange - return") ); 
    }
    
    // -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraFeedbackState
// -----------------------------------------------------------------------------
//
CHWRMVibra::TVibraFeedbackModeState CHWRMVibraCommonData::VibraFeedbackState()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraCommonData::VibraFeedbackState()" ) );

    // If there has been a problem in the profile observer, try recreating it.
    if ( iVibraFeedbackSetting == CHWRMVibra::EVibraFeedbackModeUnknown )
        {
        COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibra::VibraState - Restarting profile observer") ); 
        delete iProfileObserver;
        iProfileObserver = NULL;
        
        TRAPD( err, iProfileObserver = CHWRMVibraProfileObserver::NewL(this) );
        
        if ( err != KErrNone )
            {
            // Unable to recreate profile observer. Memory probably low. Must recreate observer next time some
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibra::VibraFeedbackState - Unable to restart profile observer") ); 
            }
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraCommonData::VibraFeedbackState - return 0x%x" ), iVibraFeedbackSetting );

    return iVibraFeedbackSetting;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraFeedbackModeChange
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonData::VibraFeedbackModeChange(CHWRMVibra::TVibraFeedbackModeState aNewState)
    {
    COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::VibraFeedbackModeChange(0x%x)"), aNewState); 

    iVibraFeedbackSetting = aNewState;
    
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::VibraFeedbackModeChange - return") ); 
    }
    
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::PSValueChanged
// Act on observed PS value changes
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonData::PSValueChanged(const TUid& aUid, TUint32 aKey, TInt aValue)
    {
    COMPONENT_TRACE4(_L("HWRM Server - CHWRMVibraCommonData::PSValueChanged(0x%x, 0x%x, 0x%x)"), aUid, aKey, aValue); 
    
    //We are currently listening to is the Accessory server and charger statuses
    if ( ( aUid == KPSUidHWRM && aKey == KHWRMAccessoryVibraBlocked ) ||
         ( aUid == KPSUidHWRMPrivate && aKey == KHWRMInternalVibraBlocked ) )
        {
        if ( aValue )
            {
            // Set bit in blockmask
            iVibraBlockMask |= aKey;
            
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::PSValueChanged - Vibra block set, mask: 0x%x"), iVibraBlockMask );

            // Explicitly shut down the vibra
            if ( iVibraService )
                {
                iVibraService->ForceVibraStop();
                PublishVibraStatus( CHWRMVibra::EVibraStatusNotAllowed );
                }
            else
                {
                COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::PSValueChanged - Error: Service not initialized, cannot stop vibra!") ); 
                }
            }
        else
            {
            // Clear bit in blockmask
            iVibraBlockMask &= ~aKey;
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::PSValueChanged - Vibra block cleared, mask: 0x%x"), iVibraBlockMask );
            
            // Status must be stopped if vibra block was just cleared
            if ( !iVibraBlockMask )
                {
                PublishVibraStatus( CHWRMVibra::EVibraStatusStopped );
                }
            }
        }
     
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::PSValueChanged - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraLocked
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraCommonData::VibraLocked() const
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::VibraLocked()")); 

    TBool retval(EFalse);

    // Vibra is considered also locked if it is blocked
    if ( iVibraBlockMask )
        {
        retval = ETrue;
        }
    else
        {        
        TTime currentTime;
        currentTime.UniversalTime();

        if ( currentTime < iVibraLockedUntil )
            {
            retval = ETrue;
            }
        }
        
    COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::VibraLocked - return 0x%x"), retval );     
    
    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraBlocked
// -----------------------------------------------------------------------------
//
TInt CHWRMVibraCommonData::VibraBlocked() const
    {
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMVibraCommonData::VibraBlocked()")) ); 

    TInt retval(EFalse);

    if ( iVibraBlockMask )
        {
        retval = iVibraBlockMask;
        }
        
    return retval;
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::VibraBlocked
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraCommonData::VibraBlocked( TInt aMask ) const
    {
    TBool retval(EFalse);
    
    if ( iConfVibraBlockMask & aMask )
        {
        retval = ETrue;
        }
        
    return retval;
    }    

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::LockVibra
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonData::LockVibra()
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::LockVibra()")); 

    iVibraLockedUntil.UniversalTime();
    iVibraLockedUntil += iMinimumOffTime;

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::LockVibra - return") );     
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::PublishVibraStatus
// -----------------------------------------------------------------------------
//
void CHWRMVibraCommonData::PublishVibraStatus(CHWRMVibra::TVibraStatus aStatus,
                                              TBool aPrivilegedClient)
    {
    COMPONENT_TRACE3(_L("HWRM Server - CHWRMVibraCommonData::PublishVibraStatus(0x%x, 0x%x)"), aStatus, aPrivilegedClient ); 

    CHWRMVibra::TVibraStatus publishStatus(aStatus);
    
    // If vibra is on by a priviledged client, or privileged client is publishing status,
    // deciding published status requires some extra manouvering.
    // -> Check if actual status should be whatever is being set or not allowed 
    if ( ( (iPrivilegedVibraOn || aPrivilegedClient)
         && aStatus != CHWRMVibra::EVibraStatusOn 
         && (VibraLocked() || iVibraSetting == CHWRMVibra::EVibraModeOFF) )
         || VibraBlocked() ) // Blocked vibra will always set EVibraStatusNotAllowed
        {
        publishStatus = CHWRMVibra::EVibraStatusNotAllowed;
        }

    if ( aStatus == CHWRMVibra::EVibraStatusOn )
        {
        if ( aPrivilegedClient )
            {            
            iPrivilegedVibraOn = ETrue;  
            }
        // Note: Vibra on-call from non-privileged client will not change iPrivilegedVibraOn state.
        }
    else 
        {
        iPrivilegedVibraOn = EFalse;
        }

    // Publish vibra status to P&S. If publishing fails, something is very wrong
    // with P&S, so ignore it. If it is memory issue, hopefully it will work 
    // next time. If it is not, we can't do anything about it anyway.
    
    TInt err(KErrNone);
	err = iProperty.Set(publishStatus);
        
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::PublishVibraStatus - Set failed: %d"), err );  
        }
    else
        {
        // In order to avoid publishing status for every short pulses, store last
        // published status.
        iLastPublishedStatus = publishStatus;
        COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::PublishVibraStatus - Set ok: %d"), publishStatus ); 
        }        
       
        
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraCommonData::PublishVibraStatus - return") );     
    }

// -----------------------------------------------------------------------------
// CHWRMVibraCommonData::IsPrivilegedSid
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraCommonData::IsPrivilegedSid(const TSecureId& aClientSid)
    {
    COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::IsPrivilegedSid(0x%x)"), aClientSid.iId ); 
    
    TBool retval(EFalse);
    
    if ( iPrivilegedSid )
        {
        if ( *iPrivilegedSid == aClientSid )
            {
            retval = ETrue;
            }
        }

    COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraCommonData::IsPrivileged - return 0x%x"), retval );        
    
    return retval;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================



//  End of File  
