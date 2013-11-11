// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonetsyTraces.h"
#endif


//INCLUDES
#include <etelmm.h>
#include <etelmmerr.h>
#include <exterror.h>

#include "cmmphonetsy.h"
#include "cmmvoicelinetsy.h"
#include "cmmdatalinetsy.h"
#include "cmmfaxlinetsy.h"
#include "cmmcalltsy.h"
#include "cmmconferencecalltsy.h"
#include "cmmussdtsy.h"
#include "cmmsecuritytsy.h"
#include "cmmbroadcasttsy.h"
#include "cmmsmstsy.h"
#include "cmmonstoretsy.h"
#include "cmmenstoretsy.h"
#include "cmmphonebookstoretsy.h"
#include "CMmCustomTsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalllist.h"
#include "cmmlinelist.h"
#include "cmmpblist.h"
#include "MmTsy_numberOfSlots.h"
#include "MmTsy_conf.h"
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "CMmCommonStaticUtility.h"
#include "CMmSIMTsy.h"
#include "cmmnettsy.h"
#include "cmmdtmftsy.h"
#include "cmmsupplservtsy.h"
#include "ctsyprivatecrpkeys.h"
#include <ctsy/serviceapi/ctsydomaincrkeys.h>
#include "et_struct.h"

// Product specific headers
#include "MmTsy_PhoneDefs.h"
#include "Cmmpacketservicetsy.h"
#include "cmmphonegsmwcdmaext.h"

#ifdef REQHANDLE_TIMER
#include "CResponseTimer.h"
#include "CResponseTimerStore.h"
#endif // REQHANDLE_TIMER

#include <ctsy/serviceapi/ctsysatmessagingbase.h>
#include "cmmphonefactorytsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/serviceapi/gsmerror.h>

// observers for HWRM
#include "BatteryInfoObserver.h"
#include "ChargingStatusObserver.h"

#ifdef SYMBIAN_BAFL_SYSUTIL
#include <bafl/sysutil.h>
#endif

// ======== Patchable-Constants ======
#ifdef __EABI__
// HWRM is supported
IMPORT_C extern const TInt32 KHWRMIsSupported;
#else
// for emulator, use HalFunction to get the value from epoc.ini
#include <u32hal.h>
#endif

// ======== MEMBER FUNCTIONS ========

EXPORT_C CMmPhoneTsy* CMmPhoneTsy::NewL (
    CMmMessageManagerBase* aMessageManager,
    CMmPhoneFactoryTsy* aMmPhoneFactoryTsy,
    MLtsyFactoryBase* aFactory )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CMMPHONETSY_NEWL_1, "TSY: CMmPhoneTsy::NewL - Phone model Id: %S", KPhoneModelId);
    CleanupStack::PushL( aMessageManager );
    CMmPhoneTsy* mmPhoneTsy = new (ELeave) CMmPhoneTsy();

    mmPhoneTsy->iMessageManager = aMessageManager;
    CleanupStack::Pop(aMessageManager);
    mmPhoneTsy->iMmPhoneFactory = aMmPhoneFactoryTsy;
    CleanupClosePushL( *mmPhoneTsy );
    //For pointer is stored for deleting licenseetsy instance
    mmPhoneTsy->iLtsyFactory = aFactory;
    mmPhoneTsy->iTelephonyAudioControl = NULL;
    if ( aFactory )
        {
        // Version number is used to specify LTSY interface 
        MLtsyFactoryBase::TCtsyInterfaceVersion vers =
            aFactory->Version();
        
        if( MLtsyFactoryBase::EIfVersion2 <= vers )
            {
            MLtsyFactoryV2* ptr_v2 = 
                static_cast<MLtsyFactoryV2*>( aFactory );           
            
            // Acquire Telephony Audio Control pointer
            mmPhoneTsy->iTelephonyAudioControl = 
                ptr_v2->GetTelephonyAudioControl( mmPhoneTsy );            
            }
        }

    mmPhoneTsy->ConstructL();
    CleanupStack::Pop( mmPhoneTsy );
    return mmPhoneTsy;
    }

CMmPhoneTsy::CMmPhoneTsy():
    iReqHandleType(EMultimodePhoneReqHandleUnknown)
    {
    }

void CMmPhoneTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CONSTRUCTL_1, "TSY: CMmPhoneTsy::ConstructL");
	iCentRep = CRepository::NewL( KCRUidCtsyMEAlsLine );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CONSTRUCTL_2, "TSY: CMmPhoneTsy::ConstructL Central Repository opened successfully");
	
	//iPublic Central Repositories
    iMWISCentRep = CRepository::NewL( KCRUidCtsyMessageWaitingIndicator );
    iCFISPrivateCentRep = CRepository::NewL( KCRUidCtsyPrivateCallForwardingIndicator );
    iCFISCentRep = CRepository::NewL( KCRUidCtsyCallForwardingIndicator );
    
    // Set all the boot states to false
    iBootState.iSIMReady = EFalse;
    iBootState.iADNReady = EFalse;
    // Security will be set ready when all needed security codes
    // (PIN/Security code) are requested in boot
    iBootState.iSecReady = EFalse;

    //Initialise transaction handler, TSY req handle store, and object lists
    InitStoresAndListsL();

    //Initialise extension modules
    InitExtensionModulesL();

    //Initialise miscellaneous internal attributes
    InitInternalAttributes();

    //register phonetsy in the message manager
    iMessageManager->RegisterTsyObject( 
        CMmMessageManagerBase::EPhoneTsy , this );
        
    iEmergencyCallInfoProperty.Define( KPSUidCtsyEmergencyCallInfo,
          KCTSYEmergencyCallInfo, RProperty::EInt );
          
    //Parameter store
	iServiceTableReq = new ( ELeave ) TGetServiceTableRequest();
	
	//Initialisation
	iCalledOnGet = EFalse;
	iServiceTableReq->iCaching = EFalse;
	iSimRefreshRegisterOk = EFalse;
	iCacheServiceTableError = KErrNone;
	iTSYSubscriberIdReq = EFalse; 
	iAlsQueryInBoot = EFalse; 
	iCspFileQueryInBoot = EFalse;
	iCspFileALS = EFalse;
	iAlsPpSupport = EFalse;
	iCspFileAlsSet = EFalse;
	iViagHomeZoneParamsChecked = EFalse;
	iIsInitializationActive = EFalse;

	// initialize HWRM & its observers
	if(IsHWRMSupported())
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CONSTRUCTL_3, "TSY: HWRM Battery API is in use.");
		iPowerManager = CHWRMPower::NewL();
		iBattery = CBatteryInfoObserver::NewL( *iPowerManager, *this ); 
		iChargingStatus = CChargingStatusObserver::NewL( *this );
		}
    
    //Create the SSM Plugin Handler on creation rather than on demand to avoid problems 
    //with Emergency Calls in Out Of Memory situations.
    iSystemStatePluginHandler = CCtsySystemStatePluginHandler::NewL();
	
//#ifdef __WINS__  SYMBIAN commented out
//   iMmPhoneExtInterface->NotifySimStatusReadyL();
//#endif
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::InitStoresAndListsL
// Initialises transaction handler, TSY req handle store,
// and object lists (Call, Line and Phonebook lists) for CMmPhoneTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::InitStoresAndListsL()
    {
#ifdef REQHANDLE_TIMER
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL( this,
        this, EMultimodePhoneMaxNumOfRequests, iPhoneReqHandles );
        
    iTimeStampStore = CResponseTimerStore::NewL ( this );
    iReqHandleTimer = CResponseTimer::NewL( this );

#else
    //create req handle store
    iTsyReqHandleStore = CMmTsyReqHandleStore::NewL(
        EMultimodePhoneMaxNumOfRequests,  iPhoneReqHandles );
        
#endif //REQHANDLE_TIMER

    // Create call list
    iCallList = CMmCallList::NewL( this );

    // Create line list
    iLineList = CMmLineList::NewL( this );

    // Create phonebook list
    iPBList = CMmPBList::NewL( this );

    // Service table
    iServiceTableDataCache = NULL;

    // Service table (USIM)
    iServiceTableUSIMDataCache = NULL;

    //air time duration timer
    iAirTimeDurationTimer = CHeartbeatRunner::NewL( NULL, this );

    // Create and init phonebook state
    iPhoneBookState = new(ELeave) CStorageInfoData();
    }


// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetMessageManager
// Set message handler
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetMessageManager(
    CMmMessageManagerBase* aMessageManager )
	{
	iMessageManager = aMessageManager;
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::InitExtensionModulesL
// Initialises extension modules for CMmPhoneTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::InitExtensionModulesL()
    {
//Warnings caused by configurational constants in if case. These are not
//really errors but act as flags. This warning is purely informational as can
//be seen from the explanation of Visual C++ help:
//Compiler Warning (level 4) C4127
//conditional expression is constant. The controlling expression of an
//if statement or while loop evaluated to a constant. As a result, the
//code in the body of the if statement or while loop either always
//executes or never executes. This warning is informational.
#pragma warning(disable : 4127)

    if ( RMobilePhone::ENetworkModeGsm == KDefaultProtocol )
        {
        //Create GSM Phone extension
        iMmPhoneExtInterface =  CMmPhoneGsmWcdmaExt::NewL( this, 
            iMessageManager );
        }

//Set warning level to normal
#pragma warning(default : 4127)

    // create DTMF Tsy instance
	iMmDtmfTsy = CMmDtmfTsy::NewL( this );

    // create NetTsy instance
	iMmNetTsy = CMmNetTsy::NewL( this );

    // create SupplServTsy instance
	iMmSupplServTsy = CMmSupplServTsy::NewL( this );

    //create SecurityTsy instance
    iMmSecurityTsy = CMmSecurityTsy::NewL( this );
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::InitInternalAttributes
// Initialises miscellaneous internal attributes
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::InitInternalAttributes()
    {
    // Battery info
    iBatteryInfo.iStatus = RMobilePhone::EPowerStatusUnknown;
    iBatteryInfo.iChargeLevel = 0x00;

    // Modem info
    iPhoneStatus.iMode          = RPhone::EModeIdle;
    iPhoneStatus.iModemDetected = RPhone::EDetectedPresent;

    //ALS line
    iAlsLine = RMobilePhone::EAlternateLineUnknown;

    //amount of active calls for air time duration
    iAmountOfActiveCalls = 0;

    //Temporary ALS line
    iTempAlsLine = RMobilePhone::EAlternateLineUnknown;

    //SMS Messaging sub session
    iMmSmsTsy = NULL;

    //Set manufacturer
    iPhoneIdentity.iManufacturer.Copy( KPhoneManufacturerId );

    //Set model
    iPhoneIdentity.iModel.Copy( KPhoneModelId );

    //Set revision
    iPhoneIdentity.iRevision.Copy( KPhoneRevisionId );

	//Set serial number to 'unknown'
	iPhoneIdentity.iSerialNumber.Zero();

    // no SIM refresh ongoing at boot
    iRefreshOngoing = EFalse;
    iSimCacheQueue = 0;
    
    iPendingServiceTableReq = ENoRequest;
    iPendingServiceTableCacheReq = ENoRequest;

    // modem not ready
    iIsModemReady = EFalse;
    }
    
CMmPhoneTsy::~CMmPhoneTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DTOR_1, "TSY: CMmPhoneTsy::~CMmPhoneTsy");

	if(IsHWRMSupported())
		{
		if ( iChargingStatus )
			{
			delete iChargingStatus;   
			}
			
		if ( iBattery )
			{
			delete iBattery;    
			}
		
		if ( iPowerManager )
			{
			delete iPowerManager;
			}
		}
    // deregister tsy object from message manager
    MessageManager()->DeregisterTsyObject( this );
    
    //Close handle to P&S (Close is not needed because we don't 
    //have a handle to this property)
    iEmergencyCallInfoProperty.Delete(KPSUidCtsyEmergencyCallInfo,
    	KCTSYEmergencyCallInfo);

    if ( iServiceTableDataCache )
        {
        delete iServiceTableDataCache;
        }

    if ( iServiceTableUSIMDataCache )
        {
        delete iServiceTableUSIMDataCache;
        }

    //delete req handle store
    if ( iTsyReqHandleStore )
        {
        delete iTsyReqHandleStore;
        }
    iTsyReqHandleStore = NULL;
    //delete phone extension
    if ( iMmPhoneExtInterface )
        {
        delete iMmPhoneExtInterface;
        }
    iMmPhoneExtInterface = NULL;
    // delete DTMF Tsy instance
    if ( iMmDtmfTsy )
        {
        delete iMmDtmfTsy;
        }
    iMmDtmfTsy = NULL;
    // delete NetTsy instance
    if ( iMmNetTsy )
        {
        delete iMmNetTsy;
        }
    iMmNetTsy = NULL;
    // delete SupplServTsy instance
    if ( iMmSupplServTsy )
        {
        delete iMmSupplServTsy;
        }
    iMmSupplServTsy = NULL;
    //delete SecurityTsy instance
    if ( iMmSecurityTsy )
        {
        delete iMmSecurityTsy;
        }
    iMmSecurityTsy = NULL;

    // Destroy the call list
    if ( iCallList )
        {
        delete iCallList;
        }
    iCallList = NULL;
    // Destroy the line list
    if ( iLineList )
        {
        delete iLineList;
        }
    iLineList = NULL;
    // Destroy the phonebook list
    if ( iPBList )
        {
        delete iPBList;
        }
    iPBList = NULL;
	
	//Release created LicenseeTsy instance
	if( iLtsyFactory )
		{
		iLtsyFactory->Release();
		}
	iLtsyFactory = NULL;
	
    // Destroy the message manager
    if ( iMessageManager )
        {
        delete iMessageManager;
        }
    iMessageManager = NULL;

#ifdef REQHANDLE_TIMER
   //delete the time store
    if ( iReqHandleTimer )
        {
        delete iReqHandleTimer;
        }
    iReqHandleTimer = NULL;
   //delete the timer
    if ( iTimeStampStore )
        {
        delete iTimeStampStore;
        }
    iTimeStampStore = NULL;
#endif //REQHANDLE_TIMER

    //delete the air time duration timer
    if ( iAirTimeDurationTimer )
        {
        delete iAirTimeDurationTimer;
        }
    iAirTimeDurationTimer = NULL;

    //delete phonebook state
    if ( iPhoneBookState )
        {
        delete iPhoneBookState;
        }
    iPhoneBookState = NULL;
	
	if( iCentRep )
		{
		delete iCentRep;	
		}	
	iCentRep = NULL;
	
	// public central repositories
    if( iCFISCentRep )
		{
		delete iCFISCentRep;	
		}	
	iCFISCentRep = NULL;

    if( iMWISCentRep )
		{
		delete iMWISCentRep;	
		}	
	iMWISCentRep = NULL;


	if ( iCFISPrivateCentRep )
		{
		delete iCFISPrivateCentRep;
		}
	iCFISPrivateCentRep = NULL;
	
    //delete service table req
    if( iServiceTableReq )
    	{
    	delete iServiceTableReq;
    	iServiceTableReq = NULL;
    	}
    	
    iSubscriberIdRequests.ResetAndDestroy();
    iCustomerServiceProfileRequests.ResetAndDestroy();
    iGetAlsLineRequests.ResetAndDestroy();
    iServiceProviderNameRequests.ResetAndDestroy();
 	
    //set pointters to NULL
    iMmPhoneFactory = NULL;
    iMmConferenceCall = NULL;
    iTsySatMessaging = NULL;
    iMmUssdTsy = NULL;
    iMmONStoreTsy = NULL;
    iMmENStoreTsy = NULL;
    iMmPhoneBookStoreTsy = NULL;
    iMmCustomTsy = NULL;
    iMmSmsTsy = NULL;
    iMmPacketServiceTsy = NULL;
    iRetPhoneIdentity = NULL;
    iRetCaps = NULL;
    iRetModemDetection = NULL;
    iRetNotifyFdnStatus = NULL;
    iRetNotifyALSLine = NULL;
    iICCCaps = NULL;
    iRetBatteryInfo = NULL;
    iRetPhoneStoreInfo = NULL;
    iRetTableData = NULL;
    iMmBroadcastTsy = NULL;
    iRetFdnSetting = NULL;
    iMmWaitingDataCall = NULL;
    iRetGetFdnStatus = NULL;
    iRetAPNEntries = NULL;
	iRetAPNname = NULL;
    iRetAclStatus = NULL;
    iSetAclStatus = NULL;
    iRetNotifyAclStatus = NULL;
    
    if ( iSystemStatePluginHandler )
        {
        iSystemStatePluginHandler->Close();
        delete iSystemStatePluginHandler;
        }
    iSystemStatePluginHandler = NULL;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DTOR_2, "TSY: CMmPhoneTsy::~CMmPhoneTsy DONE");
   }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::Init
// Initialisation method that is called from ETel Server
// Creates TRAP and calls InitL method to do the actual initialisation
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::Init()
    {
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyModemStatusReady
// Trapped version of Init. Sends mode independent initialisation
// requests to Phonet and then directs the request also to extension for
// mode dependent initialisation requests
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyModemStatusReady()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYMODEMSTATUSREADY_1, "TSY: CMmPhoneTsy::CompleteNotifyModemStatusReady");

    TRAP_IGNORE(
    //update the network registration status
        iMmPhoneExtInterface->GetNetworkRegistrationStatusL( );

    //Start SMS routing if is not activated
    if( iMmSmsTsy )
        {
        iMmSmsTsy->ActivateSmsRoutingL();
        }

    // Make the SIM status request here to get SIM initializing started
        iMmPhoneExtInterface->NotifySimStatusReadyL();
    );
    // set internal boolean to true
    iIsModemReady = ETrue;
     }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifySimStatusReadyL
// Initializes SIM dependent parts
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifySimStatusReadyL()
    {
    if ( !iBootState.iSIMReady )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYSIMSTATUSREADYL_1, "TSY: CMmPhoneTsy::CompleteNotifySimStatusReadyL - SIM is ready, start initialisations");

        iBootState.iSIMReady = ETrue;
        iBootState.iADNReady = ETrue;

        // Might leave, but we want to call also other
        // init methods here so trap possible leave.
        iMmPhoneExtInterface->InitSimL();

		// Cache Service Tables
		CacheServiceTablesL();
        // start Phonebook caching
        StartPBCachingL();

        // request ALS status
        iMessageManager->HandleRequestL( EMobilePhoneGetALSLine );
        // Request callforwarding number
        iMessageManager->HandleRequestL( EMmTsyGetCallForwardingNumberIPC );
        // Get call forwarding status
       	iMessageManager->HandleRequestL( 
       	    ECustomGetIccCallForwardingStatusIPC );
        // Get message waiting indicators
        iMessageManager->HandleRequestL( 
            EMobilePhoneGetIccMessageWaitingIndicators );
            
        TInt ret = iMessageManager->HandleRequestL(
            ECustomCheckAlsPpSupportIPC );
        
        if( KErrNone == ret )
            {
        	iAlsQueryInBoot = ETrue;
            }
        
        ret =  iMessageManager->HandleRequestL(
			EMobilePhoneGetCustomerServiceProfile );
        
        if( KErrNone == ret )
            {
        	iCspFileQueryInBoot = ETrue;
            }
        
        // If Custom TSY is open, check whether we need to read
        // VIAG homezone parameters. 
        if ( iMmCustomTsy )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYSIMSTATUSREADYL_2, "TSY: CMmPhoneTsy::CompleteNotifySimStatusReadyL - CustomTSY ready");
            // Get pointer to SIM Custom TSY
            CMmSIMTsy* simCustomTsy =
                ( ( CMmSIMTsy* ) iMmCustomTsy->GetSIMTsyPtr() );
            
            if ( simCustomTsy )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYSIMSTATUSREADYL_3, "TSY: CMmPhoneTsy::CompleteNotifySimStatusReadyL - CheckViagHomeZoneParamsL()");
                simCustomTsy->CheckViagHomezoneParamsL();
                iViagHomeZoneParamsChecked = ETrue;
                }
            }
        // If not able to read here it's tried again in customtsy    
        else
        	{
        	iViagHomeZoneParamsChecked = EFalse;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYSIMSTATUSREADYL_4, "TSY: CMmPhoneTsy::CompleteNotifySimStatusReadyL - CustomTSY NOT ready");
        	}
        } 
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifySecurityReady
// Completes NotifySecurityReady request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifySecurityReady()
    {
    iBootState.iSecReady = ETrue;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::StartPBCachingL
// Initializes Phonebook caching once the SIM is ready
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::StartPBCachingL()
    {
    // If ADN or FDN phone book storage is created, then start to fetch
    // entries from SIM into cache.
    for( TInt i = 0; i < iPBList->GetNumberOfObjects(); i++ )
        {
        // Get pbStore object
        CMmPhoneBookStoreTsy* pbStore = iPBList->GetMmPBByIndex( i );
        // Notify the store that the SIM is ready
        pbStore->SimIsReady();
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyIndicatorChange
// Register to listen to indicator changes
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyIndicatorChange(
    TUint32* aIndicatorState )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYINDICATORCHANGE_1, "TSY: CMmPhoneTsy::NotifyIndicatorChange");

    // Update indicator state, we don't keep track of it
    // unless notify is on. So we need to update starting
    // value first..
    iIndicatorState = GetIndicatorState();
    iRetIndicatorState = aIndicatorState;
    iReqHandleType = EMultimodePhoneNotifyIndicatorChange;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyIndicatorChange
// Description: Complete NotifyIndicatorChange if indicator state has changed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyIndicatorChange()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYINDICATORCHANGE_1, "TSY: CMmPhoneTsy::CompleteNotifyIndicatorChange");

    // first check if this notify is actually requested..
    if (NULL != iRetIndicatorState)
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYINDICATORCHANGE_2, "TSY: CMmPhoneTsy::CompleteNotifyIndicatorChange -- notify is on");
        TUint32 newIndicatorState = GetIndicatorState();
        if (newIndicatorState != iIndicatorState)
            {
            iIndicatorState = newIndicatorState;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYINDICATORCHANGE_3, "TSY: CMmPhoneTsy::CompleteNotifyIndicatorChange -- changed to 0x%08x", iIndicatorState);

            //reset req handle. Returns the deleted req handle
            TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                                EMultimodePhoneNotifyIndicatorChange );

            if( EMultimodePhoneReqHandleUnknown != reqHandle)
                {
                *iRetIndicatorState = iIndicatorState;
                iRetIndicatorState = NULL;
                // complete the client request
                ReqCompleted( reqHandle, KErrNone );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyIndicatorChangeCancel
// Cancels an outstanding NotifyIndicatorChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyIndicatorChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYINDICATORCHANGECANCEL_1, "TSY: CMmPhoneTsy::NotifyIndicatorChangeCancel");

    iRetIndicatorState = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyIndicatorChange );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NosBootState
// Returns current DOS boot state
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneTsy::TNosBootState* CMmPhoneTsy::NosBootState()
    {
    return &iBootState;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::PhoneBookState
// Returns current Phonebook state
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CStorageInfoData* CMmPhoneTsy::PhoneBookState()
    {
    return iPhoneBookState;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::OpenNewObjectByNameL
// Creates new object and returns a pointer to it. The ETel
// Server calls this method after some client has requested to open object
// (e.g. line, sms, sat etc.) from ETel. TSY classes must not use this method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmPhoneTsy::OpenNewObjectByNameL(
    const TDesC& aName )
    {
//Warnings caused by configurational constants in if case. These are not
//really errors but act as flags. This warning is purely informational as can
//be seen from the explanation of Visual C++ help:
//Compiler Warning (level 4) C4127
//conditional expression is constant. The controlling expression of an
//if statement or while loop evaluated to a constant. As a result, the
//code in the body of the if statement or while loop either always
//executes or never executes. This warning is informational.
#pragma warning(disable : 4127)

    RMobilePhone::TMobileService mode( RMobilePhone::EServiceUnspecified );
    TInt addLineSucceeded( KErrNone );
    TInt addPBSucceeded( KErrNone );
    TInt trapError( 0 );
    CTelObject* mmObject = NULL;
    CMmLineTsy* mmLine = NULL;

    //Voice line
    if ( KVoice1LineSupported &&
            aName.CompareF( KMmTsyVoice1LineName ) == KErrNone )
        {
        mode = RMobilePhone::EVoiceService;
        TRAP( trapError, mmLine = CMmVoiceLineTsy::NewL( this, mode, aName, 
            iMessageManager, iTelephonyAudioControl ); );
        if( KErrNone == trapError)
            {
            addLineSucceeded = iLineList->AddLineObject( mmLine, 
                mmLine->LineName() );
            mmObject = REINTERPRET_CAST( CTelObject*, mmLine );

            if ( 0 == addLineSucceeded )
                {
                if(!(iPhoneCaps.iFlags & RPhone::KCapsVoice))//complete capability change only if required
                    {
                    //set voice flag to capabilities, mask 'Unknown' caps bit away
                    iPhoneCaps.iFlags = RPhone::KCapsVoice + ( 
                            iPhoneCaps.iFlags & 0xFFFFFFFE );
                    CompleteNotifyCapsChange();
                    }
                }
            }
        }
    //Data line
    else if ( KDataLineSupported &&
        aName.CompareF( KMmTsyDataLineName ) == KErrNone )
        {
        mode = RMobilePhone::ECircuitDataService;
        TRAP( trapError, mmLine = CMmDataLineTsy::NewL( this, mode, aName, 
            iMessageManager ); );
        if( KErrNone == trapError)
            {
            addLineSucceeded = iLineList->AddLineObject( mmLine, 
                mmLine->LineName() );
            mmObject = REINTERPRET_CAST( CTelObject*, mmLine );

            if ( 0 == addLineSucceeded )
                {
                //set data flags to capabilities, mask 'Unknown' caps bit away
                iPhoneCaps.iFlags = RPhone::KCapsData +
                            RPhone::KCapsEventModemDetection +
                            RPhone::KCapsStealCommPort + ( 
                                iPhoneCaps.iFlags & 0xFFFFFFFE );
                CompleteNotifyCapsChange();
                }
           }
        }
    //Fax line
    else if ( KFaxLineSupported &&
        aName.CompareF( KMmTsyFaxLineName ) == KErrNone )
        {
        mode = RMobilePhone::EFaxService;
        TRAP( trapError, mmLine = CMmFaxLineTsy::NewL( this, mode, aName, 
            iMessageManager ); );
        if( KErrNone == trapError)
            {
            addLineSucceeded = iLineList->AddLineObject( mmLine, 
                mmLine->LineName() );
            mmObject = REINTERPRET_CAST( CTelObject*, mmLine );
            }
        }
    //Voice line 2
    else if ( KVoice2LineSupported &&
        aName.CompareF( KMmTsyVoice2LineName ) == KErrNone )
        {
        mode = RMobilePhone::EAuxVoiceService;
        TRAP( trapError, mmLine = CMmVoiceLineTsy::NewL( this, mode, aName, 
            iMessageManager, iTelephonyAudioControl ); );
        if( KErrNone == trapError)
            {
            addLineSucceeded = iLineList->AddLineObject( mmLine, mmLine->
                LineName() );
            mmObject = REINTERPRET_CAST( CTelObject*, mmLine );

            if ( 0 == addLineSucceeded )
                {     
                if(!(iPhoneCaps.iFlags & RPhone::KCapsVoice))//complete capability change only if required
                    {
                    //set voice flag to capabilities, mask 'Unknown' caps bit away
                    iPhoneCaps.iFlags = RPhone::KCapsVoice + ( 
                            iPhoneCaps.iFlags & 0xFFFFFFFE );
                    CompleteNotifyCapsChange();
                    }
                }                               
            }
        }
    //Conference call
    else if ( KConferenceCallSupported &&
        aName.CompareF( KETelConferenceCall ) == KErrNone )
        {
        TRAP( trapError, iMmConferenceCall = 
            CMmConferenceCallTsy::NewL( this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmConferenceCall );
            }
        }
    //SMS
    else if ( KSmsMessagingSupported &&
        aName.CompareF( KETelSmsMessaging ) == KErrNone )
        {
        TRAP( trapError, iMmSmsTsy = CMmSmsTsy::NewL( this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmSmsTsy );
            }
        }
    //Broadcast messaging
    else if ( KBroadcastMessagingSupported &&
        aName.CompareF( KETelBroadcastMessaging ) == KErrNone )
        {
        TRAP( trapError, iMmBroadcastTsy = CMmBroadcastTsy::NewL(
            this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmBroadcastTsy );
            }
        }
    //ON store
    else if ( KONStoreSupported &&
        aName.CompareF( KETelOwnNumberStore ) == KErrNone )
        {
        TRAP( trapError, iMmONStoreTsy = CMmONStoreTsy::NewL( this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmONStoreTsy );
            }
        }
    //EN store
    else if ( KENStoreSupported &&
        aName.CompareF( KETelEmergencyNumberStore ) == KErrNone )
        {
        TRAP( trapError, iMmENStoreTsy = CMmENStoreTsy::NewL( this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmENStoreTsy );
            }
        }
    //Phonebook
    else if ( KSimPhoneBookSupported &&
        aName.CompareF( KETelIccAdnPhoneBook ) == KErrNone ||
        aName.CompareF( KETelIccFdnPhoneBook ) == KErrNone ||
        aName.CompareF( KETelIccSdnPhoneBook ) == KErrNone ||
        aName.CompareF( KETelIccBdnPhoneBook ) == KErrNone ||
        aName.CompareF( KETelIccVoiceMailBox ) == KErrNone ||
        aName.CompareF( KETelIccMbdnPhoneBook ) == KErrNone )
        {
        TBool isPhonebookAlreadyCreated = EFalse;
        //Check that no already created object is tried to open multiple times
        for( TInt i = 0; i < iPBList->GetNumberOfObjects(); i++ )
            {
            CMmPhoneBookStoreTsy* pbStore = iPBList->GetMmPBByIndex( i );

            if ( 0 == pbStore->PhoneBookName()->CompareF( aName ) )
                {
                isPhonebookAlreadyCreated = ETrue;
                mmObject = REINTERPRET_CAST( CTelObject*, pbStore );
                }
            }

        // If not, a new instance can be safely created
        if ( !isPhonebookAlreadyCreated )
            {
            TRAP( trapError, iMmPhoneBookStoreTsy = 
                CMmPhoneBookStoreTsy::NewL( this, aName ); );

            if( KErrNone == trapError)
                {
                addPBSucceeded = iPBList->AddObject( iMmPhoneBookStoreTsy );
                mmObject = REINTERPRET_CAST( 
                    CTelObject*, iMmPhoneBookStoreTsy );
                }
            }
        }
    //SAT
    else if ( KSatToolkitSupported &&
        !aName.CompareF( KSatToolkit ) )
        {
        iTsySatMessaging = iMmPhoneFactory->NewSimAtk( aName );
        if( iTsySatMessaging )
            {
            //Only one application in the phone uses SAT
            mmObject = REINTERPRET_CAST( CTelObject*, iTsySatMessaging );
            }
        }
    //USSD
    else if ( KUssdMessagingSupported &&
        aName.CompareF( KETelUssdMessaging ) == KErrNone )
        {
        TRAP( trapError, iMmUssdTsy = CMmUssdTsy::NewL(
            this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmUssdTsy  );
            }
        }
    //Custom TSY
    else if ( KCustomAPISupported &&
        aName.CompareF( KCustomAPIName ) == KErrNone )
        {
        TRAP( trapError, iMmCustomTsy = CMmCustomTsy::NewL(
            this ); );
        if( KErrNone == trapError)
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmCustomTsy  );
            }
        }

    //Packet Data Session
    else if ( KGprsLineSupported && ( KErrNone ==
        aName.CompareF( KPacketName ) ) )
        {
        TRAP( trapError, iMmPacketServiceTsy =
            CMmPacketServiceTsy::NewL( this,
			REINTERPRET_CAST( CMmMessageManagerBase*, iMessageManager ),
			iLtsyFactory ) );
        if( KErrNone == trapError )
            {
            mmObject = REINTERPRET_CAST( CTelObject*, iMmPacketServiceTsy );
            }
        }

