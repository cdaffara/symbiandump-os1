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



//  INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonegsmwcdmaextTraces.h"
#endif

#include <etelmm.h>
#include "cmmphonegsmwcdmaext.h"
#include "cmmphonetsy.h"
#include "cmmlinetsy.h"
#include "cmmcalltsy.h"
#include "cmmcalllist.h"
#include "CMmCommonStaticUtility.h"
#include "MmTsy_conf.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmsecuritytsy.h"


// ======== MEMBER FUNCTIONS ========

CMmPhoneGsmWcdmaExt::CMmPhoneGsmWcdmaExt()
    {
    }

CMmPhoneGsmWcdmaExt::~CMmPhoneGsmWcdmaExt()
    {
    if ( iCurrentNwInfo )
        {
        delete iCurrentNwInfo;
        }
    iCurrentNwInfo = NULL;

    if ( iCurrentLocationArea )
        {
        delete iCurrentLocationArea;
        }
    iCurrentLocationArea = NULL;

    if ( iGsmHomeNetwork )
        {
        delete iGsmHomeNetwork;
        }
    iGsmHomeNetwork = NULL;

    iMmPhoneTsy = NULL;
    iMessageManager = NULL;
    }

CMmPhoneGsmWcdmaExt* CMmPhoneGsmWcdmaExt::NewL(
        CMmPhoneTsy* aMmPhoneTsy,   
        CMmMessageManagerBase* aMessageManager )
    {
        
    CMmPhoneGsmWcdmaExt* phoneGsmWcdmaExt = new( ELeave )
        CMmPhoneGsmWcdmaExt();

    CleanupStack::PushL( phoneGsmWcdmaExt );
    phoneGsmWcdmaExt->iMmPhoneTsy = aMmPhoneTsy;
    phoneGsmWcdmaExt->iMessageManager = aMessageManager;
    phoneGsmWcdmaExt->ConstructL();
    CleanupStack::Pop();

    return phoneGsmWcdmaExt;
    
    }

void CMmPhoneGsmWcdmaExt::ConstructL()
    {

    iCurrentNwInfo  = new ( ELeave ) RMobilePhone::TMobilePhoneNetworkInfoV8;
    iCurrentLocationArea = new ( ELeave )
        RMobilePhone::TMobilePhoneLocationAreaV1;

    iGsmHomeNetwork = new ( ELeave ) RMobilePhone::TMobilePhoneNetworkInfoV8;
    iNitzInfo.iNitzFieldsUsed = 0x00;

    iAdnEnabled = EFalse;
    iAdnStatusFetched = EFalse;
    iFdnStatusFetched = EFalse;

    // empty buffer is used in various places when sending isi service request
    // into network
    iEmptyBuffer.Set( KNullDesC );

    //set initial security capability values
    iSecurityCaps = KMultimodeTsyGsmSecurityInitCaps;

    //set initial lock status and setting
    iPhoneDeviceStatus = RMobilePhone::EStatusLockUnknown;    
    iPhoneDeviceSetting = RMobilePhone::ELockSetUnknown;
    iICCStatus = RMobilePhone::EStatusLockUnknown;
    iICCSetting = RMobilePhone::ELockSetUnknown;
    iPhoneToICCStatus = RMobilePhone::EStatusLockUnknown;
    iPhoneToICCSetting = RMobilePhone::ELockSetUnknown; 

    // set initial card generation type to unknown
    iCurrentICCType = EICCTypeSimUnknown;

    //initialize password value
    iSecCode.Zero();
    //set intial value for flag about home network information
    iIsHomeNetworkInfoAvailable = EFalse;

    // below it's checked if all multimode api call services are supported 
    // by tsy
    #pragma warning( disable : 4127 )
    //Warning caused by configurational constants in if case. This is not 
    //really error but acts as a flag. This warning is purely informational as
    //can be seen from the explanation of Visual C++ help:
    //Compiler Warning (level 4) C4127
    //conditional expression is constant. The controlling expression of an 
    //if statement or while loop evaluated to a constant. As a result, the 
    //code in the body of the if statement or while loop either always 
    //executes or never executes. This warning is informational.
    if ( KVoice1LineSupported &&
        KDataLineSupported &&
        KFaxLineSupported &&
        KVoice2LineSupported &&
        KSmsMessagingSupported &&
        KGprsLineSupported )
        {
        iCallServicesSupportedAll = ETrue;
        }
    else
        {
        iCallServicesSupportedAll = EFalse;
        }
    #pragma warning( default : 4127 ) 

    }
    

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::InitSimL
// Called as part of the boot sequence. This method request the sim related 
// information required as part of the TSY's boot sequence
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::InitSimL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_INITSIML_1, "TSY: CMmPhoneGsmWcdmaExt::InitSimL");

    TInt ret( KErrGeneral ); 

    //get the home network information in boot-up
    iMessageManager->HandleRequestL( EMobilePhoneGetHomeNetwork );

    // Check whether is possible to disable PIN
    iMessageManager->HandleRequestL( EMmTsyPhoneGetPin1DisableSupportedIPC );

    // Register for SIM refresh (supported only in 3.0)
    iMmPhoneTsy->SimRefreshRegisterL();

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetNetworkCaps
// Gets static GSM network capabilities
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetNetworkCaps( 
        TUint32* aCaps )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETNETWORKCAPS_1, "TSY: CMmPhoneGsmWcdmaExt::GetNetworkCaps");

    *aCaps = RMobilePhone::KCapsGetRegistrationStatus 
       | RMobilePhone::KCapsNotifyRegistrationStatus
       | RMobilePhone::KCapsGetCurrentMode
       | RMobilePhone::KCapsNotifyMode
       | RMobilePhone::KCapsGetCurrentNetwork
       | RMobilePhone::KCapsNotifyCurrentNetwork
       | RMobilePhone::KCapsGetHomeNetwork
       | RMobilePhone::KCapsGetDetectedNetworks
       | RMobilePhone::KCapsManualNetworkSelection
       | RMobilePhone::KCapsNotifyNITZInfo
       | RMobilePhone::KCapsGetNITZInfo;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkV1
