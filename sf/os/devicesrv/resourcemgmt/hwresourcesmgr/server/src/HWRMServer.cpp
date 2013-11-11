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
#include <ecom/ecom.h>
#include <centralrepository.h>
#include <featureuids.h>
#include "HWRMClient.h"            // THWRMResourceType
#include "HWRMClientServer.h"
#include "HWRMVibraClientServer.h"
#include "HWRMLightClientServer.h"
#include "HWRMPowerClientServer.h"
#include "HWRMFmTxClientServer.h"
#include "HWRMServer.h"
#include "HWRMSession.h"
#include "HWRMConfiguration.h"
#include "HWRMPluginHandler.h"  
#include "HWRMVibraCommands.h"  
#include "HWRMLightCommands.h"  
#include "HWRMPowerCommands.h" 
#include "HWRMFmTxCommands.h"
#include "HWRMtrace.h"
#include "HWRMVibraCommonService.h"
#include "HWRMVibraCommonData.h"
#include "HWRMLightCommonData.h"
#include "HWRMPowerCommonData.h"
#include "HWRMFmTxCommonData.h"
#include "HWRMFmTxStateUtility.h"
#include "HWRMReservationHandler.h"
#include "HWRMPolicy.h"
#include "HWRMPowerStatePluginHandler.h"
#include "HWRMPrivatePSKeys.h"
#include "HWRMDomainPSKeys.h"
#include "HWRMPrivateCRKeys.h"
#include "HWRMFeatureMgr.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS

// ------------------- Policy Server Security Setup ----------------------

const TUint  KRangeCount = 12;

const TInt KIPCRanges[KRangeCount] =
	{
	EHWRMCreateVibraService,			// first createSevice IPC (==0)
	EHWRMEndOfCreateServiceOpCodes,
	EHWRMLightsOn,						// first light IPC (==1000)
	EHWRMEndOfLightOpCodes,
	EHWRMStartVibraDefaultIntensity,	// first vibra IPC (==2000)
	EHWRMEndOfVibraOpCodes,
	EHWRMGetBatteryInfo,				// first power IPC (==3000)
	EHWRMEndOfPowerOpCodes,
	EHWRMFmTxOn, 						// first fmTx IPC (==4000)
	EHWRMFmTxSetRdsPty,
	EHWRMFmTxReserve,
	EHWRMEndOfFmTxOpCodes
	};

/**
 Specifies the appropriate action to take for each range in KRanges.
 The n:th element of KElementsIndex specifies the appropriate action to take for the n:th range in KRanges.
*/

#ifdef HWRM_FMTX_POLICY_CHECKING_DISABLED
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported
	};

const CPolicyServer::TPolicyElement KHWRMPolicyElements[] ={{}};

#else
const TUint8 KElementsIndex[KRangeCount] =
	{
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported,
	0,
	1,
	CPolicyServer::EAlwaysPass,
	CPolicyServer::ENotSupported
	};

/**
 Array containing the different security checks performed by this server
 */
const CPolicyServer::TPolicyElement KHWRMPolicyElements[] =
	{
	{_INIT_SECURITY_POLICY_C1(ECapabilityMultimediaDD), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C2(ECapabilityMultimediaDD, ECapabilityWriteUserData), CPolicyServer::EFailClient}
	};
#endif //HWRM_FMTX_POLICY_CHECKING_DISABLED 