//Set warning level to normal
#pragma warning(default : 4127)

    if ( KErrNone != trapError || KErrNone != addLineSucceeded
        || KErrNone != addPBSucceeded )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_OPENNEWOBJECTBYNAMEL_1, "TSY: CMmPhoneTsy::OpenNewObjectByNameL - Failed: %{TSymbianErrorCodes}",trapError);

        if ( mmLine )
            {
            mmLine->Close();
            mmLine = NULL;
            }
        if ( KErrNone == trapError )
            {
            User::Leave( KErrNoMemory );
            }
        else
            {
            User::Leave( trapError );
            }
        }
    if ( NULL != mmObject )
        {
        //if object has been opened, return it.
        return mmObject;
        }

    //Otherwise we can leave...
    User::Leave( KErrNotSupported );
    return NULL;

    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::OpenNewObjectL
// Creates new Line object and returns a pointer to it. All
// objects opened from the phone are opened by name, hence
// this method is not supported.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmPhoneTsy::OpenNewObjectL(
        TDes& )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ExtFunc
// TRAP's all CMmPhoneTsy related MM API requests in cases that
// they fail. This method functions only as a centralized TRAP for the
// DoExtFuncL method that does the actual mapping of IPC number to
// TSY method call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::ExtFunc(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_EXTFUNC_1, "TSY: CMmPhoneTsy::ExtFunc IPC:%{TIPCNamesList} Handle:%d", aIpc, aTsyReqHandle);

    TInt ret = KErrNone;
    TInt trapError = KErrNone;

    // Ensure the ReqHandleType is unset.
    // This will detect cases where this method indirectly calls itself
    // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
    // Such cases are not supported because iReqHandleType is in the context of this class instance,
    // not this request, and we don't want the values set by the inner request and the outer request
    // interfering with each other.
    __ASSERT_DEBUG(iReqHandleType==EMultimodePhoneReqHandleUnknown, User::Invariant());

    //before processing further the request, check if offline mode status
    //is enabled and if the given request can be perfomed in that case.
    if ( ERfsStateInfoInactive == iStateInfo &&
        !IsRequestPossibleInOffline( aIpc ) )
        {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_EXTFUNC_2, "TSY: Offline mode ON, request is not allowed: %{TIPCNamesList}", aIpc );
        ret = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral,
                KErrGsmOfflineOpNotAllowed );

        //Complete the request with appropiate error
        ReqCompleted ( aTsyReqHandle, ret );
        }
    else
        {

        TRAP( trapError, ret = DoExtFuncL( aTsyReqHandle, aIpc, aPackage ); );

        if ( trapError != KErrNone )
            {
			//reset request handle to indicate the request is no longer ongoing
            iTsyReqHandleStore->FindAndResetTsyReqHandle( aTsyReqHandle );
            ReqCompleted( aTsyReqHandle, trapError );
            }
        else if ( ret != KErrNone )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else if ( EMultimodePhoneReqHandleUnknown != iReqHandleType )
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( iReqHandleType, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                iReqHandleType, aTsyReqHandle );
#endif //REQHANDLE_TIMER
            }
        // We've finished with this value now. Clear it so it doesn't leak
        //  up to any other instances of this method down the call stack
        iReqHandleType = EMultimodePhoneReqHandleUnknown;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::DoExtFuncL
// ExtFunc is called by the server when it has a "extended",
// i.e. non-core ETel request for the TSY. To process a request handle,
// request type and request data are passed to the TSY.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::DoExtFuncL(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aIpc,
    const TDataPackage& aPackage )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DOEXTFUNCL_1, "TSY: CMmPhoneTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret = KErrNone;

    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();


    switch ( aIpc )
        {
      // Mobile Network
        case EMobilePhoneGetNetworkCaps:
        case EMobilePhoneGetNetworkRegistrationStatus:
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
        case EMobilePhoneGetCurrentMode:
        case EMobilePhoneNotifyModeChange:
        case EMobilePhoneGetCurrentNetwork:
        case EMobilePhoneGetNetworkName:
        case EMobilePhoneGetCurrentNetworkNoLocation:
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
        case EMobilePhoneNotifyCurrentNetworkChange:
        case EMobilePhoneGetHomeNetwork:
        case EMobilePhoneGetDetectedNetworksV2Phase1:
        case EMobilePhoneGetDetectedNetworksV2Phase2:
        case EMobilePhoneGetNetworkSelectionSetting:
        case EMobilePhoneSetNetworkSelectionSetting:
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
        case EMobilePhoneSelectNetwork:
        case EMobilePhoneGetNITZInfo:
        case EMobilePhoneNotifyNITZInfoChange:
        case EMobilePhoneGetSignalCaps:
        case EMobilePhoneGetSignalStrength:
        case EMobilePhoneNotifySignalStrengthChange:
        case EMobilePhoneGetNetworkSecurityLevel:
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        case EMobilePhoneGetCipheringIndicatorStatus:
        case EMobilePhoneAuthorizationInfoPhase1:
        case EMobilePhoneAuthorizationInfoPhase2:
        case EMobilePhoneGetCellInfo:
        case EMobilePhoneNotifyCellInfoChange:
        case EMobilePhoneGetCurrentActiveUSimApplication:                
            //direct this request to the net specific DoExtFuncL
            ret = iMmNetTsy->DoExtFuncL ( aTsyReqHandle, aIpc, aPackage );
            break;
        // Security and Phone Locks
        case EMobilePhoneGetSecurityCaps:
        case EMobilePhoneNotifySecurityCapsChange:
        case EMobilePhoneGetLockInfo:
        case EMobilePhoneNotifyLockInfoChange:
        case EMobilePhoneSetLockSetting:
        case EMobilePhoneChangeSecurityCode:
        case EMobilePhoneNotifySecurityEvent:
        case EMobilePhoneVerifySecurityCode:
        case EMobilePhoneAbortSecurityCode:
        case EMobilePhoneGetSecurityCodeInfo:
        case EMobilePhoneNotifySecurityCodeInfoChange:
            //direct this request to the security specific DoExtFuncL
            ret = iMmSecurityTsy->DoExtFuncL ( aTsyReqHandle, aIpc, aPackage );
            break;
      // Service Provider name
        case EMobilePhoneGetServiceProviderName:
            ret = GetServiceProviderNameL( aTsyReqHandle,
                aPackage.Des1n() );
            break;

      // Access to ICC (Integrated Circuit Card)
        // Get ICC Access Capabilities
        case EMobilePhoneGetIccAccessCaps:
            ret = GetIccAccessCaps( aTsyReqHandle,
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
        // Notify Change of ICC Access Caps
        case EMobilePhoneNotifyIccAccessCapsChange:
            ret = NotifyIccAccessCapsChange(
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;

      // Customer service profile
        // Get Customer Service Profile
        case EMobilePhoneGetCustomerServiceProfile:
            ret = GetCustomerServiceProfileL( aTsyReqHandle,
                aPackage.Des1n() );
            break;

      // SIM/USIM/CDMA service table
        // Get Service Table
        case EMobilePhoneGetServiceTable:
            ret = GetServiceTableCheckParametersL( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneServiceTable*,
                dataPtr ),
                aPackage.Des2n() );
            break;

      // Power Status of Phone
        // Get Battery Caps
        case EMobilePhoneGetBatteryCaps:
            ret = GetBatteryCaps( aTsyReqHandle,
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
        // Get Current Battery Information
        case EMobilePhoneGetBatteryInfo:
            ret = GetBatteryInfoL( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneBatteryInfoV1*,
                dataPtr ) );
            break;
        // Notify Change of Battery Information
        case EMobilePhoneNotifyBatteryInfoChange:
            ret = NotifyBatteryInfoChange(
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneBatteryInfoV1*,
                dataPtr ) );
            break;
      //Identity of Phone and Subscriber
        // Get Identity Caps
        case EMobilePhoneGetIdentityCaps:
            ret = GetIdentityCaps( aTsyReqHandle,
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
        // Get Phone Id
        case EMobilePhoneGetPhoneId:
            ret = GetPhoneIdL( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneIdentityV1*,
                dataPtr ) );
            break;
        // Get Subscriber Id
        case EMobilePhoneGetSubscriberId:
            ret = GetSubscriberIdL( aTsyReqHandle,
                REINTERPRET_CAST( RMobilePhone::TMobilePhoneSubscriberId*,
                dataPtr ) );
            break;
		//Get the capabilities of the phone indicators
		case EMobilePhoneGetIndicatorCaps:
			ret = GetIndicatorCaps( aTsyReqHandle,
				REINTERPRET_CAST( TUint32*,
				dataPtr ),
				REINTERPRET_CAST( TUint32*,
				dataPtr2 )
				 );
			break;
		//Get Phone Indicators current state
		case EMobilePhoneGetIndicator:
			ret = GetIndicators( aTsyReqHandle,
				REINTERPRET_CAST( TUint32*,
				dataPtr ) );
			break;
		// All calls termination
        case EMobilePhoneTerminateAllCalls:
            ret = TerminateAllCallsL( aTsyReqHandle );
            break;			
        // Active calls termination
        case EMobilePhoneTerminateActiveCalls:
            ret = TerminateActiveCallsL( aTsyReqHandle );
            break;          
      // DTMF
		// forward request to the DTMF-specific Tsy
        case EMobilePhoneGetDTMFCaps:
        case EMobilePhoneNotifyDTMFCapsChange:
        case EMobilePhoneSendDTMFTones:
        case EMobilePhoneStartDTMFTone:
        case EMobilePhoneStopDTMFTone:
        case EMobilePhoneNotifyStopInDTMFString:
        case EMobilePhoneContinueDTMFStringSending:
		case EMobilePhoneNotifyDTMFEvent:
		case EMobilePhoneReadDTMFTones:
			ret = iMmDtmfTsy->DoExtFuncL( aTsyReqHandle, aIpc, aPackage );
            break;

      // Alternate Line Service
        // Get ALS Line
        case EMobilePhoneGetALSLine:
            ret = GetALSLineL( aTsyReqHandle,
                REINTERPRET_CAST( 
                    RMobilePhone::TMobilePhoneALSLine*, dataPtr ) );
            break;
        // Set ALS Line
        case EMobilePhoneSetALSLine:
            ret = SetALSLineL( aTsyReqHandle,
                REINTERPRET_CAST( 
                    RMobilePhone::TMobilePhoneALSLine*, dataPtr ) );
            break;
        // Notify Change of ALS Line Selection
        case EMobilePhoneNotifyALSLineChange:
            ret = NotifyALSLineChange(
                REINTERPRET_CAST( 
                    RMobilePhone::TMobilePhoneALSLine*, dataPtr ) );
            break;

      // Fixed Dialling Numbers
        // Get Fixed Dialling Numbers Status
        case EMobilePhoneGetFdnStatus:
            ret = GetFdnStatusL( aTsyReqHandle,
                REINTERPRET_CAST( 
                    RMobilePhone::TMobilePhoneFdnStatus*, dataPtr ) );
            break;
        // Set Fixed Dialling Numbers Setting
        case EMobilePhoneSetFdnSetting:
            ret = SetFdnSettingL( aTsyReqHandle, aPackage );
            break;
        // Notify Fixed Dialling Numbers Status Change
        case EMobilePhoneNotifyFdnStatusChange:
            ret = NotifyFdnStatusChange(
                REINTERPRET_CAST( 
                    RMobilePhone::TMobilePhoneFdnStatus*, dataPtr ) );
            break;
        case EMobilePhoneGetMailboxNumbers:
            ret = GetMailboxNumbersL( aPackage.Des1n() );
        	break;
        		
    // Supplementary services:
    // forward request to the Supplementary Services-specific Tsy
        // Call Service Capabilities
        case EMobilePhoneGetCallServiceCaps:
        case EMobilePhoneNotifyCallServiceCapsChange:
        // Call Forwarding
        case EMobilePhoneGetCallForwardingStatusPhase1:
        case EMobilePhoneGetCallForwardingStatusPhase2:
        case EMobilePhoneSetCallForwardingStatus:
        case EMobilePhoneNotifyCallForwardingStatusChange:
        case EMobilePhoneNotifyCallForwardingActive:
        // Call Barring
        case EMobilePhoneGetBarringStatusPhase1:
        case EMobilePhoneGetBarringStatusPhase2:
        case EMobilePhoneSetCallBarringStatus:
        case EMobilePhoneNotifyCallBarringStatusChange:
        // Supplementary Services password change
        case EMobilePhoneSetSSPassword:
        // Call Waiting
        case EMobilePhoneGetWaitingStatusPhase1:
        case EMobilePhoneGetWaitingStatusPhase2:
        case EMobilePhoneSetCallWaitingStatus:
        case EMobilePhoneNotifyCallWaitingStatusChange:
        // Identity Services
        case EMobilePhoneGetIdentityServiceStatus:
        // User's Access to Network Services
        case EMobilePhoneSendNetworkServiceRequest:
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
        // Message waiting Indicators
        case EMobilePhoneGetIccMessageWaitingIndicators:
        case EMobilePhoneSetIccMessageWaitingIndicators:
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
        // Notification for send networks service requests
        case EMobilePhoneNotifySendNetworkServiceRequest:
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
            ret = iMmSupplServTsy->DoExtFuncL( 
                aTsyReqHandle, aIpc, aPackage );
            break;
        
        // Mode Capabilities of TSY
        // Get Multimode Capabilities
        case EMobilePhoneGetMultimodeCaps:
            ret = GetMultimodeCaps( aTsyReqHandle,
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
        // Get Phone Store Information and Capabilities
        case EMobilePhoneGetPhoneStoreInfo:
            switch( aPackage.Type() )
                {
            	// Switch between alternative implementations of 
            	// GetPhoneStoreInfo
                case TDataPackage::EPackage1n2u:
                    ret = GetPhoneStoreInfoL( aPackage.Des1n(),
                        REINTERPRET_CAST( 
                        RMobilePhone::TMobileName* /*TName**/, 
                        aPackage.Des2u() ) );
                    break;
                case TDataPackage::EPackage1n2n:
                    ret = KErrNotSupported;
                    break;
                default:
                    ret = KErrNotSupported;
                    break;
                }
            break;
        case EMobilePhoneNotifyIndicatorChange:
            ret = NotifyIndicatorChange(
                REINTERPRET_CAST( TUint32*, dataPtr ) );
            break;
        case EMobilePhoneIMSAuthenticate:
		    ret = ImsAuthenticationL( aPackage.Des1n() );
        	break;

        // Access point control list
        case EMobilePhoneEnumerateAPNEntries:
            ret = EnumerateAPNEntriesL( aTsyReqHandle, REINTERPRET_CAST( 
                TUint32*, dataPtr ) );
            break;
        case EMobilePhoneGetAPNname:
            ret = GetAPNnameL( aTsyReqHandle, REINTERPRET_CAST( 
                TUint32*, dataPtr ), aPackage.Des2n() );
            break;
        case EMobilePhoneAppendAPNName:
        	ret = AppendAPNNameL( REINTERPRET_CAST(     
                RMobilePhone::TAPNEntryV3*, aPackage.Ptr1() ) );
        	break;
        case EMobilePhoneDeleteAPNName:
            ret = DeleteAPNNameL( aTsyReqHandle, REINTERPRET_CAST(     
                TUint32*,  aPackage.Ptr1() ) );
            break;
        case EMobilePhoneSetAPNControlListServiceStatus:
            ret = SetAPNControlListServiceStatusL( aTsyReqHandle, 
                REINTERPRET_CAST( RMobilePhone::TAPNControlListServiceStatus*, 
                aPackage.Ptr1() ) );
            break;
        case EMobilePhoneGetAPNControlListServiceStatus:
            ret = GetAPNControlListServiceStatusL( aTsyReqHandle, 
                REINTERPRET_CAST( RMobilePhone::TAPNControlListServiceStatus*, 
                aPackage.Ptr1() ) );
            break;
        case EMobilePhoneNotifyAPNListChanged:
            ret = NotifyAPNListChanged();
            break;
        case EMobilePhoneNotifyAPNControlListServiceStatusChange:
            ret = NotifyAPNControlListServiceStatusChange( REINTERPRET_CAST( 
                RMobilePhone::TAPNControlListServiceStatus*, 
                aPackage.Ptr1() ) );
            break;
    //NOT SUPPORTED FEATURES
        case EMobilePhoneGetAlternatingCallCaps:
        case EMobilePhoneNotifyAlternatingCallCapsChange:
        case EMobilePhoneGetAlternatingCallMode:
        case EMobilePhoneSetAlternatingCallMode:
        case EMobilePhoneNotifyAlternatingCallModeChange:
        case EMobilePhoneGetIncomingCallType:
        case EMobilePhoneSetIncomingCallType:
        case EMobilePhoneNotifyIncomingCallTypeChange:
        case EMobilePhoneGetMulticallParams:
        case EMobilePhoneSetMulticallParams:
        case EMobilePhoneNotifyMulticallParamsChange:
        case EMobilePhoneGetUUSSetting:
        case EMobilePhoneSetUUSSetting:
        case EMobilePhoneNotifyUUSSettingChange:
        case EMobilePhoneSetDefaultPrivacy:
        case EMobilePhoneGetDefaultPrivacy:
        case EMobilePhoneNotifyDefaultPrivacyChange:
        case EMobilePhoneGetCCBSStatus:
        case EMobilePhoneNotifyCCBSStatusChange:
        case EMobilePhoneNotifyMessageWaiting:
        case EMobilePhoneDeactivateCCBS:
        case EMobilePhoneNotifyCCBSRecall:
        case EMobilePhoneAcceptCCBSRecall:
        case EMobilePhoneRefuseCCBSRecall:
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DOEXTFUNCL_2, "TSY: CMmPhoneTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ReqModeL
// Returns request mode for given IPC. If the IPC number
// belongs to some of the core functions, CPhoneBase's ReqModeL function
// is called.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject::TReqMode CMmPhoneTsy::ReqModeL(
    const TInt aIpc )
    {
    CTelObject::TReqMode ret = 0;

    switch ( aIpc )
        {
        // All disabled
        // TSYs wishing to implement their own buffering algorithm will place
        // all requests in this category.
        // This category will also be used by the RecoverDataPort request that
        // must be passed to the TSY to indicate that it may use the
        // communications port after a loan.

        //Get methods that do not use DOS and return immediately. Flow control
        //not required.
        case EEtelPhoneGetInfo:
        case EEtelPhoneGetCaps:
        case EEtelPhoneGetStatus:
        case EEtelPhoneGetLineInfo:
        case EMobilePhoneGetDTMFCaps:
        case EMobilePhoneGetNetworkCaps:
        case EMobilePhoneGetNetworkRegistrationStatus:
        case EMobilePhoneGetCurrentMode:
        case EMobilePhoneGetCurrentNetwork:
        case EMobilePhoneGetNetworkName:
        case EMobilePhoneGetCurrentNetworkNoLocation:
        case EMobilePhoneGetHomeNetwork:
        case EMobilePhoneGetNetworkSelectionSetting:
        case EMobilePhoneGetNITZInfo:
        case EMobilePhoneGetMultimodeCaps:
        case EMobilePhoneGetBatteryCaps:
        case EMobilePhoneGetBatteryInfo:
        case EMobilePhoneGetSignalCaps:
        case EMobilePhoneGetSignalStrength:
        case EMobilePhoneGetIdentityCaps:
        case EMobilePhoneGetPhoneId:
        case EMobilePhoneGetSubscriberId:
        case EMobilePhoneGetAlternatingCallCaps:
        case EMobilePhoneGetAlternatingCallMode:
        case EMobilePhoneGetSecurityCaps:
        case EMobilePhoneGetIccAccessCaps:
        case EMobilePhoneGetDefaultPrivacy:
        case EMobilePhoneGetCallServiceCaps:
        case EMobilePhoneGetMulticallParams:
        case EMobilePhoneGetIncomingCallType:
        case EMobilePhoneGetUUSSetting:
        case EMobilePhoneGetNetworkSecurityLevel:
        case EMobilePhoneGetCellInfo:
        case EMobilePhoneGetCurrentActiveUSimApplication:                        	
        //Other methods that do not use DOS and return immediately. 
        //Flow control not required.
        case EEtelPhoneInitialise:
        case EEtelPhoneEnumerateLines:
        case EMobilePhoneGetDetectedNetworksV2Phase2:
        case EMobilePhoneGetCallForwardingStatusPhase2:
        case EMobilePhoneGetBarringStatusPhase2:
        case EMobilePhoneGetWaitingStatusPhase2:
        case EMobilePhoneGetCompletionRequestsPhase2:
        case EMobilePhoneContinueDTMFStringSending:

        //Methods that can propably take a long time and cannot therefore be 
        //flow controlled. Solution: All these methods must check req handle 
        //tablebefore handling the request. In case that the request table 
        //indicates that same method has been called and has not been 
        //completed, the method should return KErrServerBusy.
        case EMobilePhoneTerminateAllCalls:
        case EMobilePhoneTerminateActiveCalls:
        case EMobilePhoneGetFdnStatus:
        case EMobilePhoneGetCustomerServiceProfile:
        case EMobilePhoneGetDetectedNetworksV2Phase1:
        case EMobilePhoneSetLockSetting:
        case EMobilePhoneVerifySecurityCode:
        case EMobilePhoneAbortSecurityCode:
        case EMobilePhoneStartDTMFTone:
        case EMobilePhoneStopDTMFTone:
        case EMobilePhoneGetServiceProviderName:
        case EMobilePhoneGetServiceTable:
        case EMobilePhoneSetALSLine:
        case EMobilePhoneGetALSLine:
        case EMobilePhoneSendDTMFTones:
        case EMobilePhoneAuthorizationInfoPhase1:
        case EMobilePhoneAuthorizationInfoPhase2:
        case EMobilePhoneEnumerateAPNEntries:
        case EMobilePhoneGetAPNname:
        case EMobilePhoneGetAPNControlListServiceStatus:
        case EMobilePhoneSetAPNControlListServiceStatus:
        case EMobilePhoneDeleteAPNName:
        case EMobilePhoneAppendAPNName:
        case EMobilePhoneGetSecurityCodeInfo:

        //SS related methods.
        case EMobilePhoneGetCallForwardingStatusPhase1:
        case EMobilePhoneGetBarringStatusPhase1:
        case EMobilePhoneGetWaitingStatusPhase1:
        case EMobilePhoneSetCallBarringStatus:
        case EMobilePhoneSetCallWaitingStatus:
        case EMobilePhoneSetCallForwardingStatus:
        case EMobilePhoneSetSSPassword:
        case EMobilePhoneGetIdentityServiceStatus:
        case EMobilePhoneSendNetworkServiceRequest:
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
        case EMobilePhoneGetMailboxNumbers:
        case EMobilePhoneGetIccMessageWaitingIndicators:
        case EMobilePhoneSetIccMessageWaitingIndicators:
        case EMobilePhoneNotifyDTMFEvent:
            ret = 0;
            break;

        // KReqModeFlowControlObeyed
        // Commands that change the state of the phone, e.g. clearing the AoC
        // counter, are commands that the TSY should only deal with one at
        // a time.

        //Voice call related methods. Handle one at a time.


        //NET related methods. Handle one at a time.
        case EMobilePhoneSelectNetwork:
        case EMobilePhoneSetNetworkSelectionSetting:

        //SS related methods. Handle one at a time.
        case EMobilePhoneGetCompletionRequestsPhase1:
        case EMobilePhoneSetAlternatingCallMode:
        case EMobilePhoneSetFdnSetting:
        case EMobilePhoneSetDefaultPrivacy:

        //Security related methods. Handle one at a time.
        case EMobilePhoneChangeSecurityCode:
        case EMobilePhoneGetLockInfo:

        //Other cases
        case EMobilePhoneSetMulticallParams:
        case EMobilePhoneSetUUSSetting:

        case EMobilePhoneGetIndicatorCaps:
        case EMobilePhoneGetIndicator:
        case EMobilePhoneGetPhoneStoreInfo:
        case EMobilePhoneGetCipheringIndicatorStatus:
        case EMobilePhoneIMSAuthenticate:	
            ret=KReqModeFlowControlObeyed;
            break;

        //Do not use this! Request handle table has only one place for
        //each request handle
        //ReqModePostImmediately
        //Requests that notify a client about a change of state, where the 
        //TSY needs to distinguish between different clients.
            //ret=KReqModeRePostImmediately;


        //KReqModeMultipleCompletionEnabled
        //(a) commands that may take some time, but which the TSY can handle
        //more than one of concurrently, or
        //(b) notifications that the TSY does not wish to be re-posted
        //immediately, so the server does no buffering.
            //ret=KReqModeMultipleCompletionEnabled;


        //KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed
        //A command that may take some time and which the TSY can only deal
        //with one at a time.
        //NOTE!!! (TAN) 2.11.2000. All GetXXX methods that do lead to message
        //sending to DOS can be moved under "all disabled" case(the first one)
        //The methods will also function correctly if they are here but it is
        //possible that this reqMode requires more from the ETel Server.
        //ret=KReqModeMultipleCompletionEnabled | KReqModeFlowControlObeyed;
        //break;

        //KReqModeMultipleCompletionEnabled | ReqModePostImmediately
        //Requests that notify a client about a change of state.
        //Since these requests do not require the issuing of any modem
        //commands, they do not have to obey flow control.
        //The TSY never gets more than one of these outstanding per CTelObject
        case EEtelPhoneNotifyModemDetected:
        case EETelPhoneCapsChangeNotification:
        case EMobilePhoneNotifyDTMFCapsChange:
        case EMobilePhoneNotifyStopInDTMFString:
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
        case EMobilePhoneNotifyModeChange:
        case EMobilePhoneNotifyCurrentNetworkChange:
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
        case EMobilePhoneNotifyBatteryInfoChange:
        case EMobilePhoneNotifySignalStrengthChange:
        case EMobilePhoneNotifyCallForwardingStatusChange:
        case EMobilePhoneNotifyCallBarringStatusChange:
        case EMobilePhoneNotifyCallWaitingStatusChange:
        case EMobilePhoneNotifyAlternatingCallCapsChange:
        case EMobilePhoneNotifyAlternatingCallModeChange:
        case EMobilePhoneNotifyALSLineChange:
        case EMobilePhoneNotifySecurityCapsChange:
        case EMobilePhoneNotifyLockInfoChange:
        case EMobilePhoneNotifySecurityEvent:
        case EMobilePhoneNotifyMessageWaiting:
        case EMobilePhoneNotifyFdnStatusChange:
        case EMobilePhoneNotifyIccAccessCapsChange:
        case EMobilePhoneNotifyNITZInfoChange:
        case EMobilePhoneNotifyDefaultPrivacyChange:
        case EMobilePhoneNotifyCallServiceCapsChange:
        case EMobilePhoneNotifyCallForwardingActive:
        case EMobilePhoneNotifyMulticallParamsChange:
        case EMobilePhoneNotifyIncomingCallTypeChange:
        case EMobilePhoneNotifyUUSSettingChange:
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
        case EMobilePhoneNotifyIndicatorChange:
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        case EMobilePhoneNotifySecurityCodeInfoChange:
        case EMobilePhoneNotifyAPNListChanged:
        case EMobilePhoneNotifyAPNControlListServiceStatusChange:
        case EMobilePhoneNotifySendNetworkServiceRequest:
        case EMobilePhoneNotifyAllSendNetworkServiceRequest: 
        case EMobilePhoneNotifyCellInfoChange:
            ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
            break;
        // Cancel Requests
        //It is not necessary to include the Cancel methods in ReqModeL at all
        //The ETel server never calls ReqModeL with a Cancel IPC.

        //Other variations of return values are unusable

        // Default: Call CPhoneBase's ReqModeL.
        default:
            ret=CPhoneBase::ReqModeL(aIpc);
            break;

        }
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_REQMODEL_1, "TSY: CMmPhoneTsy::ReqModeL - IPC:%{TIPCNamesList}, Mode:%d", aIpc, ret);

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NumberOfSlotsL
// Returns number of slots to be used for given IPC
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NumberOfSlotsL(
    const TInt aIpc )
    {
    // All Phone related notifications must be added here!
    TInt numberOfSlots = 1;

    switch ( aIpc )
        {
        case EEtelPhoneNotifyModemDetected:
            numberOfSlots = KMmPhoneModemDetectedSlots;
            break;
        case EETelPhoneCapsChangeNotification:
            numberOfSlots = KMmPhoneCapsChangeNotificationSlots;
            break;
        case EMobilePhoneNotifyDTMFCapsChange:
            numberOfSlots = KMmPhoneDTMFCapsChangeSlots;
            break;
        case EMobilePhoneNotifyStopInDTMFString:
            numberOfSlots = KMmPhoneStopInDTMFStringSlots;
            break;
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
            numberOfSlots = KMmPhoneNetworkRegistrationStatusChangeSlots;
            break;
        case EMobilePhoneNotifyModeChange:
            numberOfSlots = KMmPhoneModeChangeSlots;
            break;
        case EMobilePhoneNotifyCurrentNetworkChange:
            numberOfSlots = KMmPhoneCurrentNetworkChangeSlots;
            break;
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
            numberOfSlots = KMmPhoneCurrentNetworkChangeSlots;
            break;
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
            numberOfSlots = KMmPhoneNetworkSelectionSettingChangeSlots;
            break;
        case EMobilePhoneNotifyBatteryInfoChange:
            numberOfSlots = KMmPhoneBatteryInfoChangeSlots;
            break;
        case EMobilePhoneNotifySignalStrengthChange:
            numberOfSlots = KMmPhoneSignalStrengthChangeSlots;
            break;
        case EMobilePhoneNotifyCallForwardingStatusChange:
            numberOfSlots = KMmPhoneCallForwardingStatusChangeSlots;
            break;
        case EMobilePhoneNotifyCallBarringStatusChange:
            numberOfSlots = KMmPhoneCallBarringStatusChangeSlots;
            break;
        case EMobilePhoneNotifyCallWaitingStatusChange:
            numberOfSlots = KMmPhoneCallWaitingStatusChangeSlots;
            break;
        case EMobilePhoneNotifyAlternatingCallCapsChange:
            numberOfSlots = KMmPhoneAlternatingCallCapsChangeSlots;
            break;
        case EMobilePhoneNotifyAlternatingCallModeChange:
            numberOfSlots = KMmPhoneAlternatingCallModeChangeSlots;
            break;
        case EMobilePhoneNotifyALSLineChange:
            numberOfSlots = KMmPhoneALSLineChangeSlots;
            break;
        case EMobilePhoneNotifySecurityCapsChange:
            numberOfSlots = KMmPhoneSecurityCapsChangeSlots;
            break;
        case EMobilePhoneNotifyLockInfoChange:
            numberOfSlots = KMmPhoneLockInfoChangeSlots;
            break;
        case EMobilePhoneNotifySecurityEvent:
            numberOfSlots = KMmPhoneSecurityEventSlots;
            break;
        case EMobilePhoneNotifyMessageWaiting:
            numberOfSlots = KMmPhoneMessageWaitingSlots;
            break;
        case EMobilePhoneNotifyFdnStatusChange:
            numberOfSlots = KMmPhoneFdnStatusChangeSlots;
            break;
        case EMobilePhoneNotifyIccAccessCapsChange:
            numberOfSlots = KMmPhoneIccAccessCapsChangeSlots;
            break;
        case EMobilePhoneNotifyNITZInfoChange:
            numberOfSlots = KMmPhoneNITZInfoChangeSlots;
            break;
        case EMobilePhoneNotifyDefaultPrivacyChange:
            numberOfSlots = KMmPhoneDefaultPrivacyChangeSlots;
            break;
        case EMobilePhoneNotifyCallServiceCapsChange:
            numberOfSlots = KMmPhoneCallServiceCapsChangeSlots;
            break;
        case EMobilePhoneNotifyCallForwardingActive:
            numberOfSlots = KMmPhoneCallForwardingActiveSlots;
            break;
        case EMobilePhoneNotifyMulticallParamsChange:
            numberOfSlots = KMmPhoneMulticallParamsChangeSlots;
            break;
        case EMobilePhoneNotifyIncomingCallTypeChange:
            numberOfSlots = KMmPhoneIncomingCallTypeChangeSlots;
            break;
        case EMobilePhoneNotifyUUSSettingChange:
            numberOfSlots = KMmPhoneUUSSettingChangeSlots;
            break;
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
            numberOfSlots = KMmPhoneIccMessageWaitingIndicatorsChangeSlots;
            break;
        case EMobilePhoneNotifySendNetworkServiceRequest:
        	numberOfSlots = KMmPhoneNotifySendNetworkServiceRequestSlots;
        	break;            
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
            numberOfSlots = KMmPhoneNotifyAllSendNetworkServiceRequestSlots;
            break;
        case EMobilePhoneNotifyIndicatorChange:
            numberOfSlots = KMmPhoneIndicatorChangeSlots;
            break;
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
            numberOfSlots = KMmPhoneNetworkSecurityLevelChangeSlots;
            break;
        case EMobilePhoneNotifySecurityCodeInfoChange: 
            numberOfSlots = KMmPhoneSecurityCodeInfoChangeSlots;
            break;
        case EMobilePhoneNotifyAPNListChanged: 
            numberOfSlots = KMmPhoneAPNListChangedSlots;
            break;
        case EMobilePhoneNotifyAPNControlListServiceStatusChange: 
            numberOfSlots = KMmPhoneAPNControlListServiceStatusChangeSlots;
            break;
        case EMobilePhoneNotifyCellInfoChange:
        	numberOfSlots = KMmPhoneCellInfoChangeSlots;
        	break;
        default:
            // Unknown or invalid Phone IPC
            User::Leave( KErrNotSupported );
            break;
        }

    return numberOfSlots;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CancelService
// Cancels specified service
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::CancelService(
    const TInt aIpc,
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CANCELSERVICE_1, "TSY: CMmPhoneTsy::CancelService - IPC:%{TIPCNamesList}, Handle:%d", aIpc, aTsyReqHandle);

    TInt ret ( KErrNotSupported );

    // When the clients close their sub-sessions (eg. by calling RLine::Close)
    // they may not have cancelled all their outstanding asynchronous requests
    // before closing.  It is up to the ETel server to clean up in this
    // situation, so the server will find the list of outstanding requests
    // related to that sub-session object and pass these outstanding IPC
    // request numbers, one at a time, to the CancelService method in the TSY.

    switch ( aIpc )
        {        	
		// forward DTMF cancellations
        case EMobilePhoneSendDTMFTones:
        case EMobilePhoneNotifyDTMFCapsChange:
        case EMobilePhoneNotifyStopInDTMFString:
        case EMobilePhoneNotifyDTMFEvent:
            ret =  iMmDtmfTsy->CancelService( aIpc, aTsyReqHandle );
            break;
        // forward NET cancellations
        case EMobilePhoneGetCipheringIndicatorStatus:
        case EMobilePhoneNotifySignalStrengthChange:
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
        case EMobilePhoneNotifyModeChange:
        case EMobilePhoneNotifyCurrentNetworkChange:
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
        case EMobilePhoneGetNetworkRegistrationStatus:
        case EMobilePhoneNotifyNITZInfoChange:
        case EMobilePhoneGetDetectedNetworksV2Phase1:
        case EMobilePhoneGetDetectedNetworksV2Phase2:
        case EMobilePhoneSelectNetwork:
        case EMobilePhoneAuthorizationInfoPhase1:
        case EMobilePhoneAuthorizationInfoPhase2:
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        case EMobilePhoneGetCellInfo:
        case EMobilePhoneNotifyCellInfoChange:
        case EMobilePhoneGetCurrentActiveUSimApplication:        
        	ret = iMmNetTsy->CancelService( aIpc, aTsyReqHandle );
            break;
		// forward SS cancellations
        case EMobilePhoneNotifyCallForwardingStatusChange:
        case EMobilePhoneNotifyCallForwardingActive:
        case EMobilePhoneGetCallForwardingStatusPhase1:
        case EMobilePhoneGetCallForwardingStatusPhase2:
        case EMobilePhoneSetCallForwardingStatus:
        case EMobilePhoneNotifyCallBarringStatusChange:
        case EMobilePhoneGetBarringStatusPhase1:
        case EMobilePhoneGetBarringStatusPhase2:
        case EMobilePhoneSetCallBarringStatus:
        case EMobilePhoneSetSSPassword:
        case EMobilePhoneNotifyCallWaitingStatusChange:
        case EMobilePhoneGetWaitingStatusPhase1:
        case EMobilePhoneGetWaitingStatusPhase2:
        case EMobilePhoneSetCallWaitingStatus:
        case EMobilePhoneGetIdentityServiceStatus:
        case EMobilePhoneNotifyCallServiceCapsChange:
        case EMobilePhoneSendNetworkServiceRequest:
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck:
        case EMobilePhoneGetIccMessageWaitingIndicators:        
        case EMobilePhoneSetIccMessageWaitingIndicators:        
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:  
        case EMobilePhoneNotifySendNetworkServiceRequest:         
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
            ret =  iMmSupplServTsy->CancelService( aIpc, aTsyReqHandle );
            break;
        //forward SECURITY cancellations
        case EMobilePhoneNotifySecurityCapsChange:
        case EMobilePhoneGetLockInfo:
        case EMobilePhoneSetLockSetting:
        case EMobilePhoneNotifyLockInfoChange:
        case EMobilePhoneNotifySecurityEvent:
        case EMobilePhoneGetSecurityCodeInfo:
        case EMobilePhoneNotifySecurityCodeInfoChange:
           ret = iMmSecurityTsy->CancelService ( aIpc, aTsyReqHandle );
            break;

        //TSY has started a request and it is not possible to then
        //cancel this request. The best thing for the TSY to do in this case 
        //is to proceed as though the Cancel never happened. The server's call 
        //to the TSY cancel function will return synchronously. The TSY then 
        //continues to wait for the original acknowledgement and when it 
        //receives it, the TSY will complete the original request.
        case EMobilePhoneChangeSecurityCode:
        case EMobilePhoneVerifySecurityCode:
        case EMobilePhoneSetALSLine:
        case EMobilePhoneSetFdnSetting:
        case EMobilePhoneNotifyDefaultPrivacyChange:
        case EMobilePhoneSetDefaultPrivacy:
        case EMobilePhoneTerminateAllCalls:
        case EMobilePhoneTerminateActiveCalls:
        case EMobilePhoneSetNetworkSelectionSetting:        
            ret = KErrNone;
            break;

        //Cancel methods that are not supported.
        case EMobilePhoneGetBatteryInfo:
        case EMobilePhoneGetSignalStrength:
        case EMobilePhoneGetCurrentNetwork:
        case EMobilePhoneGetNetworkName:
        case EMobilePhoneGetCurrentNetworkNoLocation:
        case EMobilePhoneGetHomeNetwork:
        case EMobilePhoneSetAlternatingCallMode:
        case EMobilePhoneNotifyAlternatingCallCapsChange:
        case EMobilePhoneNotifyAlternatingCallModeChange:
        //WCDMA features, hence not supported.
        case EMobilePhoneGetUUSSetting:
        case EMobilePhoneSetUUSSetting:
        case EMobilePhoneGetIncomingCallType:
        case EMobilePhoneSetIncomingCallType:
        case EMobilePhoneNotifyIncomingCallTypeChange:
        case EMobilePhoneGetMulticallParams:
        case EMobilePhoneSetMulticallParams:
        case EMobilePhoneNotifyMulticallParamsChange:
        case EMobilePhoneNotifyUUSSettingChange:
        //Unsupported CDMA features
        case EMobilePhoneNotifyMessageWaiting:
            break;

        //Notification Cancels, no special requirements...

        case EMobilePhoneGetPhoneId:
            ret = MobilePhoneGetPhoneIdCancel( aTsyReqHandle );
            break;

        case EMobilePhoneNotifyIndicatorChange:
            ret = NotifyIndicatorChangeCancel( aTsyReqHandle );
            break;

        case EMobilePhoneNotifyIccAccessCapsChange:
            ret = NotifyIccAccessCapsChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyBatteryInfoChange:
            ret = NotifyBatteryInfoChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyALSLineChange:
            ret = NotifyALSLineChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyFdnStatusChange:
            ret = NotifyFdnStatusChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetMailboxNumbers:
        	ret = GetMailboxNumbersCancel();
        	break; 
        //Everything is taken care in the method implementation.
        //Just direct the request to the method.
        case EMobilePhoneGetPhoneStoreInfo:
            ret = GetPhoneStoreInfoCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetALSLine:
            ret = GetAlsLineCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetSubscriberId:
            ret = GetSubscriberIdCancel( aTsyReqHandle );
            break;
        case EMobilePhoneIMSAuthenticate:
            ret = ImsAuthenticationCancel();
            break;
        case EMobilePhoneEnumerateAPNEntries:
            ret = EnumerateAPNEntriesCancel();
            break;
        case EMobilePhoneGetAPNname:
            ret = GetAPNnameCancel();
            break;
        case EMobilePhoneAppendAPNName:
            ret = AppendAPNNameCancel();
            break;
        case EMobilePhoneDeleteAPNName:
            ret = DeleteAPNNameCancel();
            break;
        case EMobilePhoneSetAPNControlListServiceStatus:
            ret = SetAPNControlListServiceStatusCancel();
            break;
        case EMobilePhoneGetAPNControlListServiceStatus:
            ret = GetAPNControlListServiceStatusCancel();
            break;
        case EMobilePhoneNotifyAPNListChanged:
            ret = NotifyAPNListChangedCancel();
            break;
        case EMobilePhoneNotifyAPNControlListServiceStatusChange:
            ret = NotifyAPNControlListServiceStatusChangeCancel();
            break;
        case EMobilePhoneGetCustomerServiceProfile:
            ret = GetCustomerServiceProfileCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetServiceProviderName:
            ret = GetServiceProviderNameCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetFdnStatus:
            ret = GetFdnStatusCancel( aTsyReqHandle );
            break;

        //When WINS flag is defined cancelling EMobilePhoneGetServiceTable
        //request is made possible because the request is handled much slower
        //Otherwise KErrNone is returned.
#ifdef __WINS__
        case EMobilePhoneGetServiceTable:
        	ret = GetServiceTableCancel( aTsyReqHandle );
        	break;
#else
        case EMobilePhoneGetServiceTable:
        	ret = KErrNone;
        	break;
#endif //__WINS__

        //Default case
        default:
            ret = CPhoneBase::CancelService( aIpc, aTsyReqHandle );
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetInfo
// This CORE API method this function returns core phone
// information in the given parameter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetInfo(
    const TTsyReqHandle aTsyReqHandle,
    RPhone::TPhoneInfo* aPhoneInfo )
    {
    aPhoneInfo->iDetection = iPhoneStatus.iModemDetected;
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyCapsChange
// This CORE API method is used to request caps change
// notifications from the TSY. The request handle and the pointer are stored
// to wait for caps change event.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyCapsChange(
    const TTsyReqHandle aTsyReqHandle,
    RPhone::TCaps* aCaps )
    {
    // Save request handle and ptr to aCaps. 
    iRetCaps = aCaps;
    iTsyReqHandleStore->SetTsyReqHandle( 
        EMultimodePhoneCapsChangeNotification, aTsyReqHandle );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyCapsChangeCancel
// This CORE API method cancels caps change notifications
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyCapsChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetCaps = NULL;

    // Reset request handle and pointer
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneCapsChangeNotification );
    // Complete the request and return KErrNone
    ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyCapsChange
// Completes caps change notification. The new capabilities
// are written to the stored pointer and the client is notified
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyCapsChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneCapsChangeNotification );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Fill the pointer with phone capabilities
        iRetCaps->iFlags = iPhoneCaps.iFlags;
        // Complete the client request
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ControlledInitialisation
// This CORE API method defines TSY special start. Not supported
// by MMTSY. Set to return KErrNone only because SMS stack is expecting it
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::ControlledInitialisation(
    const TTsyReqHandle aTsyReqHandle )
    {
    // Complete the request with KErrNone    
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ControlledInitialisationCancel
// Cancels controlled initialisation. Not supported by CommonTSY
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::ControlledInitialisationCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    ReqCompleted( aTsyReqHandle, KErrNotSupported );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyModemDetected
// This CORE API method requests notification when modem has been
// detected.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyModemDetected(
    const TTsyReqHandle aTsyReqHandle,
    RPhone::TModemDetection* aModemDetection )
    {
    iRetModemDetection = aModemDetection;
    iTsyReqHandleStore->SetTsyReqHandle( EMultimodePhoneNotifyModemDetected, 
        aTsyReqHandle );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyModemDetectedCancel
// This CORE API method cancels modem detected notifications.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyModemDetectedCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetModemDetection = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneNotifyModemDetected );
    ReqCompleted( aTsyReqHandle, KErrCancel );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyModemDetected
// Complete the client request when modem has been detected
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyModemDetected(
    RPhone::TStatus aModemStatus )
    {
    iPhoneStatus = aModemStatus;

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneNotifyModemDetected );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iRetModemDetection = iPhoneStatus.iModemDetected;
        iRetModemDetection = NULL;
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetCaps
// This CORE API method returns the phone capabilities in the
// aCaps parameter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetCaps(
    const TTsyReqHandle aTsyReqHandle,
    RPhone::TCaps* aCaps )
    {
    aCaps->iFlags = KPhoneCoreCaps;

    // Complete the request
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetStatus
// This CORE API method returns the phone status in the aStatus
// parameter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetStatus(
    const TTsyReqHandle aTsyReqHandle,
    RPhone::TStatus* aStatus )
    {
    // Fill status structure.
    aStatus->iMode = iPhoneStatus.iMode;
    aStatus->iModemDetected = iPhoneStatus.iModemDetected;

    // Complete the request
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::EnumerateLines
// This CORE API method enumerates the max. number of lines
// that the TSY supports
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::EnumerateLines(
    const TTsyReqHandle aTsyReqHandle,
    TInt* aCount )
    {
    // Write maximum number of lines to server pointer
    *aCount = KMmTsyNumOfSupportedLines;

    // Complete the request
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetLineInfo
// This CORE API method returns line information in the given
// parameter
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetLineInfo(
    const TTsyReqHandle aTsyReqHandle,
    TLineInfoIndex* aLineInfoIndex )
    {
    //set return value to KErrNotFound
    TInt ret ( KErrNotFound );

    //check that line is supported
    if ( ( aLineInfoIndex->iIndex >= 0 ) &&
         ( aLineInfoIndex->iIndex < KMmTsyNumOfSupportedLines ) )
        {
        //set return value to KErrNone
        ret = KErrNone;

        // Look line by given index
        CMmLineTsy* mmLine = REINTERPRET_CAST( CMmLineTsy*,
            iLineList->GetObjectByIndex( aLineInfoIndex->iIndex ) );

        // Check if line was found and fill aInfo
        if( mmLine )
            {
            aLineInfoIndex->iInfo = mmLine->LineInfo();
            }
        else
        //no line found
            {
            switch ( aLineInfoIndex->iIndex )
                {
                case CMmLineList::ELine0Voice1:
                    aLineInfoIndex->iInfo.iName = KMmTsyVoice1LineName;
                    aLineInfoIndex->iInfo.iLineCapsFlags = 0x00010004;
                    break;
                case CMmLineList::ELine1Voice2:
                    aLineInfoIndex->iInfo.iName = KMmTsyVoice2LineName;
                    aLineInfoIndex->iInfo.iLineCapsFlags = 0x00010008;
                    break;
                case CMmLineList::ELine2Data:
                    aLineInfoIndex->iInfo.iName = KMmTsyDataLineName;
                    aLineInfoIndex->iInfo.iLineCapsFlags = 0x10001;
                    break;
                case CMmLineList::ELine3Fax:
                    aLineInfoIndex->iInfo.iName = KMmTsyFaxLineName;
                    aLineInfoIndex->iInfo.iLineCapsFlags = 0x10002;
                    break;
                default :
                    break;
                }

            //Status is set to "Unknown"
            aLineInfoIndex->iInfo.iStatus = RCall::EStatusUnknown;
            }
        }

    ReqCompleted( aTsyReqHandle, ret );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::TerminateAllCallsL
// Terminates all calls simultaneously.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::TerminateAllCallsL(const TTsyReqHandle aTsyReqHandle)
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TERMINATEALLCALLSL_1, "TSY: CMmPhoneTsy::TerminateAllCalls");

	// Check if some other client has already requested this
	TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(EMultimodePhoneTerminateAllCalls);
	
	if (EMultimodePhoneReqHandleUnknown >= reqHandle )
		{ 
		// The request is not already in processing because of a previous request
		
        CMmCallTsy* activeCall 		= iCallList->GetMmCallByStatus(RMobileCall::EStatusConnected);
        CMmCallTsy* heldCall 		= iCallList->GetMmCallByStatus(RMobileCall::EStatusHold);
        CMmCallTsy* connectingCall 	= iCallList->GetMmCallByStatus(RMobileCall::EStatusDialling);
        if ( connectingCall == NULL )
            {
            connectingCall 			= iCallList->GetMmCallByStatus(RMobileCall::EStatusConnecting);
            }

        if ( activeCall || heldCall || connectingCall )
            {
            TInt callId(0);
            if (connectingCall != NULL)
            	{
            	callId = connectingCall->CallId();
            	}
            CMmDataPackage dataPackage;
            dataPackage.PackData(&callId);
            TInt err = iMessageManager->HandleRequestL(ECtsyPhoneTerminateAllCallsReq, &dataPackage);
            
            if ( err == KErrNone )
                {
#ifdef REQHANDLE_TIMER
                SetTypeOfResponse( EMultimodePhoneTerminateAllCalls, aTsyReqHandle );
#else
                iTsyReqHandleStore->SetTsyReqHandle( 
                        EMultimodePhoneTerminateAllCalls, aTsyReqHandle );
#endif //REQHANDLE_TIMER
                }
            else
            	{
                // request failed, complete with error value
                ReqCompleted( aTsyReqHandle, err );
                }
            }
        else
            {
            ReqCompleted( aTsyReqHandle, KErrNone );
            }   
		}
	else
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TERMINATEALLCALLSL_2, "TSY: CMmPhoneTsy::TerminateAllCalls - Already requested");
		ReqCompleted( aTsyReqHandle, KErrServerBusy );
		}
	return KErrNone;
	}