// Gets the current network's information (mode, status, etc) and the 
// location area, use the version 1 specified information fields
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkV1( 
        RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKV1_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkV1");

    if ( NULL != aNetInfo )
        {
        // version 1 information, copy the values
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;
        }
    if ( NULL != aArea )
        {
        // iCurrentLocationArea can't be NULL
        *aArea = *iCurrentLocationArea;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkV2
// Gets the current network's information (mode, status, etc) and the 
// location area, use the version 2 specified information fields
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkV2( 
        RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKV2_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkV2");

    if ( NULL != aNetInfo )
        {       
        // version 1 information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;  
        // extended version 2 information
        aNetInfo->iAccess = iCurrentNwInfo->iAccess;
        }
    if ( NULL != aArea )
        {
        // iCurrentLocationArea can't be NULL
        *aArea = *iCurrentLocationArea;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkV5
// Gets the current network's information (mode, status, etc) and the 
// location area, use the version 5 information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkV5( 
        RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKV5_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkV5" );

    if ( NULL != aNetInfo )
        {        
        // version 1 information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;  
        // extended version 2 information
        aNetInfo->iAccess = iCurrentNwInfo->iAccess;
        // extended version 5 information
        aNetInfo->iHsdpaAvailableIndicator = 
            iCurrentNwInfo->iHsdpaAvailableIndicator;
        aNetInfo->iEgprsAvailableIndicator = 
            iCurrentNwInfo->iEgprsAvailableIndicator;
        }
    if ( NULL != aArea )
        {
        // iCurrentLocationArea can't be NULL
        *aArea = *iCurrentLocationArea;
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetwork
// Gets the current network's information (mode, status, etc) and the 
// location area, use the default version 8 information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetwork( 
        RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo,
        RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORK_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetwork" );

    if ( NULL != aNetInfo )
        {        
        // version 1 information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;  
        // extended version 2 information
        aNetInfo->iAccess = iCurrentNwInfo->iAccess;
        // extended version 5 information
        aNetInfo->iHsdpaAvailableIndicator = 
            iCurrentNwInfo->iHsdpaAvailableIndicator;
        aNetInfo->iEgprsAvailableIndicator = 
            iCurrentNwInfo->iEgprsAvailableIndicator;
        // extended version 8 information
        aNetInfo->iHsupaAvailableIndicator =
        	iCurrentNwInfo->iHsupaAvailableIndicator;
        }
    if ( NULL != aArea )
        {
        // iCurrentLocationArea can't be NULL
        *aArea = *iCurrentLocationArea;
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV1
// Gets the current network's information (mode, status, etc), but not the 
// location area, use the version 1 specified information fields
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV1( 
        RMobilePhone::TMobilePhoneNetworkInfoV1* aNetInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKNOLOCATIONV1_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV1" );

    if ( NULL != aNetInfo )
        {      
        // version 1 network information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName; 
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV2
// Gets the current network's information (mode, status, etc), but not the 
// location area, use the version 2 specified info.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV2( 
        RMobilePhone::TMobilePhoneNetworkInfoV2* aNetInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKNOLOCATIONV2_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV2" );

    if ( NULL != aNetInfo )
        {       
        // version 1 information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;  
        // extended version 2 information
        aNetInfo->iAccess = iCurrentNwInfo->iAccess;
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV5
// Gets the current network's information (mode, status, etc), but not the 
// location area, use the version 5 information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV5( 
        RMobilePhone::TMobilePhoneNetworkInfoV5* aNetInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKNOLOCATIONV5_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocationV5" );
    
    if ( NULL != aNetInfo )
        {       
        // version 1 information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;  
        // extended version 2 information
        aNetInfo->iAccess = iCurrentNwInfo->iAccess;
        // extended version 5 information
        aNetInfo->iHsdpaAvailableIndicator = 
            iCurrentNwInfo->iHsdpaAvailableIndicator;
        aNetInfo->iEgprsAvailableIndicator = 
            iCurrentNwInfo->iEgprsAvailableIndicator;
        }
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocation
// Gets the current network's information (mode, status, etc), but not the 
// location area, use the default version 8 information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocation( 
        RMobilePhone::TMobilePhoneNetworkInfoV8* aNetInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCURRENTNETWORKNOLOCATION_1, "TSY: CMmPhoneGsmWcdmaExt::GetCurrentNetworkNoLocation" );
    
    if ( NULL != aNetInfo )
        {       
        // version 1 information
        aNetInfo->iMode = iCurrentNwInfo->iMode;
        aNetInfo->iStatus = iCurrentNwInfo->iStatus;
        aNetInfo->iBandInfo = iCurrentNwInfo->iBandInfo;
        aNetInfo->iCountryCode = iCurrentNwInfo->iCountryCode;
        aNetInfo->iCdmaSID = iCurrentNwInfo->iCdmaSID;
        aNetInfo->iAnalogSID = iCurrentNwInfo->iAnalogSID;
        aNetInfo->iNetworkId = iCurrentNwInfo->iNetworkId;
        aNetInfo->iDisplayTag = iCurrentNwInfo->iDisplayTag;
        aNetInfo->iShortName = iCurrentNwInfo->iShortName;
        aNetInfo->iLongName = iCurrentNwInfo->iLongName;  
        // extended version 2 information
        aNetInfo->iAccess = iCurrentNwInfo->iAccess;
        // extended version 5 information
        aNetInfo->iHsdpaAvailableIndicator = 
            iCurrentNwInfo->iHsdpaAvailableIndicator;
        aNetInfo->iEgprsAvailableIndicator = 
            iCurrentNwInfo->iEgprsAvailableIndicator;
        // extended version 8 information
        aNetInfo->iHsupaAvailableIndicator =
        	iCurrentNwInfo->iHsupaAvailableIndicator;
        }
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SetCurrentNetwork
// Updates the value of the internal attributes iCurrentNwInfo and 
// iCurrentLocationArea with new values
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SetCurrentNetwork( 
        const RMobilePhone::TMobilePhoneNetworkInfoV8&  aNetInfo,
        const RMobilePhone::TMobilePhoneLocationAreaV1& aArea  ) 
        
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_SETCURRENTNETWORK_1, "TSY: CMmPhoneGsmWcdmaExt::SetCurrentNetwork" );

    *iCurrentNwInfo = aNetInfo;
    *iCurrentLocationArea = aArea;

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetNITZInfo
// Gets current NITZ info  
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetNITZInfo( 
        RMobilePhone::TMobilePhoneNITZ* aNwtTimeInfo ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETNITZINFO_1, "TSY: CMmPhoneGsmWcdmaExt::GetNITZInfo" );

    TInt ret( KErrNotFound );
    
    // If time is valid, copy it to client side
    if ( 0 != iNitzInfo.iNitzFieldsUsed )
        {
        *aNwtTimeInfo = iNitzInfo;
        //MIE: is the code below absolutely necessary?
        //don't think so. Make sure about this.
        if ( 0 != iCurrentNwInfo->iShortName.Length() )
            {
            aNwtTimeInfo->iShortNetworkId.Copy( 
                iCurrentNwInfo->iShortName );

            // Short NW name is available
            aNwtTimeInfo->iNitzFieldsUsed |=
                RMobilePhone::KCapsShortNameAvailable;
            }
        if ( 0 != iCurrentNwInfo->iLongName.Length() )
            {
            aNwtTimeInfo->iLongNetworkId.Copy( 
                iCurrentNwInfo->iLongName );

            // Long NW name is available
            aNwtTimeInfo->iNitzFieldsUsed |=
                RMobilePhone::KCapsLongNameAvailable;
            }
        ret = KErrNone;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::ReceivedNITZInfo
// Updates internal attribute iNitzInfo with received NITZ info parameters 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneGsmWcdmaExt::ReceivedNITZInfo( 
        RMobilePhone::TMobilePhoneNITZ& aNITZInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_RECEIVEDNITZINFO_1, "TSY: CMmPhoneGsmWcdmaExt::ReceivedNITZInfo" );

    // This information is not necessarily available if network time 
    // indication is received only once in bootup, before registration to NW.
    if ( 0 != iCurrentNwInfo->iShortName.Length() )
        {
        aNITZInfo.iShortNetworkId.Copy( 
            iCurrentNwInfo->iShortName );

        // Short NW name is available
        aNITZInfo.iNitzFieldsUsed |= RMobilePhone::KCapsShortNameAvailable;
        }
    if ( 0 != iCurrentNwInfo->iLongName.Length() )
        {
        aNITZInfo.iLongNetworkId.Copy( 
            iCurrentNwInfo->iLongName );

        // Long NW name is available
        aNITZInfo.iNitzFieldsUsed |= RMobilePhone::KCapsLongNameAvailable;
        }

    // Then update info into internal member
    iNitzInfo = aNITZInfo;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCellInfoL
// Send a GetCellInfo request to the LTSY 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetCellInfoL() 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETCELLINFOL_1, "TSY: CMmPhoneGsmWcdmaExt::GetCellInfoL" );
    TInt ret = iMessageManager->HandleRequestL( ECtsyPhoneCellInfoReq );
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::NotifyCellInfoChangeL
// Ask LTSY to register a CellInfoChange notification 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::NotifyCellInfoChangeL()
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_NOTIFYCELLINFOCHANGEL_1, "TSY: CMmPhoneGsmWcdmaExt::NotifyCellInfoChangeL" );
    TInt ret = iMessageManager->HandleRequestL( ECtsyPhoneCellInfoIndReq );
    return ret;	
	}

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::CompleteNotifyNetworkRegistrationStatusChange
// Network registration status changed, check if we need to 
// update internal information, which happens if we drop out of network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneGsmWcdmaExt::CompleteNotifyNetworkRegistrationStatusChange(
        RMobilePhone::TMobilePhoneRegistrationStatus aRegistrationStatus )
            // New registration status from network
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_COMPLETENOTIFYNETWORKREGISTRATIONSTATUSCHANGE_1, "TSY: CMmPhoneGsmWcdmaExt::CompleteNotifyNetworkRegistrationStatusChange" );

    // If registration status not registered, then reset network information
    switch( aRegistrationStatus )
        {
        case RMobilePhone::ERegistrationUnknown:
        case RMobilePhone::ENotRegisteredNoService:
        case RMobilePhone::ENotRegisteredEmergencyOnly:
        case RMobilePhone::ENotRegisteredSearching:
        case RMobilePhone::ERegistrationDenied:
            iCurrentNwInfo->iCountryCode.FillZ( 0 );
            iCurrentNwInfo->iNetworkId.FillZ( 0 );
            iCurrentNwInfo->iShortName.FillZ( 0 );
            iCurrentNwInfo->iLongName.FillZ( 0 );
            iCurrentLocationArea->iAreaKnown = EFalse;
            iCurrentLocationArea->iLocationAreaCode = 0x0;
            iCurrentLocationArea->iCellId = 0x0;
            break;
        // all the other statuses are ok.
        case RMobilePhone::ERegisteredBusy:
        case RMobilePhone::ERegisteredOnHomeNetwork:
        case RMobilePhone::ERegisteredRoaming:
        default:
            break;
        } 
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetHomeNetworkV1
// Gets GSM home network information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetHomeNetworkV1(
        RMobilePhone::TMobilePhoneNetworkInfoV1* aHomeNetInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETHOMENETWORKV1_1, "TSY: CMmPhoneGsmWcdmaExt::GetHomeNetworkV1" );

    TInt ret( KErrNone );

    // If the home network info is available..
    if ( iIsHomeNetworkInfoAvailable )
        { 
        // version 1 information
        aHomeNetInfo->iMode = iGsmHomeNetwork->iMode;
        aHomeNetInfo->iStatus = iGsmHomeNetwork->iStatus;
        aHomeNetInfo->iBandInfo = iGsmHomeNetwork->iBandInfo;
        aHomeNetInfo->iCountryCode = iGsmHomeNetwork->iCountryCode;
        aHomeNetInfo->iCdmaSID = iGsmHomeNetwork->iCdmaSID;
        aHomeNetInfo->iAnalogSID = iGsmHomeNetwork->iAnalogSID;
        aHomeNetInfo->iNetworkId = iGsmHomeNetwork->iNetworkId;
        aHomeNetInfo->iDisplayTag = iGsmHomeNetwork->iDisplayTag;
        aHomeNetInfo->iShortName = iGsmHomeNetwork->iShortName;
        aHomeNetInfo->iLongName = iGsmHomeNetwork->iLongName;            
        }
    // If home network object has not been created, return suitable
    // error code so that phone can request information again.
    else 
        {
        ret = KErrNotFound;
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetHomeNetworkV2
// Gets GSM home network information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetHomeNetworkV2(
        RMobilePhone::TMobilePhoneNetworkInfoV2* aHomeNetInfo )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETHOMENETWORKV2_1, "TSY: CMmPhoneGsmWcdmaExt::GetHomeNetworkV2" );

    TInt ret( KErrNone );

    // If the home network info is available..
    if ( iIsHomeNetworkInfoAvailable )
        {
        // version 1 network information
        aHomeNetInfo->iMode = iGsmHomeNetwork->iMode;
        aHomeNetInfo->iStatus = iGsmHomeNetwork->iStatus;
        aHomeNetInfo->iBandInfo = iGsmHomeNetwork->iBandInfo;
        aHomeNetInfo->iCountryCode = iGsmHomeNetwork->iCountryCode;
        aHomeNetInfo->iCdmaSID = iGsmHomeNetwork->iCdmaSID;
        aHomeNetInfo->iAnalogSID = iGsmHomeNetwork->iAnalogSID;
        aHomeNetInfo->iNetworkId = iGsmHomeNetwork->iNetworkId;
        aHomeNetInfo->iDisplayTag = iGsmHomeNetwork->iDisplayTag;
        aHomeNetInfo->iShortName = iGsmHomeNetwork->iShortName;
        aHomeNetInfo->iLongName = iGsmHomeNetwork->iLongName;  
        // extended version 2 information
        aHomeNetInfo->iAccess = iGsmHomeNetwork->iAccess;          
        }
    // If home network object has not been created, return suitable
    // error code so that phone can request information again.
    else 
        {
        ret = KErrNotFound;
        }
        
    return ret; 
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetHomeNetworkV5
// Gets GSM home network information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetHomeNetworkV5(
        RMobilePhone::TMobilePhoneNetworkInfoV5* aHomeNetInfo )
        // pointer to HomeNetInfo
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETHOMENETWORKV5_1, "TSY: CMmPhoneGsmWcdmaExt::GetHomeNetworkV5" );

    TInt ret( KErrNone );

    // If the home network info is available..
    if ( iIsHomeNetworkInfoAvailable )
        {       
        // version 1 network information
        aHomeNetInfo->iMode = iGsmHomeNetwork->iMode;
        aHomeNetInfo->iStatus = iGsmHomeNetwork->iStatus;
        aHomeNetInfo->iBandInfo = iGsmHomeNetwork->iBandInfo;
        aHomeNetInfo->iCountryCode = iGsmHomeNetwork->iCountryCode;
        aHomeNetInfo->iCdmaSID = iGsmHomeNetwork->iCdmaSID;
        aHomeNetInfo->iAnalogSID = iGsmHomeNetwork->iAnalogSID;
        aHomeNetInfo->iNetworkId = iGsmHomeNetwork->iNetworkId;
        aHomeNetInfo->iDisplayTag = iGsmHomeNetwork->iDisplayTag;
        aHomeNetInfo->iShortName = iGsmHomeNetwork->iShortName;
        aHomeNetInfo->iLongName = iGsmHomeNetwork->iLongName;  
        // extended version 2 information
        aHomeNetInfo->iAccess = iGsmHomeNetwork->iAccess; 
        // extended version 5 information
        aHomeNetInfo->iHsdpaAvailableIndicator = 
            iGsmHomeNetwork->iHsdpaAvailableIndicator;
        aHomeNetInfo->iEgprsAvailableIndicator = 
            iGsmHomeNetwork->iEgprsAvailableIndicator;   
        }
    // If home network object has not been created, return suitable
    // error code so that phone can request information again.
    else 
        {
        ret = KErrNotFound;
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetHomeNetwork
// Gets GSM home network information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetHomeNetwork(
        RMobilePhone::TMobilePhoneNetworkInfoV8* aHomeNetInfo )
        // pointer to HomeNetInfo
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_GETHOMENETWORK_1, "TSY: CMmPhoneGsmWcdmaExt::GetHomeNetwork" );

    TInt ret( KErrNone );

    // If the home network info is available..
    if ( iIsHomeNetworkInfoAvailable )
        {       
        // version 1 network information
        aHomeNetInfo->iMode = iGsmHomeNetwork->iMode;
        aHomeNetInfo->iStatus = iGsmHomeNetwork->iStatus;
        aHomeNetInfo->iBandInfo = iGsmHomeNetwork->iBandInfo;
        aHomeNetInfo->iCountryCode = iGsmHomeNetwork->iCountryCode;
        aHomeNetInfo->iCdmaSID = iGsmHomeNetwork->iCdmaSID;
        aHomeNetInfo->iAnalogSID = iGsmHomeNetwork->iAnalogSID;
        aHomeNetInfo->iNetworkId = iGsmHomeNetwork->iNetworkId;
        aHomeNetInfo->iDisplayTag = iGsmHomeNetwork->iDisplayTag;
        aHomeNetInfo->iShortName = iGsmHomeNetwork->iShortName;
        aHomeNetInfo->iLongName = iGsmHomeNetwork->iLongName;  
        // extended version 2 information
        aHomeNetInfo->iAccess = iGsmHomeNetwork->iAccess; 
        // extended version 5 information
        aHomeNetInfo->iHsdpaAvailableIndicator = 
            iGsmHomeNetwork->iHsdpaAvailableIndicator;
        aHomeNetInfo->iEgprsAvailableIndicator = 
            iGsmHomeNetwork->iEgprsAvailableIndicator;   
        // extended version 8 information
        aHomeNetInfo->iHsupaAvailableIndicator = 
        	iGsmHomeNetwork->iHsupaAvailableIndicator;
        }
    // If home network object has not been created, return suitable
    // error code so that phone can request information again.
    else 
        {
        ret = KErrNotFound;
        }
        
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetFdnStatusL
// This method gets current fixed dialling numbers service status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetFdnStatusL()
    { 
    return iMessageManager->HandleRequestL( EMobilePhoneGetFdnStatus );
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SetFdnSetting
// This method sets current fixed dialling numbers service status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SetFdnSetting(
        RMobilePhone::TMobilePhoneFdnSetting* aFdnSetting ) 
    { 
    TInt ret( KErrNotFound );
    
    CMmDataPackage dataPackage;
    dataPackage.PackData( aFdnSetting );

    TRAPD( trapError,
        ret = iMessageManager->HandleRequestL(
            EMobilePhoneSetFdnSetting, &dataPackage );
        );
        
    if ( KErrNone != trapError )
        {
        ret = trapError;
        }
        
    return ret;

    }
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetServiceProviderNameL
// This method requests service provider name
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetServiceProviderNameL()
    {
    return iMessageManager->HandleRequestL(
        EMobilePhoneGetServiceProviderName );  
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetICCTypeL
// This method requests the type of SIM card in use (SIM/USIM)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetICCTypeL()
    {
    return iMessageManager->HandleRequestL( EMmTsySimGetICCType );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::CompleteGetICCType
// This method sets the received ICC type (SIM/USIM/Unknown) into 
// iCurrentICCType and requests the state of the PIN code (in USIM's case it
// can be either the PIN or the UPIN code)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneGsmWcdmaExt::CompleteGetICCType(
        CMmDataPackage* aDataPackage ) // data package
    {
    //unpack the data
    TICCType* aType = NULL; 
    aDataPackage->UnPackData( &aType );

    //set the new ICC type
    iCurrentICCType = *aType;

    // If the current ICC is USIM, first make a request for getting the active 
    // PIN code, after that we make another request whether the PIN is enabled 
    // or not. Othwerwise just request whether PIN is enabled. Can't do 
    // nothing if adaptation returns error so return value is not handled here
    SimActivePinState( );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::NotifySimStatusReady
// This method requests the DOS Tsy to inform the LTSY when the SIM is ready 
// so it's information is accessible.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::NotifySimStatusReadyL()
    {
    return iMessageManager->HandleRequestL( 
        EMmTsyBootNotifySimStatusReadyIPC );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::MtcStateQueryL
// This method requests the MTC status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::MtcStateQueryL()
    { 
    return iMessageManager->HandleRequestL ( EMmTsyBootGetRFStatusIPC );
    }
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::UpdateSecurityCaps
// Updates security capabilities based on received security Event
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::UpdateSecurityCaps( 
        RMobilePhone::TMobilePhoneSecurityEvent aEvent )
    {

    // Store old capabilities
    TUint32 oldSecurityCaps = iSecurityCaps;

    iSecurityCaps = 0;

    // Read capabilities
    switch( aEvent )
        {
        case RMobilePhone::ENoICCFound: 
        case RMobilePhone::EICCTerminated:
            // if these capabilities already exist, they remain in this case
            if ( oldSecurityCaps & RMobilePhone::KCapsLockPhone )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockPhone;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPhonePassword )
                {
                iSecurityCaps |= RMobilePhone::KCapsAccessPhonePassword;
                }
            break;
        case RMobilePhone::EPin1Required:
        case RMobilePhone::EPuk1Required:
        case RMobilePhone::EPin2Required:
        case RMobilePhone::EPuk2Required:
            // if these capabilities already exist, they may remain in this 
            // case
            if ( oldSecurityCaps & RMobilePhone::KCapsLockPhone )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockPhone;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPhonePassword )
                {
                iSecurityCaps |= RMobilePhone::KCapsAccessPhonePassword;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsLockPhoneToICC )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockPhoneToICC;
                }
            break;
        case RMobilePhone::EPhonePasswordRequired:
            // if these capabilities already exist, they may remain in this 
            // case
            if ( oldSecurityCaps & RMobilePhone::KCapsLockICC )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockICC;
                }
            // Access to PIN1
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPin1 )
                {
                iSecurityCaps |= RMobilePhone::KCapsAccessPin1;
                }
            // Access to PIN2
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPin2 )
                {
                iSecurityCaps |= RMobilePhone::KCapsAccessPin2;
                }
            break;
        case RMobilePhone::EPin1Verified:
        case RMobilePhone::EPin2Verified:
        case RMobilePhone::EPuk1Verified:
        case RMobilePhone::EPuk2Verified:
            // These capabilities exist for sure in this case
            iSecurityCaps = RMobilePhone::KCapsLockICC |
                            RMobilePhone::KCapsAccessPin1 |
                            RMobilePhone::KCapsAccessPin2;
            // if these capabilities already exist, they may remain in this
            // case
            if ( oldSecurityCaps & RMobilePhone::KCapsLockPhone )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockPhone;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPhonePassword )
                {
                iSecurityCaps |= RMobilePhone::KCapsAccessPhonePassword;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsLockPhoneToICC )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockPhoneToICC;
                }
            break;
        case RMobilePhone::EPhonePasswordVerified:
            // These capabilities exist for sure in this case
            iSecurityCaps = RMobilePhone::KCapsLockPhone |
                            RMobilePhone::KCapsAccessPhonePassword |
                            RMobilePhone::KCapsLockPhoneToICC;
            // If these capabilities already exist for sure in this case
            if ( oldSecurityCaps & RMobilePhone::KCapsLockICC )
                {
                iSecurityCaps |= RMobilePhone::KCapsLockICC;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPin1 )
                {
                iSecurityCaps |= RMobilePhone::KCapsAccessPin1;
                }
            if ( oldSecurityCaps & RMobilePhone::KCapsAccessPin2 )
                {
                iSecurityCaps|= RMobilePhone::KCapsAccessPin2;
                }
            break;
        default:
            break;
        //lint -e{788} don't warn about missing cases
        }

    // Additional caps if UPIN and UPUK are supported in SIM
    if ( iCurrentICCType == EICCTypeSim3G )
        {
        // Read capabilities
        switch( aEvent )
            {
            case RMobilePhone::EUniversalPinRequired:
            case RMobilePhone::EUniversalPukRequired: 
                // if these capabilities already exist, they may remain in 
                // this case
                if ( oldSecurityCaps & RMobilePhone::KCapsLockPhone )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsLockPhone;
                    }
                if ( oldSecurityCaps & 
                        RMobilePhone::KCapsAccessPhonePassword )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessPhonePassword;
                    }
                if ( oldSecurityCaps & RMobilePhone::KCapsLockPhoneToICC )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsLockPhoneToICC;
                    }
                break;
            case RMobilePhone::EPhonePasswordRequired:
                // Access to Universal PIN
                if ( oldSecurityCaps & RMobilePhone::KCapsAccessUniversalPin )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessUniversalPin;
                    }
                // Access to hidden key (phonebook)
                if ( oldSecurityCaps & RMobilePhone::KCapsAccessHiddenKey )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessHiddenKey;
                    }
                // Access to USIM app. PIN (files referenced in accesss rules)
                if ( oldSecurityCaps & RMobilePhone::KCapsAccessUSIMAppPin )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessUSIMAppPin;
                    }
                // Access to USIM app. PIN (files referenced in accesss rules)
                if ( oldSecurityCaps &
                     RMobilePhone::KCapsAccessUSIMAppSecondPin )
                    {
                    iSecurityCaps |=
                        RMobilePhone::KCapsAccessUSIMAppSecondPin;
                    }
                break;
            case RMobilePhone::EUniversalPinVerified:
            case RMobilePhone::EUniversalPukVerified:
                // These capabilities exist for sure in this case
                iSecurityCaps = RMobilePhone::KCapsLockICC |
                                RMobilePhone::KCapsAccessPin1 |
                                RMobilePhone::KCapsAccessPin2 |
                                RMobilePhone::KCapsAccessUniversalPin;
                // if these capabilities already exist,
                // they may remain in this case
                if ( RMobilePhone::KCapsLockPhone ==
                    ( oldSecurityCaps & RMobilePhone::KCapsLockPhone ) )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsLockPhone;
                    }
                if ( oldSecurityCaps & 
                        RMobilePhone::KCapsAccessPhonePassword )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessPhonePassword;
                    }
                if ( oldSecurityCaps & RMobilePhone::KCapsLockPhoneToICC )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsLockPhoneToICC;
                    }
                break;
            case RMobilePhone::EPhonePasswordVerified:
                // If these capabilities already exist,
                // they also remain in this case
                if ( oldSecurityCaps & RMobilePhone::KCapsAccessUniversalPin )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessUniversalPin;
                    }
                // Access to hidden key (phonebook)
                if ( oldSecurityCaps & RMobilePhone::KCapsAccessHiddenKey )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessHiddenKey;
                    }
                // Access to USIM app. PIN (files referenced in accesss rules)
                if ( oldSecurityCaps & RMobilePhone::KCapsAccessUSIMAppPin )
                    {
                    iSecurityCaps |= RMobilePhone::KCapsAccessUSIMAppPin;
                    }
                // Access to USIM app. PIN (files referenced in accesss rules)
                if ( oldSecurityCaps &
                     RMobilePhone::KCapsAccessUSIMAppSecondPin )
                    {
                    iSecurityCaps |=
                        RMobilePhone::KCapsAccessUSIMAppSecondPin;
                    }
                break;
            default:
                break;
            //lint -e{788} don't warn about missing cases
            } // end switch

        } // end if 3G SIM


    // Check if capabilities have changed
    if ( iSecurityCaps != oldSecurityCaps )
        {
        // Notify client about the change
        iMmPhoneTsy->GetSecurityTsy()->CompleteNotifySecurityCapsChange( 
            iSecurityCaps );
        }

    return KErrNone;
    
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::ChangeSecurityCodeL
// Request a security Change code to the DOS TSY layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::ChangeSecurityCodeL(
        const TDataPackage& aPackage ) 
    {
    return iMessageManager->HandleRequestL ( EMobilePhoneChangeSecurityCode,
        &aPackage );   
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::ChangeSecurityCode
// Request a security code verification to the DOS TSY layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::VerifySecurityCodeL(
        const TDataPackage& aPackage ) 
    {
    return iMessageManager->HandleRequestL( EMobilePhoneVerifySecurityCode,
        &aPackage );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::AbortSecurityCodeL
// Request aborting of an ongoing security related request to the DOS TSY
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::AbortSecurityCodeL(
        const TDataPackage& aPackage )
    {       
    return iMessageManager->HandleRequestL( EMobilePhoneAbortSecurityCode,
        &aPackage );        
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetSecurityCaps
// This function returns current security capabilities
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetSecurityCaps( 
        TUint32* aCaps )   
    {
    *aCaps = iSecurityCaps;

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::DeliverCode
// Copies the deliverd code into the internal attribute iSecCode
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::DeliverCode( 
        RMobilePhone::TCodeAndUnblockCode aCodes )    
    {
    iSecCode.Copy( aCodes.iCode );

OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_DELIVERCODE_1,  "TSY: CMmPhoneGsmWcdmaExt::DeliverCode iSecCode: %S", iSecCode );

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::DeliverCode
// Updates security lock info based on received security Event
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::UpdateLockInfo( 
        RMobilePhone::TMobilePhoneLockStatus aStatus,           
        RMobilePhone::TMobilePhoneLockSetting aSetting,         
        RMobilePhone::TMobilePhoneLock aLock )                  
    {

    TInt ret = KErrNotSupported;

    // Read settings and status
    switch( aLock )
        {
        case RMobilePhone::ELockPhoneDevice:
            if ( iPhoneDeviceSetting != aSetting )
                {
                ret = KErrNone;
                iPhoneDeviceSetting = aSetting;
                }
            if ( iPhoneDeviceStatus != aStatus )
                {
                ret = KErrNone;
                iPhoneDeviceStatus = aStatus;
                }
            break;
        case RMobilePhone::ELockICC:
            if ( iICCSetting != aSetting )
                {
                ret = KErrNone;
                iICCSetting = aSetting;
                }
            if ( iICCStatus != aStatus )
                {
                ret = KErrNone;
                iICCStatus = aStatus;
                }
            break;
        case RMobilePhone::ELockPhoneToICC:
            if ( iPhoneToICCSetting != aSetting )
                {
                ret = KErrNone;
                iPhoneToICCSetting = aSetting;
                }
            if ( iPhoneToICCStatus != aStatus )
                {
                ret = KErrNone;
                iPhoneToICCStatus = aStatus;
                }
            break;
        case RMobilePhone::ELockUniversalPin:
            if ( EICCTypeSim3G == iCurrentICCType )
                {
                // When multiapplication support added check here if UPIN
                // in use for this application before setting status/setting.
                // Do the same for all applications.
                if ( iICCSetting != aSetting )
                    {
                    ret = KErrNone;
                    iICCSetting = aSetting;
                    }
                if ( iICCStatus != aStatus )
                    {
                    ret = KErrNone;
                    iICCStatus = aStatus;
                    }
                }
            break;
        case RMobilePhone::ELockPhoneToFirstICC:
        case RMobilePhone::ELockOTA:
        default:
            break;
        //lint -e{788} don't warn about missing cases
        }

    return ret;
    
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SimActivePinState
// Requests the state of the currently active PIN from DOS TSY layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SimActivePinState()
    {
    
    TInt ret( KErrNone );
    
    TRAPD( trapError,
        ret = iMessageManager->HandleRequestL(
        EMmTsySecurityGetSimActivePinStateIPC );
        );
        
    if ( KErrNone != trapError )
        {
        ret = trapError;
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SetLockSettingL
// Requests the DOS tsy layer to set a new lock setting
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SetLockSettingL( 
        CMmDataPackage* aPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_SETLOCKSETTINGL_1, "LTSY: CMmPhoneGsmWcdmaExt::SetLockSettingL" );

    TLockAndSetting lockAndSetting;
    aPackage->UnPackData( &lockAndSetting.iLock, &lockAndSetting.iSetting );

    //check if there is a code available for this message
    //some lock settings requires code
    if ( iSecCode.Length() != 0 )
        {
        //re-package the original package and
        //include the code
        aPackage->PackData(&lockAndSetting, &iSecCode );
        }
    else 
        {
        //pack only the lock and the setting, the code will be null
        aPackage->PackData( &lockAndSetting );
        }
    TInt ret = iMessageManager->HandleRequestL( EMobilePhoneSetLockSetting,
            aPackage );

    //code is not required anymore, reset the value
    iSecCode.Zero();
    
    return ret;
    
    }     

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetStaticExtensionMode
// Returns the mode that this extension supports.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
TInt CMmPhoneGsmWcdmaExt::GetStaticExtensionMode(
        RMobilePhone::TMobilePhoneNetworkMode* aMode ) 
    {
        
    *aMode = RMobilePhone::ENetworkModeGsm;
    return KErrNone;
    
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SetNetworkModeInExtensionPhone
// Sets a network mode into iCurrentNwInfo.iMode variable.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneGsmWcdmaExt::SetNetworkModeInExtensionPhone(
        RMobilePhone::TMobilePhoneNetworkMode aMode ) 
    {
    iCurrentNwInfo->iMode = aMode;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt:: GetDetectedNetworksPhase1L
// Gets the list of available networks
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 

TInt CMmPhoneGsmWcdmaExt::GetDetectedNetworksPhase1L()
    {
    return iMessageManager->HandleRequestL(
        EMobilePhoneGetDetectedNetworksV2Phase1  );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt:: SelectNetworkL
// Selects a network either manually or automatically
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SelectNetworkL( 
        const TDataPackage& aDataPackage ) 
    {
    return iMessageManager->HandleRequestL( EMobilePhoneSelectNetwork,
        &aDataPackage );        
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt:: SelectNetworkCancelL
// Cancels an outgoing network selection
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SelectNetworkCancelL()
    {        
    return iMessageManager->HandleRequestL( EMobilePhoneSelectNetworkCancel );    
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt:: GetDetectedNetworksCancelL
// Cancels an outgoing available network search
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetDetectedNetworksCancelL() 
    {
    return iMessageManager->HandleRequestL(
        EMobilePhoneGetDetectedNetworksCancel );   
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt:: GetIccAccessCaps
// This methods gets the ICC Access capabilites
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetIccAccessCaps( 
        TUint32* aCaps ) //Ptr to the capabilites
    {
    //set the capabilites
    *aCaps = RMobilePhone::KCapsSimAccessSupported;

    return KErrNone;
    
    }

// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SetNetworkSelectionSettingL
// Sets a network selection setting (Automatic, manual)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::SetNetworkSelectionSettingL( 
        const TDataPackage& aDataPackage ) 
    {       
    return iMessageManager->HandleRequestL(
        EMobilePhoneSetNetworkSelectionSetting, &aDataPackage );   
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetNetworkRegistrationStatusL
// Gets the network registration status from the DOS TSY layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetNetworkRegistrationStatusL() 
    {        
    return iMessageManager->HandleRequestL(
        EMobilePhoneGetNetworkRegistrationStatus );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::CompleteGetHomeNetwork
// Completes a GetHomeNetwork request, saves the home network information into
// the internal attribute iGsmHomeNetwork and set the flag 
// iIsHomeNetworkInfoAvailable ON.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmPhoneGsmWcdmaExt::CompleteGetHomeNetwork( 
        CMmDataPackage* aDataPackage ) 
    {    
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEGSMWCDMAEXT_COMPLETEGETHOMENETWORK_1, "TSY: CMmPhoneGsmWcdmaExt::CompleteGetHomeNetwork" );

    RMobilePhone::TMobilePhoneNetworkInfoV8* 	homeInfo = NULL;
    aDataPackage->UnPackData ( &homeInfo );
    if ( KEtelExtMultimodeV8 == homeInfo->ExtensionId() )
    	{
    	*iGsmHomeNetwork = *homeInfo;
    	}
    else
    	{
    	// only in version 8
    	iGsmHomeNetwork->iHsupaAvailableIndicator = EFalse;

    	// Version 1 info
    	iGsmHomeNetwork->iMode = homeInfo->iMode;
    	iGsmHomeNetwork->iStatus = homeInfo->iStatus;
    	iGsmHomeNetwork->iBandInfo = homeInfo->iBandInfo;
    	iGsmHomeNetwork->iCountryCode = homeInfo->iCountryCode;
    	iGsmHomeNetwork->iCdmaSID = homeInfo->iCdmaSID;
    	iGsmHomeNetwork->iAnalogSID = homeInfo->iAnalogSID;
    	iGsmHomeNetwork->iNetworkId = homeInfo->iNetworkId;
    	iGsmHomeNetwork->iDisplayTag = homeInfo->iDisplayTag;
    	iGsmHomeNetwork->iShortName = homeInfo->iShortName;
    	iGsmHomeNetwork->iLongName = homeInfo->iLongName;            
    	
    	if ( KETelExtMultimodeV1 == homeInfo->ExtensionId() )
    		{
    		// Version 2 onward info default
    		iGsmHomeNetwork->iAccess = RMobilePhone::ENetworkAccessUnknown; 
    		iGsmHomeNetwork->iHsdpaAvailableIndicator = EFalse;
    		iGsmHomeNetwork->iEgprsAvailableIndicator = EFalse; 
    		}
    	else
    		{
    		// Version 2 info
    		iGsmHomeNetwork->iAccess =  homeInfo->iAccess;
    		if ( KETelExtMultimodeV2 == homeInfo->ExtensionId() )
    			{
    			// version 5 info default
        		iGsmHomeNetwork->iHsdpaAvailableIndicator = EFalse;
        		iGsmHomeNetwork->iEgprsAvailableIndicator = EFalse; 
    			}
    		else
    			{
    			// version 5 info
        		iGsmHomeNetwork->iHsdpaAvailableIndicator = homeInfo->iHsdpaAvailableIndicator;
        		iGsmHomeNetwork->iEgprsAvailableIndicator = homeInfo->iEgprsAvailableIndicator;
    			}
    		}
    	}
    	
    //Enable flag, home network information is available
    iIsHomeNetworkInfoAvailable = ETrue;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetNetworkSecurityLevel
// Gets cached network's security level (encription level).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneGsmWcdmaExt::GetNetworkSecurityLevel ( 
        RMobilePhone::TMobilePhoneNetworkSecurity& aNetworkSecurity ) const
            // network security paramater
    {        
    aNetworkSecurity = iNetworkSecurity;
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::SetNetworkSecurityLevel
// Sets cached network's security level (encription level)
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//        
TInt CMmPhoneGsmWcdmaExt::SetNetworkSecurityLevel( 
        RMobilePhone::TMobilePhoneNetworkSecurity& aNetworkSecurity )
    { 
    iNetworkSecurity = aNetworkSecurity;
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetCipheringIndicatorStatusL
// Gets the status of the ciphering indicator
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//             
TInt CMmPhoneGsmWcdmaExt::GetCipheringIndicatorStatusL( 
        const TDataPackage* aDataPackage ) 
    {
    return iMessageManager->HandleRequestL ( 
        EMobilePhoneGetCipheringIndicatorStatus, aDataPackage );
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::CurrentICCType
// This methods returns the currently active SIM card type
// (other items were commented in a header).
// ---------------------------------------------------------------------------
// 
TInt CMmPhoneGsmWcdmaExt::CurrentICCType()
    { 
    return iCurrentICCType;
    }
    
// ---------------------------------------------------------------------------
// CMmPhoneGsmWcdmaExt::GetLockInfoL
// Requests the information of a given security lock
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
TInt CMmPhoneGsmWcdmaExt::GetLockInfoL(
        const TDataPackage& aPackage ) 
    {
    return iMessageManager->HandleRequestL(
        EMobilePhoneGetLockInfo, &aPackage );
    }
    

//  End of File