/**
 Setup a security policy.
*/
const CPolicyServer::TPolicy KHWRMServerPolicy =
	{
	CPolicyServer::EAlwaysPass, // connection attempts always pass
	KRangeCount,
	KIPCRanges,
	KElementsIndex,
	KHWRMPolicyElements
	}; 

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "HWRMServer" );
const TInt KDefaultPluginTimeout = 10000000;

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMServer::CHWRMServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMServer::CHWRMServer( const TInt aPriority, const TServerType aType  )
    : CPolicyServer( aPriority, KHWRMServerPolicy, aType ),
    iPluginTimeout(KDefaultPluginTimeout)
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CHWRMServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMServer::ConstructL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::ConstructL()" ) );
 
    TInt err(KErrNone);
    TInt chargerBlockVibra(ETrue);
    TInt chargerBlockFeedbackVibra(ETrue);
    
    // Open settings repository and get necessary values
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL(KCRUidHWRMSettings) );
    
    if ( err == KErrNone )
        {
        TInt value(0);
        err = repository->Get(KHWRMPluginTimeout, value);
        if ( err == KErrNone )
            {
            if ( value > 0 )
                {
                iPluginTimeout = value*1000; // Value is in milliseconds
                }
            }
        else
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - KHWRMPluginTimeout value getting failed: %d, using default" ), err);
            }
            
        err = repository->Get(KHWRMInternalSettings, value);
        if ( err == KErrNone )
            {
            chargerBlockVibra = (value & KHWRMInternalSettingsChargerBlockVibra) ? ETrue : EFalse;
            chargerBlockFeedbackVibra = (value & KHWRMInternalSettingsChargerBlockFeedback) ? ETrue : EFalse;
            }
        else
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - KHWRMInternalSettings value getting failed: %d, using default" ), err );
            }
        }
    else
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - KCRUidHWRMSettings repository opening failed: %d" ), err);
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - pluginTimeout: %d" ), iPluginTimeout);
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - chargerBlockVibra: %d" ), chargerBlockVibra);
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - chargerBlockFeedbackVibra: %d" ), chargerBlockFeedbackVibra);
    
    // Read priority setting and set HWRM priority accordingly
    TInt priority(0);
    err = repository->Get(KHWRMPriority, priority);
    if ( err == KErrNone )
        {
        TProcessPriority procPrio( static_cast<TProcessPriority>(priority) );
        COMPONENT_TRACE3(_L("HWRM Server - CHWRMServer::ConstructL - current priority: %d \
            new priority %d" ), RProcess().Priority(), procPrio);
        RProcess().SetPriority( procPrio );
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL - priority now: %d" ), RProcess().Priority() );
        }
    
    // Cleanup repository
    delete repository;
    repository = NULL;

    // Create light handlers and common data (vibra ones are created when vibra is first needed)
    TRAP(err, iLightPluginHandler = CHWRMPluginHandler::NewL( KHWRMLightApiEComMatch, iPluginTimeout ));
    if ( err != KErrNone )
        {
        // Light not supported by the device, set handler to null
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL() - Light not supported, error: %d" ), err );
        iLightPluginHandler = NULL;
        }
    else
        {
        TRAP(err, iLightReservationHandler = CHWRMReservationHandler::NewL( KHWRMLightMaxTargets, KLightsPolicyFilename ));
        if ( err != KErrNone )
            {
            // Light not supported by the device, set handler to null
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL() - iLightReservationHandler creation, error: %d" ), err );
            iLightReservationHandler = NULL;
            }
        else
            {
            TRAP(err, iLightCommonData = CHWRMLightCommonData::NewL(iLightPluginHandler, iLightReservationHandler));
            if ( err != KErrNone )
                {
                // Light not supported by the device, set common data to null
                COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL() - CHWRMLightCommonData creation error: %d" ), err );
                iLightCommonData = NULL;
                }
            }
        }

    // Initialize vibra blocking key
    User::LeaveIfError(RProperty::Define(KPSUidHWRMPrivate, KHWRMInternalVibraBlocked, RProperty::EInt, KNoCapability, KWriteDeviceDataPolicy));

    // Initialize keys of HWRM Device Switch API
    User::LeaveIfError(RProperty::Define( KPSUidHWRM, KHWRMTwistStatus, RProperty::EInt, KAlwaysPassPolicy, KWriteDeviceDataPolicy ));
    RProperty::Set( KPSUidHWRM, KHWRMTwistStatus, EPSHWRMTwistStatusUninitialized );
    User::LeaveIfError(RProperty::Define( KPSUidHWRM, KHWRMFlipStatus, RProperty::EInt, KAlwaysPassPolicy, KWriteDeviceDataPolicy  ));
    RProperty::Set( KPSUidHWRM, KHWRMFlipStatus, EPSHWRMFlipStatusUninitialized );
    User::LeaveIfError(RProperty::Define( KPSUidHWRM, KHWRMGripStatus, RProperty::EInt, KAlwaysPassPolicy, KWriteDeviceDataPolicy ));
    RProperty::Set( KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripStatusUninitialized );
    
    // Initialize FmTx common data that is needed immediately like P&S keys etc
    TRAP( err, iFmTxCommonData = CHWRMFmTxCommonData::NewL() );
    if ( err != KErrNone )
       {
       COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL() - FmTx common data, error: %d" ), err );
       iFmTxCommonData = NULL;
       }

    // create power state plugin    
    TRAP( err, iPowerStatePluginHandler = CHWRMPowerStatePluginHandler::NewL(chargerBlockVibra, chargerBlockFeedbackVibra) );
    if ( err != KErrNone )
        {
        // Power state API not supported by the device, set handler to null
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::ConstructL() - Power state not supported, error: %d" ), err );
        iPowerStatePluginHandler = NULL;
        }

    // Set server process system critical
    User::SetProcessCritical(User::ESystemPermanent);
    
    // Add server to active scheduler
    StartL( KServerProcessName );
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::ConstructL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMServer* CHWRMServer::NewL(const TInt aPriority)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::NewL(0x%x)" ), aPriority );

    CHWRMServer* self = CHWRMServer::NewLC(aPriority);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::NewL - return 0x%x" ), self );

    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMServer* CHWRMServer::NewLC(const TInt aPriority)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::NewLC(0x%x)" ), aPriority );

    CHWRMServer* self = new( ELeave ) CHWRMServer(aPriority, EUnsharableSessions);
    
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::NewLC - return 0x%x" ), self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMServer::~CHWRMServer()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::~CHWRMServer()" ) );

    // Close all open sessions
	CHWRMSession* session=NULL;
	iSessionIter.SetToFirst();
	while((session = static_cast<CHWRMSession*>(iSessionIter++)) != NULL)
	{
		delete session;
	}

    // Delete plugin handlers    
    delete iPowerStatePluginHandler;
    delete iVibraPluginHandler;
    delete iLightPluginHandler;
	delete iPowerPluginHandler;
    delete iFmTxPluginHandler;
    delete iFmTxWatcherPluginHandler;

    // Delete reservation handlers    
    delete iVibraReservationHandler;
    delete iLightReservationHandler;
    delete iFmTxReservationHandler;

    // Delete vibra common service
    delete iVibraCommonService;

    // Delete common data
    delete iVibraCommonData;
    delete iLightCommonData;
	delete iPowerCommonData;
	delete iFmTxStateUtility;
    delete iFmTxCommonData;

	// Signal final closure of ecom session
	REComSession::FinalClose();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::~CHWRMServer - return") );
    }