void CMmPhoneTsy::CompleteTerminateAllCallsReq(TInt aErrorCode)
	{
	// Reset req handle. Returns the deleted req handle
	 TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
	     EMultimodePhoneTerminateAllCalls );
	
	 if ( EMultimodePhoneReqHandleUnknown != reqHandle )
	     {
         ReqCompleted( reqHandle, aErrorCode );
	     }	 
	}


// ---------------------------------------------------------------------------
// CMmPhoneTsy::TerminateActiveCallsL
// Terminates all active calls simultaneously.
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::TerminateActiveCallsL(const TTsyReqHandle aTsyReqHandle)
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TERMINATEACTIVECALLSL_1, "TSY: CMmPhoneTsy::TerminateActiveCalls");

    // Check if some other client has already requested this
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(EMultimodePhoneTerminateActiveCalls);
    
    if (EMultimodePhoneReqHandleUnknown >= reqHandle )
        {
        // The request is not already in processing because of a previous request

        TInt err = iMessageManager->HandleRequestL(ECtsyPhoneTerminateActiveCallsReq);
        if ( err == KErrNone )
            {
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EMultimodePhoneTerminateActiveCalls, aTsyReqHandle );
#else
            iTsyReqHandleStore->SetTsyReqHandle( 
                    iReqHandleType, aTsyReqHandle );
#endif //REQHANDLE_TIMER
            }
        else
            {
            // request failed, complete with error value
            ReqCompleted( aTsyReqHandle, err );
            } 
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TERMINATEACTIVECALLSL_2, "TSY: CMmPhoneTsy::TerminatActiveCalls - Already requested"); 
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteTerminateActiveCallsReq
// Description: Complete TerminateActiveCallsRequest
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteTerminateActiveCallsReq(TInt aErrorCode)
    {
    // Reset req handle. Returns the deleted req handle
     TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
         EMultimodePhoneTerminateActiveCalls );
    
     if ( EMultimodePhoneReqHandleUnknown != reqHandle )
         {
         ReqCompleted( reqHandle, aErrorCode );
         }   
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::RegisterNotification
// This function is used by ETel Server to subscribe
// notifications from DOS. Currently does not really do anything but returns
// KErrNone to ETel server in case of known notification request type
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::RegisterNotification (
    const TInt aIpc )
    {
    TInt ret = KErrNone;

    switch (aIpc)
        {
        case EEtelPhoneNotifyModemDetected:
        case EETelPhoneCapsChangeNotification:
        case EMobilePhoneNotifyDTMFCapsChange:
        case EMobilePhoneNotifyStopInDTMFString:
        case EMobilePhoneNotifyDTMFEvent:
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
        case EMobilePhoneNotifyModeChange:
        case EMobilePhoneNotifyCurrentNetworkChange:
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
        case EMobilePhoneNotifyBatteryInfoChange:
        case EMobilePhoneNotifySignalStrengthChange:
        case EMobilePhoneNotifyCallForwardingStatusChange:
        case EMobilePhoneNotifyCallBarringStatusChange:
        case EMobilePhoneNotifyCallWaitingStatusChange:
        case EMobilePhoneNotifyAlternatingCallCapsChange:
        case EMobilePhoneNotifyAlternatingCallModeChange:
        case EMobilePhoneNotifyALSLineChange:
        case EMobilePhoneNotifySecurityCapsChange:
        case EMobilePhoneNotifyLockInfoChange:
        case EMobilePhoneNotifySecurityEvent:
        case EMobilePhoneNotifyMessageWaiting:
        case EMobilePhoneNotifyFdnStatusChange:
        case EMobilePhoneNotifyIccAccessCapsChange:
        case EMobilePhoneNotifyNITZInfoChange:
        case EMobilePhoneNotifyDefaultPrivacyChange:
        case EMobilePhoneNotifyCallServiceCapsChange:
        case EMobilePhoneNotifyCallForwardingActive:
        case EMobilePhoneNotifyMulticallParamsChange:
        case EMobilePhoneNotifyIncomingCallTypeChange:
        case EMobilePhoneNotifyUUSSettingChange:
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
        case EMobilePhoneNotifyIndicatorChange:
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        case EMobilePhoneNotifySecurityCodeInfoChange:
        case EMobilePhoneNotifyAPNListChanged:
        case EMobilePhoneNotifyAPNControlListServiceStatusChange:
        case EMobilePhoneNotifySendNetworkServiceRequest:
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
        case EMobilePhoneNotifyCellInfoChange:
            ret = KErrNone;
            break;
        default:
            // Not supported
            ret = KErrNotSupported;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::DeregisterNotification
// This function is used to unsubscribe given notification
// from DOS. Currently does not really do anything but returns
// KErrNone to ETel server in case of known notification request type
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::DeregisterNotification(
    const TInt aIpc )
    {
    TInt ret = KErrNone;

    switch (aIpc)
        {
        case EEtelPhoneNotifyModemDetected:
        case EETelPhoneCapsChangeNotification:
        case EMobilePhoneNotifyDTMFCapsChange:
        case EMobilePhoneNotifyStopInDTMFString:
        case EMobilePhoneNotifyDTMFEvent:		
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
        case EMobilePhoneNotifyModeChange:
        case EMobilePhoneNotifyCurrentNetworkChange:
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
        case EMobilePhoneNotifyBatteryInfoChange:
        case EMobilePhoneNotifySignalStrengthChange:
        case EMobilePhoneNotifyCallForwardingStatusChange:
        case EMobilePhoneNotifyCallBarringStatusChange:
        case EMobilePhoneNotifyCallWaitingStatusChange:
        case EMobilePhoneNotifyAlternatingCallCapsChange:
        case EMobilePhoneNotifyAlternatingCallModeChange:
        case EMobilePhoneNotifyALSLineChange:
        case EMobilePhoneNotifySecurityCapsChange:
        case EMobilePhoneNotifyLockInfoChange:
        case EMobilePhoneNotifySecurityEvent:
        case EMobilePhoneNotifyMessageWaiting:
        case EMobilePhoneNotifyFdnStatusChange:
        case EMobilePhoneNotifyIccAccessCapsChange:
        case EMobilePhoneNotifyNITZInfoChange:
        case EMobilePhoneNotifyDefaultPrivacyChange:
        case EMobilePhoneNotifyCallServiceCapsChange:
        case EMobilePhoneNotifyCallForwardingActive:
        case EMobilePhoneNotifyMulticallParamsChange:
        case EMobilePhoneNotifyIncomingCallTypeChange:
        case EMobilePhoneNotifyUUSSettingChange:
        case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange:
        case EMobilePhoneNotifyIndicatorChange:
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        case EMobilePhoneNotifySecurityCodeInfoChange:
        case EMobilePhoneNotifyAPNListChanged:
        case EMobilePhoneNotifyAPNControlListServiceStatusChange:
        case EMobilePhoneNotifySendNetworkServiceRequest:
        case EMobilePhoneNotifyAllSendNetworkServiceRequest:
        case EMobilePhoneNotifyCellInfoChange:
            ret = KErrNone;
            break;
        default:
            // Not supported
            ret = KErrNotSupported;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetActivePhoneExtension
// Returns a pointer to the active phone extension. The phone
// extension implements phone functionality that is specific
// for selected mode. There should always be some extension active
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneExtInterface* CMmPhoneTsy::GetActivePhoneExtension()
    {
    return iMmPhoneExtInterface;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CallList
// Returns a pointer to the call list. Call groups are used
// only inside the TSY module, not from the client or from ETel Server.
// When a new CMmCallTsy object is created in the TSY and added to the ETel
// Server's Phone Manager database, it should also be added to the call list.
// The call list has some same functionality as call groups had. Call groups
// were removed from new Mobile API
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallList* CMmPhoneTsy::CallList()
    {
    return iCallList;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetActivePhoneExtension
// Returns a pointer to the line list.
// When a new CMmLineTsy object is created in the TSY and added to the ETel
// Server's Phone Manager database, it should also be added to the line list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmLineList* CMmPhoneTsy::LineList()
    {
    return iLineList;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::PBList
// Returns a pointer to the phonebook list.
// When a new CMmPhoneBookStoreTsy object is created in the TSY, it should
// be also added to the phone book list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPBList* CMmPhoneTsy::PBList()
    {
    return iPBList;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetMultimodeCaps
// This method returns the multimode support capabilities of he phone
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetMultimodeCaps(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aCaps )
    {
    *aCaps = KMultimodeCaps;
    ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetPhoneStoreInfoL
// Request information about the phone store specified by aStoreName
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetPhoneStoreInfoL(
    TDes8* aInfo,
    const RMobilePhone::TMobileName* aStoreName )
    {
    TInt ret( KErrArgument );
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETPHONESTOREINFOL_1, "TSY: CMmPhoneTsy::GetPhoneStoreInfoL" );
    
    if(sizeof(RMobilePhoneStore::TMobilePhoneStoreInfoV1) <= aInfo->MaxLength())
      	{
        ret = KErrNone;
        
        RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg* info = 
            reinterpret_cast<RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*>( aInfo );
          
        RMobilePhoneStore::TMobilePhoneStoreInfoV1& storeInfo = ( *info )();
             
        if ( KETelExtMultimodeV1 != storeInfo.ExtensionId() )
    		{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETPHONESTOREINFOL_2, "TSY: Phonebook not supported. Only KETelExtMultimodeV1 types supported for RMobilePhone::GetPhoneStoreInfo" );
    		ret = KErrNotSupported;
    		}
    	else
    	    {
    		// Not supported features
    	    if ( 0x0 == aStoreName->Compare( KETelMeAdnPhoneBook )       || 
    	         0x0 == aStoreName->Compare( KETelMeDialledPhoneBook )   || 
    	         0x0 == aStoreName->Compare( KETelMeMissedPhoneBook )    || 
    	         0x0 == aStoreName->Compare( KETelMeReceivedPhoneBook )  || 
    	         0x0 == aStoreName->Compare( KETelCombinedAdnPhoneBook ) || 
    	         0x0 == aStoreName->Compare( KETelCombinedSmsStore )     || 
    	         0x0 == aStoreName->Compare( KETelMeSmsStore )              
    	       )
    	        {
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETPHONESTOREINFOL_3, "TSY: Phonebook name is not supported. " );
    	        ret = KErrNotSupported;
    	        }
    	    else
    	        {
    		    // packed parameter for DOS call: RMobilePhone::TMobileName
    			CMmDataPackage dataPackage;

    			RMobilePhone::TMobileName* nonConstStoreName =
    				const_cast<RMobilePhone::TMobileName*>( aStoreName );
    			dataPackage.PackData( nonConstStoreName );
    	        ret = iMessageManager->HandleRequestL( 
    	              EMobilePhoneGetPhoneStoreInfo, &dataPackage );
    	        }

    	    // Check the request sending status
    	    if ( KErrNone == ret )
    	        {
    	        // Save last req handle type
    	        iReqHandleType = EMultimodePhoneGetPhoneStoreInfo;
    			iRetPhoneStoreInfo = &storeInfo;
    	        }
    		}
      	}
    return ret;    
    }

    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetPhoneStoreInfo
// Returns the phone store info that is stored on the SIM to the client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetPhoneStoreInfo(
    RMobilePhoneStore::TMobilePhoneStoreInfoV1 aPhoneStoreInfo,
    TInt aErrorCode )
    {
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneGetPhoneStoreInfo );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
		if ( iRetPhoneStoreInfo )
			{
			// copy result to client
			*iRetPhoneStoreInfo = aPhoneStoreInfo;
			// reset pointer to client
			iRetPhoneStoreInfo = NULL;
			}
        // Complete the client request
        ReqCompleted( reqHandle, aErrorCode );
		}
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetPhoneStoreInfoCancel
// Cancels a previously placed GetPhoneStoreInfo request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetPhoneStoreInfoCancel(
    const TTsyReqHandle )
    {
    TTsyReqHandle reqHandle =
     	iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneGetPhoneStoreInfo );
   
    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // reset pointer to client
        iRetPhoneStoreInfo = NULL;
        //complete with cancel
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetCustomerServiceProfileL
// Request the whole Customer Service Profile file that is stored on the SIM.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetCustomerServiceProfileL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aCspFile )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCUSTOMERSERVICEPROFILEL_1, "TSY: CMmPhoneTsy::GetCustomerServiceProfileL" );
    
    TInt ret = KErrArgument;
    
    if ( sizeof( RMobilePhone::TMobilePhoneCspFileV1 ) == 
         aCspFile->MaxLength() )
        {
        RMobilePhone::TMobilePhoneCspFileV1Pckg* infoPckg =
        reinterpret_cast< RMobilePhone::TMobilePhoneCspFileV1Pckg* >
            ( aCspFile );
        RMobilePhone::TMobilePhoneCspFileV1* cspFile = &( ( *infoPckg )() );    
            
        TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
            EMultimodePhoneGetCustomerServiceProfile );

        if ( 0 < reqHandle )
            {
 OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCUSTOMERSERVICEPROFILEL_2, "TSY: CMmPhoneTsy::GetCustomerServiceProfileL one req already ongoing" );
            ret = KErrNone;
            //The request is already in processing because of previous request
            //Complete request with status value informing the client about
    		TCustomerServiceProfileIdRequest* req = 
                new (ELeave) TCustomerServiceProfileIdRequest();
    		req->iReqHandle = aTsyReqHandle;
    		req->iRetCspFile = cspFile;
    		iCustomerServiceProfileRequests.AppendL( req );
            }
        else
            {
    		// no packed parameters for DOS call
    		ret =  iMessageManager->HandleRequestL(
    			EMobilePhoneGetCustomerServiceProfile );

    		// Check the request sending status
            if ( KErrNone == ret )
                {
               	// DOS call successful, save request handle and return pointer
    			// for completion
                iReqHandleType = EMultimodePhoneGetCustomerServiceProfile;
    			// save request in queue for completion
    			TCustomerServiceProfileIdRequest* req = 
                    new (ELeave) TCustomerServiceProfileIdRequest();
    			req->iReqHandle = aTsyReqHandle;
    			req->iRetCspFile = cspFile;
    			iCustomerServiceProfileRequests.AppendL( req );
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetCustomerServiceProfile
// Completes a GetCustomerServiceProfile request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetCustomerServiceProfile(
    RMobilePhone::TMobilePhoneCspFileV1 aCspFile,
    TInt aErrorCode )
    {

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETCUSTOMERSERVICEPROFILE_1, "TSY: CMmPhoneTsy::CompleteGetCustomerServiceProfile" );
    
    // Check if the get was called internally during boot
    if( iCspFileQueryInBoot )
        {
        // Examine Csp file's ALS state
    	if ( RMobilePhone::KCspALS & aCspFile.iCphsTeleservices )
    	    {
    	    iCspFileAlsSet = ETrue;
    		SetAlsState( ETrue );
    		iCspFileAlsSet = EFalse;
    	    }
    	iCspFileQueryInBoot = EFalse;
        }
        
    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneGetCustomerServiceProfile );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
			
		TInt customerServiceProfileRequestsCount = 
            iCustomerServiceProfileRequests.Count();

		for ( TInt i=0; i < customerServiceProfileRequestsCount ; i++ )
			{
            TCustomerServiceProfileIdRequest* req = 
                iCustomerServiceProfileRequests[ i ];
            
            //set value in client side
            *(req->iRetCspFile) = aCspFile;
			//reset the return pointer
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETCUSTOMERSERVICEPROFILE_2, "TSY: CMmPhoneTsy::CompleteGetCustomerServiceProfile queue handling" );
            ReqCompleted( req->iReqHandle, aErrorCode );
			}
        // Complete the client request
		//Destroy Array 
 		iCustomerServiceProfileRequests.ResetAndDestroy();
        }
    }