// -----------------------------------------------------------------------------
// CHWRMServer::NewSessionL
// Creates a new CSession2
// -----------------------------------------------------------------------------
//
CSession2* CHWRMServer::NewSessionL( const TVersion& aVersion,
                                     const RMessage2& /*aMessage*/ ) const
    {
#if defined(_DEBUG) && defined(COMPONENT_TRACE_FLAG)
    TVersionName name = aVersion.Name();
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::NewSessionL(%S)" ), &name);
#endif

    if ( !User::QueryVersionSupported( TVersion( KServerVersionMajor, 
                                                 KServerVersionMinor, 
                                                 KServerVersionBuild ), 
                                                 aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    CSession2* session = CHWRMSession::NewL(*const_cast<CHWRMServer*>(this));

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::NewSessionL - return 0x%x" ), session );

    return( session );
    }

// -----------------------------------------------------------------------------
// CHWRMServer::PanicClient
// RMessage2::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// -----------------------------------------------------------------------------
//
void CHWRMServer::PanicClient(const RMessage2& aMessage,THWRMPanic aPanic)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMServer::PanicClient(0x%x, 0x%x)" ), aMessage.Handle(), aPanic);

	aMessage.Panic(KPanicCategory,aPanic);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::PanicClient - return" ) );
    }

   
// -----------------------------------------------------------------------------
// CHWRMServer::GetPluginHandler
// Returns pointer to correct plugin handler.
// In case resource is not supported, returns NULL
// -----------------------------------------------------------------------------
//  
CHWRMPluginHandler* CHWRMServer::GetPluginHandler(THWRMResourceType aType)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetPluginHandler(0x%x)" ), aType );
    
    CHWRMPluginHandler* retval = NULL;
    
    switch(aType)
        {
        case HWRMResourceTypeVibra:
            if ( !iVibraInitAttempted )
                {
                InitVibra();
                }
            retval = iVibraPluginHandler;
            break;            
        case HWRMResourceTypeLight:
            retval = iLightPluginHandler;
            break;        
        case HWRMResourceTypePower:
        	if ( !iPowerInitAttempted )
        		{
            	InitPower();
            	}
        	retval = iPowerPluginHandler;
        	break;        
        case HWRMResourceTypeFmTx:    
            if ( !iFmTxInitAttempted )
                {
                TRAPD( initErr, InitFmTxL() );
                if ( initErr != KErrNone )
                	{
                	COMPONENT_TRACE2( _L("HWRM Server - CHWRMServer::InitFmTxL() error = %d"), initErr );            	
                	}
                }
        	retval = iFmTxPluginHandler;
            break;                   
        case HWRMResourceTypeFmTxWatcher:
            retval = iFmTxWatcherPluginHandler;
        	break;
        default:
            // invalid type, just break (i.e. return NULL)
            break;   
        }
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetPluginHandler - return 0x%x" ), retval );

    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::GetReservationHandler
// Returns pointer to correct reservation handler.
// In case resource is not supported, returns NULL
// -----------------------------------------------------------------------------
//  
CHWRMReservationHandler* CHWRMServer::GetReservationHandler(THWRMResourceType aType)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetReservationHandler(0x%x)" ), aType );
    
    CHWRMReservationHandler* retval = NULL;
    
    switch(aType)
        {
        case HWRMResourceTypeVibra:
            retval = iVibraReservationHandler;
            break;            
        case HWRMResourceTypeLight:
            retval = iLightReservationHandler;
            break;         
        case HWRMResourceTypeFmTx:
        	retval = iFmTxReservationHandler;
        	break;
        default:
            // invalid type, just break (i.e. return NULL)
            break;   
        }
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetReservationHandler - return 0x%x" ), retval );

    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::GetVibraCommonService
// Returns pointer to Vibra common service used to handle overlapping vibration 
// requests from all sessions.
// -----------------------------------------------------------------------------
//  
CHWRMVibraCommonService* CHWRMServer::GetVibraCommonService()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::GetVibraCommonService()" ));

    CHWRMVibraCommonService* retval = iVibraCommonService;
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetVibraCommonService - return 0x%x" ), retval );

    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::GetVibraCommonData
// Returns pointer to Vibra common data
// -----------------------------------------------------------------------------
//  
CHWRMVibraCommonData* CHWRMServer::GetVibraCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::GetVibraCommonData()" ));
    
    CHWRMVibraCommonData* retval = iVibraCommonData;
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetVibraCommonData - return 0x%x" ), retval );

    return retval;
    }
// -----------------------------------------------------------------------------
// CHWRMServer::GetLightCommonData
// Returns pointer to Light common data
// -----------------------------------------------------------------------------
//  
CHWRMLightCommonData* CHWRMServer::GetLightCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::GetLightCommonData()" ));
    
    CHWRMLightCommonData* retval = iLightCommonData;
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetLightCommonData - return 0x%x" ), retval );

    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::GetPowerCommonData
// Returns pointer to Power common data
// -----------------------------------------------------------------------------
//  
CHWRMPowerCommonData* CHWRMServer::GetPowerCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::GetPowerCommonData()" ));
    
    CHWRMPowerCommonData* retval = iPowerCommonData;
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::GetPowerCommonData - return 0x%x" ), retval );

    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::GetFmTxCommonData
// Returns pointer to FM Tx common data
// -----------------------------------------------------------------------------
//  
CHWRMFmTxCommonData* CHWRMServer::GetFmTxCommonData()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMServer::GetFmTxCommonData()" ) );
    
    CHWRMFmTxCommonData* retval = iFmTxCommonData;
    
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMServer::GetFmTxCommonData - return 0x%x" ), retval );

    return retval;
    }

// -----------------------------------------------------------------------------
// CHWRMServer::InitVibra
// Initialises vibra
// -----------------------------------------------------------------------------
//  
void CHWRMServer::InitVibra()
    {
    if ( !iVibraInitAttempted )
        {
        iVibraInitAttempted = ETrue;    
        TInt err;
        
        //The feature UID NFeature::KVibra is defined in featureuids.h
        TBool vibraSupported = HWRMFeatureMgr::IsFeatureSupported(NFeature::KVibra);

        if ( vibraSupported )
            {
            TRAP(err, iVibraPluginHandler = CHWRMPluginHandler::NewL( KHWRMVibraApiEComMatch, iPluginTimeout ));
            if ( err != KErrNone )
                {
                // Vibra not supported by the device, set handler to null
                COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::InitVibra() - Vibra not supported, error: %d" ), err );
                iVibraPluginHandler = NULL;
                }
            else
                {
                TRAP(err, iVibraReservationHandler = CHWRMReservationHandler::NewL( KHWRMVibraMaxTargets, KVibraPolicyFilename ));
                if ( err != KErrNone )
                    {
                    // Vibra not supported by the device, set handler to null
                    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::InitVibra() - iVibraReservationHandler creation, error: %d" ), err );
                    iVibraReservationHandler = NULL;
                    }
                else
                	{                	               
                    TRAP(err, iVibraCommonService = CHWRMVibraCommonService::NewL(iVibraPluginHandler, iVibraReservationHandler));
                    if ( err != KErrNone )
                        {
                        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::InitVibra() - iVibraCommonService creation, error: %d" ), err );
                        iVibraCommonService = NULL;
                        }                	
                	else
                    	{                    	
						TRAP(err, iVibraCommonData = CHWRMVibraCommonData::NewL(iVibraPluginHandler, iVibraReservationHandler, *iVibraCommonService));
                    	if ( err != KErrNone )
                        	{
                        	COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::InitVibra() - iVibraCommonData creation, error: %d" ), err );
                        	}
                    	}                               
                	}
                }
            }
        else
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServer::InitVibra() - Vibra not supported in the device." ) );
            }
        }
    }