//
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetCustomerServiceProfileCancel
// Cancels an outstanding GetCustomerServiceProfile request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetCustomerServiceProfileCancel( const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCUSTOMERSERVICEPROFILECANCEL_1, "TSY: CMmPhoneTsy::GetCustomerServiceProfileCancel aTsyReqHandle: %d", aTsyReqHandle);

    TInt customerServiceProfileRequestsCount = iCustomerServiceProfileRequests.Count();

    //reset the req handle if only one req remains
    if( 1 == customerServiceProfileRequestsCount )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCUSTOMERSERVICEPROFILECANCEL_2, "TSY: CMmPhoneTsy::GetCustomerServiceProfileCancel - one req");
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                EMultimodePhoneGetCustomerServiceProfile );       
        }

    // go through array if one or more requests remain and find the correct request handle to cancel
    for ( TInt i=0; i < customerServiceProfileRequestsCount ; i++ )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCUSTOMERSERVICEPROFILECANCEL_3, "TSY: CMmPhoneTsy::GetCustomerServiceProfileCancel - queued requests");

        TCustomerServiceProfileIdRequest* req =
        iCustomerServiceProfileRequests[ i ];

        // find correct handle for cancelling
        if( aTsyReqHandle == req->iReqHandle  )
            {
            ReqCompleted( req->iReqHandle, KErrCancel );
            // remove correct handle from queue
            delete iCustomerServiceProfileRequests[ i ];
            iCustomerServiceProfileRequests.Remove(i);
            break; 
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetBatteryCaps
// Returns the battery information capabilities of the phone
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetBatteryCaps(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aCaps )
    {
    //Get battery capability information
    *aCaps = KBatteryCaps;
    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetBatteryInfo
// Returns current battery information of the phone. If the current power
// status is unknown, it is requested from DOS.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetBatteryInfoL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETBATTERYINFOL_1, "TSY: CMmPhoneTsy::GetBatteryInfoL");

    // Battery info is requested when phone is booted up and updated when
    // EM server sends indications.
    *aInfo = iBatteryInfo;

    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyBatteryInfoChange
// Notifies the client when the battery info changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyBatteryInfoChange(
    RMobilePhone::TMobilePhoneBatteryInfoV1* aInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYBATTERYINFOCHANGE_1, "TSY: CMmPhoneTsy::NotifyBatteryInfoChange");

    iRetBatteryInfo = aInfo;
    iReqHandleType = EMultimodePhoneNotifyBatteryInfoChange;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyBatteryInfoChange
// This method completes an outstanding asynchronous NotifyALSLineChange
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyBatteryInfoChange(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYBATTERYINFOCHANGE_1, "TSY: CMmPhoneTsy::CompleteNotifyBatteryInfoChange");

    if ( KErrNone == aErrorCode )
        {
        // unpack data...
        RMobilePhone::TMobilePhoneBatteryInfoV1* batteryInfo = NULL;
        aDataPackage->UnPackData(&batteryInfo);
        iBatteryInfo = *batteryInfo;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYBATTERYINFOCHANGE_2, "TSY: CMmPhoneTsy::CompleteNotifyBatteryInfoChange, battery status: %d", (TInt)iBatteryInfo.iStatus);

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneNotifyBatteryInfoChange );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            *iRetBatteryInfo = iBatteryInfo;
            }
        iRetBatteryInfo = NULL;
        // complete the client request
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyBatteryInfoChange
// Cancels an outstanding NotifyBatteryInfoChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyBatteryInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYBATTERYINFOCHANGECANCEL_1, "TSY: CMmPhoneTsy::NotifyBatteryInfoChangeCancel");

    iRetBatteryInfo = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyBatteryInfoChange );

    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyBatteryInfo
// This method allows a client to be notified if battery info is changed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::NotifyBatteryInfo( TInt aErrorCode,
    CHWRMPower::TBatteryConsumptionData aBatteryData )
    {
    if ( iBatteryInfo.iChargeLevel != aBatteryData.iRemainingPercentageCapacity )
        {
        iBatteryInfo.iChargeLevel = aBatteryData.iRemainingPercentageCapacity;
        
        // reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePhoneNotifyBatteryInfoChange );

        if( EMultimodePhoneReqHandleUnknown != reqHandle )
            {
            if ( KErrNone == aErrorCode )
                {
                *iRetBatteryInfo = iBatteryInfo;
                }
            iRetBatteryInfo = NULL;
            // complete the client request
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }
        
// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyChargingStatus
// This method allows a client to be notified if battery charging status is
// changed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::NotifyChargingStatus( TInt aErrorCode,
    RMobilePhone::TMobilePhoneBatteryInfoV1 aBatteryInfo )
    {
    if ( iBatteryInfo.iStatus != aBatteryInfo.iStatus )
        {
        iBatteryInfo.iStatus = aBatteryInfo.iStatus;
        // reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePhoneNotifyBatteryInfoChange );

        if( EMultimodePhoneReqHandleUnknown != reqHandle )
            {
            if ( KErrNone == aErrorCode )
                {
                *iRetBatteryInfo = iBatteryInfo;
                }
            iRetBatteryInfo = NULL;
            // complete the client request
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetServiceProviderNameL
// Get service provider name
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetServiceProviderNameL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aName )
    {
    TInt ret = KErrNone;
    
    if ( iBootState.iSIMReady )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMEL_1, "TSY: CMmPhoneTsy::GetServiceProviderNameL -Sim ready");

        if ( sizeof( RMobilePhone::TMobilePhoneServiceProviderNameV2 ) >
             aName->MaxLength() )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMEL_2, "TSY: CMmPhoneTsy::GetServiceProviderNameL -Invalid parameter!");
            ret = KErrArgument;
            }
		else
		    {
    	    //Send a request to LTSY if request handle does not exist.
        	TTsyReqHandle reqhandle = iTsyReqHandleStore->GetTsyReqHandle( 
    			EMultimodePhoneGetServiceProviderName );
    		
    		if( EMultimodePhoneReqHandleUnknown == reqhandle )
    			{
    			ret = iMmPhoneExtInterface->GetServiceProviderNameL();  			
    			}
    	    }

        if ( KErrNone == ret )
            {
            // save reqhandle to queue
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMEL_3, "TSY: CMmPhoneTsy::GetServiceProviderNameL - Client to queue");
            RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg* nameV2Pckg =
                REINTERPRET_CAST( 
                    RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg*, 
                        aName );
                    
            RMobilePhone::TMobilePhoneServiceProviderNameV2* 
                serviceProviderName = &( ( *nameV2Pckg ) () );
            
            //Save request handle in queue
			TServiceProviderNameRequest* req = 
				new (ELeave) TServiceProviderNameRequest();
			req->iReqHandle = aTsyReqHandle;
			req->iServiceProviderName = serviceProviderName;
			iServiceProviderNameRequests.AppendL( req );
            //Save tsy req handle type
            iReqHandleType = EMultimodePhoneGetServiceProviderName;
            }
         }
     else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMEL_4, "TSY: CMmPhoneTsy::GetServiceProviderNameL - Sim not ready");
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetServiceProviderName
// This method completes an outstanding asynchronous
// GetServiceProviderName request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetServiceProviderName(
    CMmDataPackage* aDataPackage,
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICEPROVIDERNAME_1, "TSY: CMmPhoneTsy::CompleteGetServiceProviderName Error %{TSymbianErrorCodes}", aErrorCode );
   
    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneGetServiceProviderName );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {     
        RMobilePhone::TMobilePhoneServiceProviderNameV2* name = NULL;
        TInt requestCount = iServiceProviderNameRequests.Count();
        
        if ( KErrNone == aErrorCode )
            {
            aDataPackage->UnPackData( &name );

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICEPROVIDERNAME_2, "TSY: CMmPhoneTsy::CompleteGetServiceProviderName name: %S", name->iSPName );
		    for ( TInt i=0; i < requestCount ; i++ )
		        {
		        TServiceProviderNameRequest* req = 
		            iServiceProviderNameRequests[ i ];
		        *(req->iServiceProviderName) = *name;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICEPROVIDERNAME_3, "TSY: CMmPhoneTsy::CompleteGetServiceProviderName client %d", i );
		        ReqCompleted( req->iReqHandle, aErrorCode );
		        }
            }
        else
        	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICEPROVIDERNAME_4, "TSY: CMmPhoneTsy::CompleteGetServiceProviderName Error %{TSymbianErrorCodes}", aErrorCode );
        	for ( TInt i=0; i < requestCount ; i++ )
		        {
		        TServiceProviderNameRequest* req = 
		            iServiceProviderNameRequests[ i ];
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICEPROVIDERNAME_5, "TSY: CMmPhoneTsy::CompleteGetServiceProviderName Error queue: %d", i );
		        ReqCompleted( req->iReqHandle, aErrorCode );
		        }	
        	}
        //Delete array and it's content	
        iServiceProviderNameRequests.ResetAndDestroy();
        
        }
    }

//
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetServiceProviderNameCancel
// Cancels an outstanding GetServiceProviderName request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetServiceProviderNameCancel( const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMECANCEL_1, "TSY: CMmPhoneTsy::GetServiceProviderNameCancel aTsyReqHandle: %d", aTsyReqHandle);

    TInt requestCount = iServiceProviderNameRequests.Count();
    
    //reset the req handle if only one req remains
    if( 1 == requestCount )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMECANCEL_2, "TSY: CMmPhoneTsy::GetCustomerServiceProfileCancel - one req");
        TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                EMultimodePhoneGetServiceProviderName );       
        }

    // qo throw array if several requests and find correct reqhandle to cancel
    for ( TInt i = 0; i < requestCount; i++ )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICEPROVIDERNAMECANCEL_3, "TSY: CMmPhoneTsy::GetServiceProviderNameCancel - queued requests");

        TServiceProviderNameRequest* req =
        iServiceProviderNameRequests[i];

        // find correct handle for cancelling
        if( aTsyReqHandle == req->iReqHandle  )
            {
            ReqCompleted( req->iReqHandle, KErrCancel );
            // remove correct handle from queue
            delete iServiceProviderNameRequests[ i ];
            iServiceProviderNameRequests.Remove(i);
            break; 
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetIdentityCaps
// Gets the identity information capabilities of the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetIdentityCaps(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aCaps )
    {
    *aCaps = KIdentityCaps;

    ReqCompleted( aTsyReqHandle, KErrNone );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetPhoneIdL
// Gets the phone's manufacturer, model, revision and serialNumber. The serial
// number is requested from DOS if currently unknown.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetPhoneIdL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneIdentityV1* aId )
    {    
   
    // This is a get request. First check if another request is going through
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(EMultimodePhoneGetPhoneId);
     
    // if this is not the first request for Phone id information then add it to the
    // list of waiting
    if(reqHandle > 0)
    	{
        TGetPhoneIdRequest* req = new (ELeave) TGetPhoneIdRequest();
    	req->iReqHandle = aTsyReqHandle;
    	req->iRetPhoneId = aId;
    	CleanupStack::PushL(req);
    	iGetPhoneIdRequests.AppendL( req );
    	CleanupStack::Pop(req);
    	}
    else
    	{
    	DoGetPhoneIdL(aTsyReqHandle, aId);    	
    	}
   
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::DoGetPhoneId
// Gets the iPhoneIdentitys iSerialNumber and completes GetPhoneId.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::DoGetPhoneIdL(const TTsyReqHandle aTsyReqHandle,
		                        RMobilePhone::TMobilePhoneIdentityV1* aId)
	{
#ifndef USING_CTSY_DISPATCHER
//ifdefed out as when using the dispatcher all the information is retrieved from the LTSY
#ifdef SYMBIAN_BAFL_SYSUTIL
	
	CDeviceTypeInformation* deviceAtts = SysUtil::GetDeviceTypeInfoL();
	CleanupStack::PushL(deviceAtts);
	
	// For manufacturerName, modelName and revisionId we check if there is data 
	// available in the result given from SysUtil::GetDeviceTypeInfoL. Where data
	// is present the default value from initalization is overwritten.
	// The retrieved buffers can be 64 characters long but we 
	// have only got space in iPhoneIdentity for 50 due to the design of 
	// TMobilePhoneIdentityV1. Therfore we truncate if the buffer is to big to fit.	
			  		   
	TPtrC16 manufacturerName;
 	if(User::LeaveIfError(deviceAtts->GetManufacturerName(manufacturerName)) == KErrNone)
 		{
 		if(manufacturerName.Length() != 0)
 			{
			iPhoneIdentity.iManufacturer.Copy(manufacturerName.Left(RMobilePhone::KPhoneManufacturerIdSize));
 			}
 		}
    
 	TPtrC16 modelName;
    if(User::LeaveIfError(deviceAtts->GetModelName(modelName)) == KErrNone)
    	{
		if(modelName.Length() != 0)
			{
			iPhoneIdentity.iModel.Copy(modelName.Left(RMobilePhone::KPhoneModelIdSize));
			}
    	}
    
	TPtrC16 revisionId;
	if(User::LeaveIfError(deviceAtts->GetRevisionID(revisionId)) == KErrNone)
		{   
		if(revisionId.Length() != 0)
			{
			iPhoneIdentity.iRevision.Copy(revisionId.Left(RMobilePhone::KPhoneRevisionIdSize));
			}
		}
				
	// delete deviceAtts as its no longer required
	CleanupStack::PopAndDestroy();
#endif
#endif //USING_CTSY_DISPATCHER
	
   if (0 == iPhoneIdentity.iSerialNumber.Length())
      {		
	  TInt ret = iMessageManager->HandleRequestL(EMobilePhoneGetPhoneId);

	  if(KErrNone == ret)
	     {
	     // DOS call successful, save request handle and return pointer
	     // for completion
	     iRetPhoneIdentity = aId;
         iReqHandleType = EMultimodePhoneGetPhoneId;
            
         // save request handle in queue
         TGetPhoneIdRequest* req = new (ELeave) TGetPhoneIdRequest();
         req->iReqHandle = aTsyReqHandle;
         req->iRetPhoneId = aId;
         iGetPhoneIdRequests.AppendL( req );            
	     }
	  else
	     {
	     // DOS call failed, immediately complete with error
	     ReqCompleted( aTsyReqHandle, ret );
	     }
      }
   else
      {
      OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DOGETPHONEIDL_1, "TSY: CMmPhoneTsy::GetPhoneIdL - iManufacturer: %S, iModel: %S", iPhoneIdentity.iManufacturer, iPhoneIdentity.iModel );
      OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DOGETPHONEIDL_2, "TSY: CMmPhoneTsy::GetPhoneIdL - iRevision: %S, iSerialNumber: %S", iPhoneIdentity.iRevision, iPhoneIdentity.iSerialNumber);
     	  
	  *aId = iPhoneIdentity;
	  ReqCompleted( aTsyReqHandle, KErrNone );	   
      }	
   }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetPhoneId
// Sets iPhoneIdentitys iSerialNumber and completes GetPhoneId.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetPhoneId(
    TBuf8<KSerialNumberLength> aSerialData, 
    TInt aError )
    {
	if ( ( KErrNone == aError ) && ( 0 < aSerialData.Length() ) )
		{
        // Copy serial number
        iPhoneIdentity.iSerialNumber.Copy(aSerialData.Left( aSerialData.Length() ) );
        
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETPHONEID_1, "TSY: CMmPhoneTsy::CompletePhoneSerialNumber - iSerialNumber: %S", iPhoneIdentity.iSerialNumber );
        }

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(EMultimodePhoneGetPhoneId );
   
    TInt getPhoneIdRequestsCount = iGetPhoneIdRequests.Count();

    for ( TInt i=0; i < getPhoneIdRequestsCount ; i++ )
        {
        TGetPhoneIdRequest* req = iGetPhoneIdRequests[i];
    
        *(req->iRetPhoneId) = iPhoneIdentity;
        ReqCompleted(req->iReqHandle, aError);   
        }
    iGetPhoneIdRequests.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::MobilePhoneGetPhoneIdCancel
// Cancels get phone id request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::MobilePhoneGetPhoneIdCancel(const TTsyReqHandle aTsyReqHandle )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MOBILEPHONEGETPHONEIDCANCEL_1, "TSY: CMmPhoneTsy::MobilePhoneGetPhoneIdCancel");
    
    TInt reqCount = iGetPhoneIdRequests.Count();
    
    // 1 means that last ongoing request is going to be cancelled.
    if ( reqCount == 1 )
        {
        //Request handle storage is used in this case like a semaphore. When the
        //request handle is stored in iTsyReqHandleStore then CTSY knows that 
        //there is at least one ongoing GetPhoneId request. Code must remove 
        //GetPhoneId request handle from storage when the last ongoing request 
        //is cancelled.
        iTsyReqHandleStore->ResetTsyReqHandle(EMultimodePhoneGetPhoneId);
        }            
    
    for ( TInt i = 0; i < reqCount; i++ )
       {
       if (aTsyReqHandle == iGetPhoneIdRequests[i]->iReqHandle)
          {
          // remove the request from the queue and destroy it.
          TGetPhoneIdRequest* req = iGetPhoneIdRequests[i];
          iGetPhoneIdRequests.Remove( i );
          delete req;
                    
          // cancel the request
          ReqCompleted(aTsyReqHandle, KErrCancel);
     
          // end the FOR-loop, there can be only one identical reqhandle
          // so there is no reason why the array-loop should continue
          break;
          }
        }     
    return KErrNone;
    }    


// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSubscriberIdL
// Gets the subscriber's identity as described by its IMSI.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetSubscriberIdL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneSubscriberId* aId )
    {
	if ( 0 == iSubscriberId.Length() )
		{
		TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(EMultimodePhoneGetSubscriberId );
		if ( reqHandle == EMultimodePhoneReqHandleUnknown )
			{
			// IMSI has to be asynchronously requested from DOS.
			// (no packed parameters)
			TInt ret = iMessageManager->HandleRequestL( EMobilePhoneGetSubscriberId );
	
			if ( KErrNone == ret )
				{
				// DOS call successful, save request handle and return pointer
				// for completion
	            iReqHandleType = EMultimodePhoneGetSubscriberId;
				// save request handle in queue
				TSubscriberIdRequest* req = new (ELeave) TSubscriberIdRequest();
				req->iReqHandle = aTsyReqHandle;
				req->iRetSubscrId = aId;
				iSubscriberIdRequests.AppendL( req );
				}
			else
				{
				// DOS call failed, immediately complete with error
				ReqCompleted( aTsyReqHandle, ret );
				}
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSUBSCRIBERIDL_1, "TSY: CMmPhoneTsy::GetSubscriberIdL - queued requests add req to queue");
			TSubscriberIdRequest* req = new (ELeave) TSubscriberIdRequest();
			req->iReqHandle = aTsyReqHandle;
			req->iRetSubscrId = aId;
			iSubscriberIdRequests.AppendL( req );
			}
		}
	else
		{
		// Complete with current IMSI
	    *aId = iSubscriberId;
		ReqCompleted( aTsyReqHandle, KErrNone );
		}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetIndicatorCaps
// Returns the supported indicator capabilities
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetIndicatorCaps(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aActCaps,
    TUint32* aIndCaps )
    {
    TInt ret( KErrGeneral );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETINDICATORCAPS_1, "TSY: CMmPhoneTsy::GetIndicatorCaps" );
    if(	 NULL != aActCaps && NULL != aIndCaps )
    	{
    	*aActCaps = RMobilePhone::KCapsGetIndicator | 
            RMobilePhone::KCapsNotifyIndicatorChange; 
    	*aIndCaps = RMobilePhone::KIndChargerConnected | 
            RMobilePhone::KIndNetworkAvailable | 
            RMobilePhone::KIndCallInProgress; 
    	ret = KErrNone;
    	}

    ReqCompleted( aTsyReqHandle, ret );

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetIndicators
// Returns the phone indicators information to the client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetIndicators(
    const TTsyReqHandle aTsyReqHandle,
    TUint32* aIndicators )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETINDICATORS_1,  "TSY: CMmPhoneTsy::GetIndicators" );
    TInt ret( KErrGeneral );

    if( aIndicators )
    	{
    	*aIndicators = GetIndicatorState();
		ret = KErrNone;
    	}

    ReqCompleted( aTsyReqHandle, ret );

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetIndicators
// Gets the current indicators state from different components
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TUint32 CMmPhoneTsy::GetIndicatorState()
	{
	TUint32 indicators( 0 );

	//Charger connected?
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETINDICATORSTATE_1,  "TSY: CMmPhoneTsy::GetIndicatorState charger: %d", (TInt)iBatteryInfo.iStatus );
	if( RMobilePhone::EBatteryConnectedButExternallyPowered == 
        iBatteryInfo.iStatus )
		{
		indicators |= RMobilePhone::KIndChargerConnected;
		}

	//Network available?
	if( iMmNetTsy )
		{
		RMobilePhone::TMobilePhoneRegistrationStatus networkRegStatus = 
        iMmNetTsy->GetNetworkRegistrationStatus();
		if( RMobilePhone::ERegisteredOnHomeNetwork == networkRegStatus || 
            RMobilePhone::ERegisteredRoaming == networkRegStatus )
			{
			indicators |= RMobilePhone::KIndNetworkAvailable;
			}
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETINDICATORSTATE_2,  "TSY: CMmPhoneTsy::GetIndicatorState network: %d", (TInt)networkRegStatus );
		}

	//Call in progress?
	//-Browses through the call list to find any calls that are currently 
    //connected
	TInt numberOfCallObjects = CallList()->GetNumberOfObjects();
	for ( TInt i=0; i<numberOfCallObjects; i++ )
    	{
		CMmCallTsy* mmCall = CallList()->GetMmCallByIndex( i );
		if ( RMobileCall::EStatusConnected == mmCall->MobileCallStatus() )
			{
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETINDICATORSTATE_3,  "TSY: CMmPhoneTsy::GetIndicatorState call id: %d, status: %d", mmCall->CallId(), (TInt)mmCall->MobileCallStatus() );
			indicators |= RMobilePhone::KIndCallInProgress;
			break;
			}
		}
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETINDICATORSTATE_4,  "TSY: CMmPhoneTsy::GetIndicatorState - ok, Indicators: %d", indicators );

    return indicators;
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetSubscriberIdL
// Completes GetSubscriberId and delivers IMSI to client.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetSubscriberIdL(
    TBuf8<RMobilePhone::KIMSISize> aImsiData,
    TInt aError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_1, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL, aError=%{TSymbianErrorCodes}", aError );

    // Set the flag to EFalse
    iTSYSubscriberIdReq = EFalse;
    
    RMobilePhone::TMobilePhoneSubscriberId subscriberIdTmp;
    subscriberIdTmp.Zero();  
  
    // Retrieve previous IMSI value from the Central Repository
	iCFISPrivateCentRep->Get( KCtsyIMSI, subscriberIdTmp );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_2, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL - Get old IMSI from CenRep: %S", subscriberIdTmp );
	
    if ( ( KErrNone == aError ) && ( 0 < aImsiData.Length() ) )
        {
        // Copy new IMSI value to the member variable
        iSubscriberId.Copy( aImsiData.Left( aImsiData.Length() ) );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_3, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL - Copy new IMSI: %S", iSubscriberId );
        }
    
    // check the boot state security, is the security ready (PIN query)
    if ( iBootState.iSecReady )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_4, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL - Security ready");
  
        // Check has the IMSI changed and is there previous info: or an error
        if ( KErrNotFound == subscriberIdTmp.Match( iSubscriberId ) && 
        	subscriberIdTmp.Length() != 0 ||
        	KErrNone != aError )     
            {
            // Reset ME ALS info
            ResetMEAlsInfo();
            //Reset CFIS data from public CR
            iCFISCentRep->Reset();
            //Reset MWIS data from public CR
            iMWISCentRep->Reset();
            //Reset private side CR
            iCFISPrivateCentRep->Reset();
            // Reset MWIS support
            iMmSupplServTsy->SetIccSupportForMWIS( ETrue );   
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_5, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL - Central Repositories have been reseted!");
            
            // intialize the previous SIM related data
            if ( iMmCustomTsy )
                { 
                iMmCustomTsy->SetIccCfBootUpFlag( ETrue );
                }
            iMmSupplServTsy->SetIccMwisBootUpFlag( ETrue );
            
            // get new infromation from lower levels, if sim is ready
            if ( iBootState.iSIMReady )
                {                       
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_6, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL - SIM ready, retrive intial ICC SIM data.");

                // update ALS line information 
                iMessageManager->HandleRequestL( EMobilePhoneGetALSLine );
                // update call forward information
                iMessageManager->HandleRequestL( 
                    EMmTsyGetCallForwardingNumberIPC );
                // Get Icc call forwarding status
                iMessageManager->HandleRequestL( 
                    ECustomGetIccCallForwardingStatusIPC );
                // Get message waiting indicators
                iMessageManager->HandleRequestL( 
                    EMobilePhoneGetIccMessageWaitingIndicators );
                }   
            }
        }
        
    if ( ( KErrNone == aError ) && 
        ( 0 < aImsiData.Length() ) && 
        KErrNotFound == subscriberIdTmp.Match( iSubscriberId ) )
        {    
    	// Write new IMSI value to the central repository
    	iCFISPrivateCentRep->Set( KCtsyIMSI, iSubscriberId );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSUBSCRIBERIDL_7, "TSY: CMmPhoneTsy::CompleteGetSubscriberIdL - Write new IMSI to the CenRep: %S", iSubscriberId );
        }
        
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneGetSubscriberId );

    for ( TInt i=0; i < iSubscriberIdRequests.Count(); i++ )
        {
        TSubscriberIdRequest* req = iSubscriberIdRequests[ i ];
        *(req->iRetSubscrId) = iSubscriberId;
        ReqCompleted( req->iReqHandle, aError );
        }
        
    iSubscriberIdRequests.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSubscriberIdCancel
// Cancels an outstanding GetSubscriberId request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetSubscriberIdCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    // If there is only one request, reset the req handle
    if( 1 == iSubscriberIdRequests.Count() )
    	{
		TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        	EMultimodePhoneGetSubscriberId );    	
    	}

    // Delete and remove cancelled request from the array
	for ( TInt i = 0; i < iSubscriberIdRequests.Count(); i++ )
		{
	  	TSubscriberIdRequest* req = iSubscriberIdRequests[ i ];
	  	
	  	if( aTsyReqHandle == req->iReqHandle  )
	  		{
	  	    delete iSubscriberIdRequests[ i ];
	  		iSubscriberIdRequests.Remove(i);
	  		ReqCompleted( aTsyReqHandle, KErrCancel );
	  		break;	
	  		}
		}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetALSLineL
// This method allows a client to find out which voice line is currently
// selected as the active ALS line for outgoing voice calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetALSLineL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneALSLine* aALSLine )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETALSLINEL_1, "TSY: CMmPhoneTsy::GetALSLineL");
	
    TInt ret ( KErrNone );
	TBool alsSupportedBySim;
	
	//Get als  support info
	iCentRep->Get( KCtsyMEAlsSupported, alsSupportedBySim );
    
	//Get TSY req. handle
    TTsyReqHandle phoneGetAlsLineHandle = iTsyReqHandleStore->GetTsyReqHandle( 
		EMultimodePhoneGetAlsLine );
    
    //The request is already in processing because of previous request
    //Complete request with status value informing the client about
	if( EMultimodePhoneReqHandleUnknown != phoneGetAlsLineHandle )
		{
		TGetAlsLineRequest* req = new (ELeave) TGetAlsLineRequest();
		req->iReqHandle = aTsyReqHandle;
		req->iALSLine = aALSLine;
		iGetAlsLineRequests.AppendL( req );
		}
	else
		{
		if ( RMobilePhone::EAlternateLineUnknown == iAlsLine && 
			alsSupportedBySim ) 
			{ 
			// no packed parameters for DOS call 
			ret = iMessageManager->HandleRequestL( EMobilePhoneGetALSLine ); 
			//check success
			if ( KErrNone == ret )
				{
				//save tsy req handle type 
				iReqHandleType = EMultimodePhoneGetAlsLine; 
				
				// save request in queue for completion
				TGetAlsLineRequest* req = new (ELeave) TGetAlsLineRequest();
				req->iReqHandle = aTsyReqHandle;
				req->iALSLine = aALSLine;
				iGetAlsLineRequests.AppendL( req );
				}
			}
		 
		else if( RMobilePhone::EAlternateLineUnknown != iAlsLine && 
			alsSupportedBySim ) 
			{ 
			// set value in client side 
			*aALSLine = iAlsLine; 
			// complete immediately 
			ReqCompleted ( aTsyReqHandle, KErrNone ); 
			}
			
		else if ( !alsSupportedBySim )
		    {	    
		    TInt tempLine(0);
		    
		    //Get Default als info from Central Repository
		    ret = iCentRep->Get( KCtysMEAlsLineInfo, tempLine );		    
		   	
		   	RMobilePhone::TMobilePhoneALSLine alsline = MapMEALSLine( tempLine );
		   	
		   	if( KErrNone == ret )
		   		{
		   		// set default value in client side 
				*aALSLine = alsline; 
		   		}
			// complete immediately 
			ReqCompleted ( aTsyReqHandle, ret );
		    } 
		}

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAlsLineCancel
// Cancels an outstanding GetAlsLine request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetAlsLineCancel(
	const TTsyReqHandle aTsyReqHandle )
    {

    //reset the req handle
    if( 1 == iGetAlsLineRequests.Count() )
    	{
		TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        	EMultimodePhoneGetAlsLine );    	
    	}
	
	for ( TInt i = 0; i < iGetAlsLineRequests.Count(); i++ )
		{
	  	TGetAlsLineRequest* req = iGetAlsLineRequests[ i ];
	  	
	  	if( aTsyReqHandle == req->iReqHandle  )
	  		{
	  		ReqCompleted( req->iReqHandle, KErrCancel );
	  		delete iGetAlsLineRequests[ i ];
	  		iGetAlsLineRequests.Remove(i);
	  		break;	
	  		}
		}
		
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetAlsLine
// Completes a GetAlsLine request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetAlsLine(
    RMobilePhone::TMobilePhoneALSLine aAlsStatus,
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETALSLINE_1, "TSY: CMmPhoneTsy::CompleteGetAlsLine - ALS line" );

    //check if value has been updated
    if ( aAlsStatus != iAlsLine )
        {
        //save value in tsy
        iAlsLine = aAlsStatus;
        iCentRep->Set( KCtysMEAlsLineInfo, aAlsStatus );
        }

	//reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
		EMultimodePhoneGetAlsLine );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
		for ( TInt i = 0; i < iGetAlsLineRequests.Count(); i++ )
			{
		  	TGetAlsLineRequest* req = iGetAlsLineRequests[ i ];
			//set value in client side
		  	*(req->iALSLine) = aAlsStatus;
            // Complete the client request
            ReqCompleted( req->iReqHandle, aErrorCode );
			}
		//Destroy Array 
		iGetAlsLineRequests.ResetAndDestroy();
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetALSLineL
// This method allows a client to set which voice line is currently
// selected as the active ALS line for outgoing voice calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::SetALSLineL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneALSLine* aALSLine )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SETALSLINEL_1, "TSY: CMmPhoneTsy::SetALSLineL");
    TInt ret ( KErrNone );
	TBool alsSupportedBySim ( EFalse );
	
	//Get als support info
	iCentRep->Get( KCtsyMEAlsSupported, alsSupportedBySim );
	
	//check if handle is not used for Get and Set
	if ( ( EMultimodePhoneReqHandleUnknown == iTsyReqHandleStore->
           GetTsyReqHandle( EMultimodePhoneGetAlsLine ) ) &&
	     ( EMultimodePhoneReqHandleUnknown == iTsyReqHandleStore->
		   GetTsyReqHandle( EMultimodePhoneSetAlsLine ) ) 
		   && alsSupportedBySim )
        {
        //save the new ALS value temporarily, will be used in the completion
        iTempAlsLine = *aALSLine;

        CMmDataPackage data;
		data.PackData ( aALSLine );

		// parameter for DOS call: RMobilePhone::TMobilePhoneALSLine
		ret = iMessageManager->HandleRequestL( EMobilePhoneSetALSLine, &data );

        //check success
        if ( KErrNone == ret )
        	{
        	//save tsy req handle type
            iReqHandleType = EMultimodePhoneSetAlsLine;
        	}
        }
    else if ( ( EMultimodePhoneReqHandleUnknown == iTsyReqHandleStore->
           GetTsyReqHandle( EMultimodePhoneGetAlsLine ) ) &&
	     ( EMultimodePhoneReqHandleUnknown == iTsyReqHandleStore->
		   GetTsyReqHandle( EMultimodePhoneSetAlsLine ) ) && 
		   !alsSupportedBySim )
        {        
        //Reset previous Info
        iCentRep->Reset();
        
        //ME based ALS is used
       	ret = iCentRep->Set( KCtysMEAlsLineInfo, *aALSLine );	
        
        //Set default ALS Value
        if( KErrNone == ret )
	        {
	        iAlsLine = *aALSLine;
	        //NotifyClient about als line change
	        CompleteNotifyALSLineChange();
	        ReqCompleted( aTsyReqHandle, ret );	
	        }
		else	
			{
			ret = KErrWrite;
			} 
        }
	else
		{
		ret = KErrServerBusy;	
		}       

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteSetAlsLine
// Completes SetAlsLine request. If notification of ALS line change has been 
// requested by the client, the notification is completed as well.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteSetAlsLine(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETESETALSLINE_1, "TSY: CMmPhoneTsy::CompleteSetAlsLine - ALS line: %d", iTempAlsLine );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
    	EMultimodePhoneSetAlsLine );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        //if operation has been successful
        if ( KErrNone == aErrorCode )
            {
            //save the temporary value in iAlsLine
            iAlsLine = iTempAlsLine;

            CompleteNotifyALSLineChange();
            }
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyALSLineChange
// This method allows a client to be notified if the currently selected
// ALS line changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyALSLineChange(
    RMobilePhone::TMobilePhoneALSLine* aALSLine )
    {
    iRetNotifyALSLine = aALSLine;
    //save tsy req handle type
    iReqHandleType = EMultimodePhoneNotifyAlsLineChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyALSLineChangeCancel
// This method cancels an outstanding asynchronous NotifyALSLineChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyALSLineChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
    iRetNotifyALSLine = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyAlsLineChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyALSLineChange
// This method completes an outstanding asynchronous NotifyALSLineChange
// request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyALSLineChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
  		EMultimodePhoneNotifyAlsLineChange );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        //copy data in client space
        *iRetNotifyALSLine = iAlsLine;
        //reset pointer to client space
        iRetNotifyALSLine = NULL;

        // Complete the client request
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetFdnStatusL
// Retrieves the current status of the Fixed Dialling Numbers service.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetFdnStatusL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TMobilePhoneFdnStatus* aFdnStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETFDNSTATUSL_1,  "TSY: CMmPhoneTsy::GetFdnStatusL" );

    TInt ret ( KErrServerBusy );

    //check if handle is not in use already
    TTsyReqHandle phoneSetLockSettingHandle =
            iTsyReqHandleStore->GetTsyReqHandle( EMultimodePhoneGetFdnStatus );

	if ( EMultimodePhoneReqHandleUnknown == phoneSetLockSettingHandle )
        {
        //save the setting information for later use
        iRetGetFdnStatus = aFdnStatus;

        //call the extension to handle this request
        ret = iMmPhoneExtInterface->GetFdnStatusL( );

        if ( KErrNone == ret )
            {
            //Set tsy req handle. Must be done already here so that
            //CompleteNotifySecurityEvent can complete request if it
            //unable to notify client about the situation.
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EMultimodePhoneGetFdnStatus, aTsyReqHandle );      
#else
            // Never comes here. See SetTypeOfResponse.
            iTsyReqHandleStore->SetTsyReqHandle(
                    EMultimodePhoneGetFdnStatus, aTsyReqHandle );
#endif // REQHANDLE_TIMER
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetFdnStatus
// Completes an outstanding asynchronous GetFdnStatus request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetFdnStatus(
        CMmDataPackage* aDataPackage,
        TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETFDNSTATUS_1, "TSY: CMmPhoneTsy::CompleteGetFdnStatus - Error: %{TSymbianErrorCodes}", aErrorCode );


    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                                EMultimodePhoneGetFdnStatus );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if( KErrNone == aErrorCode )
            {
            //unpack the data
            RMobilePhone::TMobilePhoneFdnStatus* fdnStatus = NULL;
            aDataPackage->UnPackData (&fdnStatus );
            *iRetGetFdnStatus = *fdnStatus;
            }
        
        //null the pointer to client side
        iRetGetFdnStatus = NULL;
        // Complete the client request
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetFdnStatusCancel
// This method cancels an outstanding asynchronous GetFdnStatus request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetFdnStatusCancel(
    const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETFDNSTATUSCANCEL_1,  "TSY: CMmPhoneTsy::GetFdnStatusCancel" );
    iRetGetFdnStatus = NULL;
    
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneGetFdnStatus );
    
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetFdnSettingL
// This method activates (ON) or deactivates (OFF) the Fixed
// Dialling Numbers (FDN) service
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::SetFdnSettingL(
    const TTsyReqHandle aTsyReqHandle,
    const TDataPackage& aPackage )
    {
    //PIN2 must be verified prior setting the fdn setting, launch
    //a security event to inform the client. Wait until
    //VerifySecurityCode is completed
    iMmSecurityTsy->CompleteNotifySecurityEventL( RMobilePhone::EPin2Required,
                                         KErrNone );

    //save the setting information for later use
    iRetFdnSetting = reinterpret_cast<RMobilePhone::TMobilePhoneFdnSetting*>
        ( aPackage.Ptr1() );

    //the SetFdnSetting is already ongoing, save the request type.
    //don't wait for DoExtFuncL to store it ( the normal procedure )
    //as CompleteNotifySecurityEvent
    //will trigger a notification repost and this repost
    //will overwrite the internal attribute iReqHandleType
    iTsyReqHandleStore->SetTsyReqHandle( EMultimodePhoneSetFdnSetting,
                                             aTsyReqHandle );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteSetFdnSettingL
// This method completes an outstanding asynchronous SetFdnSetting request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteSetFdnSettingL(
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETESETFDNSETTINGL_1, "TSY: CMmPhoneTsy::CompleteSetFdnSettingL - Error: %{TSymbianErrorCodes}",aErrorCode);

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                                EMultimodePhoneSetFdnSetting );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        RMobilePhone::TMobilePhoneFdnSetting retFdnSetting = *iRetFdnSetting;

        if ( KErrNone == aErrorCode )
            {            
            //trigger the FDN notification completion
            CompleteNotifyFdnStatusChange();

            //reset pointer to the client space.
            iRetFdnSetting = NULL;

            //Complete the request before checking if the ADN
            //Should be cached, the UI gets freezed for a couple of seconds
            //if the ADN is being cached before completeing the Fdn setting
            ReqCompleted( reqHandle, KErrNone );

            //check if the Fdn has been set off, the ADN cache is not
            //done if the phone boots with FDN on, thefore we have to
            //cache it at this point
            if ( RMobilePhone::EFdnSetOff == retFdnSetting )
                {
                for( TInt i = 0; i < iPBList->GetNumberOfObjects(); i++ )
                    {
                    //Get pbStore object
                    CMmPhoneBookStoreTsy* pbStore = iPBList->
                        GetMmPBByIndex( i );
                    //check if the current phonebook is ADN
                    if ( 0 == pbStore->PhoneBookName()->CompareF(
                                                    KETelIccAdnPhoneBook ) )
                        {
                        //Start to cache phonebook entries.
                        pbStore->CacheEntriesL();
                        }
                    }
                }
            }
        else
            {
            //reset pointer to the client space.
            iRetFdnSetting = NULL;

            ReqCompleted( reqHandle, aErrorCode );
            }

        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyFdnStatusChange
// This request allows a client to be notified when the status
// of the FDN service has changed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyFdnStatusChange(
    RMobilePhone::TMobilePhoneFdnStatus* aStatus )
    {
    iRetNotifyFdnStatus = aStatus;
    //save tsy req handle type
    iReqHandleType = EMultimodePhoneNotifyFdnStatusChange;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyFdnStatusChangeCancel
// This method cancels an outstanding asynchronous NotifyFdnStatusChange req.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyFdnStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle )
    {

    iRetNotifyFdnStatus = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle
        ( EMultimodePhoneNotifyFdnStatusChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );


    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyFdnStatusChange
// This method cancels an outstanding asynchronous NotifyFdnStatusChange req.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyFdnStatusChange()
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                                EMultimodePhoneNotifyFdnStatusChange );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        //map the value of the used in the setting
        //we asume first that the status is on
        *iRetNotifyFdnStatus = RMobilePhone::EFdnActive;
        //if the status is off change the value
        if (  RMobilePhone::EFdnSetOff == *iRetFdnSetting )
            {
            *iRetNotifyFdnStatus = RMobilePhone::EFdnNotActive;
            }
        // Complete the client request
        ReqCompleted( reqHandle, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::PhoneExtInterface
// This method returns pointer to active phone extension object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPhoneExtInterface* CMmPhoneTsy::PhoneExtInterface()
    {
    return iMmPhoneExtInterface;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetConferenceCall
// This method returns a pointer to
// CMmConferenceCallMessHandlerGsmWcdmaExt object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetConferenceCall(
    CMmConferenceCallTsy* aMmConferenceCallTsy )
    {
    iMmConferenceCall = aMmConferenceCallTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ConferenceCall
// This method returns a pointer to
// CMmConferenceCallMessHandlerGsmWcdmaExt object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmConferenceCallTsy* CMmPhoneTsy::ConferenceCall()
    {

    return iMmConferenceCall;

    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SmsSession
// Gives pointer to SMS messaging session
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmSmsTsy* CMmPhoneTsy::SmsSession()
    {
    return iMmSmsTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SmsService
// Gives pointer to SMS messaging session as a MCtsySatService
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MCtsySatService& CMmPhoneTsy::SmsService()
    {
    return *iMmSmsTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetSmsSession
// Set SMS messaging session
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetSmsSession(
    CMmSmsTsy* aMmSmsTsy )
    {
    iMmSmsTsy = aMmSmsTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::PacketDataSession
// Gives pointer to Packet Data session
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmPacketServiceTsy* CMmPhoneTsy::PacketDataSession()
    {
    return iMmPacketServiceTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetCustomTsy
// Gives pointer to CustomTsy extension
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCustomTsy* CMmPhoneTsy::GetCustomTsy()
    {
    return iMmCustomTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetCustomTsy
// CustomTsy can set iMmCustomTsy pointer to NULL when it is closed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetCustomTsy(
    CMmCustomTsy* aMmCustomTsy )
    {
    iMmCustomTsy = aMmCustomTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSmsTsy
// Gives pointer to SmsTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmSmsTsy* CMmPhoneTsy::GetSmsTsy()
    {
    return iMmSmsTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetIccAccessCaps
// Get ICC Access Capabilities. ICC capabilities are
// network mode dependent
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetIccAccessCaps(
    const TTsyReqHandle aTsyReqHandle,
    TUint32*            aCaps )
    {
    TInt ret( KErrNone );

    //set the default value for the capabilites
    *aCaps = 0x0;

    // Check mode ICC type
    TInt currentIccType( iMmPhoneExtInterface->CurrentICCType() );

    // 2G SIM
    if ( CMmPhoneGsmWcdmaExt::EICCTypeSim2GGsm ==  currentIccType)
        {
        *aCaps = RMobilePhone::KCapsSimAccessSupported;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETICCACCESSCAPS_1, "TSY: CMmPhoneTsy::GetIccAccessCaps: aCaps = KCapsSimAccessSupported.");
        }
    // 3G USIM
    else if ( CMmPhoneGsmWcdmaExt::EICCTypeSim3G == currentIccType )
        {
        *aCaps = RMobilePhone::KCapsUSimAccessSupported +
            RMobilePhone::KCapsSimAccessSupported;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETICCACCESSCAPS_2, "TSY: CMmPhoneTsy::GetIccAccessCaps: aCaps = KCapsUSimAccessSupported + KCapsSimAccessSupported.");
        }
    ReqCompleted( aTsyReqHandle, ret );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyIccAccessCapsChange
// Notify Change of ICC Access Caps. ICC capabilities are 
// network mode dependent
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyIccAccessCapsChange(
    TUint32* aCaps )
    {
    iReqHandleType = EMultimodePhoneNotifyIccAccessCapsChange;
    iICCCaps = aCaps;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyIccAccessCapsChangeCancel
// Cancels an outstanding NotifyIccAccessCapsChange method
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyIccAccessCapsChangeCancel (
    const TTsyReqHandle aTsyReqHandle )
    {
    // As cancel is called, the client memory address pointed to is no longer
    // valid -> NULL it
    iICCCaps = NULL;
    iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyIccAccessCapsChange );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyIccAccessCapsChange
// Complete Notify Change of ICC Access Caps. ICC capabilities
// are network mode dependent
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyIccAccessCapsChange()
    {
    TInt ret ( KErrNone );

    // Reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
                                EMultimodePhoneNotifyIccAccessCapsChange );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {

        switch ( iMmPhoneExtInterface->CurrentICCType() )
            {
            case CMmPhoneGsmWcdmaExt::EICCTypeSim2GGsm:     // 2G SIM
                *iICCCaps = RMobilePhone::KCapsSimAccessSupported;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYICCACCESSCAPSCHANGE_1, "TSY: CMmPhoneTsy::CompleteNotifyIccAccessCapsChange: KCapsSimAccessSupported.");
                break;
            case CMmPhoneGsmWcdmaExt::EICCTypeSim3G:   // 3G USIM
                *iICCCaps = RMobilePhone::KCapsUSimAccessSupported;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYICCACCESSCAPSCHANGE_2, "TSY: CMmPhoneTsy::CompleteNotifyIccAccessCapsChange: KCapsUSimAccessSupported.");
                break;

            default:
                *iICCCaps = 0x0;

                // If the TSY/ME does not support any ICC access
                // then the request should be completed with KErrNotSupported.
                ret = KErrNotSupported;
            }

        // Complete the client request
        ReqCompleted( reqHandle, ret );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetServiceTableCheckParametersL
// Check validity of client parameters and pass request to GetServiceTableL().
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetServiceTableCheckParametersL(
    const TTsyReqHandle             aTsyReqHandle,
    RMobilePhone::TMobilePhoneServiceTable* aTable,
    TDes8* aTableData )
    {   
    TInt ret = KErrArgument;

    //Incorrectly packaged parameter or wrong parameter may cause crash in TSY
    //when the service table data is copied to aTableData. Check validity of 
    //parameter delivered by ETel API client.
    if ( sizeof( RMobilePhone::TMobilePhoneServiceTableV1 ) <= 
         aTableData->MaxLength() )
        {
        RMobilePhone::TMobilePhoneServiceTableV1Pckg* serviceTableV1Pckg =
            REINTERPRET_CAST( RMobilePhone::TMobilePhoneServiceTableV1Pckg*, 
                aTableData );
                
        RMobilePhone::TMobilePhoneServiceTableV1* 
            serviceTableV1 = &( ( *serviceTableV1Pckg ) () );
    
        ret = GetServiceTableL( aTsyReqHandle, aTable, serviceTableV1 );
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLECHECKPARAMETERSL_1, "TSY: CMmPhoneTsy::GetServiceTableCheckParametersL - ret:%{TSymbianErrorCodes}",ret);
    return ret;
    }        

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetServiceTableL
// Request Service Table (SST/UST)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetServiceTableL(
    const TTsyReqHandle             aTsyReqHandle,
    RMobilePhone::TMobilePhoneServiceTable* aTable,
    RMobilePhone::TMobilePhoneServiceTableV1* aTableData )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLEL_1, "TSY: CMmPhoneTsy::GetServiceTableL - Handle:%d", aTsyReqHandle);
    TInt ret( KErrServerBusy );

    TTsyReqHandle serviceTableHandle =
        iTsyReqHandleStore->GetTsyReqHandle( EMultimodePhoneGetServiceTable );

    if ( 0 < serviceTableHandle || KErrNone != iCacheServiceTableError  )
        {
        //The request is already in processing because of previous request
        //Complete request with KErrServerBusy informing the client about
        //the situation. Or if cache is failed when called from GetServiceTable
        // complete with that error
        
        //Cache failed update return code
        if( !iSimRefreshRegisterOk && ( iServiceTableReq->iCaching ) )
        	{
        	ret = iCacheServiceTableError;
        	}
        ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {

        // check if the request is supported for this service table 
        // and current mode
        switch ( *aTable )
            {
            case RMobilePhone::ESIMServiceTable:
                // SST is always supported
                ret = KErrNone;
                break;
            case RMobilePhone::EUSIMServiceTable:
            // KJT
                // UST is only supported if the used ICC is USIM.
                if ( CMmPhoneGsmWcdmaExt::EICCTypeSim3G  ==
                     iMmPhoneExtInterface->CurrentICCType() )
                    {
                    ret = KErrNone;
                    }
                else
                    {
                    ret = KErrMMEtelWrongMode;
                    }
                break;
            case RMobilePhone::ECDMAServiceTable:
            case RMobilePhone::EUSIMEnabledServiceTable:
            default:   // CDMA and "USIM enabled" service table are not supported
                ret = KErrNotFound;
            }

        if ( KErrNone == ret )
            {
            // if SIM is not ready, store request and complete it when SIM is ready
            if ( !iBootState.iSIMReady )
		        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLEL_2, "TSY: CMmPhoneTsy::GetServiceTableL - request when SIM not ready - store req" );
		    	TServiceTableRequests* req = new (ELeave) TServiceTableRequests();
		    	req->iReqHandle = aTsyReqHandle;
		    	req->iTable = aTable;
		    	req->iTableData = aTableData;
		    	iServiceTableRequests.Append( req );
		        }
		    else
            {
            if( RMobilePhone::ESIMServiceTable == 
                    *aTable && iServiceTableDataCache )
            	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLEL_3, "TSY: CMmPhoneTsy::GetServiceTableL ESIMServiceTable returned from cache");
            	*aTableData = *iServiceTableDataCache;
            	ReqCompleted( aTsyReqHandle, ret );
            	return KErrNone;
            	}
           	else if ( RMobilePhone::EUSIMServiceTable== 
                          *aTable && iServiceTableUSIMDataCache )
           		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLEL_4, "TSY: CMmPhoneTsy::GetServiceTableL EUSIMServiceTable returned from cache");
           		*aTableData = *iServiceTableUSIMDataCache;
            	ReqCompleted( aTsyReqHandle, ret );
            	return KErrNone;
           		}
            else
            	{
            	if( !iSimRefreshRegisterOk && !( iServiceTableReq->iCaching ) )
            		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLEL_5, "TSY: CMmPhoneTsy::GetServiceTableL sim refresh registration failed - possible old data - cache and complete ");
            		//Store reguest parameters for completion
            		iServiceTableReq->iReqHandle = aTsyReqHandle;
            		iServiceTableReq->iTable = aTable;
					iServiceTableReq->iTableData = aTableData;
					iServiceTableReq->iCaching = ETrue;
									
					//Do necessary internal intialisation
					iCacheServiceTableError = KErrNone;
					ret = KErrNone;
					iCalledOnGet = ETrue;	
					iSimCacheQueue = KCacheServiceTable;
					iRefreshOngoing = ETrue;
					iPendingServiceTableReq = ESIMRequest;
				
					//Request new caching	
            		CacheServiceTablesL();
            		}
            	else
            		{            	
	            	if( RMobilePhone::ESIMServiceTable == *aTable )
	            		{	
		            	iPendingServiceTableReq = ESIMRequest;
	            		}
	            	else
		            	{
		            	iPendingServiceTableReq = EUSIMRequest;
		            	}
	            	
	            	// If caching is ongoing do not request service table
	            	if( ENoRequest == iPendingServiceTableCacheReq )
		            	{	
			            // Forward the request to DOS
			            // packed parameter: RMobilePhone::TMobilePhoneServiceTable
			            CMmDataPackage dataPackage;
			            dataPackage.PackData( aTable );
			            ret = iMessageManager->HandleRequestL( 
	                          EMobilePhoneGetServiceTable, &dataPackage );
		            	}
		            //Ready to process new request 
		            iServiceTableReq->iCaching = EFalse;
	            	}
            	
            }
            }

        // Check the request sending status
        if ( KErrNone == ret )
            {
            // Save last req handle type and return pointer
            iReqHandleType = EMultimodePhoneGetServiceTable;
            iRetTableData = aTableData;
            }
        else
            {
            // Complete request if error occured
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
        else
        	{
            ReqCompleted( aTsyReqHandle, ret );
        	}
    	}
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CacheServiceTables
// Starts the service table caching process
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CacheServiceTablesL()
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHESERVICETABLESL_1, "TSY: CMmPhoneTsy::CacheServiceTables.");

	// Start caching if it is not already ongoing
	if ( (((iSimCacheQueue && ( KCacheServiceTable & iSimCacheQueue )) )
		&& IsRefreshOngoing()) || !IsRefreshOngoing() )
		{
		RMobilePhone::TMobilePhoneServiceTable serviceTable = 
            RMobilePhone::ESIMServiceTable;
				
		iPendingServiceTableCacheReq = ESIMRequest;

		// Forward the request to DOS
	    // packed parameter: RMobilePhone::TMobilePhoneServiceTable
	    CMmDataPackage dataPackage;
	    dataPackage.PackData( &serviceTable );

	    TInt ret = iMessageManager->HandleRequestL( EMobilePhoneGetServiceTable,
	        &dataPackage );
		}
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteCacheServiceTables
// Completes the service table caching process
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteCacheServiceTablesL( const TInt aError )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_1, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL Error = %{TSymbianErrorCodes}", aError);
	if ( ( iSimCacheQueue && ( KCacheServiceTable & iSimCacheQueue ) ) 
            && IsRefreshOngoing() )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_2, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL");
		CompleteCacheSimL( KCacheServiceTable, aError );
		}
		
	TInt ret = KErrNone;
		
	// check pending requests and proceed if any
    TInt serviceTableRequestsCount = iServiceTableRequests.Count();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_3, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - serviceTableRequestsCount: %d", serviceTableRequestsCount );

	 if ( serviceTableRequestsCount )
	 	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_4, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - Completing pending ServiceTableRequests");
		// check request type for every pending request and complete
		for ( TInt i = 0; i < serviceTableRequestsCount; i++ )
			{
			TServiceTableRequests* req = iServiceTableRequests[ i ];
			if ( RMobilePhone::ESIMServiceTable == *req->iTable )
				{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_5, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - pending ESIMServiceTableRequest");
				*(req->iTableData) = *iServiceTableDataCache;
				}
			else if ( RMobilePhone::EUSIMServiceTable == *req->iTable )
				{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_6, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - pending ESIMServiceTableRequest");
				*(req->iTableData) = *iServiceTableUSIMDataCache;
				}
			else 
				{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_7, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - pending BadRequest - KErrGeneral");
				ret = KErrGeneral;	
				}
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_8, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - ReqCompleted for pending req withHandle:%d", req->iReqHandle);
			ReqCompleted(req->iReqHandle, ret);
			}
		// Reset req handle
		iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneGetServiceTable );
		iServiceTableRequests.ResetAndDestroy();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_9, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - Reset and ResetAndDestroy!");
		}
		
	if ( ENoRequest != iPendingServiceTableReq )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_10, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL completing client request");
		// Complete client request if pending
		
		// Reset req handle. Returns the deleted req handle
	    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePhoneGetServiceTable );

	    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
	        {
	        ret = KErrNone;
	        
	    	if( ESIMRequest == iPendingServiceTableReq )
	    		{
	    		if( iServiceTableDataCache )
	    			{
	        		*iRetTableData = *iServiceTableDataCache;
	    			}
	    		else
	    			{
	    			ret = KErrGeneral;
	    			}
	    		}
	    	else
	        	{
	        	if( iServiceTableUSIMDataCache )
	        		{	
	        		*iRetTableData = *iServiceTableUSIMDataCache;
	        		}
	        	else
	    			{
	    			ret = KErrGeneral;
	    			}
	        	}
	     	
	     	if ( iCalledOnGet && ( iServiceTableReq->iCaching ) )
				{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_11, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - Called when reading service tables - ");
				iCalledOnGet = EFalse;
				iSimCacheQueue = 0;
				iRefreshOngoing = EFalse;
				GetServiceTableL( iServiceTableReq->iReqHandle,iServiceTableReq->iTable, 
					iServiceTableReq->iTableData );
				iPendingServiceTableReq = ENoRequest;
				
				if( KErrNone != aError )
					{
					iCacheServiceTableError = KErrNotFound;
					}
				
				}
			else
				{	   	
	        	iPendingServiceTableReq = ENoRequest;
	        	
	        	// Complete request if error occured
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESERVICETABLESL_12, "TSY: CMmPhoneTsy::CompleteCacheServiceTablesL - Completing req if error occurred");
	        	ReqCompleted( reqHandle, ret );
				}
	        }	
		}
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetServiceTableL
// Returns the Service Table file that is
// stored on the SIM to the client
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetServiceTableL(
    RMobilePhone::TMobilePhoneServiceTableV1 aTableData,
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_1, "TSY: CMmPhoneTsy::CompleteGetServiceTableL Error = %{TSymbianErrorCodes}", aErrorCode);
    // Check if timeout has happend, complete to all pending ServiceTable
    // requests and clear queue
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_2, "TSY: CMmPhoneTsy::CompleteGetServiceTableL");
	if ( KErrTimedOut == aErrorCode )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_3, "TSY: CMmPhoneTsy::CompleteGetServiceTableL - Timeout happened");
		// Reset req handle. Returns the deleted req handle
	    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePhoneGetServiceTable );

	    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_4, "TSY: CMmPhoneTsy::CompleteGetServiceTableL - in EMultimodePhoneReqHandleUnknown != reqHandle" );
	    	{
			// count pending requests and proceed if any
			TInt serviceTableRequestsCount = iServiceTableRequests.Count();

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_5, "TSY: CMmPhoneTsy::CompleteGetServiceTableL - serviceTableRequestsCount: %d", serviceTableRequestsCount );
	 		if ( serviceTableRequestsCount )
	 			{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_6, "TSY: CMmPhoneTsy::CompleteGetServiceTableL - Completing pending ServiceTableRequests");
				// complete pending requests
				for ( TInt i = 0; i < serviceTableRequestsCount; i++ )
					{
					TServiceTableRequests* req = iServiceTableRequests[ i ];

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_7, "TSY: CMmPhoneTsy::CompleteGetServiceTableL - ReqCompleted for pending req withHandle:%d", req->iReqHandle);
					ReqCompleted(req->iReqHandle, aErrorCode);
					}
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_8, "TSY: CMmPhoneTsy::CompleteGetServiceTableL - in iServiceTableRequests.ResetAndDestroy()" );
				iServiceTableRequests.ResetAndDestroy();	
				}
	        }
		}

    if ( ESIMRequest == iPendingServiceTableCacheReq )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_9, "TSY: CMmPhoneTsy::CompleteGetServiceTableL got Service Table");
		
		delete iServiceTableDataCache;
		iServiceTableDataCache = NULL;
		
		if ( KErrNone == aErrorCode )
			{	
			iServiceTableDataCache =
				new (ELeave) RMobilePhone::TMobilePhoneServiceTableV1();

			*iServiceTableDataCache = aTableData;
			}
			
		// Request USIM Service Table
		if ( ( CMmPhoneGsmWcdmaExt::EICCTypeSim3G  ==
            iMmPhoneExtInterface->CurrentICCType() ) &&
            ENoRequest != iPendingServiceTableCacheReq )
			{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_10, "TSY: CMmPhoneTsy::CompleteGetServiceTableL requesting USIM Service Table");

			RMobilePhone::TMobilePhoneServiceTable serviceTable = 
				RMobilePhone::EUSIMServiceTable;
				
			iPendingServiceTableCacheReq = EUSIMRequest;

			// Forward the request to DOS
			// packed parameter: RMobilePhone::TMobilePhoneServiceTable
			CMmDataPackage dataPackage;
			dataPackage.PackData( &serviceTable );
			TInt ret = iMessageManager->HandleRequestL( 
				EMobilePhoneGetServiceTable, &dataPackage );

			}
		// Complete caching if it is ongoing
		else
			{
			iPendingServiceTableCacheReq = ENoRequest;
			CompleteCacheServiceTablesL( aErrorCode );
			}

		}
	else if ( EUSIMRequest == iPendingServiceTableCacheReq )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_11, "TSY: CMmPhoneTsy::CompleteGetServiceTableL got USIM Service Table");

		delete iServiceTableUSIMDataCache;
		iServiceTableUSIMDataCache = NULL;
		
		if ( KErrNone == aErrorCode )
			{
			iServiceTableUSIMDataCache =
				new (ELeave) RMobilePhone::TMobilePhoneServiceTableV1();

			*iServiceTableUSIMDataCache = aTableData;
			}
			
		iPendingServiceTableCacheReq = ENoRequest;

		// Complete caching if it is ongoing
		CompleteCacheServiceTablesL( aErrorCode );
		}
	
	// Update data only if caching is not ongoing and client request
	// is pending	
	if ( ENoRequest != iPendingServiceTableReq &&
        ENoRequest == iPendingServiceTableCacheReq )
		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_12, "TSY: CMmPhoneTsy::CompleteGetServiceTableL completing client request");
		// Complete client request if pending
		
		// Reset req handle. Returns the deleted req handle
	    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
            EMultimodePhoneGetServiceTable );

	    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
	        {
	    	if( ESIMRequest == iPendingServiceTableReq )
	    		{
	    		delete iServiceTableDataCache;
				iServiceTableDataCache = NULL;
		
	    		if( KErrNone == aErrorCode )
	    			{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_13, "TSY: CMmPhoneTsy::CompleteGetServiceTableL copy SIM ST data to client");
	    			iServiceTableDataCache = new (ELeave) 
					    RMobilePhone::TMobilePhoneServiceTableV1();
	    			*iServiceTableDataCache	= aTableData;
	        		*iRetTableData = aTableData;
	    			}
	    		}
	    	else // EUSIMRequest
	        	{
	        	delete iServiceTableUSIMDataCache;
				iServiceTableUSIMDataCache = NULL;
				
	        	if( KErrNone == aErrorCode )
	        		{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETSERVICETABLEL_14, "TSY: CMmPhoneTsy::CompleteGetServiceTableL copy USIM ST data to client");
	        		iServiceTableUSIMDataCache = new (ELeave) 
					    RMobilePhone::TMobilePhoneServiceTableV1();
	        		*iServiceTableUSIMDataCache = aTableData;	
	        		*iRetTableData = aTableData;
	        		}
	        	}
	        
	        iPendingServiceTableReq = ENoRequest;
	        	
	        // Complete request if error occured
	        ReqCompleted( reqHandle, aErrorCode );
	        }	
		}
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetServiceTableCancel
// Cancels GetServiceTable request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetServiceTableCancel (
	const TTsyReqHandle aTsyReqHandle )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSERVICETABLECANCEL_1, "TSY: CMmPhoneTsy::GetServiceTableCancel");
    iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneGetServiceTable );
    ReqCompleted( aTsyReqHandle, KErrCancel );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SimRefreshRegisterL
// This method registers TSY for receiving the SIM refresh indications.
// Returns KErrNotSupported (?) if the HW does not support this request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::SimRefreshRegisterL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SIMREFRESHREGISTERL_1,  "TSY: CMmPhoneTsy::SimRefreshRegisterL" );

    TInt ret( KErrNone );

    // Add all supported file types to be registered for refresh
    TUint16 filesToRegister = KCacheAdn | KCacheFdn | KCacheServiceTable
        | KCacheALSline;

    // parameter: files to register
    CMmDataPackage dataPackage;
    dataPackage.PackData( &filesToRegister );

    // send request to DOS (no packed parameters).
    ret = iMessageManager->HandleRequestL(
        EMmTsySimRefreshRegisterIPC, &dataPackage );

    if ( KErrNone != ret )
        {
        // the sending has failed
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SIMREFRESHREGISTERL_2,  "TSY: CMmPhoneTsy::SimRefreshRegisterL, Registration for Refresh failed" );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteSimRefreshRegisterL
// This method is called when SIM refresh register is completed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteSimRefreshRegister(
     TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETESIMREFRESHREGISTER_1, "TSY: CMmPhoneTsy::CompleteSimRefreshRegisterL Error = %{TSymbianErrorCodes}", aErrorCode );

	if( KErrNone == aErrorCode )
		{
		iSimRefreshRegisterOk = ETrue;		
		}		
    }

//---------------------------------------------------------------------------
// CMmPhoneTsy::GetSimRefreshRegister
// This method returns Sim refresh registration status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::GetSimRefreshRegister()
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSIMREFRESHREGISTER_1,  "TSY: CMmPhoneTsy::GetSimRefreshRegister iSimRefreshRegisterOk = %d", iSimRefreshRegisterOk );

	return iSimRefreshRegisterOk;		
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteUpdateLifeTimer
// This method is called on completion update life time request
// ---------------------------------------------------------------------------
//    
void CMmPhoneTsy::CompleteUpdateLifeTimer(
     TInt aErrorCode )
    {
	if( KErrNone != aErrorCode)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEUPDATELIFETIMER_1, "TSY: CMmPhoneTsy::CompleteUpdateLifeTimer Error = %{TSymbianErrorCodes}", aErrorCode );
		}
    }    

//---------------------------------------------------------------------------
// CMmPhoneTsy::GetSimRefreshRegister
// This method starts new cache for for specific phonebook
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::StartNewCacheL( 
	TUint8 aPhoneBookType )
    {

	TInt ret( KErrNone );	
	
	//Check cache by phonebook type
	if( KADNPhoneBook == aPhoneBookType )
		{
		iSimCacheQueue = KCacheAdn;
		}
	else if( KFDNPhoneBook == aPhoneBookType )
		{
		iSimCacheQueue = KCacheFdn;	
		}
	else
		ret = KErrArgument;
	
	//If Ok Start cache
	if( KErrNone == ret )
		{
		CacheNextL();
		}
		
	return ret; 
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::IsRefreshOngoing
// This method is called when it is needed to know whether caching has been
// initialized by SIM (refresh)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::IsRefreshOngoing()
    {
    return iRefreshOngoing;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteSimRefreshNowL
// This method is called when "refresh now" indication has been received from
// by SIM and re-caching is needed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteSimRefreshNowL(
    TUint16 aFilesToRefresh )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETESIMREFRESHNOWL_1, "TSY: CMmPhoneTsy::CompleteSimRefreshNowL filemask: %d", aFilesToRefresh);

    // Set flag to indicate that refresh has started
    iRefreshOngoing = ETrue;

    // Fill cache queue
    iSimCacheQueue = aFilesToRefresh;

    // Check if the IMSI has changed and reset ME ALS info
    GetSubscriberIdForTSYL();

	// Cache service tables first
	if( iSimCacheQueue && ( KCacheServiceTable & iSimCacheQueue ) )
        {
        CacheServiceTablesL();
        }

	// Start caching..
	if( iSimCacheQueue && ( KCacheServiceTable != iSimCacheQueue ) )
  		{
  		CacheNextL(); 
  		}
    }


// ---------------------------------------------------------------------------
// CMmPhoneTsy::CacheNextL
// This method is called when SIM caching is needed ( boot or refresh, for now
// this is only implemented for refresh case ) for some SIM file.
//
// The following restriction applies to caching:
//   ADN can not be cached if FDN is activated
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::CacheNextL()
    {
    TBool result(ETrue);

    //Some files required for caching?
    if( iSimCacheQueue )
        {
        // REFRESH ADN or FDN
        TBuf<0x80>  pbRequested;
        TUint16     resetFlag(0);
        TBool       cacheStarted = EFalse;

        //FDN requested? (FDN has the first priority)
        if( KCacheFdn & iSimCacheQueue )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_1,  "TSY: CMmPhoneTsy::CacheNextL - FDN" );
            pbRequested = KETelIccFdnPhoneBook;
            resetFlag = KCacheFdn;
            }
        else if( KCacheALSline & iSimCacheQueue )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_2,  "TSY: CMmPhoneTsy::CacheNextL - ALS" );
			ResetMEAlsInfo();
			pbRequested = KNullDesC;
			cacheStarted = ETrue;
            }
        //ADN requested?
        else if( KCacheAdn & iSimCacheQueue )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_3,  "TSY: CMmPhoneTsy::CacheNextL - ADN" );
            //NOTE: Here should be added the checking of FDN active state
            pbRequested = KETelIccAdnPhoneBook;
            resetFlag = KCacheAdn;
            }
        //UNKNOWN request (erroneous requests, all valid cases have been 
        //checked and the queue can be reset)
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_4,  "TSY: CMmPhoneTsy::CacheNextL - Unknown" );
            pbRequested = KNullDesC;
            resetFlag = iSimCacheQueue;
            }

        //PHONE BOOK Request: Is there a phonebook available?
        if( (pbRequested!=KNullDesC) && (iPBList) )
            {
            TInt numberOfPBooks = iPBList->GetNumberOfObjects();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_5,  "TSY: CMmPhoneTsy::CacheNextL - Num Of PBs: %d", numberOfPBooks );

            //Browse through the phonebook names for the one required
            for( TInt i=0; i < numberOfPBooks; i++ )
                {
                CMmPhoneBookStoreTsy* pbStore = iPBList->GetMmPBByIndex( i );

                //If the correct phonebook is found, cache it's entries
                if ( !pbStore->PhoneBookName()->CompareF( pbRequested ) )
                    {
                    // Cancel cache first
                    if ( ( pbRequested == KETelIccFdnPhoneBook ) ||  
                            ( pbRequested == KETelIccAdnPhoneBook ) )
                    	{
                    	// Create package
                    	CPhoneBookDataPackage package;
                    	// Set package data
                    	package.SetPhoneBookName( pbRequested );
                    	TRAP_IGNORE( MessageManager()->HandleRequestL( 
                            EMmTsyPhoneBookStoreCacheCancelIPC, &package ); );


                    	if ( pbRequested == KETelIccFdnPhoneBook ) 
                    	    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_6,  "TSY: CMmPhoneTsy::CacheNextL - CancelCache for : KETelIccFdnPhoneBook" );
                            }
                    	else 
                    	    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_7,  "TSY: CMmPhoneTsy::CacheNextL - CancelCache for : KETelIccAdnPhoneBook" );
                            }
                    	}                   
                    // perform the various stages of phonebook 
                    // cache re-initialisation
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_8,  "TSY: CMmPhoneTsy::CacheNextL - Resetting cache.");
                    pbStore->ResetCacheReady();
                    pbStore->ResetInitialisationStatus();
                    pbStore->ResetCache();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_9,  "TSY: CMmPhoneTsy::CacheNextL -> Cache entries" );
                    pbStore->CacheEntriesL();

                    cacheStarted = ETrue;
                    break;
                    }
                }
            }

        //CACHING not started
        //phonebook does not exist, or unknown request,
        //so the file is removed from the cache queue
        if( !cacheStarted )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_10,  "TSY: CMmPhoneTsy::CacheNextL - No PB Could be found, or Unknown request" );
            CompleteCacheSimL( resetFlag, ETrue ); //complete with error
            }
        }
    //more files in the queue?
    result = ( iSimCacheQueue )? ETrue : EFalse;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_CACHENEXTL_11,  "TSY: CMmPhoneTsy::CacheNextL: (Success/Fail = 1/0) %d",(TInt)result );
    return result;
    }


// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteCacheSimL
// This method is called when caching for any of SIM files is finished. The 
// list of files left to be cached is updated here.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteCacheSimL(
    TUint8	aFileType,
    TBool	aReadError)
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESIML_1,  "TSY: CMmPhoneTsy::CompleteCacheSimL, FileType: 0x%x", aFileType );

    TBool filesInCacheQueue(EFalse); //By default: "no more files in cache"

    // Remove file from caching queue
    iSimCacheQueue &= ~aFileType;

    // Start caching the next on queue, if queue is not empty
    // If Service Table cahing is completed and ather caching is ongoing
    // do not cache next
#ifdef USING_CTSY_DISPATCHER
    //believed to be a defect, comparing to aFileType will cause a call to CacheNextL on a queue 
    //containing only KCacheServiceTable and result in a Refresh Done IPC, with ETrue as an error
    if ( iSimCacheQueue == KCacheServiceTable )
#else
    if ( aFileType == KCacheServiceTable )
#endif //USING_CTSY_DISPATCHER
    	{
		filesInCacheQueue = iSimCacheQueue;
		}
	else
		{
    	filesInCacheQueue = (!iSimCacheQueue)?EFalse:CacheNextL();
		}

    // Here it must be checked whether caching was finished.
    // Server must still be notified for completion. Complete
    // also if error occured, this means that caching is
    // interrupted
    if( ( !filesInCacheQueue || aReadError ) && !(iBootState.iCachingActive) )
        {
        // Complete the refresh
        NotifySimRefreshCompletionL( aReadError );
        }
     else
     	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETECACHESIML_2,  "TSY: CMmPhoneTsy::CompleteCacheSimL, caching still in progress." );
     	}
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifySimRefreshCompletionL
// Notifying server that refresh has been completed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::NotifySimRefreshCompletionL(
    TBool aReadError )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYSIMREFRESHCOMPLETIONL_1,  "TSY: CMmPhoneTsy::NotifySimRefreshCompletionL error = %{TSymbianErrorCodes}",aReadError );

    // Refresh is finished, update flag
    iRefreshOngoing = EFalse;

    // Make sure that the list gets empty
    iSimCacheQueue = 0;

    // Parameter: error indicator
    CMmDataPackage dataPackage;
    dataPackage.PackData( &aReadError );

    // Notify server for completion of refresh
    iMessageManager->HandleRequestL(
		EMmTsySimRefreshDoneIPC, &dataPackage );
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::IMSAuthenticationL
// Requests authentication data generatation from input parameters
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::ImsAuthenticationL( 
    TDes8* aAuthenticationData )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_IMSAUTHENTICATIONL_1, "TSY: CMmPhoneTsy::ImsAuthenticationL");

	TInt ret = KErrServerBusy;
    
    // Check if request handle already exists
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePhoneIMSAuthenticate );

    if ( EMultimodePhoneReqHandleUnknown == reqHandle )
        {
        ret = KErrArgument;
        if ( sizeof( RMobilePhone::TImsAuthenticateDataV5Pckg ) <=
             aAuthenticationData->MaxLength() )
            {                     
    	   	// Unpack entry parameter
        	RMobilePhone::TImsAuthenticateDataV5Pckg* entryPckg =
            REINTERPRET_CAST
            ( RMobilePhone::TImsAuthenticateDataV5Pckg*, aAuthenticationData );
       		RMobilePhone::TImsAuthenticateDataV5& authData = ( *entryPckg )();
       		
       		if ( KEtelExtMultimodeV5 == authData.ExtensionId() )
       		    {
        	    // Save pointer to client space
        	    iIMSAuthenticationData = aAuthenticationData;
        		
        	    CMmDataPackage authenticationDataPackage;
        	    authenticationDataPackage.PackData( &authData );

        	    ret = ( MessageManager()->HandleRequestL(
        	        EMobilePhoneIMSAuthenticate, &authenticationDataPackage ) );

        	    // Store request handle if KErrNone returned.
        	    if ( KErrNone == ret )
        	        {
        	        iReqHandleType = EMultimodePhoneIMSAuthenticate;	
        	        }
        	    }
            }
	    }
	    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteImsAuthentication
// Requests authentication data generatation from input parameters
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteImsAuthentication( 
    CMmDataPackage* aDataPackage, 
    TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEIMSAUTHENTICATION_1, "TSY: CMmPhoneTsy::CompleteImsAuthentication");

    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneIMSAuthenticate );
    if ( KErrNone == aResult )
        {
        if ( EMultimodePhoneReqHandleUnknown != reqHandle )
            { 
            RMobilePhone::TImsAuthenticateDataV5* authenticationData;
            
            aDataPackage->UnPackData( &authenticationData );
			
			RMobilePhone::TImsAuthenticateDataV5Pckg* entryPckg =
        	REINTERPRET_CAST
        	( RMobilePhone::TImsAuthenticateDataV5Pckg*, 
                iIMSAuthenticationData );
   			RMobilePhone::TImsAuthenticateDataV5& authData = ( *entryPckg )();

            // Copy data to client
            authData.iRES.Copy( authenticationData->iRES );
            authData.iCK.Copy( authenticationData->iCK );
            authData.iIK.Copy( authenticationData->iIK );
            
            iIMSAuthenticationData = NULL;

            ReqCompleted( reqHandle, aResult );
            }
        }
    else if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrTimedOut == aResult )
            {
            iIMSAuthenticationData = NULL;
            }
        else
            {
            RMobilePhone::TImsAuthenticateDataV5* authenticationData;
            aDataPackage->UnPackData( &authenticationData );
			
			RMobilePhone::TImsAuthenticateDataV5Pckg* entryPckg =
        	REINTERPRET_CAST
        	( RMobilePhone::TImsAuthenticateDataV5Pckg*, 
                iIMSAuthenticationData );
   			RMobilePhone::TImsAuthenticateDataV5& authData = ( *entryPckg )();
   		
            // It is possible that request is completed whithout any data
            // inside datapackage
            if ( NULL != authenticationData )
                {
                authData.iAUTS.Copy( authenticationData->iAUTS );

                iIMSAuthenticationData = NULL;
                }
            }
        ReqCompleted( reqHandle, aResult );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ImsAuthenticationCancel