// -----------------------------------------------------------------------------
// CHWRMServer::InitPower
// Initialises power
// -----------------------------------------------------------------------------
//  
void CHWRMServer::InitPower()
	{
	if ( !iPowerInitAttempted )
        {
        iPowerInitAttempted = ETrue;  
        TInt err;
	
        TRAP(err, iPowerPluginHandler = CHWRMPluginHandler::NewL( KHWRMPowerApiEComMatch, iPluginTimeout));        
		if ( err != KErrNone )
		    {
		    // Power not supported by the device, set handler to null
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::InitPower() - Power not supported, error: %d" ), err );
            iPowerPluginHandler = NULL;		    
		    }
		else
		    {
		    TRAP(err, iPowerCommonData = CHWRMPowerCommonData::NewL(iPowerPluginHandler));

		    if ( err != KErrNone )
	            {
	            // Power not supported by the device, set common data to null
	            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServer::InitPower() - CHWRMPowerCommonData creation error: %d" ), err );
			    iPowerCommonData = NULL;
	            }		        
		    }
        }
	}


// -----------------------------------------------------------------------------
// CHWRMServer::InitFmTxL
// Initialises FM Tx
// -----------------------------------------------------------------------------
//  
void CHWRMServer::InitFmTxL()
    {
	COMPONENT_TRACE1( _L( "HWRM Server - CHWRMServer::InitFmTxL()" ) );
    	
    if ( !iFmTxInitAttempted )
        {
		COMPONENT_TRACE1( _L( "HWRM Server - CHWRMServer::InitFmTxL() first attempt" ) );
        	
        iFmTxInitAttempted = ETrue;    

       
        //The feature UID NFeature::KFmTx is defined in featureuids.h
        TBool fmTxSupported = HWRMFeatureMgr::IsFeatureSupported(NFeature::KFmTx);
      
        if ( fmTxSupported )
            {
			COMPONENT_TRACE1( _L( "HWRM Server - CHWRMServer::InitFmTxL() - FM Tx supported in the device" ) );
            	
            delete iFmTxPluginHandler;
            iFmTxPluginHandler = NULL;
            iFmTxPluginHandler = CHWRMPluginHandler::NewL( KHWRMFmTxApiEComMatch, iPluginTimeout );
            
            delete iFmTxWatcherPluginHandler;
            iFmTxWatcherPluginHandler = NULL;
            iFmTxWatcherPluginHandler = CHWRMPluginHandler::NewL( KHWRMFmTxWatcherApiEComMatch,
                                                                  iPluginTimeout );

            delete iFmTxReservationHandler;
            iFmTxReservationHandler = NULL;
			iFmTxReservationHandler = CHWRMReservationHandler::NewL( KHWRMFmTxSubResourceCount, KFmTxPolicyFilename );
			
            delete iFmTxCommonData;
            iFmTxCommonData = NULL;
			iFmTxCommonData = CHWRMFmTxCommonData::NewL();		
			
			iFmTxPluginHandler->RegisterForIndicationsL(iFmTxCommonData);
			
			iFmTxStateUtility = CHWRMFmTxStateUtility::NewL(iFmTxPluginHandler, 
			         iFmTxWatcherPluginHandler, iFmTxReservationHandler, iFmTxCommonData);

			iFmTxWatcherPluginHandler->RegisterForIndicationsL(iFmTxStateUtility);
            }
        else
            {
            COMPONENT_TRACE1( _L( "HWRM Server - CHWRMServer::InitFmTx() - FM Tx not supported in the device." ) );
            }
        }
    }    

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File  