// Cancels sim authentication request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::ImsAuthenticationCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_IMSAUTHENTICATIONCANCEL_1, "TSY: CMmPhoneTsy::ImsAuthenticationCancel");
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneIMSAuthenticate );

    // reset pointer to client data
    iIMSAuthenticationData = NULL;

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
    	{
    	// complete
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetMailboxNumbers
// Requests mailbox numbers identifier information   
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetMailboxNumbersL(
    TDes8* aMailBox )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETMAILBOXNUMBERSL_1, "TSY: CMmPhoneTsy::GetMailboxNumbersL");
    TInt ret = KErrServerBusy;
    
    // Check if request handle already exists
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePhoneGetMailboxNumbers );   

    if ( EMultimodePhoneReqHandleUnknown == reqHandle )
        {
        ret = KErrArgument;        
        if ( sizeof( RMobilePhone::TMobilePhoneVoicemailIdsV3 ) <=
             aMailBox->MaxLength() )
            {
            // Unpack entry parameter
            RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg* entryPckg =
            REINTERPRET_CAST
            ( RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*, aMailBox );
            
            iMailboxData = ( *entryPckg )();

            if ( KETelExtMultimodeV3 == iMailboxData.ExtensionId() )
                {
                // Save pointer to client space
                iMailBoxData = aMailBox;

                // Check that VMBX book store was initilized...
                TBool found = EFalse;
                for( TInt i = 0; (!found) && (i < iPBList->GetNumberOfObjects()); i++ )
                    {
                    //Get pbStore object
                    CMmPhoneBookStoreTsy* pbStore = iPBList->GetMmPBByIndex( i );
                    //Check if the current PB is the object that we are trying to find.
                    if ( 0 == pbStore->PhoneBookName()->CompareF( KETelIccVoiceMailBox ) )
                        {
                        found = ETrue;
                        if( !pbStore->IsPBInitDone() )
                            {
                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETMAILBOXNUMBERSL_2, "TSY: CMmPhoneTsy::GetMailboxNumbersL VoiceMailBox initilizing was not complete... Waiting for completion");
                            iReqHandleType = EMultimodePhoneGetMailboxNumbers;
                            return KErrNone;
                            }
                        }
                    }
                if(!found)
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETMAILBOXNUMBERSL_3, "TSY: CMmPhoneTsy::GetMailboxNumbersL VoiceMailBox should be created...");
                    iMmPhoneBookStoreTsy = 
                        CMmPhoneBookStoreTsy::NewL( this, KETelIccVoiceMailBox );
                    
                    TInt addPBSucceeded = iPBList->AddObject( iMmPhoneBookStoreTsy );
                    if( !iMmPhoneBookStoreTsy->IsPBInitDone() )
                        {
                        iReqHandleType = EMultimodePhoneGetMailboxNumbers;
                        return addPBSucceeded;
                        }
                    }
                

                CMmDataPackage mailBoxDataPackage;
                mailBoxDataPackage.PackData( &iMailboxData );

                ret = ( MessageManager()->HandleRequestL(
                    EMobilePhoneGetMailboxNumbers, &mailBoxDataPackage ) );

                // Store request handle if KErrNone
                if ( KErrNone == ret )
                    {
                    iReqHandleType = EMultimodePhoneGetMailboxNumbers;
                    }
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetMailboxNumbers
// Completes GetMailboxNumbers request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetMailboxNumbers( 
    CMmDataPackage* aDataPackage, 
    TInt aResult )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETMAILBOXNUMBERS_1, "TSY: CMmPhoneTsy::CompleteGetMailboxNumbers");

    // reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneGetMailboxNumbers );
        
    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        { 
        if ( KErrNone == aResult )
        	{	
	        RMobilePhone::TMobilePhoneVoicemailIdsV3* mailboxData;
	        
	        aDataPackage->UnPackData( mailboxData );
			
			RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg* entryPckg =
	    	REINTERPRET_CAST
	    	( RMobilePhone::TMobilePhoneVoicemailIdsV3Pckg*, iMailBoxData );
	    	
			RMobilePhone::TMobilePhoneVoicemailIdsV3& clientData = ( *entryPckg )();

	        // Copy data to client
	        clientData = *mailboxData;
		
	        iMailBoxData = NULL;
        	}
        ReqCompleted( reqHandle, aResult );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetMailboxNumbersCancel
// Cancels GetMailboxNumbers request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetMailboxNumbersCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETMAILBOXNUMBERSCANCEL_1, "TSY: CMmPhoneTsy::GetMailboxNumbersCancel");
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodePhoneGetMailboxNumbers );

    // reset pointer to client data
    iMailBoxData = NULL;

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
    	{
    	// complete
    	ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }
     
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetStaticExtensionMode
// This method can be used to find out what protocol mode is
// currently active
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::GetStaticExtensionMode(
    RMobilePhone::TMobilePhoneNetworkMode* aMode )
    {
    iMmPhoneExtInterface->GetStaticExtensionMode( aMode );
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::RemoveLine
// Marks a line as removed in LineList and recalculates
// phone capablities
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::RemoveLine(
    const TName& aLineName )
    {
    TUint newCaps ( 0 );
    CMmLineTsy* mmLine = NULL;
	RMobilePhone::TMobileService lineMode ( RMobilePhone::EServiceUnspecified );

    // remove line
    iLineList->RemoveLine(aLineName);

    // recalculate phone caps
    for( TInt i = 0; i < iLineList->GetNumberOfObjects(); i++ )
        {
        mmLine = REINTERPRET_CAST( CMmLineTsy*,
                 iLineList->GetObjectByIndex(i) );
        if ( mmLine )
            {
            lineMode = mmLine->LineMode();
            if ( RMobilePhone::EVoiceService == lineMode )
                {
                newCaps += RPhone::KCapsVoice;
                }
            else if ( RMobilePhone::EAuxVoiceService == lineMode )
                {
                newCaps += RPhone::KCapsVoice;
                }
            else if ( RMobilePhone::ECircuitDataService == lineMode )
                {
                newCaps += RPhone::KCapsData +
                       RPhone::KCapsEventModemDetection +
                       RPhone::KCapsStealCommPort;
                }
            }
    }

    //Notify caps change if they have changed.
    if ( newCaps != iPhoneCaps.iFlags )
        {
        iPhoneCaps.iFlags = newCaps;
        CompleteNotifyCapsChange();
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::MessageManager
// Returns a pointer to iMessageManager object
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmMessageManagerBase* CMmPhoneTsy::MessageManager()
    {
    return iMessageManager;
    }

#ifdef REQHANDLE_TIMER

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetTimeStampStore
// Returns pointer to the response timer store
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CResponseTimerStore* CMmPhoneTsy::GetTimeStampStore()
    {
    return iTimeStampStore;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetTimer
// Returns pointer to the response timer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CResponseTimer* CMmPhoneTsy::GetTimer()
    {
    return iReqHandleTimer;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::TimerExpired
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::TimerExpired(
    CTelObject *aObject,
    TInt aReqHandleType,
    TInt aIPC )
    {
    //Because the timer has expired, send this default error value to
    //the compting object.
    TInt error( KErrTimedOut );

   //Find the correct tsy object and call its' Complete function

    //Check if the object is the Phone object
    if( this == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_1, "TSY: CMmPhoneTsy::TimerExpired - this == aObject");
        //call phone object to complete this request
        this->Complete( aReqHandleType, error );
        }

    //Line does not require timer
    TInt i( 0 );

    //Check if the object is a Call object
    for ( i = 0; i < iCallList->GetNumberOfObjects(); i++ )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_2, "TSY: CMmPhoneTsy::TimerExpired - iCallList->GetNumberOfObjects()");
        //Get call object
        CMmCallTsy* call = iCallList->GetMmCallByIndex( i );
        //check if the current call is the object that we are
        //trying to find
        if( call == aObject )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_3, "TSY: CMmPhoneTsy::TimerExpired - call == aObject");
            //call the complete method
            call->Complete( aReqHandleType, error );
            //object found, break
            break;
            }
        }

    // Check if the object is a ConferenceCall object
    if ( iMmConferenceCall == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_4, "TSY: CMmPhoneTsy::TimerExpired - iMmConferenceCall");
        iMmConferenceCall->Complete( aReqHandleType, error );
        }
    //OTHER MODULES

    //Check if the object is a PBStore object
    for ( i = 0; i < iPBList->GetNumberOfObjects(); i++ )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_5, "TSY: CMmPhoneTsy::TimerExpired - iPBList->GetNumberOfObjects()");
        //Get PBStore object
        CMmPhoneBookStoreTsy* pbStore = iPBList->GetMmPBByIndex( i );
        //check if the current PB is the object that we are
        //trying to find
        if ( pbStore == aObject )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_6, "TSY: CMmPhoneTsy::TimerExpired - aPBStore == aObject");
            //call the complete method
			pbStore->Complete( aReqHandleType, error );
            //object found, break
            break;
            }
        }

    // Check if the object is the SMS object
    if ( iMmSmsTsy == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_7, "TSY: CMmPhoneTsy::TimerExpired - iMmSmsTsy == aObject");
        //call the complete method
        iMmSmsTsy->Complete( aReqHandleType, error );
        }
    // Check if the object is the Broadcast object
    else if ( iMmBroadcastTsy == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_8, "TSY: CMmPhoneTsy::TimerExpired - iMmBroadcastTsy == aObject");
        //call the complete method
        iMmBroadcastTsy->Complete( aReqHandleType, error );
        }
    // Check if the object is the USSD object
    else if ( iMmUssdTsy == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_9, "TSY: CMmPhoneTsy::TimerExpired - iMmUssdTsy == aObject");
        //call the complete method
        iMmUssdTsy->Complete( aReqHandleType, error );
        }
    // Check if the object is the ONstore object
    else if ( iMmONStoreTsy == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_10, "TSY: CMmPhoneTsy::TimerExpired - iMmONStoreTsy == aObject");
        //call the complete method
        iMmONStoreTsy->Complete( aReqHandleType, error );
        }
    // Check if the object is the ENstore object
    else if ( iMmENStoreTsy == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_11, "TSY: CMmPhoneTsy::TimerExpired - iMmENStoreTsy == aObject");
        //call the complete method
        iMmENStoreTsy->Complete( aReqHandleType, error );
        }
    else if ( iMmCustomTsy == aObject )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_12, "TSY: CMmPhoneTsy::TimerExpired - iMmCustomTsy == aObject");
        iMmCustomTsy->Complete( aReqHandleType, error , aIPC );
        }

    //THESE DO NOT USE REQ HANDLE STORE -> CANNOT USE TIMER
    // SAT

    // If Packet Data is included, complete method from Packet Service TSY
    // must be called to handler completes if aObject is part of Packet
    // Data session
    else if ( NULL != iMmPacketServiceTsy )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_13, "TSY: CMmPhoneTsy::TimerExpired - NULL != iMmPacketServiceTsy");
        //call the complete method
        iMmPacketServiceTsy->Complete( aObject, aReqHandleType, error );
        }
    else
    	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_TIMEREXPIRED_14, "TSY: CMmPhoneTsy::TimerExpired - else");
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetTypeOfResponse
// Sets the type of response for a given Handle. Automatic
// mode includes an automatic response in case of non response
// from the LTSY layer in a specified time
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetTypeOfResponse(
    const TInt aReqHandleType,
    const TTsyReqHandle aTsyReqHandle )
    {
    TInt timeOut = 0;

    //All possible TSY req handle types are listed in the
    //switch case below.
    switch ( aReqHandleType )
        {
      //DTMF
        case EMultimodePhoneSendDTMFTones:
            timeOut = KMmPhoneSendDTMFTonesTimeOut;
            break;
        case EMultimodePhoneStartDTMFTone:
            timeOut = KMmPhoneStartDTMFToneTimeOut;
            break;
        case EMultimodePhoneStopDTMFTone:
            timeOut = KMmPhoneStopDTMFToneTimeOut;
            break;
      // All Calls Termination
        case EMultimodePhoneTerminateAllCalls:
            timeOut = KMmPhoneTerminateAllCallsTimeOut;
            break;
      // Active Calls Termination
        case EMultimodePhoneTerminateActiveCalls:
            timeOut = KMmPhoneTerminateActiveCallsTimeOut;
            break;
      //NET
        case EMultimodePhoneSelectNetwork:
            timeOut = KMmPhoneSelectNetworkTimeOut;
            break;
        case EMultimodePhoneSendNetworkServiceRequest:
            timeOut = KMmPhoneSendNetworkServiceRequestTimeOut;
            break;
        case EMultimodePhoneSendNetworkServiceRequestNoFdnCheck:
            timeOut = KMmPhoneSendNetworkServiceRequestNoFdnCheckTimeOut;
            break;                        
        case EMultimodePhoneGetDetectedNetworksPhase1:
            timeOut = KMmPhoneGetDetectedNetworksPhase1TimeOut;
            break;
        case EMultimodePhoneSetNWSelectionSetting:
            timeOut = KMmPhoneSetNWSelectionSettingTimeOut;
            break;
        case EMultimodePhoneGetCurrentActiveUSimApplication:        
        	timeOut = KMmPhoneGetCurrentActiveUSimApplicationTimeOut;        	
            break;                        
        case EMultimodePhoneGetAuthorizationInfoPhase1:
            timeOut = KMmPhoneGetAuthorizationInfoPhase1TimeOut;
            break;

      //SECURITY
        case EMultimodePhoneGetLockInfo:
            timeOut = KMmPhoneGetLockInfoTimeOut;
            break;
        case EMultimodePhoneAbortSecurityCode:
            timeOut = KMmPhoneAbortSecurityCodeTimeOut;
            break;
        case EMultimodePhoneVerifySecurityCode:
            timeOut = KMmPhoneVerifySecurityCodeTimeOut;
            break;
        case EMultimodePhoneChangeSecurityCode:
            timeOut = KMmPhoneChangeSecurityCodeTimeOut;
            break;
        case EMultimodePhoneGetSecurityCodeInfoPin1:
        case EMultimodePhoneGetSecurityCodeInfoPin2:
        case EMultimodePhoneGetSecurityCodeInfoPuk1:
        case EMultimodePhoneGetSecurityCodeInfoPuk2:
        case EMultimodePhoneGetSecurityCodeInfoPhonePassword:
        case EMultimodePhoneGetSecurityCodeInfoSPC:
        case EMultimodePhoneGetSecurityCodeInfoPhonebookHiddenKey:
        case EMultimodePhoneGetSecurityCodeInfoUSIMAppPin:
        case EMultimodePhoneGetSecurityCodeInfoSecondUSIMAppPin:
        case EMultimodePhoneGetSecurityCodeInfoUniversalPin:
        case EMultimodePhoneGetSecurityCodeInfoUniversalPuk:
            timeOut = KMmPhoneGetSecurityCodeInfoTimeOut;
            break;

      //SS
        case EMultimodePhoneSetCallForwardingStatus:
            timeOut = KMmPhoneSetCallForwardingStatusTimeOut;
            break;
        case EMultimodePhoneSetCallBarringStatus:
            timeOut = KMmPhoneSetCallBarringStatusTimeOut;
            break;
        case EMultimodePhoneGetCallForwardingStatusPhase1:
            timeOut = KMmPhoneGetCallForwardingStatusPhase1TimeOut;
            break;
        case EMultimodePhoneGetCallBarringStatusPhase1:
            timeOut = KMmPhoneGetCallBarringStatusPhase1TimeOut;
            break;
        case EMultimodePhoneGetCallWaitingStatusPhase1:
            timeOut = KMmPhoneGetCallWaitingStatusPhase1TimeOut;
            break;
        case EMultimodePhoneSetSSPassword:
            timeOut = KMmPhoneSetSSPasswordTimeOut;
            break;
        case EMultimodePhoneSetCallWaitingStatus:
            timeOut = KMmPhoneSetCallWaitingStatusTimeOut;
            break;
        case EMultimodePhoneGetIdentityServiceStatus:
           timeOut = KMmPhoneGetIdentityServiceStatusTimeOut;
            break;
        case EMultimodePhoneGetAlsLine:
            timeOut = KMmPhoneGetAlsLineTimeOut;
            break;
        case EMultimodePhoneSetAlsLine:
            timeOut = KMmPhoneSetAlsLineTimeOut;
            break;
        // SIM access and capabilities
        case EMultimodePhoneGetServiceTable:
            timeOut = KMmPhoneGetServiceTableTimeOut;
            break;
        case EMultimodePhoneGetCustomerServiceProfile:
            timeOut = KMmPhoneGetCustomerServiceProfileTimeOut;
            break;
        case EMultimodePhoneGetPhoneStoreInfo:
            timeOut = KMmPhoneGetPhoneStoreInfoTimeOut;
            break;
        case EMultimodePhoneGetServiceProviderName:
            timeOut = KMmPhoneGetServiceProvicedNameTimeOut;
            break;
        case EMultimodePhoneIMSAuthenticate:
            timeOut = KMmPhoneIMSAuthenticateTimeOut;
            break;
       	case EMultimodePhoneGetMailboxNumbers:
       		timeOut = KMmPhoneGetMailboxNumbersTimeOut;
       		break;
        case EMultimodePhoneEnumerateAPNEntries:
            timeOut = KMmPhoneEnumerateAPNEntriesTimeOut;
            break;
        case EMultimodePhoneGetAPNname:
            timeOut = KMmPhoneGetAPNnameTimeOut;
            break;
        case EMultimodePhoneGetIccMessageWaitingIndicators:
            timeOut = KMmPhoneGetIccMessageWaitingIndicators;
            break;
        case EMultimodePhoneSetIccMessageWaitingIndicators:
            timeOut = KMmPhoneSetIccMessageWaitingIndicators;
            break;
        case EMultimodePhoneAppendAPNName:
            timeOut = KMmPhoneAppendAPNNameTimeOut;
            break;
        case EMultimodePhoneDeleteAPNName:
            timeOut = KMmPhoneDeleteAPNNameTimeOut;
            break;
        case EMultimodePhoneSetAPNControlListServiceStatus:
            timeOut = KMmPhoneSetAPNControlListServiceStatusTimeOut;
            break;
        case EMultimodePhoneGetAPNControlListServiceStatus:
            timeOut = KMmPhoneGetAPNControlListServiceStatusTimeOut;
            break;
        case EMultimodePhoneGetFdnStatus:
            timeOut = KMmPhoneGetFdnStatusTimeOut;
            break;

      //Do not use timer in these cases
      // - all notification requests
        //case EMultimodePhoneNotifyModemDetected:
        //case EMultimodePhoneCapsChangeNotification:
        //case EMultimodePhoneNotifyDTMFCaps:
        //case EMultimodePhoneNotifyNetworkRegistrationStatusChange:
        //case EMultimodePhoneNotifyModeChange:
        //case EMultimodePhoneNotifyCurrentNetworkChange:
        //case EMultimodePhoneNotifySignalStrengthChange:
        //case EMultimodePhoneNotifyNITZInfoChange:
        //case EMultimodePhoneNotifyNWSelectionSettingChange:
        //case EMultimodePhoneNotifyNWTimeInfoChange:
        //case EMultimodePhoneNotifyCallForwardingStatusChange:
        //case EMultimodePhoneNotifyCallBarringStatusChange:
        //case EMultimodePhoneNotifyCallWaitingStatusChange:
        //case EMultimodePhoneNotifyAlsLineChange:
        //case EMultimodePhoneNotifyFdnStatusChange:
        //case EMultimodePhoneNotifySecurityCapsChange:
        //case EMultimodePhoneNotifyLockInfoChange:
        //case EMultimodePhoneNotifySecurityEvent:
        //case EMultimodePhoneNotifyBatteryInfoChange:
        //case EMultimodePhoneNotifyStopInDTMFString:
        //case EMultimodePhoneNotifyCallServiceCapsChange:
        //case EMultimodePhoneNotifyCallForwardingActive:
        //case EMultimodePhoneNotifyIccAccessCapsChange:
        //case EMultimodePhoneNotifyIccMessageWaitingIndicatorsChange:
        //case EMultimodePhoneNotifySecurityCodeInfoChange:
        //case EMultimodePhoneNotifySendNetworkRequest:

      //SS features that require PIN verification. Therefore the
      //request is 2-phased which means that possibility for dead-lock
      //is so high that timer is not used.
        //case EMultimodePhoneSetFdnSetting:
        //case EMultimodePhoneSetLockSetting:

        default:
              iTsyReqHandleStore->SetTsyReqHandle(
                    aReqHandleType,
                    aTsyReqHandle );
              break;
        }

    //if timeout is requested
    if ( timeOut > 0 )
        {
        //set time out
        iTsyReqHandleStore->SetTsyReqHandle(
                aReqHandleType,
                aTsyReqHandle,
                timeOut );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::Complete
// Completes the request due timer expiration
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::Complete(
    TInt aReqHandleType,
    TInt aError )
    {
	CMmDataPackage dataPackage;
	RMobilePhone:: TMobilePhoneLockStatus lock;
	RMobilePhone::TMobilePhoneLockSetting setting;
	RMobilePhone::TMobilePhoneCspFileV1 emptyCspFile;
	RMobilePhone::TMobilePhoneServiceTableV1 emptyServiceTable;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 emptyPhoneStoreInfo;
	TInt trapError = KErrNone;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETE_1, "TSY: CMmPhoneTsy::Complete - ReqHandleType: %d", aReqHandleType);
    switch( aReqHandleType )
        {
        case EMultimodePhoneTerminateAllCalls:
        	CompleteTerminateAllCallsReq(aError);
        	break;
        case EMultimodePhoneTerminateActiveCalls:
            CompleteTerminateActiveCallsReq(aError);
            break;
      //DTMF
        case EMultimodePhoneSendDTMFTones:
        case EMultimodePhoneStartDTMFTone:
        case EMultimodePhoneStopDTMFTone:
			// forward to DTMF Tsy
			iMmDtmfTsy->Complete( aReqHandleType, aError );
            break;

	  //SS
        case EMultimodePhoneSetCallForwardingStatus:
        case EMultimodePhoneGetCallForwardingStatusPhase1:
        case EMultimodePhoneGetCallBarringStatusPhase1:
        case EMultimodePhoneSetCallBarringStatus:
        case EMultimodePhoneSetSSPassword:
        case EMultimodePhoneGetCallWaitingStatusPhase1:
        case EMultimodePhoneSetCallWaitingStatus:
        case EMultimodePhoneGetIdentityServiceStatus:
        case EMultimodePhoneSendNetworkServiceRequest:
        case EMultimodePhoneSendNetworkServiceRequestNoFdnCheck:
        case EMultimodePhoneGetIccMessageWaitingIndicators:
        case EMultimodePhoneSetIccMessageWaitingIndicators:
            iMmSupplServTsy->Complete( aReqHandleType, aError );
            break;

		//NET
        case EMultimodePhoneSelectNetwork:
            iMmNetTsy->CompleteSelectNetwork( aError );
            break;
        case EMultimodePhoneGetDetectedNetworksPhase1:
            // Catch possible leave. If this call leaves, there is nothing
            // we can do.
            TRAP_IGNORE( iMmNetTsy->ProcessGetDetectedNetworksPhase1L( NULL, 
            	aError ) );
            break;
        case EMultimodePhoneSetNWSelectionSetting:
            iMmNetTsy->CompleteSetNetworkSelectionSetting( aError );
            break;
        case EMultimodePhoneGetAuthorizationInfoPhase1:
        	TRAP_IGNORE( iMmNetTsy->ProcessGetAuthorizationInfoPhase1L( NULL,
        			aError ));
            break;
        case EMobilePhoneGetCurrentActiveUSimApplication:
            break;            

      //SECURITY
        case EMultimodePhoneGetLockInfo:
            lock = RMobilePhone::EStatusLockUnknown;
            setting = RMobilePhone::ELockSetUnknown;

            dataPackage.PackData(&lock, &setting );
            iMmSecurityTsy->CompleteGetLockInfo( &dataPackage,
                aError );
            break;
        case EMultimodePhoneAbortSecurityCode:
            iMmSecurityTsy->CompleteAbortSecurityCode( aError );
            break;
        case EMultimodePhoneVerifySecurityCode:
            TRAP( trapError,
            	  iMmSecurityTsy->CompleteVerifySecurityCodeL( aError ); );
            break;
        case EMultimodePhoneChangeSecurityCode:
            iMmSecurityTsy->CompleteChangeSecurityCode( aError );
            break;
        case EMultimodePhoneGetSecurityCodeInfoPin1:
        case EMultimodePhoneGetSecurityCodeInfoPin2:
        case EMultimodePhoneGetSecurityCodeInfoPuk1:
        case EMultimodePhoneGetSecurityCodeInfoPuk2:
        case EMultimodePhoneGetSecurityCodeInfoPhonePassword:
        case EMultimodePhoneGetSecurityCodeInfoSPC:
        case EMultimodePhoneGetSecurityCodeInfoPhonebookHiddenKey:
        case EMultimodePhoneGetSecurityCodeInfoUSIMAppPin:
        case EMultimodePhoneGetSecurityCodeInfoSecondUSIMAppPin:
        case EMultimodePhoneGetSecurityCodeInfoUniversalPin:
        case EMultimodePhoneGetSecurityCodeInfoUniversalPuk:
        	TimeoutGetSecurityCodeInfo(aReqHandleType, aError);
            break;

      //SS
        case EMultimodePhoneGetAlsLine:
            CompleteGetAlsLine( RMobilePhone::EAlternateLineNotAvailable,
                                aError );
            break;
        case EMultimodePhoneSetAlsLine:
            CompleteSetAlsLine( aError );
            break;
        //SIM access and capabilities
        case EMultimodePhoneGetServiceTable:
            TRAP_IGNORE( CompleteGetServiceTableL( 
            		emptyServiceTable, aError ); );
            break;
        case EMultimodePhoneGetCustomerServiceProfile:
            CompleteGetCustomerServiceProfile( emptyCspFile, aError );
            break;
        case EMultimodePhoneGetPhoneStoreInfo:
            CompleteGetPhoneStoreInfo( emptyPhoneStoreInfo, aError );
            break;
        case EMultimodePhoneIMSAuthenticate:
                CompleteImsAuthentication( NULL, aError );
             break;
        case EMultimodePhoneEnumerateAPNEntries:
            CompleteEnumerateAPNEntries( NULL, aError );
            break;
        case EMultimodePhoneGetAPNname:
            CompleteGetAPNname( NULL, aError );
            break;
        case EMultimodePhoneAppendAPNName:
            CompleteAppendAPNName( aError );
            break;
        case EMultimodePhoneDeleteAPNName:
            CompleteDeleteAPNName( aError );
            break;
        case EMultimodePhoneSetAPNControlListServiceStatus:
            CompleteSetAPNControlListServiceStatus( aError );
            break;
        case EMultimodePhoneGetAPNControlListServiceStatus:
            CompleteGetAPNControlListServiceStatus( NULL, aError );
            break;
        case EMultimodePhoneGetServiceProviderName:
            CompleteGetServiceProviderName( NULL, aError );            
            break;     
        case EMultimodePhoneGetFdnStatus:
            CompleteGetFdnStatus( NULL, aError );
            break;         
        default:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETE_2, "TSY: CMmPhoneTsy::Complete default handle type" );
            if ( iTsyReqHandleStore->GetTsyReqHandle(
                    aReqHandleType ))
            	{
            	ReqCompleted( iTsyReqHandleStore->ResetTsyReqHandle(
                    aReqHandleType ), aError );
            	}
            break;
        }

    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::TimeoutGetSecurityCodeInfo
// Sets the security code when the request completes due to timer expiration
// and then it calls the completion function to handle the timeout event.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::TimeoutGetSecurityCodeInfo(
		const TInt aReqHandleType, 
		const TInt aError)
	{
	CMmDataPackage dataPackage;
  	RMobilePhone::TMobilePhoneSecurityCode secCode;
   	RMobilePhone::TMobilePhoneSecurityCodeInfoV5 codeV5;

	switch (aReqHandleType)
		{
		case EMultimodePhoneGetSecurityCodeInfoPin1:
			secCode = RMobilePhone::ESecurityCodePin1;
			break;
        case EMultimodePhoneGetSecurityCodeInfoPin2:
        	secCode = RMobilePhone::ESecurityCodePin2;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoPuk1:
        	secCode = RMobilePhone::ESecurityCodePuk1;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoPuk2:
        	secCode = RMobilePhone::ESecurityCodePuk2;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoPhonePassword:
        	secCode = RMobilePhone::ESecurityCodePhonePassword;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoSPC:
        	secCode = RMobilePhone::ESecurityCodeSPC;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoPhonebookHiddenKey:
        	secCode = RMobilePhone::ESecurityHiddenKey;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoUSIMAppPin:
        	secCode = RMobilePhone::ESecurityUSIMAppPin;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoSecondUSIMAppPin:
        	secCode = RMobilePhone::ESecuritySecondUSIMAppPin;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoUniversalPin:
        	secCode = RMobilePhone::ESecurityUniversalPin;
        	break;
        case EMultimodePhoneGetSecurityCodeInfoUniversalPuk:
        	secCode = RMobilePhone::ESecurityUniversalPuk;
        	break;
		}
	dataPackage.PackData(&secCode, &codeV5 );

    iMmSecurityTsy->CompleteGetSecurityCodeInfo( 
        &dataPackage, aError );
	}

#endif


#ifdef TF_LOGGING_ENABLED
// ---------------------------------------------------------------------------
// CMmPhoneTsy::ReqCompleted
// Overloads CTelObject::ReqCompleted for logging purposes. It
// prints the aTsyReqHandle and aError variable in the log file and then
// calls CTelObject::ReqCompleted
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::ReqCompleted(
    const TTsyReqHandle aTsyReqHandle,
    const TInt aError  )
    {
OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_REQCOMPLETED_1, "TSY: CMmPhoneTsy::Request Completed. Handle:%d Error:%{TSymbianErrorCodes} Object:0x%08x", aTsyReqHandle, aError, this);

    CTelObject::ReqCompleted( aTsyReqHandle, aError );

OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_REQCOMPLETED_2, "<-- TSY: CMmPhoneTsy::ReqCompleted");
    }
#endif

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetWaitingCallForData
// Set iMmWaitingDataCall pointer to call object waiting to
// answer incoming call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetWaitingCallForData(
    CMmCallTsy* aMmCall )
    {
    iMmWaitingDataCall = aMmCall;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::WaitingCallForData
// Return WaitingCallForData pointer of call object wishing to
// answer incoming call
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmPhoneTsy::WaitingCallForData()
    {
    return iMmWaitingDataCall;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::ResetPacketDataSession
// Sets Packet Data pointer to NULL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::ResetPacketDataSession()
    {
    iMmPacketServiceTsy = NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::AirTimeTimerCheckStart
// Starts the air time duration timer when the first call is
// in active state. Increases the amount of active calls
// stored in the internal variable iAmountOfActiveCalls
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::AirTimeTimerCheckStart()
    {
    //check if any call is active already
    if ( 0 == iAmountOfActiveCalls )
        {
        //this is first call in active state, start the timer
        iAirTimeDurationTimer->StartAirTimeDuration();
        }
    //increase the count of active calls
    iAmountOfActiveCalls++;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::AirTimeTimerCheckStop
// Stops the air time duration timer when all calls are in idle
// state. Decreases the amount of active calls
// stored in the internal variable iAmountOfActiveCalls
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::AirTimeTimerCheckStop()
    {
    //check if any call is active
    if( iAmountOfActiveCalls > 0 )
        {
        //decrease the amount of active calls
        iAmountOfActiveCalls--;
        }

    //check that there are no calls active
    if ( 0 == iAmountOfActiveCalls )
        {
        iAirTimeDurationTimer->Stop();
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAirTimeDuration
// Gets the value of the air time duration timer
// set the number of beats of iAirTimeDurationTimer in aTime
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::GetAirTimeDuration( TTimeIntervalSeconds &aTime )
    {
    //number of beats can hold two possible values:
    //if at least one call is active it will return the current seconds of the
    //ongoing air time. If no call is active, it will return the amount of
    //seconds of the last air duration time

    aTime = iAirTimeDurationTimer->NumberOfBeats();
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSatMessaging
// Returns pointer to the SAT service interface.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MStkTsySatService* CMmPhoneTsy::GetSatMessaging()
    {
    return iTsySatMessaging;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetRfStateInfo
// Returns StateInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TRfStateInfo CMmPhoneTsy::GetRfStateInfo()
    {
    return iStateInfo;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetRfStateInfo
// Sets StateInfo
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetRfStateInfo( TRfStateInfo aStateInfo )
    {
    iStateInfo = aStateInfo;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetRfStatus
// Sets RfStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetRfStatus(
    TRfStatus aRfStatus )
    {
    iRfStatusFlag = aRfStatus;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetRfStatus
// Returns RfStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TRfStatus CMmPhoneTsy::GetRfStatus()
    {
    return iRfStatusFlag;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetNetTsy
// Returns NetTsy instance
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmNetTsy* CMmPhoneTsy::GetNetTsy()
    {
    return iMmNetTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetONStoreTsy
// ONStoreTsy can set iMmONStoreTsy pointer to NULL when it is closed
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetONStoreTsy(
        CMmONStoreTsy* aMmONStoreTsy
        )
    {
    iMmONStoreTsy = aMmONStoreTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetONStoreTsy
// Returns pointer to CMmONStoreTsy
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmONStoreTsy* CMmPhoneTsy::GetONStoreTsy()
    {
    return iMmONStoreTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetDtmfTsy
// Get pointer to DTMF Tsy.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmDtmfTsy* CMmPhoneTsy::GetDtmfTsy()
    {
    return iMmDtmfTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSupplServTsy
// Get pointer to Supplementary Services Tsy.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmSupplServTsy* CMmPhoneTsy::GetSupplServTsy()
    {
    return iMmSupplServTsy;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSecurityTsy
//  Returns SecurityTsy instance
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmSecurityTsy* CMmPhoneTsy::GetSecurityTsy()
    {
    return iMmSecurityTsy;
    }


// ---------------------------------------------------------------------------
// CMmPhoneTsy::IsRequestPossibleInOffLine
// Checks wether a ETel request can be performed or not while offline mode is
// enabled'
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::IsRequestPossibleInOffline(
    TInt aIpc )
    {
    // we assume here that most of the requests are possible, thefore
    // we switch for the not possible ones, optimizing a bit the checking.
    TBool isRequestPossible ( ETrue );
    
    switch ( aIpc )
        {
        //SS
        //case EMobilePhoneGetCallServiceCaps: //OK
        //case EMobilePhoneNotifyCallServiceCapsChange://OK
        case EMobilePhoneGetCallForwardingStatusPhase1://NO
        case EMobilePhoneGetCallForwardingStatusPhase2://NO
        case EMobilePhoneSetCallForwardingStatus://NO
        //case EMobilePhoneNotifyCallForwardingStatusChange: //OK
        //case EMobilePhoneNotifyCallForwardingActive://OK
        case EMobilePhoneGetBarringStatusPhase1://NO
        case EMobilePhoneGetBarringStatusPhase2://NO
        case EMobilePhoneSetCallBarringStatus://NO
        //case EMobilePhoneNotifyCallBarringStatusChange: //OK
        case EMobilePhoneSetSSPassword://NO
        case EMobilePhoneGetWaitingStatusPhase1://NO
        case EMobilePhoneGetWaitingStatusPhase2://NO
        case EMobilePhoneSetCallWaitingStatus://NO
        //case EMobilePhoneNotifyCallWaitingStatusChange://OK
        case EMobilePhoneGetIdentityServiceStatus://NO
        //case EMobilePhoneSendNetworkServiceRequest://OK
        case EMobilePhoneSendNetworkServiceRequestNoFdnCheck: //NO
        //case EMobilePhoneNotifyAllSendNetworkServiceRequest: //OK         
        //case EMobilePhoneGetIccMessageWaitingIndicators://OK          
        //case EMobilePhoneSetIccMessageWaitingIndicators://OK 
        //case EMobilePhoneNotifyIccMessageWaitingIndicatorsChange://OK

        //NET
        //case EMobilePhoneGetNetworkCaps: //ok
        //case EMobilePhoneGetNetworkRegistrationStatus: //ok
        //case EMobilePhoneNotifyNetworkRegistrationStatusChange: //ok
        //case EMobilePhoneGetCurrentMode: //ok
        //case EMobilePhoneNotifyModeChange: //ok
        //case EMobilePhoneGetCurrentNetwork://ok
        //case EMobilePhoneGetCurrentNetworkNoLocation://ok
        //case EMobilePhoneNotifyCurrentNetworkChange://ok
        // case EMobilePhoneNotifyCurrentNetworkNoLocationChange: ok
        //case EMobilePhoneGetHomeNetwork://ok
        case EMobilePhoneGetDetectedNetworksV2Phase1: //NO
        case EMobilePhoneGetDetectedNetworksV2Phase2: //NO
        //case EMobilePhoneGetNetworkSelectionSetting: //ok
        //case EMobilePhoneSetNetworkSelectionSetting: //ok
        //case EMobilePhoneNotifyNetworkSelectionSettingChange: //ok
        case EMobilePhoneSelectNetwork: //NO
        //case EMobilePhoneGetNITZInfo: //ok
        //case EMobilePhoneNotifyNITZInfoChange: //ok
        //case EMobilePhoneGetSignalCaps: //ok
        //case EMobilePhoneGetSignalStrength: //ok
        //case EMobilePhoneNotifySignalStrengthChange: //ok
        //case EMobilePhoneGetNetworkSecurityLevel://ok
        //case EMobilePhoneNotifyNetworkSecurityLevelChange: //ok
        //case EMobilePhoneGetCipheringIndicatorStatus://ok

        //SECURITY
        //case EMobilePhoneGetSecurityCaps: //ok
        //case EMobilePhoneNotifySecurityCapsChange: //ok
        //case EMobilePhoneGetLockInfo: //ok
        //case EMobilePhoneNotifyLockInfoChange://ok
        //case EMobilePhoneSetLockSetting://ok
        //case EMobilePhoneChangeSecurityCode://ok
        //case EMobilePhoneNotifySecurityEvent://ok
        //case EMobilePhoneVerifySecurityCode://ok
        //case EMobilePhoneAbortSecurityCode://ok
        //case EMobilePhoneGetSecurityCodeInfo://ok
        //case EMobilePhoneNotifySecurityCodeInfoChange://ok

        //MISC
        //case EMobilePhoneGetServiceProviderName: //ok
        //case EMobilePhoneGetIccAccessCaps: //ok
        //case EMobilePhoneNotifyIccAccessCapsChange: //ok
        //case EMobilePhoneGetCustomerServiceProfile: //ok
        //case EMobilePhoneGetServiceTable: //ok
        //case EMobilePhoneGetBatteryCaps://ok
        //case EMobilePhoneGetBatteryInfo: //ok
        //case EMobilePhoneNotifyBatteryInfoChange: //ok
        //case EMobilePhoneGetIdentityCaps://ok
        //case EMobilePhoneGetPhoneId://ok
        //case EMobilePhoneGetSubscriberId://ok
        //case EMobilePhoneGetALSLine://ok
        //case EMobilePhoneSetALSLine://ok
        //case EMobilePhoneNotifyALSLineChange: //ok
        //case EMobilePhoneGetFdnStatus: //ok
        //case EMobilePhoneSetFdnSetting: //ok
        //case EMobilePhoneNotifyFdnStatusChange: //ok
        //case EMobilePhoneGetMultimodeCaps: //ok
        //case EMobilePhoneGetPhoneStoreInfo: //ok
        //case EMobilePhoneNotifyIndicatorChange: //ok
        //case EMobilePhoneEnumerateAPNEntries: //ok
        //case EMobilePhoneGetAPNname: //ok
        //case EMobilePhoneAppendAPNName: //ok
        //case EMobilePhoneDeleteAPNName: //ok
        //case EMobilePhoneSetAPNControlListServiceStatus: //ok
        //case EMobilePhoneGetAPNControlListServiceStatus: //ok
        //case EMobilePhoneNotifyAPNListChanged: //ok
        //case EMobilePhoneNotifyAPNControlListServiceStatusChange: //ok

        // DTMF
        //case EMobilePhoneGetDTMFCaps: //ok
        //case EMobilePhoneNotifyDTMFCapsChange: //ok
        case EMobilePhoneSendDTMFTones: //NO
        case EMobilePhoneStartDTMFTone: //NO
        case EMobilePhoneStopDTMFTone: //NO
        case EMobilePhoneNotifyStopInDTMFString: //NO
        case EMobilePhoneContinueDTMFStringSending: //NO
        case EMobilePhoneNotifyDTMFEvent: //NO
        case EMobilePhoneReadDTMFTones: //NO
            {
            isRequestPossible = EFalse;
            break;
            }
            
        default:
            break;
        }

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_ISREQUESTPOSSIBLEINOFFLINE_1, "TSY: CMmPhoneTsy::IsRequestPossibleInOffline - IPC:%{TIPCNamesList}, ret:%d", aIpc, isRequestPossible);
    return isRequestPossible;
    }

// ----------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetCallForwardingNumber
// Completes when call forwarding number is ready to be written to P&S
// ----------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetCallForwardingNumber( 
    TInt /*aError*/, 
    const CMmDataPackage* aDataPackage )
    {
    //Copy data from package
    iCallForwardingNumber.Zero();
    aDataPackage->UnPackData( iCallForwardingNumber );

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETCALLFORWARDINGNUMBER_1, "TSY: CMmPhoneTsy::CompleteGetCallForwardingNumber number = %S ", iCallForwardingNumber);

    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::EnumerateAPNEntriesL
// Gets the total number of APN in access point control list (ACL)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::EnumerateAPNEntriesL( 
    const TTsyReqHandle aTsyReqHandle, 
    TUint32* aIndex )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_ENUMERATEAPNENTRIESL_1, "TSY: CMmPhoneTsy::EnumerateAPNEntries");

    // Check if request handle already exists
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePhoneEnumerateAPNEntries );
    
    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // The request is already in processing because of previous request.
        // Complete request with status value informing the client about 
        // the situation.
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        iRetAPNEntries = aIndex;

        // allow for EMultimodePhoneEnumerateAPNEntries immediate completion
#ifdef REQHANDLE_TIMER
        SetTypeOfResponse( EMultimodePhoneEnumerateAPNEntries, 
            aTsyReqHandle );
#else
        // Never comes here. See SetTypeOfResponse.
        iTsyReqHandleStore->SetTsyReqHandle( 
            EMultimodePhoneEnumerateAPNEntries, aTsyReqHandle );
#endif // REQHANDLE_TIMER

        //Send request to the Domestic OS layer.
        TInt ret = MessageManager()->HandleRequestL( 
            EMobilePhoneEnumerateAPNEntries );
        
        // DOS layer returned with error without completing request
        if ( KErrNone != ret )
            {
            //reset req handle
            iTsyReqHandleStore->ResetTsyReqHandle( 
                EMultimodePhoneEnumerateAPNEntries );
            ReqCompleted( aTsyReqHandle, ret );
            }
        }
        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::EnumerateAPNEntriesCancel
// Cancels an outstanding EnumerateAPNEntries request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::EnumerateAPNEntriesCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_ENUMERATEAPNENTRIESCANCEL_1, "CMmPhoneTsy::EnumerateAPNEntriesCancel");
    iRetAPNEntries = NULL;
    
    //reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneEnumerateAPNEntries );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
    	{
    	//complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteEnumerateAPNEntries
// Completes an outstanding asynchronous EnumerateAPNEntries request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteEnumerateAPNEntries( 
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEENUMERATEAPNENTRIES_1, "TSY: CMmPhoneTsy::CompleteEnumerateAPNEntries Error: %{TSymbianErrorCodes}", aErrorCode);

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneEnumerateAPNEntries );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            TUint32* apnEntries( NULL );
            aDataPackage->UnPackData( &apnEntries );
            *iRetAPNEntries = *apnEntries;
            
            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAPNnameL
// Get the name of APN in access point control list according to index
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetAPNnameL( 
    const TTsyReqHandle aTsyReqHandle, 
    TUint32* aIndex, 
    TDes8* aAPN )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETAPNNAMEL_1, "TSY: CMmPhoneTsy::GetAPNnameL");

    TInt ret = KErrServerBusy;
    
    // Check if request handle already exists
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePhoneGetAPNname );
    
    if ( EMultimodePhoneReqHandleUnknown == reqHandle )
        {
        if ( sizeof( RMobilePhone::TAPNEntryV3 ) > aAPN->MaxLength() )
            {
            ret = KErrArgument;
            }
        else
            {
            iRetAPNname = aAPN;
        
         // allow for EMobilePhoneGetAPNname immediate completion
#ifdef REQHANDLE_TIMER
            SetTypeOfResponse( EMultimodePhoneGetAPNname, aTsyReqHandle );
#else
            // Never comes here. See SetTypeOfResponse.
            iTsyReqHandleStore->SetTsyReqHandle( 
                EMultimodePhoneGetAPNname, aTsyReqHandle );
#endif // REQHANDLE_TIMER
        
            CMmDataPackage dataPackage;
    	    dataPackage.PackData( aIndex );
    	
            // Send request to the Domestic OS layer.
            ret = MessageManager()->HandleRequestL( 
                EMobilePhoneGetAPNname, &dataPackage );
            
            // In error case reset ReqHandle
            if ( KErrNone != ret )
                {
                //Reset req handle
                iTsyReqHandleStore->ResetTsyReqHandle( 
                    EMultimodePhoneGetAPNname );
                }
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAPNnameCancel
// Cancels an outstanding GetAPNname request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetAPNnameCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETAPNNAMECANCEL_1, "CMmPhoneTsy::GetAPNnameCancel");
    iRetAPNname = NULL;
    
    //reset the req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneGetAPNname );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
    	{
    	//complete with cancel
    	ReqCompleted( reqHandle, KErrCancel );
    	}
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetAPNname
// Completes an outstanding asynchronous GetAPNname request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetAPNname( 
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETAPNNAME_1, "TSY: CMmPhoneTsy::CompleteGetAPNname Error: %{TSymbianErrorCodes}", aErrorCode);
    
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneGetAPNname );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {

            // APN Entry
            RMobilePhone::TAPNEntryV3* apnEntryName;
            // Unpack data
            aDataPackage->UnPackData( &apnEntryName );
            
            RMobilePhone::TAPNEntryV3Pckg* apnEntryPckg = REINTERPRET_CAST( 
                RMobilePhone::TAPNEntryV3Pckg*, iRetAPNname );
            RMobilePhone::TAPNEntryV3& apnEntry = ( *apnEntryPckg )();
            
            apnEntry.iApn.Copy( apnEntryName->iApn );
 
            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aErrorCode );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetAlsState
// Sets ALS support state
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetAlsState( 
	TBool aState )
	{
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SETALSSTATE_1, "TSY: CMmPhoneTsy::SetAlsState Error: %d", aState);
	
	// Check where set was called
	if( iCspFileAlsSet )
	    {
	    // Set Csp file's ALS state
		iCspFileALS = aState;
	    }
	else
	    {
	    // Set ALS state
	    iAlsPpSupport = aState;
	    }
	
	// Check if ALS is supported
	if( iCspFileALS || iAlsPpSupport )
	    {
		//Set ALS status to ETrue in Central Repository
	    iCentRep->Set( KCtsyMEAlsSupported, ETrue );
	    }
	else
	    {
	    //Set ALS status to EFalse Central Repository
	    iCentRep->Set( KCtsyMEAlsSupported, EFalse );
	    }
	
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::MapMEALSLine
// Maps ALS information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneALSLine CMmPhoneTsy::MapMEALSLine( 
	TInt aAlsLineInfo )
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MAPMEALSLINE_1, "TSY: CMmPhoneTsy::MapMEALSLine");

	RMobilePhone::TMobilePhoneALSLine lineInfo;
	
	switch ( aAlsLineInfo )
		{
		case 0://TInt from CentRep
			lineInfo = RMobilePhone::EAlternateLinePrimary;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MAPMEALSLINE_2, "TSY: CMmPhoneTsy::MapMEALSInfo Line line = EAlternateLinePrimary");
			break;
		case 1://TInt from CentRep
			lineInfo = RMobilePhone::EAlternateLineAuxiliary;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MAPMEALSLINE_3, "TSY: CMmPhoneTsy::MapMEALSLine line = EAlternateLineAuxiliary");
			break;
		case 2://TInt from CentRep
			lineInfo = RMobilePhone::EAlternateLineUnknown;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MAPMEALSLINE_4, "TSY: CMmPhoneTsy::MapMEALSLine line = EAlternateLineUnknown");
			break;
		case 3://TInt from CentRep
			lineInfo = RMobilePhone::EAlternateLineNotAvailable;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MAPMEALSLINE_5, "TSY: CMmPhoneTsy::MapMEALSLine line = EAlternateLineNotAvailable");
			break;
		default:
			lineInfo = RMobilePhone::EAlternateLineUnknown;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_MAPMEALSLINE_6, "TSY: CMmPhoneTsy::MapMEALSLine line = EAlternateLineUnknown");
			break;
		}
		
	return lineInfo;
	}
	
// ---------------------------------------------------------------------------
// CMmPhoneTsy::ResetMEAlsInfo
// Resets ALS information in CentRep.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::ResetMEAlsInfo()
	{
	
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_RESETMEALSINFO_1, "TSY: CMmPhoneTsy::ResetMEAlsInfo");
	
	iCentRep->Reset();
	
	if( iSimCacheQueue && ( KCacheALSline & iSimCacheQueue ) )
	    { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_RESETMEALSINFO_2, "TSY: CMmPhoneTsy::ResetMEAlsInfo -> CompleteCacheSimL");
        // function can leave, so there for it must be trapped, the error 
        // value is ingnored, because the functionality doesn't depend on it
	    TRAP_IGNORE( CompleteCacheSimL( KCacheALSline, EFalse ) );	
	    }
	
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetCfisCenRep
// Returns pointer to CFIS central repository 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CRepository* CMmPhoneTsy::GetCfisCenRep()
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCFISCENREP_1, "TSY: CMmPhoneTsy::GetCfisCenRep");

	if( iCFISCentRep )
		{
		return iCFISCentRep;
		}
	else 
		return NULL;
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetMwisCenRep
// Returns pointer to MWIS central repository
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CRepository* CMmPhoneTsy::GetMwisCenRep()
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETMWISCENREP_1, "TSY: CMmPhoneTsy::GetMwisCenRep");

	if( iMWISCentRep )
		{
		return iMWISCentRep;
		}
	else 
		return NULL;
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetMwisCenRep
// Returns pointer to MWIS central repository
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CRepository* CMmPhoneTsy::GetCfisPRCenRep()
	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETCFISPRCENREP_1, "TSY: CMmPhoneTsy::GetCfisPRCenRep");

	if( iCFISPrivateCentRep )
		{
		return iCFISPrivateCentRep;
		}
	else 
		return NULL;
	}
			
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetSubscriberIdForTSYL
// Get IMSI for TSY internal use.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::GetSubscriberIdForTSYL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSUBSCRIBERIDFORTSYL_1, "TSY: CMmPhoneTsy::GetSubscriberIdForTSYL");
    
    // Check if some client has already requested this
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePhoneGetSubscriberId );
    
    if ( EMultimodePhoneReqHandleUnknown == reqHandle 
        && !iTSYSubscriberIdReq )
        {
        iTSYSubscriberIdReq = ETrue;
        iMessageManager->HandleRequestL( EMobilePhoneGetSubscriberId );
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETSUBSCRIBERIDFORTSYL_2, "TSY: CMmPhoneTsy::GetSubscriberIdForTSYL - Already requested");
        }
    }
	
// ---------------------------------------------------------------------------
// CMmPhoneTsy::AppendAPNNameL
// Append an entry to the access point name (APN) control list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::AppendAPNNameL( 
    RMobilePhone::TAPNEntryV3* aAPNEntry )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_APPENDAPNNAMEL_1, "TSY: CMmPhoneTsy::AppendAPNNameL");

    TInt ret ( KErrServerBusy );

    //check if handle is not in use already
    TTsyReqHandle phoneAppendAPNNameHandle =
                iTsyReqHandleStore->GetTsyReqHandle( 
                EMultimodePhoneAppendAPNName );       

    if ( EMultimodePhoneReqHandleUnknown == phoneAppendAPNNameHandle )
        {
        RMobilePhone::TAPNEntryV3* apnEntry = aAPNEntry;
        CMmDataPackage dataPackage;
        dataPackage.PackData( apnEntry );
        ret = MessageManager()->HandleRequestL( 
                                EMobilePhoneAppendAPNName, &dataPackage );
        if ( KErrNone == ret )
            {
            // Save the req handle type
            iReqHandleType = EMultimodePhoneAppendAPNName;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::AppendAPNNameCancel
// Cancel AppendAPNNameL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::AppendAPNNameCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_APPENDAPNNAMECANCEL_1, "TSY: CMmPhoneTsy::AppendAPNNameCancel");
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneAppendAPNName );
    
    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // complete
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteAppendAPNName
// Complete Append APN name request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteAppendAPNName( 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEAPPENDAPNNAME_1, "TSY: CMmPhoneTsy::CompleteAppendAPNName. Error: %{TSymbianErrorCodes}", aErrorCode);
    
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneAppendAPNName );
    
    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::DeleteAPNNameL
// Delete an entry from the access point name (APN) control list
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::DeleteAPNNameL( 
    const TTsyReqHandle aTsyReqHandle, 
    TUint32* aIndex )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DELETEAPNNAMEL_1, "TSY: CMmPhoneTsy::DeleteAPNNameL, index = %d", aIndex);

    // get the handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePhoneDeleteAPNName );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( aIndex );

        TInt ret = MessageManager()->HandleRequestL( 
            EMobilePhoneDeleteAPNName, &dataPackage );

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = EMultimodePhoneDeleteAPNName;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::DeleteAPNNameCancel
// Cancel DeleteAPNEntry
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::DeleteAPNNameCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_DELETEAPNNAMECANCEL_1, "TSY: CMmPhoneTsy::DeleteAPNNameCancel");
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneDeleteAPNName );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteDeleteAPNName
// Complete delete APN entry
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteDeleteAPNName( 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEDELETEAPNNAME_1, "TSY: CMmPhoneTsy::CompleteDeleteAPNName. Error: %{TSymbianErrorCodes}", aErrorCode);

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneDeleteAPNName );
    
    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyAPNListChanged
// This function is used to request to receive notification of when the APN 
// control list changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyAPNListChanged()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYAPNLISTCHANGED_1, "TSY: CMmPhoneTsy::NotifyAPNListChanged");

    iReqHandleType = EMultimodePhoneNotifyAPNListChange;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyAPNListChangedCancel
// This function cancels an outstanding access point name (APN) control list 
// change notification request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyAPNListChangedCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYAPNLISTCHANGEDCANCEL_1, "TSY: CMmPhoneTsy::NotifyAPNListChangedCancel");
    
    TTsyReqHandle reqHandle = iTsyReqHandleStore->
        ResetTsyReqHandle( EMultimodePhoneNotifyAPNListChange );

     if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyAPNListChanged
// Complete notification request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyAPNListChanged( 
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYAPNLISTCHANGED_1, "TSY: CMmPhoneTsy::CompleteNotifyAPNListChanged");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyAPNListChange );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // complete the client request
        ReqCompleted( reqHandle, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetAPNControlListServiceStatusL
// Enabling and disabling the access point name (APN) control list service
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::SetAPNControlListServiceStatusL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SETAPNCONTROLLISTSERVICESTATUSL_1, "TSY: CMmPhoneTsy::SetAPNControlListServiceStatusL.");

    iSetAclStatus = aAPNControlListServiceStatus;
    
    // get the handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePhoneSetAPNControlListServiceStatus );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        CMmDataPackage dataPackage;
        dataPackage.PackData( iSetAclStatus );

        TInt ret = MessageManager()->HandleRequestL( 
            EMobilePhoneSetAPNControlListServiceStatus, &dataPackage );

        if ( KErrNone != ret )
            {
            ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the req handle type
            iReqHandleType = EMultimodePhoneSetAPNControlListServiceStatus;
            }
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetAPNControlListServiceStatusCancel
// Set ACL Status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::SetAPNControlListServiceStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SETAPNCONTROLLISTSERVICESTATUSCANCEL_1, "TSY: CMmPhoneTsy::SetAPNControlListServiceStatusCancel");
    
    // erase pointer to client data
    iSetAclStatus = NULL;

    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneSetAPNControlListServiceStatus );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteSetAPNControlListServiceStatus
// Complete Set APN service status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteSetAPNControlListServiceStatus( 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETESETAPNCONTROLLISTSERVICESTATUS_1, "TSY: CMmPhoneTsy::CompleteSetAPNControlListServiceStatus. Error: %{TSymbianErrorCodes}", aErrorCode);

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneSetAPNControlListServiceStatus );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, aErrorCode );
        }
    iSetAclStatus = NULL;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAPNControlListServiceStatusL
// Get the status of the access point name (APN) control list service
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetAPNControlListServiceStatusL( 
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETAPNCONTROLLISTSERVICESTATUSL_1, "TSY: CMmPhoneTsy::GetAPNControlListServiceStatusL");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle( 
        EMultimodePhoneGetAPNControlListServiceStatus );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // save pointer to client data
        iRetAclStatus = aAPNControlListServiceStatus;

        // call DOS (no packed parameters)
        TInt ret = MessageManager()->HandleRequestL( 
            EMobilePhoneGetAPNControlListServiceStatus );

        // check the result of the call
        if ( KErrNone != ret )
            {
            ReqCompleted ( aTsyReqHandle, ret );
            }
        else
            {
            // Store the request handle
            iReqHandleType = EMultimodePhoneGetAPNControlListServiceStatus;
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAPNControlListServiceStatusCancel
// Cancel the request to get ACL status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::GetAPNControlListServiceStatusCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_GETAPNCONTROLLISTSERVICESTATUSCANCEL_1, "TSY: CMmPhoneTsy::GetAPNControlListServiceStatusCancel");
    // reset the reqhandle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneGetAPNControlListServiceStatus );

    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, KErrCancel );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteGetAPNControlListServiceStatus
// Complete Get APN service status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteGetAPNControlListServiceStatus( 
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETEGETAPNCONTROLLISTSERVICESTATUS_1, "TSY: CMmPhoneTsy::CompleteGetAPNControlListServiceStatus. Error %{TSymbianErrorCodes}", aErrorCode);
    
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneGetAPNControlListServiceStatus );
    
    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            RMobilePhone::TAPNControlListServiceStatus aclStatus;
            aDataPackage->UnPackData( aclStatus );
            
            // set the information for the client
            *iRetAclStatus = aclStatus;
            
            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aErrorCode );
            }

        // reset the internal variable
        iRetAclStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyAPNControlListServiceStatusChange
// This function is used to request to receive notification of when the 
// status of the APN control list service changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyAPNControlListServiceStatusChange( 
    RMobilePhone::TAPNControlListServiceStatus* aAPNControlListServiceStatus )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYAPNCONTROLLISTSERVICESTATUSCHANGE_1, "TSY: CMmPhoneTsy::NotifyAPNControlListServiceStatusChange");
    
    iRetNotifyAclStatus = aAPNControlListServiceStatus;
    iReqHandleType = EMultimodePhoneNotifyAPNControlListServiceStatusChange;
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyAPNControlListServiceStatusChangeCancel
// This function cancels an outstanding access point name (APN) control list 
// service status change notification request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneTsy::NotifyAPNControlListServiceStatusChangeCancel()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYAPNCONTROLLISTSERVICESTATUSCHANGECANCEL_1, "TSY: CMmPhoneTsy::NotifyAPNControlListServiceStatusChangeCancel");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyAPNControlListServiceStatusChange );
    
    
    if ( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        ReqCompleted( reqHandle, KErrCancel );
        iRetNotifyAclStatus = NULL;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyAPNControlListServiceStatusChange
// Complete service status notification
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyAPNControlListServiceStatusChange( 
    CMmDataPackage* aDataPackage, 
    TInt aErrorCode )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYAPNCONTROLLISTSERVICESTATUSCHANGE_1, "TSY: CMmPhoneTsy::CompleteNotifyAPNControlListServiceStatusChange");

    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodePhoneNotifyAPNControlListServiceStatusChange );

    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if ( KErrNone == aErrorCode )
            {
            RMobilePhone::TAPNControlListServiceStatus aclStatus;
            aDataPackage->UnPackData( aclStatus );
            
            // set the information for the client
            *iRetNotifyAclStatus = aclStatus;
            
            //Complete request to success
            ReqCompleted( reqHandle, KErrNone );
            }
        else
            {
            // Complete with error
            ReqCompleted( reqHandle, aErrorCode );
            }

        // reset the internal variable
        iRetNotifyAclStatus = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetAlsQueryInBoot
// Sets internal boolean, which indicates is als query done during boot.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetAlsQueryInBoot( TBool aInBoot )
    {
	iAlsQueryInBoot = aInBoot;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetAlsQueryInBoot
// Sets internal boolean, which indicates is als query done during boot.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::GetAlsQueryInBoot()
    {
	return iAlsQueryInBoot;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::GetHomeZoneParamsChecked
// Returns value of iViagHomeZoneParamsChecked.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::GetHomeZoneParamsChecked()
	{
	return iViagHomeZoneParamsChecked;
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetHomeZoneParamsChecked
// Sets internal boolean, which indicates is HomeZoneParams 
// query done in SimStatusReady
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::SetHomeZoneParamsChecked( TBool aHomeZoneChecked )
	{
	iViagHomeZoneParamsChecked = aHomeZoneChecked;
	}
	
// ---------------------------------------------------------------------------
// CMmPhoneTsy::CompleteNotifyCallDurationChange
// Complete notification when duration changes (every 10sec). Timer causes calling of
// this completion every 10 second. 
// ---------------------------------------------------------------------------
//
void CMmPhoneTsy::CompleteNotifyCallDurationChange()
    {
	TUint32 duration = KLifeTimeUpdateInterval;		
    CMmDataPackage dataPackage;
    dataPackage.PackData( &duration );

    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_COMPLETENOTIFYCALLDURATIONCHANGE_1, "TSY: CMmPhoneTsy::CompleteNotifyCallDurationChange - \duration: %d", duration);

#ifdef USING_CTSY_DISPATCHER	
    TRAP_IGNORE(iMessageManager->HandleRequestL( 
    				ECtsyUpdateLifeTimeReq, &dataPackage );
        ); 
#else	
    TRAP_IGNORE(iMessageManager->HandleRequestL( 
    				EMmTsyUpdateLifeTimeIPC, &dataPackage );
        ); 
#endif //USING_CTSY_DISPATCHER 
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneTsy::NotifyTelephonyAudioControlError
// Receives error notifications from Telephony audio control
// ---------------------------------------------------------------------------
//
#if (OST_TRACE_CATEGORY & OST_TRACE_CATEGORY_DEBUG)
void CMmPhoneTsy::NotifyTelephonyAudioControlError( const TName& aCallName, 
                                                    const TInt& aError )
    {       
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_NOTIFYTELEPHONYAUDIOCONTROLERROR_1, "TSY: CMmPhoneTsy::NotifyAudioControlError. Call name:%S, Error code %{TSymbianErrorCodes}(%d)", aCallName, aError, aError );
#else
void CMmPhoneTsy::NotifyTelephonyAudioControlError( const TName& /*aCallName*/, 
                                                    const TInt& /*aError*/ )
    {
#endif // 
    
    TInt callCount( iCallList->GetNumberOfObjects() );
    // find all the audio calls and hang up them all
    for ( TInt i = callCount; 0 < i; i-- )
        {
        CMmCallTsy* call = iCallList->GetMmCallById( i );
        if ( NULL != call )
            {
            if ( RMobilePhone::EVoiceService == call->CallMode() ||
                 RMobilePhone::EAuxVoiceService == call->CallMode() )
                {
                call->HangUp( CMmCallTsy::EMultimodeCallReqHandleUnknown );
                }
            }
        }

    }
	
// ---------------------------------------------------------------------------
// CMmPhoneTsy::SetPBInitActiveStatus
// Is PB intitialization active.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CMmPhoneTsy::SetPBInitActiveStatus( TBool aIsInitializationActive )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SETPBINITACTIVESTATUS_1, "TSY: CMmPhoneTsy::SetPBInitActiveStatus: - prev.status %u", (TUint)iIsInitializationActive);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SETPBINITACTIVESTATUS_2, "TSY: CMmPhoneTsy::SetPBInitActiveStatus: %u", (TUint)aIsInitializationActive);
    
    iIsInitializationActive = aIsInitializationActive;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::IsPBInitActive
// Is PB intitialization active.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CMmPhoneTsy::IsPBInitActive()
    {   
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_ISPBINITACTIVE_1, "TSY: CMmPhoneTsy::IsPBInitActive: %x", (TUint)iIsInitializationActive);
    
    return iIsInitializationActive;
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::IsHWRMSupported
// availability of Hareware Resource Manager in the current ROM
// Returns TTrue indicate HWRM is available. TFalse indicate it is unavailable
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::IsHWRMSupported()
	{
#ifdef __EABI__
	return KHWRMIsSupported != 0;
#else	// WINSCW
	    // for the emulator the SecureId of Master phone application is patched via the epoc32\data\epoc.ini file
		// for example, define
		// ctsy_KHWRMIsSupported 1
	TInt HWRMSupported = 0;
    UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,(TAny*)"ctsy_KHWRMIsSupported", &HWRMSupported);
    return HWRMSupported != 0;
#endif
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::IsSIMReady
// Is SIM ready for query.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmPhoneTsy::IsSIMReady()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_ISSIMREADY_1, "TSY: CMmPhoneTsy::IsSIMReady: %u", (TUint)iBootState.iSIMReady);
	return iBootState.iSIMReady;
	}

// ---------------------------------------------------------------------------
// CMmPhoneTsy::SaveFdnInfoDetails
// FDN details saved in case of FDN Pb not created but info received
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//   
void CMmPhoneTsy::SaveFdnInfoDetails( TInt aResult, CMmDataPackage* aDataPackage )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SAVEFDNINFODETAILS_1, "TSY: CMmPhoneTsy::SaveFdnInfoDetails aResult:  %{TSymbianErrorCodes}", aResult );

   // Store FDN info only in successfull case
    if ( KErrNone == aResult )
       {
       TPBFdnInfo pbFdnInfo;
       aDataPackage->UnPackData( pbFdnInfo );
       iPhoneBookState->iFDNNumOfEntries = pbFdnInfo.iFDNNumOfEntries;
       iPhoneBookState->iFDNNumberLengthMax = pbFdnInfo.iFDNNumberLengthMax;
       iPhoneBookState->iFDNTextLengthMax = pbFdnInfo.iFDNTextLengthMax;

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SAVEFDNINFODETAILS_2, "TSY: CMmPhoneTsy::SaveFdnInfoDetails - iFDNNumOfEntries: %d",iPhoneBookState->iFDNNumOfEntries );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SAVEFDNINFODETAILS_3, "TSY: CMmPhoneTsy::SaveFdnInfoDetails - iFDNNumberLengthMax: %d",iPhoneBookState->iFDNNumberLengthMax );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_SAVEFDNINFODETAILS_4, "TSY: CMmPhoneTsy::SaveFdnInfoDetails - iFDNTextLengthMax: %d",iPhoneBookState->iFDNTextLengthMax );
       }
    }

// ---------------------------------------------------------------------------
// CMmPhoneTsy::IsModemStatusReady
// Is modem status ready
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TBool CMmPhoneTsy::IsModemStatusReady() const
    {   
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_ISMODEMSTATUSREADY_1, "TSY: CMmPhoneTsy::IsModemStatusReady: %u", (TUint)iIsModemReady );
    
    return iIsModemReady;
    }
	
// ---------------------------------------------------------------------------
// CMmPhoneTsy::PhoneBookStoreInitCompleteL
// The phone book store initilization was complete
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
void CMmPhoneTsy::PhoneBookStoreInitCompleteL(TInt aError)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONETSY_PHONEBOOKSTOREINITCOMPLETEL_1, "TSY: CMmPhoneTsy::PhoneBookStoreInitCompleteL ");
    TTsyReqHandle reqHandle = iTsyReqHandleStore->GetTsyReqHandle(
        EMultimodePhoneGetMailboxNumbers );   
    if( EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        if( KErrNone != aError )
            {
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneGetMailboxNumbers );
            ReqCompleted(reqHandle, aError);
            return;
            }
        CMmDataPackage mailBoxDataPackage;
        mailBoxDataPackage.PackData( &iMailboxData );

        TInt ret = ( MessageManager()->HandleRequestL(
                EMobilePhoneGetMailboxNumbers, &mailBoxDataPackage ) );
        if(ret != KErrNone)
            {
            iTsyReqHandleStore->ResetTsyReqHandle( EMultimodePhoneGetMailboxNumbers );
            ReqCompleted(reqHandle, ret);
            }
        
        }
    if(GetONStoreTsy() != NULL)
        {
        GetONStoreTsy()->PhoneBookStoreInitCompleteL(aError);
        }
    }

//  End of File

