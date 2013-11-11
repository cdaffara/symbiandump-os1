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



//INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmnettsyTraces.h"
#endif

#include "cmmnettsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagemanagerbase.h"
#include "cmmphoneextinterface.h"
#include "cmmcalllist.h"
#include "cmmcalltsy.h"
#include "CMmCommonStaticUtility.h"
#include <ctsy/serviceapi/gsmerror.h>

// ======== MEMBER FUNCTIONS ========

CMmNetTsy* CMmNetTsy::NewL(    
    CMmPhoneTsy* aPhoneTsy )
    {
    CMmNetTsy* const mmNetTsy = new ( ELeave ) CMmNetTsy();
    CleanupStack::PushL( mmNetTsy );
    mmNetTsy->iMmPhoneTsy = aPhoneTsy;
    mmNetTsy->ConstructL();
    CleanupStack::Pop();
    return mmNetTsy;
    }

CMmNetTsy::CMmNetTsy()
    {
    }

void CMmNetTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_CONSTRUCTL_1, "TSY: CMmNetTsy::ConstructL");

    //set current NW mode
    iMode = RMobilePhone::ENetworkModeUnknown;

    //initialise network related attributes
    iGetDetectedNetworks = new ( ELeave ) 
                                        CArrayPtrFlat<CListReadAllAttempt>(1);
    iRetGetDetectedNwSize = NULL;
    iNWRegistrationStatus = RMobilePhone::ERegistrationUnknown;
    
	iAuthInfoBuffer = NULL;

    //Network selection setting
    iNWSelectionSettingMethod = RMobilePhone::ENetworkSelectionUnknown;

    //signal strength not available yet
    iSignalRssi = 0;

    //no signal Bar information available yet
    iSignalBars = -1;

    //flag for forcing ciphering indicator off (operator variant specific)
    iCipheringIndicatorForcedOff = EFalse;
    
    //read the ini file with the value of operator variant specific flag
    ReadVariantOptions();

    RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
    cellInfo.iMode = RMobilePhone::ECellModeUnknown;
    iCachedCellInfoV9.SetValue(cellInfo);
    iCachedCellInfoV9.SetCacheDirty();
    }
    
CMmNetTsy::~CMmNetTsy()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_DTOR_1, "TSY: CMmNetTsy::~CMmNetTsy");

    // Delete get detected networks list
    if ( iGetDetectedNetworks )
        {
        iGetDetectedNetworks->ResetAndDestroy();
        delete iGetDetectedNetworks;
        iGetDetectedNetworks = NULL;
        }

    if ( iAuthInfoBuffer ) 
        { 
        delete iAuthInfoBuffer;
        iAuthInfoBuffer = NULL;
        }
    
    // Closes the arrays and frees all memory allocated to the arrays.
    iNotifyCurrentNwRequests.Close();
    iNotifyCurrentNwNoLocationRequests.Close();
    iCellInfoRequests.Close();
    iNotifyCellInfoRequests.Close();
    
    // Destroy array        			
    iRegisterationStatusRequests.ResetAndDestroy();
    }
    
    
// ---------------------------------------------------------------------------
// CMmNetTsy::DoExtFunc
// NET-specific functionality of CMmPhoneTsy::DoExtFuncL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::DoExtFuncL( 
    const TTsyReqHandle aTsyReqHandle,  // Request handle
    const TInt aIpc, // IPC number of request          
    const TDataPackage& aPackage ) // Contains parameters for request
    {
    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_DOEXTFUNCL_1, "TSY: CMmNetTsy::DoExtFuncL. IPC:%{TIPCNamesList} Handle:%u Object:0x%08x", (TUint)aIpc, (TUint)aTsyReqHandle, (TUint)this);

    TInt ret ( KErrNone );

    TAny* dataPtr = aPackage.Ptr1();
    TAny* dataPtr2 = aPackage.Ptr2();

    switch ( aIpc )
        {
        // Get Network Caps
        case EMobilePhoneGetNetworkCaps:
            ret = GetNetworkCaps( aTsyReqHandle, 
                reinterpret_cast<TUint32*>( dataPtr ) );
            break;
        // Get Network Registration Status
        case EMobilePhoneGetNetworkRegistrationStatus:
            ret = GetNetworkRegistrationStatusL( aTsyReqHandle, 
              reinterpret_cast<RMobilePhone::TMobilePhoneRegistrationStatus*>( 
                dataPtr ) );
            break;
        // Notify Network Registration Status Change
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
            ret = NotifyNetworkRegistrationStatusChange(
              reinterpret_cast<RMobilePhone::TMobilePhoneRegistrationStatus*>( 
                dataPtr )); 
            break;
        // Get Current Network Mode
        case EMobilePhoneGetCurrentMode:
            ret = GetCurrentMode( aTsyReqHandle, 
                reinterpret_cast< RMobilePhone::TMobilePhoneNetworkMode*>( 
                dataPtr ) );
            break;
        // Notify Network Mode Change
        case EMobilePhoneNotifyModeChange:
            ret = NotifyModeChange(
                reinterpret_cast<RMobilePhone::TMobilePhoneNetworkMode*>( 
                dataPtr ) );
            break;
        // Get Current Network
        case EMobilePhoneGetCurrentNetwork:
            ret = GetCurrentNetwork( aTsyReqHandle, aPackage.Des1n(),
                reinterpret_cast<RMobilePhone::TMobilePhoneLocationAreaV1*>( 
                dataPtr2 ) );
            break;
        case EMobilePhoneGetNetworkName:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_DOEXTFUNCL_2, "TSY: CMmNetTsy::DoExtFuncL - EMobilePhoneGetNetworkName");
            ret = GetNetworkName( 
                aTsyReqHandle, aPackage.Des1n(), aPackage.Des2n() );
            break;
        // Get Current Network, no location
        case EMobilePhoneGetCurrentNetworkNoLocation:
            ret = GetCurrentNetworkNoLocation( 
                aTsyReqHandle, aPackage.Des1n() );
            break;
        // Get Cell information
        case EMobilePhoneGetCellInfo:
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_DOEXTFUNCL_3, "TSY: CMmNetTsy::DoExtFuncL - EMobilePhoneGetCellInfo");
        	ret = GetCellInfoL(aTsyReqHandle, aPackage.Des1n());
        	break;
        // Notify cell information change
        case EMobilePhoneNotifyCellInfoChange:
        	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_DOEXTFUNCL_4, "TSY: CMmNetTsy::DoExtFuncL - EMobilePhoneNotifyCellInfoChange");
        	ret = NotifyCellInfoChangeL(aTsyReqHandle, aPackage.Des1n());
        	break;
        // Notify Current Network Change, no location
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
            ret = NotifyCurrentNetworkNoLocationChangeL( 
                aTsyReqHandle, aPackage.Des1n() );
            break;
        // Notify Current Network Change
        case EMobilePhoneNotifyCurrentNetworkChange:
            ret = NotifyCurrentNetworkChangeL( aTsyReqHandle, aPackage.Des1n(), 
                reinterpret_cast<RMobilePhone::TMobilePhoneLocationAreaV1*>( 
                dataPtr2 ) );
            break;
        // Get Home Network 
        case EMobilePhoneGetHomeNetwork:
            ret = GetHomeNetwork( aTsyReqHandle, aPackage.Des1n() );
            break;
        // Get Detected Networks - Phase1
        case EMobilePhoneGetDetectedNetworksV2Phase1:
            ret = GetDetectedNetworksPhase1L( aTsyReqHandle, 
                reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ),
                reinterpret_cast<TInt*>( dataPtr2) );
            break;
        // Get Detected Networks - Phase2
        case EMobilePhoneGetDetectedNetworksV2Phase2:
            ret = GetDetectedNetworksPhase2( aTsyReqHandle, 
                reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ), 
                aPackage.Des2n() );
            break;            
        // Get Network Selection Setting
        case EMobilePhoneGetNetworkSelectionSetting:
            ret = 
                GetNetworkSelectionSetting( aTsyReqHandle, aPackage.Des1n());
            break;
        // Set Network Selection Setting
        case EMobilePhoneSetNetworkSelectionSetting:
            ret = SetNetworkSelectionSettingL( aTsyReqHandle, aPackage );
            break;
        // Notify Network Selection Setting Change
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
            ret = NotifyNetworkSelectionSettingChange( aPackage.Des1n() );
            break;
        // Select Network
        case EMobilePhoneSelectNetwork:
            ret = SelectNetworkL( aTsyReqHandle, aPackage );
            break;
        // Get NITZ information
        case EMobilePhoneGetNITZInfo:
            ret = GetNITZInfo( aTsyReqHandle, 
              reinterpret_cast<RMobilePhone::TMobilePhoneNITZ*>( dataPtr ) );
            break;
        // Notify NITZ Information Change
        case EMobilePhoneNotifyNITZInfoChange:
            ret = NotifyNITZInfoChange(
              reinterpret_cast<RMobilePhone::TMobilePhoneNITZ*>( dataPtr ) );
            break;
        // Get Signal Capabilities
        case EMobilePhoneGetSignalCaps:
            ret = GetSignalCaps( aTsyReqHandle, 
                reinterpret_cast<TUint32*> (dataPtr ) );
            break;
        // Get Signal Strength
        case EMobilePhoneGetSignalStrength:
            ret = GetSignalStrength( aTsyReqHandle, 
                reinterpret_cast<TInt32*>( dataPtr ),
                reinterpret_cast<TInt8*>( dataPtr2 ) );
            break;
        // Notify Change of Signal Strength
        case EMobilePhoneNotifySignalStrengthChange:
            ret = NotifySignalStrengthChange(
                reinterpret_cast<TInt32*>( dataPtr ),
                reinterpret_cast<TInt8*>( dataPtr2 ) );
            break;
        case EMobilePhoneGetNetworkSecurityLevel:
            ret = GetNetworkSecurityLevel( aTsyReqHandle,
                reinterpret_cast<RMobilePhone::TMobilePhoneNetworkSecurity*>(
                dataPtr ));
            break;
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
            ret = NotifyNetworkSecurityLevelChange(
                reinterpret_cast<RMobilePhone::TMobilePhoneNetworkSecurity*>(
                dataPtr ));
            break;
        case EMobilePhoneGetCipheringIndicatorStatus:
            ret = GetCipheringIndicatorStatusL( aTsyReqHandle, aPackage );
            break;
        case EMobilePhoneAuthorizationInfoPhase1:
            ret = GetAuthorizationInfoPhase1L( aTsyReqHandle,
                reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ),
                reinterpret_cast<TInt*>( dataPtr2 ) );
            break;
        case EMobilePhoneAuthorizationInfoPhase2:
            ret = GetAuthorizationInfoPhase2L( aTsyReqHandle,
                reinterpret_cast<RMobilePhone::TClientId*>( dataPtr ),
                aPackage.Des2n() );
            break;
        // Get Current Active USim Application 
        case EMobilePhoneGetCurrentActiveUSimApplication:
            ret = GetCurrentActiveUSimApplicationL( aTsyReqHandle,
                reinterpret_cast<RMobilePhone::TAID*>( dataPtr ) );
            break;                        
        default:
            ret = KErrNotSupported;
            break;
        }

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_DOEXTFUNCL_5, "TSY: CMmNetTsy::DoExtFuncL, error=%{TSymbianErrorCodes}", ret);
    return ret;

    }

// ---------------------------------------------------------------------------
// CMmNetTsy::CancelService
// Cancels NET requests
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::CancelService(    
    const TInt aIpc, // IPC number of request.
    const TTsyReqHandle aTsyReqHandle ) // Request handle of given request.
    {
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_CANCELSERVICE_1, "TSY: CMmNetTsy::CancelService IPC: %{TIPCNamesList} ret: %d", aIpc, aTsyReqHandle);
    TInt ret ( KErrNotSupported );
    
    //When the clients close their sub-sessions (eg. by calling RLine::Close), 
    //they may not have cancelled all their outstanding asynchronous requests 
    //before closing.  It is up to the ETel server to clean up in this 
    //situation, so the server will find the list of outstanding requests 
    //related to that sub-session object and pass these outstanding IPC 
    //request numbers, one at a time, to the CancelService method in the TSY.

    switch ( aIpc )
        {

        //folllowing methods are handled in CMmPhoneTsy::CancelService
        //case EMobilePhoneGetSignalStrength:
        //case EMobilePhoneGetCurrentNetwork:
        //case EMobilePhoneGetCurrentNetworkNoLocation:
        //case EMobilePhoneGetHomeNetwork:
        //case EMobilePhoneSetNetworkSelectionSetting:

        case EMobilePhoneNotifySignalStrengthChange:
            ret = NotifySignalStrengthChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyNetworkRegistrationStatusChange:
            ret = 
                NotifyNetworkRegistrationStatusChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyModeChange:
            ret = NotifyModeChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyCurrentNetworkChange:
            ret = NotifyCurrentNetworkChangeCancel ( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyCurrentNetworkNoLocationChange:
            ret = 
                NotifyCurrentNetworkNoLocationChangeCancel ( aTsyReqHandle );
            break;
        case EMobilePhoneGetCellInfo:
        	ret = GetCellInfoCancel(aTsyReqHandle);
        	break;
        case EMobilePhoneNotifyCellInfoChange:
        	ret = NotifyCellInfoChangeCancel(aTsyReqHandle);
        	break;
        case EMobilePhoneNotifyNetworkSelectionSettingChange:
            ret = NotifyNetworkSelectionSettingChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneGetNetworkRegistrationStatus:
            ret = GetNetworkRegistrationStatusCancel( aTsyReqHandle );
            break;
        case EMobilePhoneNotifyNITZInfoChange:
            ret = NotifyNITZInfoChangeCancel( aTsyReqHandle );
            break;
        case EMobilePhoneAuthorizationInfoPhase1:
            ret = GetAuthorizationInfoPhase1Cancel();
            break;
        case EMobilePhoneNotifyNetworkSecurityLevelChange:
        	ret = NotifyNetworkSecurityLevelChangeCancel();
        	break;
        case EMobilePhoneGetCurrentActiveUSimApplication:
            ret = GetCurrentActiveUSimApplicationCancel();
            break;
        case EMobilePhoneGetCipheringIndicatorStatus:
            ret = GetCipheringIndicatorStatusCancel();
            break;
        //the following methods has to be 
        //cancelled in the the DOS side as well.
        case EMobilePhoneGetDetectedNetworksV2Phase1:
        case EMobilePhoneSelectNetwork:
            {
            // Ensure the ReqHandleType is unset.
            // This will detect cases where this method indirectly calls itself
            // (e.g. servicing a client call that causes a self-reposting notification to complete and thus repost).
            // Such cases are not supported because iReqHandleType is in the context of this class instance,
            // not this request, and we don't want the values set by the inner request and the outer request
            // interfering with each other.
            __ASSERT_DEBUG(iMmPhoneTsy->iReqHandleType==CMmPhoneTsy::EMultimodePhoneReqHandleUnknown, User::Invariant());
            
            TInt trapError( KErrNone );

            //call cancel handling
            TRAP( trapError, (ret = CancelToDosL( aIpc )));

            if ( KErrNone != trapError )
                {
                //Store trap error to ret.
                ret = trapError;                
                }
            
            if ( KErrNone != ret )
                {
                //Remove request handle from store because it can not be used
                //after the request is completed.
                switch ( aIpc )
                    {
                    case EMobilePhoneGetDetectedNetworksV2Phase1:
                        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
                            CMmPhoneTsy::EMultimodePhoneGetDetectedNetworksPhase1 );                    
                        break;
                    case EMobilePhoneSelectNetwork:
                        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
                            CMmPhoneTsy::EMultimodePhoneSelectNetwork );
                        break;
                    default:
                        //Do nothing.
                        break;
                    }
                
                iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
                
                //Request is completed. Return KErrNone to ETel server.
                ret = KErrNone;
                } 

            //save request handle
           if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
               iMmPhoneTsy->iReqHandleType )
                {
                iMmPhoneTsy->iTsyReqHandleStore->SetTsyReqHandle( 
                    iMmPhoneTsy->iReqHandleType, aTsyReqHandle );
                
                // We've finished with this value now. Clear it so it doesn't leak
                //  up to any other instances of this method down the call stack
                iMmPhoneTsy->iReqHandleType=CMmPhoneTsy::EMultimodePhoneReqHandleUnknown;
                }
            break;
            }    
        //Default case
        default:
            ret = KErrNone; 
            break;
        }

    return ret;

    }
// ---------------------------------------------------------------------------
// CMmNetTsy::CancelToDosL
// Forwards a cancel request to the DOS layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::CancelToDosL(    
    const TInt aIpc ) // IPC number of request.
    {
    TInt ret ( KErrNone );

    switch ( aIpc )
        {
        case EMobilePhoneGetDetectedNetworksV2Phase1:
            ret = iMmPhoneTsy->MessageManager()->HandleRequestL( 
                EMobilePhoneGetDetectedNetworksCancel );
            break;

        case EMobilePhoneSelectNetwork:
            ret = iMmPhoneTsy->MessageManager()->HandleRequestL( 
                EMobilePhoneSelectNetworkCancel );
            break;

        default:
            break;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetSignalCaps
// Returns the signal strength and BER capabilities of the phone
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetSignalCaps(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    TUint32* aCaps ) const // pointer to the capabilities     
    {
    *aCaps = KMmTsySignalCaps;

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetSignalStrength
// Returns current signal strength and signal bars
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetSignalStrength(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    TInt32* aSignalStrength, // signal strength
    TInt8* aBar ) const // percentage of bars (0-100%)
    {
    TInt ret( KErrNotFound );

    // Signal strength is not available yet when value is 0.
    // Signal bars has no information available when value is -1,
    //  0 is one of the signal bar values.
    if ( 0 != iSignalRssi || -1 != iSignalBars )
        {
        *aSignalStrength = iSignalRssi;
        //this is currently a value between 0-100, not absolute number of bars
        *aBar = iSignalBars;
        ret = KErrNone;
        }

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifySignalStrengthChange
// Notifies the client if the signal strength changes
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifySignalStrengthChange(
    TInt32* aSignalStrength, // signal strength
    TInt8* aBar ) // absolute number of bars
    {
    iMmPhoneTsy->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifySignalStrengthChange;

    //Save the address of client's status variable for future use
    iRetSignalRssi = aSignalStrength; 
    iRetSignalBars = aBar; 

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifySignalStrengthChangeCancel
// Cancels an outstanding NotifySignalStrengthChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifySignalStrengthChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) // Tsy request handle
    {
    //As cancel is called, the client memory address pointed to is no longer 
    //valid -> NULL it
    iRetSignalRssi = NULL;
    iRetSignalBars = NULL;
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySignalStrengthChange );

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifySignalStrengthChange
// Completes NotifySignalStrengthChange to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifySignalStrengthChange(
    CMmDataPackage* aDataPackage ) // Contains information for client side
    {
    aDataPackage->UnPackData ( iSignalBars, iSignalRssi );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifySignalStrengthChange );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Update the client side data through pointers
        *iRetSignalRssi = iSignalRssi;
        *iRetSignalBars = iSignalBars;

        // Null pointers as notification is completed!
        // THIS HAS TO BE DONE BEFORE REQCOMPLETED!!
        iRetSignalRssi = NULL;
        iRetSignalBars = NULL;

        iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetNITZInfo
// Returns the current time information received from network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNITZInfo(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    RMobilePhone::TMobilePhoneNITZ* aNWTimeInfo ) const // NW time info 
    {
    TInt ret( iMmPhoneTsy->iMmPhoneExtInterface->GetNITZInfo( aNWTimeInfo ) );
    
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
   
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNITZInfoChange
// Notifies client when network time and date info changes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNITZInfoChange(
    RMobilePhone::TMobilePhoneNITZ* aNWTimeInfo ) // Ptr to NW time info
    {
    iMmPhoneTsy->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyNWTimeInfoChange;

    //Save the address of client's status variable for future use
    iRetNWTimeInfo = aNWTimeInfo; 

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNITZInfoChangeCancel
// Cancels an outstanding NotifyNITZInfoChange request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNITZInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) // Tsy request handle
    {
    //As cancel is called, the client memory address pointed to is no longer 
    // valid -> NULL it
    iRetNWTimeInfo = NULL; 
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyNWTimeInfoChange );

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifyNITZInfoChange
// Completes NotifyNITZInfoChange to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifyNITZInfoChange(
    CMmDataPackage* aDataPackage ) // Contains information for client side
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyNWTimeInfoChange );

    RMobilePhone::TMobilePhoneNITZ* nwTimeInfo = NULL;

    aDataPackage->UnPackData ( &nwTimeInfo );

    //notify the extension about the change
    iMmPhoneTsy->iMmPhoneExtInterface->ReceivedNITZInfo( *nwTimeInfo );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        *iRetNWTimeInfo = *nwTimeInfo;

        // THIS HAS TO BE DONE BEFORE REQCOMPLETED!!
        iRetNWTimeInfo = NULL;

        iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetCurrentMode
// Returns information about the current network mode to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCurrentMode(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePhoneNetworkMode* aNWMode ) const   
    {
    *aNWMode = iMode;

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyModeChange
// Notifies client of changes in the current network mode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyModeChange(
    RMobilePhone::TMobilePhoneNetworkMode* aNWMode )
    {
    iMmPhoneTsy->iReqHandleType = 
       CMmPhoneTsy::EMultimodePhoneNotifyModeChange;

    iRetNWMode = aNWMode;

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyModeChangeCancel
// Cancels an outstanding NotifyModeChange method.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyModeChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    // As cancel is called, the client memory address pointed to is no longer 
    // valid -> NULL it
    iRetNWMode = NULL; 
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyModeChange );
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifyModeChange
// Completes NotifyModeChange to client side. Updates the TSY's internal state
// according to the new mode.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifyModeChange(
    CMmDataPackage* aDataPackage, // Contains information for client side
    TInt aResult ) // success code 
    {
    //unpack data
    RMobilePhone::TMobilePhoneNetworkMode* mode = NULL;
    aDataPackage->UnPackData ( &mode );

    //continue only if mode has changed
    if ( iMode != *mode )
        {
        //save the mode in the phone
        iMode = *mode;
            
        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle =
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneNotifyModeChange );

        //check if any client is interested in notification
        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {        
            //set the mode for return
            *iRetNWMode = *mode;
            // Null ret pointer
            iRetNWMode = NULL;
            // Complete the client request
            iMmPhoneTsy->ReqCompleted( reqHandle, aResult );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkSelectionSetting
// Returns the current network selection setting.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNetworkSelectionSetting(
    const TTsyReqHandle aTsyReqHandle,  
    TDes8* aSettingPckg ) 
    {
    TInt ret = KErrArgument;
    
    if ( sizeof( RMobilePhone::TMobilePhoneNetworkSelectionV1 ) == 
            aSettingPckg->MaxLength() )
        {
        TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1>* networkSettingPckg = 
            ( TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1>* ) aSettingPckg;
        RMobilePhone::TMobilePhoneNetworkSelectionV1& setting = 
            ( *networkSettingPckg )();

        // Set the common information
        setting.iMethod = iNWSelectionSettingMethod;
        // set the CDMA information to default values
        setting.iBandClass = RMobilePhone::ENetworkBandClassUnknown;
        setting.iOperationMode = RMobilePhone::ENetworkOperationUnknown;
   
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
        
        ret = KErrNone;
        }
    
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::SetNetworkSelectionSettingL
// Sets a new network selection setting
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::SetNetworkSelectionSettingL(
    const TTsyReqHandle aTsyReqHandle, // request handle
    const TDataPackage& aDataPackage ) // new setting mode
    {
    TDes8* settingPckg = aDataPackage.Des1n ();
    
    if ( sizeof( RMobilePhone::TMobilePhoneNetworkSelectionV1 ) > 
            settingPckg->MaxLength() )
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrArgument );
        return KErrNone;
        }
    
    TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1>* networkSettingPckg = 
        ( TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1>* )settingPckg;
    RMobilePhone::TMobilePhoneNetworkSelectionV1& setting = 
        ( *networkSettingPckg )();
    
    TInt ret( KErrNone );

    // If unknown selection, return KErrNotSupported
    if ( setting.iMethod == RMobilePhone::ENetworkSelectionUnknown )
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNotSupported );
        }
    //proceed only if value is changed
    else if ( setting.iMethod != iNWSelectionSettingMethod )
        {
        //forward request to the extension
        ret = iMmPhoneTsy->iMmPhoneExtInterface->SetNetworkSelectionSettingL( 
             aDataPackage );

        // And handle error conditions here
        if ( KErrNone != ret )
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            iMmPhoneTsy->iReqHandleType = 
                CMmPhoneTsy::EMultimodePhoneSetNWSelectionSetting;
            }
        }
    // Client requested to change to the same setting, complete request.
    else
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
        }
        
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteSetNetworkSelectionSetting
// Completes SetNetworkSelectionSetting to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteSetNetworkSelectionSetting( 
    TInt aSuccessCode ) 
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle setNetworkSelectionSettingHandle = 
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSetNWSelectionSetting );

    //check the availability of the handler    
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
         setNetworkSelectionSettingHandle )
        {
        iMmPhoneTsy->ReqCompleted(
            setNetworkSelectionSettingHandle, aSuccessCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNetworkSelectionSettingChange
// Notifies client of changes in the network selection setting.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNetworkSelectionSettingChange(
    TDes8* aSettingPckg ) // pointer to the setting
    {
    TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1>* networkSettingPckg = 
        ( TPckg<RMobilePhone::TMobilePhoneNetworkSelectionV1>* ) aSettingPckg;
    RMobilePhone::TMobilePhoneNetworkSelectionV1& setting = 
        ( *networkSettingPckg )();
    
     //save the handle and the request type
    iMmPhoneTsy->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyNWSelectionSettingChange;

    //Save the address of client's status variable for future use
    iRetNWSelectionSetting = &setting; 

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNetworkSelectionSettingChangeCancel
// Cancels an outstanding NotifyNetworkSelectionSettingChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNetworkSelectionSettingChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) // Tsy request handle
    {
    // As cancel is called, the client memory address pointed to is no longer 
    // valid -> NULL it
    iRetNWSelectionSetting = NULL;
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
        CMmPhoneTsy::EMultimodePhoneNotifyNWSelectionSettingChange );
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifyNetworkSelectionSettingChange
// Completes a NotifyNetworkSelectionSettingChange request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifyNetworkSelectionSettingChange(
    CMmDataPackage* aDataPackage ) // Contains information for client side
    {
    RMobilePhone::TMobilePhoneNetworkSelectionV1* settings = NULL;

    aDataPackage->UnPackData( &settings );

    // continue only if the new selection is different than the old one
    if ( (*settings).iMethod != iNWSelectionSettingMethod )
        {
        iNWSelectionSettingMethod = (*settings).iMethod;
        // set the CDMA information to default values
        (*settings).iBandClass = RMobilePhone::ENetworkBandClassUnknown;
        (*settings).iOperationMode = RMobilePhone::ENetworkOperationUnknown;

        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle =
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneNotifyNWSelectionSettingChange );

        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {
            // Update the client side data through pointer
            *iRetNWSelectionSetting = *settings;

            // Null pointer as notification is completed!
            iRetNWSelectionSetting = NULL;

            iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::SelectNetworkL
// Selects a new network to register to
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::SelectNetworkL(
    const TTsyReqHandle aTsyReqHandle, // Request handle
    const TDataPackage& aDataPackage ) // Contains parameters for request
    {  
    //call the extension to handle the request
    TInt ret( iMmPhoneTsy->iMmPhoneExtInterface->SelectNetworkL( 
         aDataPackage ));

    //check result of previous call
    if ( KErrNone != ret )
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        //save the handle and the request type
        iMmPhoneTsy->iReqHandleType = 
            CMmPhoneTsy::EMultimodePhoneSelectNetwork;
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::SelectNetworkCancelL
// Cancels via DOS TSY an outstanding asynchronous SetNetwork request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::SelectNetworkCancelL(    
    const TTsyReqHandle aTsyReqHandle ) 
    {
    // First check if it is still possible to cancel the request!
    TTsyReqHandle cancelSelectNetworkHandle = 
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneSelectNetwork );

    // Reset handle only if cancel is successful! See completition.
   if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown !=
       cancelSelectNetworkHandle )
        {
        // Send cancel message
        TInt ret( iMmPhoneTsy->iMmPhoneExtInterface->SelectNetworkCancelL() );
        
        if ( KErrNone != ret )
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        }
    // Not possible to cancel, complete request.
    else
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNotFound );
        } 
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteSelectNetwork
// Completes a SelectNetwork request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteSelectNetwork(
    TInt aErrorCode ) // Error code
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneSelectNetwork );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteSelectNetworkCancel
// Completes a SelectNetworkCancel request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteSelectNetworkCancel(
    TInt aErrorCode ) // Error code
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle completeSelectNetworkHandle = 
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
       CMmPhoneTsy::EMultimodePhoneSelectNetwork );

    // check if the SelectNetwork has already been completed
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
         completeSelectNetworkHandle )
        {

        // If cancel succeeded, complete req with KErrCancel
        if ( KErrNone == aErrorCode )
            {
            aErrorCode = KErrCancel;
            }
        iMmPhoneTsy->ReqCompleted( completeSelectNetworkHandle, aErrorCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkCaps
// Gets the network capabilities of the phone.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNetworkCaps(
    const TTsyReqHandle aTsyReqHandle, //Tsy request handle
    TUint32* aCaps ) const // Capabilities parameter
        
    {
    // Get capabilities of the current mode
    TInt ret ( iMmPhoneTsy->iMmPhoneExtInterface->GetNetworkCaps( aCaps ));
    //Complete the request using the return value from extension
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetHomeNetwork
// Gets the home network information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetHomeNetwork(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aNetworkInfoPckg ) const 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETHOMENETWORK_1, "TSY: CMmNetTsy::GetHomeNetwork" );

    TInt ret ( KErrNotSupported );
    
    if ( sizeof( RMobilePhone::TMobilePhoneNetworkInfoV1Pckg ) > aNetworkInfoPckg->MaxLength() )
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrArgument );
        return KErrNone;
        }
    
    TPckg<RMobilePhone::TMultimodeType>* networkInfoTempPckg = 
        REINTERPRET_CAST( TPckg<RMobilePhone::TMultimodeType>*, 
            aNetworkInfoPckg );       
    RMobilePhone::TMultimodeType& networkinfotemp = 
        ( *networkInfoTempPckg )();
    
    if ( KETelExtMultimodeV1 == networkinfotemp.ExtensionId() )
		{
        // create reference in to data on the end of the clients pointer
        RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkInfoV1Pckg = 
    		( RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* ) aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfoV1 = 
		    ( *networkInfoV1Pckg )();
		
		//get the data from the extension
		ret = iMmPhoneTsy->iMmPhoneExtInterface->GetHomeNetworkV1( 
    		&networkInfoV1 );    
		}
	else if ( KETelExtMultimodeV2 == networkinfotemp.ExtensionId() )
		{
		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* networkInfoV2Pckg = 
		    ( RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* ) aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV2& networkInfoV2 = 
		    ( *networkInfoV2Pckg )();

		ret = iMmPhoneTsy->iMmPhoneExtInterface->GetHomeNetworkV2( 
    		&networkInfoV2 );
		}
    else if ( KEtelExtMultimodeV5 == networkinfotemp.ExtensionId() )
		{
        RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* networkInfoV5Pckg = 
            ( RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* ) aNetworkInfoPckg;
        RMobilePhone::TMobilePhoneNetworkInfoV5& networkInfoV5 = 
            ( *networkInfoV5Pckg )();

		ret = iMmPhoneTsy->iMmPhoneExtInterface->GetHomeNetworkV5( 
            &networkInfoV5 );
		}
	else if ( KEtelExtMultimodeV8 == networkinfotemp.ExtensionId() )
		{
        RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* networkInfoV8Pckg = 
            ( RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* ) aNetworkInfoPckg;
        RMobilePhone::TMobilePhoneNetworkInfoV8& networkInfoV8 = 
            ( *networkInfoV8Pckg )();

		ret = iMmPhoneTsy->iMmPhoneExtInterface->GetHomeNetwork( 
            &networkInfoV8 );
		}

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
    
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetCurrentNetwork
// Gets information about the currently registered network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCurrentNetwork(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aNetworkInfoPckg,
    RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) const 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCURRENTNETWORK_1, "TSY: CMmNetTsy::GetCurrentNetwork");

    TInt ret( KErrArgument );
    if ( ERfsStateInfoInactive == iMmPhoneTsy->GetRfStateInfo() )  
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCURRENTNETWORK_2, "TSY: CMmNetTsy::GetCurrentNetwork Offline mode ON, request is not allowed! ");
        ret = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral,
                KErrGsmOfflineOpNotAllowed );
                
        // Complete the request with appropiate error        
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );	
        }   
    else
        {
        if ( sizeof( RMobilePhone::TMobilePhoneNetworkInfoV1Pckg ) <= 
            aNetworkInfoPckg->MaxLength() )
            {
            TPckg<RMobilePhone::TMultimodeType>* networkInfoTempPckg = 
                REINTERPRET_CAST( TPckg<RMobilePhone::TMultimodeType>*,
                aNetworkInfoPckg );       
            RMobilePhone::TMultimodeType& networkinfotemp = 
                ( *networkInfoTempPckg )();
            
            if ( KETelExtMultimodeV1 == networkinfotemp.ExtensionId() )
        		{
                // create reference in to data on the end of the clients pointer
            	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkInfoV1Pckg = 
            		( RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* ) 
            		aNetworkInfoPckg;
        		RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfoV1 = 
        		    ( *networkInfoV1Pckg )();
        		
        		// get the data from the extension
        		ret = iMmPhoneTsy->iMmPhoneExtInterface->GetCurrentNetworkV1( 
            		&networkInfoV1, aArea );    
        		}	
        	else if ( KETelExtMultimodeV2 == networkinfotemp.ExtensionId() )
        		{
        		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* networkInfoV2Pckg = 
                    ( RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* ) 
                    aNetworkInfoPckg;
        		RMobilePhone::TMobilePhoneNetworkInfoV2& networkInfoV2 = 
        		    ( *networkInfoV2Pckg )();
            
        	    ret = iMmPhoneTsy->iMmPhoneExtInterface->GetCurrentNetworkV2( 
        	        &networkInfoV2, aArea );
        		}
            else if ( KEtelExtMultimodeV5 == networkinfotemp.ExtensionId() )
                {
                RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* networkInfoV5Pckg = 
                    ( RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* ) 
                    aNetworkInfoPckg;
                RMobilePhone::TMobilePhoneNetworkInfoV5& networkInfoV5 = 
                    ( *networkInfoV5Pckg )();
                
        	    ret = iMmPhoneTsy->iMmPhoneExtInterface->GetCurrentNetworkV5( 
                    &networkInfoV5, aArea );
    		    }
    		else if ( KEtelExtMultimodeV8 == networkinfotemp.ExtensionId() )
                {
                RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* networkInfoV8Pckg = 
                    ( RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* ) 
                    aNetworkInfoPckg;
                RMobilePhone::TMobilePhoneNetworkInfoV8& networkInfoV8 = 
                    ( *networkInfoV8Pckg )();
                
    	        ret = iMmPhoneTsy->iMmPhoneExtInterface->GetCurrentNetwork( 
                &networkInfoV8, aArea );
                }
            else
                {
                ret = KErrNotSupported;
                }                    
            }
		       
    	iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetCellInfoL
// Gets information about the currently cell.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCellInfoL(const TTsyReqHandle aTsyReqHandle, TDes8* aCellInfoPckg)
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCELLINFOL_1, "TSY: CMmNetTsy::GetCellInfoL; Handle : %d", aTsyReqHandle);
    if(sizeof(RMobilePhone::TMobilePhoneCellInfoV9) > aCellInfoPckg->MaxLength())
    	{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCELLINFOL_2, "TSY: CMmNetTsy::GetCellInfo Bad size argument");
        // Complete the request with appropiate error        
        return KErrArgument;
    	}
    TPckg<RMobilePhone::TMultimodeType>* cellInfoTempPckg = 
    	reinterpret_cast<TPckg<RMobilePhone::TMultimodeType>*> (aCellInfoPckg);       
    RMobilePhone::TMultimodeType& cellInfoTemp = ( *cellInfoTempPckg )();
	if(cellInfoTemp.ExtensionId()!=KEtelExtMultimodeV9)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCELLINFOL_3, "TSY: CMmNetTsy::GetCellInfo Bad version argument");
        // Complete the request with appropiate error        
        return KErrArgument;
		}
    TInt ret( KErrNotSupported );
    if ( ERfsStateInfoInactive == iMmPhoneTsy->GetRfStateInfo() )  
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCELLINFOL_4, "TSY: CMmNetTsy::GetCellInfo Offline mode ON, request is not allowed! ");
        ret = CMmCommonStaticUtility::EpocErrorCode( KErrGeneral, KErrGsmOfflineOpNotAllowed );
                
        // Complete the request with appropiate error
        if(ret == KErrNone)
        	{
        	iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
        	}
        return ret;
        }
 
    // if cell info is cached, return the cached value straight away
    if (iCachedCellInfoV9.IsCacheValid())
    	{
    	// return cached information to client
    	if(cellInfoTemp.ExtensionId() == KEtelExtMultimodeV9)
    		{
    		TPckg<RMobilePhone::TMobilePhoneCellInfoV9>* cellInfoPckg = 
    			reinterpret_cast<TPckg<RMobilePhone::TMobilePhoneCellInfoV9>*> (aCellInfoPckg);       
    		(*cellInfoPckg)() = iCachedCellInfoV9.Value();
    		}    		
        // Complete the request  
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
    	return KErrNone;
    	}
	else if (iCachedCellInfoV9.CacheAbility() == ECacheUnknown)
		{
		// we don't know about the cache ability, so we have to send and IndReq to the LTSY as well.
		// The cache status (clean/dirty) is irrelevant, as we don't know how reliable it is.
		TInt ret(KErrNone);
		ret = DoNotifyCellInfoChangeL(aTsyReqHandle);
		if(ret == KErrNone)
			{
			ret = DoGetCellInfoReqL(aTsyReqHandle, cellInfoTempPckg);
			}
		return ret;
		}
	else
		{ // cache is disabled or dirty, so need to send the request to the LTSY
		// send request
        TInt ret = DoGetCellInfoReqL(aTsyReqHandle, cellInfoTempPckg);
		return ret;
		}
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::DoGetCellInfoReqL
// Gets information about the currently cell.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::DoGetCellInfoReqL(const TTsyReqHandle aTsyReqHandle, 
		TPckg<RMobilePhone::TMultimodeType>* aCellInfoPckg)
	{
	// send request	
    TInt ret(KErrNone);
    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
         GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneGetCellInfo );

     if (0 >= reqHandle )
         {  //The request is not already in processing because of previous request
         ret = iMmPhoneTsy->iMmPhoneExtInterface->GetCellInfoL();
         if ( KErrNone == ret )
             {
             //save tsy req handle type
             iMmPhoneTsy->iReqHandleType = CMmPhoneTsy::EMultimodePhoneGetCellInfo;
             }
         }
     if( ret == KErrNone )
    	 {
    	 TCellInformationRequest req;
    	 req.iReqHandle = aTsyReqHandle;
    	 req.iCellInfoPckg = aCellInfoPckg;
    	 iCellInfoRequests.AppendL( req );
    	 
    	 }
     return ret;
	}

// ---------------------------------------------------------------------------
// CMmNetTsy::DoNotifyCellInfoChangeL
// Register notifier for cell information change event.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::DoNotifyCellInfoChangeL(const TTsyReqHandle aTsyReqHandle)
	{
	TInt ret = KErrNone;
    TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
         GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneNotifyCellInfoChange );

     if (0 >= reqHandle )
         { 
         ret = iMmPhoneTsy->iMmPhoneExtInterface->NotifyCellInfoChangeL();
         if ( KErrNone == ret )
             {
             //save tsy req handle type
             iMmPhoneTsy->SetTypeOfResponse( CMmPhoneTsy::EMultimodePhoneNotifyCellInfoChange, aTsyReqHandle );
             }
         }
     return ret;
	}


// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteCellInfoReq
// The LTSY has completed a cell information get request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteCellInfoReq(
		CMmDataPackage* aDataPackage,
		TInt aError )
	{
	//reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(CMmPhoneTsy::EMultimodePhoneGetCellInfo);

    // Check the validity of the handler
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        HandleCellInfoUpdate(aDataPackage,aError,iCellInfoRequests);
        }      
    HandleCellInfoUpdate(aDataPackage,aError,iNotifyCellInfoRequests);
	}

// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteCellInfoIndReq
// The LTSY has completed a request for notifications of changes in the cell information.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteCellInfoIndReq(CMmDataPackage* aDataPackage, TInt aError)
	{
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle(CMmPhoneTsy::EMultimodePhoneNotifyCellInfoChange );
		
	// update cache ability according to aError
	if (aError == KErrNone)
		{
		iCachedCellInfoV9.SetCacheEnabled();
		}
	else
		{
		iCachedCellInfoV9.SetCacheDisabled();
		HandleCellInfoUpdate(aDataPackage,aError,iNotifyCellInfoRequests);
        }
	
	}

// ---------------------------------------------------------------------------
// CMmNetTsy::HandleCellInfoUpdate
// This functions is called when new cell information is received. It updates 
//   the cache and completes the client requests.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::HandleCellInfoUpdate(CMmDataPackage* aDataPackage, 
		TInt aError,
		RArray< TCellInformationRequest >& aRequests)
	{
   	RMobilePhone::TMobilePhoneCellInfoV9 cellInfo;
    if (aError == KErrNone)
    	{
        aDataPackage->UnPackData ( cellInfo );
        
       	// update cache
        iCachedCellInfoV9.SetValue(cellInfo);  	
    	}
    else
    	{
    	iCachedCellInfoV9.SetCacheDirty();
    	}
    	
    // complete clients
    const TInt KRequestCount = aRequests.Count();
	for ( TInt i=0; i < KRequestCount; i++ )
		{		
        TCellInformationRequest req = aRequests[ i ];
    
        if (aError == KErrNone)
        	{
	        //set value in client side
        	if((*req.iCellInfoPckg)().ExtensionId() == KEtelExtMultimodeV9)
        		{

        		RMobilePhone::TMobilePhoneCellInfoV9Pckg* clientCellInfoPckgPtr = 
	        		reinterpret_cast<RMobilePhone::TMobilePhoneCellInfoV9Pckg*>(req.iCellInfoPckg);
	        	(*clientCellInfoPckgPtr)() = cellInfo;
        		}
        	}

        //Complete the client request
        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_HANDLECELLINFOUPDATE_1, "TSY: CMmNetTsy::CompleteCellInfoReq client: %d; Handle: %d", i, req.iReqHandle );
        iMmPhoneTsy->ReqCompleted( req.iReqHandle, aError );
		}
    // Destroy array   
	for ( TInt i=0; i < KRequestCount; i++ ) // Remove only the completed ones. 
											 // During the ReqCompleted ETEL 
											 // reposts notifiers, so the array might 
											 // be bigger then KRequestCount and we must remove 
											 // only the ones that completed. 
		{		
		aRequests.Remove(0);
		}
    if(KRequestCount>0)
        {
        aRequests.Compress();
        }
	}

// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkName
// Gets name information about the currently registered network.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNetworkName(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aNetworkNamePckg,
    TDes8* aPlmnPckg ) const   
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKNAME_1, "TSY: CMmNetTsy::GetNetworkName");
    TInt ret ( KErrGeneral );

    if( sizeof(RMobilePhone::TMobilePhoneNetworkNameV3) > aNetworkNamePckg->MaxLength() ||
        sizeof(RMobilePhone::TMobilePhoneOPlmnV3) > aPlmnPckg->MaxLength() )
	  	{
	  	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKNAME_2, "TSY: CMmNetTsy::GetNetworkName bad size argument");
	  	// Complete the request with appropiate error        
	  	ret = KErrArgument;
	  	}

    else
        {
        //unpack the paramters
        TPckg<RMobilePhone::TMobilePhoneNetworkNameV3>* networknamePckg = 
        ( TPckg<RMobilePhone::TMobilePhoneNetworkNameV3>* ) aNetworkNamePckg;
        RMobilePhone::TMobilePhoneNetworkNameV3& networkName = 
            ( *networknamePckg )();
        
        TPckg<RMobilePhone::TMobilePhoneOPlmnV3>* plmnPckg = 
            ( TPckg<RMobilePhone::TMobilePhoneOPlmnV3>* ) aPlmnPckg;
        RMobilePhone::TMobilePhoneOPlmnV3& plmn = ( *plmnPckg )();

        RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfo;
        ret = iMmPhoneTsy->iMmPhoneExtInterface->
            GetCurrentNetworkNoLocation( &networkInfo );

        networkName.iLongName.Copy( networkInfo.iLongName );
        networkName.iShortName.Copy( networkInfo.iShortName );
        plmn.iCountryCode.Copy( networkInfo.iCountryCode );
        plmn.iNetworkId.Copy( networkInfo.iNetworkId );
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKNAME_3, "TSY: CMmNetTsy::GetNetworkName, iLongName: %S", networkName.iLongName);

        if( KErrNone == ret )
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKNAME_4, "TSY: CMmNetTsy::GetNetworkName - completing, ret: %{TSymbianErrorCodes}", ret);
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetCurrentNetworkNoLocation
// Gets information about the currently registered network, but not about the
// location (LAC and Cell Id).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCurrentNetworkNoLocation(
    const TTsyReqHandle aTsyReqHandle, 
    TDes8* aNetworkInfoPckg ) const 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCURRENTNETWORKNOLOCATION_1, "TSY: CMmNetTsy::GetCurrentNetworkNoLocation");
  
    TInt ret( KErrNotSupported );
    
    TPckg<RMobilePhone::TMultimodeType>* networkInfoTempPckg = 
        REINTERPRET_CAST( TPckg<RMobilePhone::TMultimodeType>*,
        aNetworkInfoPckg );       
    RMobilePhone::TMultimodeType& networkinfotemp = 
        ( *networkInfoTempPckg )();
    
    if ( KETelExtMultimodeV1 == networkinfotemp.ExtensionId() )
		{
		// create reference in to data on the end of the clients pointer
    	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* networkInfoV1Pckg = 
    		( RMobilePhone::TMobilePhoneNetworkInfoV1Pckg* ) aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV1& networkInfoV1 = 
		    ( *networkInfoV1Pckg )();

		//get the data from the extension
		ret = iMmPhoneTsy->iMmPhoneExtInterface->
		    GetCurrentNetworkNoLocationV1( &networkInfoV1 );    
		}
	else if ( KETelExtMultimodeV2 == networkinfotemp.ExtensionId() )
		{
		RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* networkInfoV2Pckg = 
		    ( RMobilePhone::TMobilePhoneNetworkInfoV2Pckg* ) aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV2& networkInfoV2 = 
		    ( *networkInfoV2Pckg )();

		ret = iMmPhoneTsy->iMmPhoneExtInterface->
		    GetCurrentNetworkNoLocationV2( &networkInfoV2 );
		}
    else if ( KEtelExtMultimodeV5 == networkinfotemp.ExtensionId() )
		{
        RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* networkInfoV5Pckg = 
            ( RMobilePhone::TMobilePhoneNetworkInfoV5Pckg* ) aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV5& networkInfoV5 = 
		    ( *networkInfoV5Pckg )();

        ret = iMmPhoneTsy->iMmPhoneExtInterface->
            GetCurrentNetworkNoLocationV5( &networkInfoV5 );
		}
	else if ( KEtelExtMultimodeV8 == networkinfotemp.ExtensionId() )
		{
        RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* networkInfoV8Pckg = 
            ( RMobilePhone::TMobilePhoneNetworkInfoV8Pckg* ) aNetworkInfoPckg;
		RMobilePhone::TMobilePhoneNetworkInfoV8& networkInfoV8 = 
		    ( *networkInfoV8Pckg )();

        ret = iMmPhoneTsy->iMmPhoneExtInterface->
            GetCurrentNetworkNoLocation( &networkInfoV8 );
		}
        
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyCurrentNetworkNoLocationChangeL
// Notifies client of changes to the current network
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyCurrentNetworkNoLocationChangeL( 
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aNetworkInfoPckg )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCURRENTNETWORKNOLOCATIONCHANGEL_1, "TSY: CMmNetTsy::NotifyCurrentNetworkNoLocationChangeL");
     
    // save the handle and the request type
    iMmPhoneTsy->iReqHandleType =
        CMmPhoneTsy::EMultimodePhoneNotifyCurrentNetworkNoLocationChange;

    // save request handle in queue
    TNwInfoRequest req;
    req.iReqHandle = aTsyReqHandle;
    req.iRetNwInfoPckg = REINTERPRET_CAST( 
        TPckg<RMobilePhone::TMultimodeType>*, aNetworkInfoPckg ); 
    iNotifyCurrentNwNoLocationRequests.AppendL( req );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyCurrentNetworkNoLocationChangeCancel
// Cancels an outstanding NotifyCurrentNetworkChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyCurrentNetworkNoLocationChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCURRENTNETWORKNOLOCATIONCHANGECANCEL_1, "TSY: CMmNetTsy::NotifyCurrentNetworkNoLocationChangeCancel");
    
    // find the correct ReqHandle...  
    for ( TInt i = 0; i < iNotifyCurrentNwNoLocationRequests.Count(); i++ )
        {
        if ( aTsyReqHandle == 
            iNotifyCurrentNwNoLocationRequests[i].iReqHandle )
            {
            iMmPhoneTsy->iTsyReqHandleStore->
                ResetTsyReqHandle( CMmPhoneTsy
                ::EMultimodePhoneNotifyCurrentNetworkNoLocationChange );
                
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
            
            iNotifyCurrentNwNoLocationRequests.Remove( i );
            iNotifyCurrentNwNoLocationRequests.Compress();
            // end the FOR-loop, there can be only one identical reqhandle
            // so there is no reason why the array-loop should continue
            break;
            }
        }  
        
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyCurrentNetworkChangeL
// Notifies client of changes to the current network
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyCurrentNetworkChangeL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aNetworkInfoPckg, 
    RMobilePhone::TMobilePhoneLocationAreaV1* aArea ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCURRENTNETWORKCHANGEL_1, "TSY: CMmNetTsy::NotifyCurrentNetworkChangeL");

    TInt ret = KErrArgument;
    TInt length = aNetworkInfoPckg->MaxLength();
    
    if ( sizeof( RMobilePhone::TMobilePhoneNetworkInfoV1 ) == length ||
         sizeof( RMobilePhone::TMobilePhoneNetworkInfoV2 ) == length ||
         sizeof( RMobilePhone::TMobilePhoneNetworkInfoV5 ) == length ||
         sizeof( RMobilePhone::TMobilePhoneNetworkInfoV8 ) == length )
        {
        ret = KErrNone;
        // save the handle and the request type
        iMmPhoneTsy->iReqHandleType =
            CMmPhoneTsy::EMultimodePhoneNotifyCurrentNetworkChange;

        // save request handle and client-side pointer and append
        // it in to a queue
        TNwInfoRequestWithLocation req;
        req.iReqHandle = aTsyReqHandle;
        req.iRetNwInfoPckg = REINTERPRET_CAST( 
            TPckg<RMobilePhone::TMultimodeType>*, aNetworkInfoPckg );
        req.iRetLocationArea = REINTERPRET_CAST( 
            RMobilePhone::TMobilePhoneLocationAreaV1*, aArea );
        iNotifyCurrentNwRequests.AppendL( req );
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyCurrentNetworkChangeCancel
// Cancels an outstanding NotifyCurrentNetworkChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyCurrentNetworkChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCURRENTNETWORKCHANGECANCEL_1, "TSY: CMmNetTsy::NotifyCurrentNetworkChangeCancel");

    // find the correct ReqHandle...  
    for ( TInt i = 0; i < iNotifyCurrentNwRequests.Count(); i++ )
        {
        if ( aTsyReqHandle == iNotifyCurrentNwRequests[i].iReqHandle )
            {
            // cancel the request...
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneNotifyCurrentNetworkChange );
                
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
            
            // remove the request from the queue
            iNotifyCurrentNwRequests.Remove( i );
            iNotifyCurrentNwRequests.Compress();
            // end the FOR-loop, there can be only one identical reqhandle
            // so there is no reason why the array-loop should continue
            break;
            }
        }        
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifyCurrentNetworkChange
// Completes a NotifyCurrentNetworkChange request to client side
// Completes a NotifyCurrentNetworkNoLocationChange request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifyCurrentNetworkChange(
    CMmDataPackage* aDataPackage, 
    TInt aResult ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_1, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange");
    
    // create a copy of the NW info sended by the LTSY
    RMobilePhone::TMobilePhoneNetworkInfoV8 networkInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
    
    aDataPackage->UnPackData ( networkInfo, locationArea );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_2, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, new extension ID: %d", networkInfo.ExtensionId() );
   
    // empty additional extension information...
    if ( KETelExtMultimodeV1 == networkInfo.ExtensionId() )
        {
        networkInfo.iAccess = RMobilePhone::ENetworkAccessUnknown; 
        networkInfo.iHsdpaAvailableIndicator = EFalse;
        networkInfo.iEgprsAvailableIndicator = EFalse; 
        networkInfo.iHsupaAvailableIndicator = EFalse;
        }   
    else if ( KETelExtMultimodeV2 == networkInfo.ExtensionId() )
        {
        networkInfo.iHsdpaAvailableIndicator = EFalse;
        networkInfo.iEgprsAvailableIndicator = EFalse;
        networkInfo.iHsupaAvailableIndicator = EFalse;
        }
    else if ( KEtelExtMultimodeV5 == networkInfo.ExtensionId() )
        {
        networkInfo.iHsupaAvailableIndicator = EFalse;
        }
    
    // Get the previous NW information    
    RMobilePhone::TMobilePhoneNetworkInfoV8 previousInfo;
    RMobilePhone::TMobilePhoneLocationAreaV1 previousLocationArea;

    iMmPhoneTsy->iMmPhoneExtInterface->GetCurrentNetwork( &previousInfo,
        &previousLocationArea );

    // continue only if values have changed; NW and location information
    if ( IsNetworkInfoChanged( previousInfo, networkInfo ) ||
         IsAreaInfoChanged( previousLocationArea, locationArea ) )
        {
        if ( KErrNone == aResult )
            {
            // inform the extension of the network change
            iMmPhoneTsy->iMmPhoneExtInterface->SetCurrentNetwork( 
                networkInfo, locationArea );
            }
        // reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle =
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
            CMmPhoneTsy::EMultimodePhoneNotifyCurrentNetworkChange );            
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_3, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, req handler =%u", reqHandle );

        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {
            TInt requestCount = iNotifyCurrentNwRequests.Count();            
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_4, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, requestCount: %d ", requestCount );

            // Complete all notification requests...
            for ( TInt i = 0; i < requestCount; i++ )
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_5, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, Loc; array: %d ", i );
               
                // copy the infromation to client, 0 = first index
                switch ( ( *( iNotifyCurrentNwRequests[ 0 ].
                    iRetNwInfoPckg ) )().ExtensionId() )
                    {
                    case KETelExtMultimodeV1:    
                        {                  
                        // create reference to the client side data 
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV1& 
                            retNetworkInfoV1 = ( *( REINTERPRET_CAST( 
                            TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1>*, 
                            iNotifyCurrentNwRequests[ 0 ].
                            iRetNwInfoPckg ) ) )();
                            
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocationV1( 
                            &retNetworkInfoV1 );
                        } break;
                    case KETelExtMultimodeV2:
                        {
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV2& 
                            retNetworkInfoV2 = ( *( REINTERPRET_CAST( 
                            TPckg<RMobilePhone::TMobilePhoneNetworkInfoV2>*,
                            iNotifyCurrentNwRequests[ 0 ].
                            iRetNwInfoPckg ) ) )();
                            
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocationV2( 
                            &retNetworkInfoV2 );
                        } break;
                    case KEtelExtMultimodeV5:
                        {
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV5& 
                            retNetworkInfoV5 = ( *( REINTERPRET_CAST( 
                            TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5>*, 
                            iNotifyCurrentNwRequests[ 0 ].
                            iRetNwInfoPckg ) ) )();
                            
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocationV5( 
                            &retNetworkInfoV5 );
                        } break;
                    case KEtelExtMultimodeV8:
                        {
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV8& 
                            retNetworkInfoV8 = (*(reinterpret_cast<TPckg<RMobilePhone::TMobilePhoneNetworkInfoV8>*>
                            (iNotifyCurrentNwRequests[ 0 ].iRetNwInfoPckg)))();
                            
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocation(&retNetworkInfoV8 );
                        } break;
                    default:
                        // error case, complete with not supported
                        aResult = KErrNotSupported;
                        break;  
                    }
                // copy location data to client side  
                // 0 = first and also current index of the array              
                iMmPhoneTsy->iMmPhoneExtInterface->
                    GetCurrentNetwork( NULL, 
                    iNotifyCurrentNwRequests[ 0 ].iRetLocationArea );      
  
                // completing will start automatically new notification, 
                // this will be appended to the end of the array
                // 0 = first and also current index of the array
                iMmPhoneTsy->ReqCompleted( iNotifyCurrentNwRequests[ 0 ].
                    iReqHandle, aResult );
                    
                // remove the current notification from the array
                // 0 = first and also current index of the array
                iNotifyCurrentNwRequests.Remove( 0 );
                }            	
        	// compress the array  
            iNotifyCurrentNwRequests.Compress();
            } 
        }

    // also complete the CompleteNotifyCurrentNetworkNoLocationChange
    if ( IsNetworkInfoChanged( previousInfo, networkInfo ) )
        {
        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle =
          iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
          CMmPhoneTsy::EMultimodePhoneNotifyCurrentNetworkNoLocationChange );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_6, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, req handler NoLoc =%u", reqHandle );

        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {
            TInt requestCount = iNotifyCurrentNwNoLocationRequests.Count();            
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_7, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, requestCount NoLoc: %d ", requestCount );
            
            // Complete all requests...
            for ( TInt indx = 0; indx < requestCount; indx++ )
                {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYCURRENTNETWORKCHANGE_8, "TSY: CMmNetTsy::CompleteNotifyCurrentNetworkChange, NoLoc; array: %d ", indx );
                
                // copy the infromation to client, 0 = first index
                switch ( ( *( iNotifyCurrentNwNoLocationRequests[ 0 ].
                    iRetNwInfoPckg ) )().ExtensionId() )
                    {
                    case KETelExtMultimodeV1: 
                        {                     
                        // create reference to the client side data 
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV1& 
                            retNetworkInfoV1 = ( *( REINTERPRET_CAST( 
                            TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1>*, 
                            iNotifyCurrentNwNoLocationRequests[ 0 ].
                            iRetNwInfoPckg ) ) )();
                            
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocationV1 ( 
                            &retNetworkInfoV1 );
                        } break;
                    case KETelExtMultimodeV2:
                        {
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV2& 
                            retNetworkInfoV2 = ( *( REINTERPRET_CAST( 
                            TPckg<RMobilePhone::TMobilePhoneNetworkInfoV2>*,
                            iNotifyCurrentNwNoLocationRequests[ 0 ].
                            iRetNwInfoPckg ) ) )();
                            
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocationV2 ( 
                            &retNetworkInfoV2 );
                        } break;
                    case KEtelExtMultimodeV5:
                        {
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV5& 
                            retNetworkInfoV5 = ( *( REINTERPRET_CAST( 
                            TPckg<RMobilePhone::TMobilePhoneNetworkInfoV5>*,
                            iNotifyCurrentNwNoLocationRequests[ 0 ].
                            iRetNwInfoPckg ) ) )();
                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocationV5 ( &retNetworkInfoV5 );
                        } break;
                    case KEtelExtMultimodeV8:
                        {
                        // 0 = first and current index of the array
                        RMobilePhone::TMobilePhoneNetworkInfoV8& 
                            retNetworkInfoV8 = (*(reinterpret_cast<TPckg<RMobilePhone::TMobilePhoneNetworkInfoV8>*>
                            (iNotifyCurrentNwNoLocationRequests[ 0 ].iRetNwInfoPckg)))();                        

                        iMmPhoneTsy->iMmPhoneExtInterface->
                            GetCurrentNetworkNoLocation ( &retNetworkInfoV8 );

                        
                        } break;
                    default:
                        // error case, complete with not supported
                        aResult = KErrNotSupported;
                        break;
                    }
                // completing will start automatically new notification, 
                // this will be appended to the end of the array
                // 0 = first and also current index of the array
                iMmPhoneTsy->ReqCompleted( 
                    iNotifyCurrentNwNoLocationRequests[ 0 ].
                    iReqHandle, aResult );
                    
                // remove the current notification from the array
                // 0 = first and also current index of the array
                iNotifyCurrentNwNoLocationRequests.Remove( 0 );
                }
        	// compress the array 
            iNotifyCurrentNwNoLocationRequests.Compress();
            }        
        }
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyCellInfoChangeL
// Notifies client of changes to the cell information
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyCellInfoChangeL(
    const TTsyReqHandle aTsyReqHandle,
    TDes8* aCellInfoPckg) 
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCELLINFOCHANGEL_1, "TSY: CMmNetTsy::NotifyCellInfoChangeL; Handel: %d", aTsyReqHandle);
    if(sizeof(RMobilePhone::TMobilePhoneCellInfoV9) > aCellInfoPckg->MaxLength())
    	{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCELLINFOCHANGEL_2, "TSY: CMmNetTsy::NotifyCellInfoChangeL Bad size argument");
        // Complete the request with appropiate error        
        return KErrArgument;
    	}
    TPckg<RMobilePhone::TMultimodeType>* cellInfoTempPckg = 
        reinterpret_cast< TPckg<RMobilePhone::TMultimodeType>*> (aCellInfoPckg );       
    RMobilePhone::TMultimodeType& cellInfoTemp = ( *cellInfoTempPckg )();
	if(cellInfoTemp.ExtensionId()!=KEtelExtMultimodeV9)
		{
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCELLINFOCHANGEL_3, "TSY: CMmNetTsy::GetCellInfo Bad version argument");
        // Complete the request with appropiate error        
        return KErrArgument;
		}
	TInt ret = KErrNone;
	if (iCachedCellInfoV9.CacheAbility() == ECacheUnknown)
		{
		ret = DoNotifyCellInfoChangeL(aTsyReqHandle);
		}
	else if (iCachedCellInfoV9.CacheAbility() == ECacheDisabled)
		{
		ret = KErrNotSupported;
		}
	if(ret == KErrNone)
		{
        TCellInformationRequest req;
        req.iReqHandle = aTsyReqHandle;
        req.iCellInfoPckg = cellInfoTempPckg;
        iNotifyCellInfoRequests.AppendL( req );
		}
    return ret;
    }
    

// ---------------------------------------------------------------------------
// CMmNetTsy::GetCellInfoCancel
// Cancels an outstanding cell information getter request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCellInfoCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCELLINFOCANCEL_1, "TSY: CMmNetTsy::GetCellInfoCancel; Handle: %d", aTsyReqHandle);
    // find the correct ReqHandle...  
	const TInt KReqCount = iCellInfoRequests.Count();
    for ( TInt i = 0; i < KReqCount; i++ )
        {
        if ( aTsyReqHandle == iCellInfoRequests[i].iReqHandle )
            {
            // cancel the request
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneGetCellInfo );
                
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
            
            // remove the request from the queue
            iCellInfoRequests.Remove( i );
            iCellInfoRequests.Compress();
            // end the FOR-loop, there can be only one identical reqhandle
            // so there is no reason why the array-loop should continue
            break;
            }
        }        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyCurrentNetworkChangeCancel
// Cancels an outstanding NotifyCurrentNetworkChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyCellInfoChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) 
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYCELLINFOCHANGECANCEL_1, "TSY: CMmNetTsy::NotifyCellInfoChangeCancel; Handle: %d", aTsyReqHandle);

    // find the correct ReqHandle...  
	const TInt KReqCount = iNotifyCellInfoRequests.Count();
    for ( TInt i = 0; i < KReqCount; i++ )
        {
        if ( aTsyReqHandle == iNotifyCellInfoRequests[i].iReqHandle )
            {
            // cancel the request
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
                CMmPhoneTsy::EMultimodePhoneNotifyCellInfoChange );
                
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
            
            // remove the request from the queue
            iNotifyCellInfoRequests.Remove( i );
            iNotifyCellInfoRequests.Compress();
            // end the FOR-loop, there can be only one identical reqhandle
            // so there is no reason why the array-loop should continue
            break;
            }
        }        
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkRegistrationStatusL
// Gets the current network registration status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNetworkRegistrationStatusL(
    const TTsyReqHandle aTsyReqHandle, 
    RMobilePhone::TMobilePhoneRegistrationStatus* aStatus ) 
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSL_1, "TSY: CMmNetTsy::GetNetworkRegistrationStatusL Handle: %d", aTsyReqHandle);
    
	// If the modem is not ready (Common TSY has not received EMmTsyBootNotifyModemStatusReadyIPC
	// from LTSY), we don't need to ask. Update registration status and complete client immediately. 
	if (!iMmPhoneTsy->IsModemStatusReady())
		{
		iNWRegistrationStatus = RMobilePhone::ERegistrationUnknown;
		*aStatus = iNWRegistrationStatus;
		iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
		return KErrNone;
		}
	
	if (RMobilePhone::ERegistrationUnknown == iNWRegistrationStatus)	// modem is ready
        {
        TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
            GetTsyReqHandle( CMmPhoneTsy::EMultimodePhoneGetNetworkRegistrationStatus );

        if (0 < reqHandle )
            {
            //The request is already in processing because of previous request
   			TGetNetworkRegisterationRequest* req = 
   				new (ELeave) TGetNetworkRegisterationRequest();
   			req->iReqHandle = aTsyReqHandle;
   			req->iRetGetNWRegistrationStatus = aStatus;
  			iRegisterationStatusRequests.AppendL( req );
            }
        else
            {
			//get mode specific information 
			TInt ret ( iMmPhoneTsy->iMmPhoneExtInterface->
					GetNetworkRegistrationStatusL() );
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSL_2, "TSY: CMmNetTsy::GetNetworkRegistrationStatusL - Sending request to LTSY" );
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSL_3, "TSY: CMmNetTsy::GetNetworkRegistrationStatusL - Modem not ready" );

            if ( KErrNone != ret )
                {
                OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSL_4, "TSY: CMmNetTsy::GetNetworkRegistrationStatusL - Complete with error %{TSymbianErrorCodes}", ret );
                iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
                }
            else
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSL_5, "TSY: CMmNetTsy::GetNetworkRegistrationStatusL - Client to buffer" );
                // save request handle in queue
    			TGetNetworkRegisterationRequest* req = 
    				new (ELeave) TGetNetworkRegisterationRequest();
    			req->iReqHandle = aTsyReqHandle;
    			req->iRetGetNWRegistrationStatus = aStatus;
    			
    			iRegisterationStatusRequests.AppendL( req );
                
                //save tsy req handle type
                iMmPhoneTsy->iReqHandleType = 
                    CMmPhoneTsy::EMultimodePhoneGetNetworkRegistrationStatus;
                }
            }
        }
    else
        {
        *aStatus = iNWRegistrationStatus;
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
        }

    return KErrNone; 
    }    
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkRegistrationStatusCancel
// Cancels an outstanding GetNetworkRegistrationStatus request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNetworkRegistrationStatusCancel( 
    const TTsyReqHandle aTsyReqHandle )
    {     
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSCANCEL_1, "TSY: CMmNetTsy::GetNetworkRegistrationStatusCancel Handle: %d", aTsyReqHandle);

    TInt requestCount = iRegisterationStatusRequests.Count(); 
   
    if( 1 == requestCount )
    	{
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKREGISTRATIONSTATUSCANCEL_2, "TSY: CMmNetTsy::GetNetworkRegistrationStatusCancel 1 == requestCount  ");
        //reset the req handle
		TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        	CMmPhoneTsy::EMultimodePhoneGetNetworkRegistrationStatus );    	
    	}
	
	for ( TInt i = 0; i < requestCount; i++ )
		{
	  	TGetNetworkRegisterationRequest* req = iRegisterationStatusRequests[ i ];
	  	
	  	if( aTsyReqHandle == req->iReqHandle  )
	  		{
	  		// Complete request with cancel
	  		iMmPhoneTsy->ReqCompleted( req->iReqHandle, KErrCancel );
	  		delete iRegisterationStatusRequests[ i ];
	  		iRegisterationStatusRequests.Remove(i);
	  		break;	
	  		}
		}
    
    return KErrNone;		
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteGetNetworkRegistrationStatus
// Completes a GetNetworkRegistrationStatus request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteGetNetworkRegistrationStatus(
    TInt aErrorValue )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETEGETNETWORKREGISTRATIONSTATUS_1, "TSY: CMmNetTsy::CompleteGetNetworkRegistrationStatus");
     // Reset the req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetNetworkRegistrationStatus );
    
    // Check the validity of the handler
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        { 
        TInt requestCount = iRegisterationStatusRequests.Count();
        
		for ( TInt i=0; i < requestCount; i++ )
			{
            TGetNetworkRegisterationRequest* req = 
                iRegisterationStatusRequests[ i ];
            
            //set value in client side
            *(req->iRetGetNWRegistrationStatus) = iNWRegistrationStatus;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETEGETNETWORKREGISTRATIONSTATUS_2, "TSY: CMmNetTsy::CompleteGetNetworkRegistrationStatus client: %d", i );
        
			//Complete the client request
            iMmPhoneTsy->ReqCompleted( req->iReqHandle, aErrorValue );
			}
        }        
    
    // Destroy array        			
    iRegisterationStatusRequests.ResetAndDestroy();
    }
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNetworkRegistrationStatusChange
// Notifies of changes in current network registration status.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNetworkRegistrationStatusChange(
    RMobilePhone::TMobilePhoneRegistrationStatus* aStatus ) // Reg status
    {
    //save the handle type
    iMmPhoneTsy->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyNetworkRegistrationStatusChange;

    // Save the pointer of client's status variable for future use
    iRetNWRegistrationStatus = aStatus; 
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNetworkRegistrationStatusChangeCancel
// Cancels an outstanding NotifyNetworkRegistrationStatusChange request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNetworkRegistrationStatusChangeCancel(
    const TTsyReqHandle aTsyReqHandle ) //Tsy request handle
    {
    // As cancel is called, the client memory address pointed to is no longer  
    // valid -> NULL it
    iRetNWRegistrationStatus = NULL; 
    iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneNotifyNetworkRegistrationStatusChange );
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrCancel );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifyNetworkRegistrationStatusChange
// Completes a NotifyNetworkRegistrationStatusChange request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifyNetworkRegistrationStatusChange(
    CMmDataPackage* aDataPackage, //Contains information for client side
    TInt aResult ) // result code 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYNETWORKREGISTRATIONSTATUSCHANGE_1,  "TSY: CMmNetTsy::CompleteNotifyNetworkRegistrationStatusChange" );

    RMobilePhone::TMobilePhoneRegistrationStatus* regStatus = NULL;
    RMobilePhone::TMobilePhoneNetworkStatus* nwStatus = NULL;

    aDataPackage->UnPackData ( &regStatus, &nwStatus );

    if ( *regStatus != iNWRegistrationStatus )
        {
        // Inform extension about the change
        iMmPhoneTsy->iMmPhoneExtInterface->
            CompleteNotifyNetworkRegistrationStatusChange( *regStatus );

        // Store NEW registration status
        iNWRegistrationStatus = *regStatus;
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETENOTIFYNETWORKREGISTRATIONSTATUSCHANGE_2, "TSY: CMmNetTsy::CompleteNotifyNetworkRegistrationStatusChange, status: %d", iNWRegistrationStatus );

        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle = iMmPhoneTsy->iTsyReqHandleStore->
          ResetTsyReqHandle( 
          CMmPhoneTsy::EMultimodePhoneNotifyNetworkRegistrationStatusChange );

        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {
            //Fill the pointer with phone capabilities
            *iRetNWRegistrationStatus = *regStatus;
            //Set pointer to NULL.
            iRetNWRegistrationStatus = NULL; 
            //Complete the client request
            iMmPhoneTsy->ReqCompleted( reqHandle, aResult );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetDetectedNetworksPhase1L
// First phase of fetching detected networks. Starts the search via DOS layer
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetDetectedNetworksPhase1L(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    RMobilePhone::TClientId* aClient, // Client id
    TInt* aBufSize ) // Pointer to the buffer size
    {
    TTsyReqHandle getDetectedNetworksPhase1Handle =
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetDetectedNetworksPhase1 );
    
    //Check if operation is already ongoing, flow control done by TSY
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != 
         getDetectedNetworksPhase1Handle )
        {
        //Complete request with status value informing the client about 
        //the situation.
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {       
        //forward request to the extension
        TInt ret ( iMmPhoneTsy->iMmPhoneExtInterface->
            GetDetectedNetworksPhase1L());

        // Check if error occured
        if ( KErrNone != ret )
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
             // Save the address of the client attribute
            iRetGetDetectedNwSize = aBufSize;
            // Presently internal attribute, used only by this method.
            iRetDetectedNwClientId = aClient;

            //save the handle and the request type
            iMmPhoneTsy->iReqHandleType = 
                    CMmPhoneTsy::EMultimodePhoneGetDetectedNetworksPhase1;
            }
        }

    // Otherwise complete request when response is received from DOS side.
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::ProcessGetDetectedNetworksPhase1L
// Processes the list gathered as result of GetDetectedNetworksPhase1
// calculates its size and completes this information to client side. Caches
// the list for future completion in GetDetectedNetworksPhase2
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::ProcessGetDetectedNetworksPhase1L(
    CMmDataPackage* aDataPackage, // Contains information for client side
    TInt aErrorCode ) // Error code
    {
    // Fetch the request handle from array
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetDetectedNetworksPhase1 );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // retrieve detected networks from phone, 
        // store each network as a list entry,
        // stream the list and then return size of this buffer to client
        if ( KErrNone == aErrorCode )
            {
            // Store the streamed list and the client ID
            CListReadAllAttempt* read = 
                CListReadAllAttempt::NewL( iRetDetectedNwClientId );
            CleanupStack::PushL( read );
            //unpack the data from DOS TSY, 
            //cleanup of this data is done in DOS TSY
            CMobilePhoneNetworkListV2* list = NULL;
            aDataPackage->UnPackData ( list );            
            read->iListBuf = list->StoreLC();
            CleanupStack::Pop(); // pop the CBufFlat allocated by StoreLC
            __ASSERT_DEBUG( iGetDetectedNetworks, MmTsyPanic( EPtrNull ) );
            iGetDetectedNetworks->AppendL( read );
            CleanupStack::Pop(); // pop the CListReadAllAttempt
            // return the CBufFlat's size to client
            *iRetGetDetectedNwSize = ( read->iListBuf )->Size();
            // Complete first phase of list retrieval
            iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
            }
        // If something went wrong while receiving ISI message,
        // then handle the error situation here
        else
            {
            // Complete first phase of list retrieval with error
            iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
            }
        }
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetDetectedNetworksPhase2
// Completes the GetDetectedNetworks request initiated in 
// GetDetectedNetworksPhase1. Copies the cached detected network list to the
// client side.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetDetectedNetworksPhase2(
    const TTsyReqHandle aTsyReqHandle, // Tsy request handle
    RMobilePhone::TClientId* aClient, // Client id
    TDes8* aBuf ) // Buffer
    {
    CListReadAllAttempt* read = NULL;
    // Find the get detected network attempt from this client
    __ASSERT_DEBUG( iGetDetectedNetworks, MmTsyPanic( EPtrNull ) );
    for ( TInt i = 0; i < iGetDetectedNetworks->Count(); ++i )
        {
        read = iGetDetectedNetworks->At( i );
        if ( ( read->iClient.iSessionHandle == aClient->iSessionHandle ) &&
          ( read->iClient.iSubSessionHandle == aClient->iSubSessionHandle ) )
            {
            TPtr8 bufPtr( ( read->iListBuf )->Ptr( 0 ) );
            // Copy the streamed list to the client
            aBuf->Copy( bufPtr );
            delete read;
            iGetDetectedNetworks->Delete( i );
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
            return KErrNone;
            }
        }
    // Should handle error case of not finding the matching client from
    // read all phase 1
    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNotFound );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetDetectedNetworksCancelL
// Cancels via DOS TSY an outstanding GetDetectedNetworks request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetDetectedNetworksCancelL(
    const TTsyReqHandle aTsyReqHandle ) const // Tsy request handle
    {
    // Fetch the request handle from array
    TTsyReqHandle processGetDetNwPhase1Handle =
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetDetectedNetworksPhase1 );

    // is there a network detection ongoing?
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown !=
        processGetDetNwPhase1Handle )
        {       
        // forward message to the extension
        TInt ret ( 
            iMmPhoneTsy->iMmPhoneExtInterface->GetDetectedNetworksCancelL() );

        // Check out for message allocation failure
        if ( KErrNone != ret )
            {
            // Complete the failed request to client
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        }
    else 
        {
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNotFound );
        }
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteGetDetectedNetworksCancel
// Completes a GetDetectedNetworksCancel request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteGetDetectedNetworksCancel(
    TInt aErrorCode ) const // Error code
    {
    // get & reset the req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetDetectedNetworksPhase1);

    // Check if request handle exists
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // If cancel succeeded, complete req with KErrCancel
        if ( KErrNone == aErrorCode )
            {
            aErrorCode = KErrCancel;
            }

        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorCode );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkMode
// Returns pointer to iMode internal attribute
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
RMobilePhone::TMobilePhoneNetworkMode CMmNetTsy::GetNetworkMode() const
    {
    return iMode;
    }
  
// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkRegistrationStatus
// Returns the known network status
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//  
RMobilePhone::TMobilePhoneRegistrationStatus 
    CMmNetTsy::GetNetworkRegistrationStatus() const
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKMODE_1, "TSY: CMmNetTsy::GetNetworkRegistrationStatus: %d", iNWRegistrationStatus );
    return iNWRegistrationStatus;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetNetworkSecurityLevel
// Gets the security level of the current network
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetNetworkSecurityLevel(
    const TTsyReqHandle aTsyReqHandle, // Request handle
    RMobilePhone::TMobilePhoneNetworkSecurity*
        aNetworkSecurityLevel ) const //Network security level
        
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETNETWORKSECURITYLEVEL_1, "TSY: CMmNetTsy::GetNetworkSecurityLevel");
    iMmPhoneTsy->PhoneExtInterface()->GetNetworkSecurityLevel (
        *aNetworkSecurityLevel );

    iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNetworkSecurityLevelChange
// Notifies of the network security level change
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNetworkSecurityLevelChange( 
    RMobilePhone::TMobilePhoneNetworkSecurity*
        aNetworkSecurityLevel ) 
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYNETWORKSECURITYLEVELCHANGE_1, "TSY: CMmNetTsy::NotifyNetworkSecurityLevelChange");
    iMmPhoneTsy->iReqHandleType = 
        CMmPhoneTsy::EMultimodePhoneNotifyNetworkSecurityLevelChange;
    iRetNetworkSecurityLevel = aNetworkSecurityLevel;
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CMmNetTsy::NotifyNetworkSecurityLevelChangeCancel
// Returns pointer to iMode internal attribute
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::NotifyNetworkSecurityLevelChangeCancel()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_NOTIFYNETWORKSECURITYLEVELCHANGECANCEL_1, "TSY: CMmNetTsy::NotifyNetworkSecurityLevelChangeCancel");
    iRetNetworkSecurityLevel = NULL;
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneNotifyNetworkSecurityLevelChange );
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle)
    	{
    	iMmPhoneTsy->ReqCompleted( reqHandle, KErrCancel );
    	}
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteNotifyNetworkSecurityLevelChange
// Completes an outstanding NotifyNetworkSecurityLevelChange
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteNotifyNetworkSecurityLevelChange( 
    CMmDataPackage *aDataPackage ) 
    {
    RMobilePhone::TMobilePhoneNetworkSecurity* currentSecurityLevel = NULL;
        aDataPackage->UnPackData ( &currentSecurityLevel );

    //check the previous value of the security
    RMobilePhone::TMobilePhoneNetworkSecurity previousSecurityLevel;
    iMmPhoneTsy->PhoneExtInterface()->GetNetworkSecurityLevel ( 
        previousSecurityLevel );
    
    if ( previousSecurityLevel != *currentSecurityLevel )
        {
        //set the new level in the extension
        iMmPhoneTsy->PhoneExtInterface()->SetNetworkSecurityLevel ( 
        *currentSecurityLevel );
        //reset req handle. Returns the deleted req handle
        TTsyReqHandle reqHandle =
            iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
            CMmPhoneTsy::EMultimodePhoneNotifyNetworkSecurityLevelChange );

        if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
            {

            // Update the client side data through pointers
            *iRetNetworkSecurityLevel = *currentSecurityLevel;
            iRetNetworkSecurityLevel = NULL;

            iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetCipheringIndicatorStatusL
// Gets the status of the ciphering indicator
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCipheringIndicatorStatusL(
    const TTsyReqHandle aTsyReqHandle, 
    const TDataPackage& aDataPackage ) 
    {
    
    if ( CipheringIndicatorForcedOff() )
        {
        //unpack the pointer
        iRetCipheringIndicator = 
        reinterpret_cast<RMobilePhone::TMobileCallCipheringIndicator*>( 
        aDataPackage.Ptr1() );

        *iRetCipheringIndicator = RMobilePhone::ECipheringDisplayOff;
        iRetCipheringIndicator = NULL;

        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrNone );
        }

    else //normal ciphering proceeds
        {
        //Save pointer to the client side
        iRetCipheringIndicator = 
            reinterpret_cast<RMobilePhone::TMobileCallCipheringIndicator*>( 
            aDataPackage.Ptr1() );

        //Forward the request to the interface
        TInt ret = iMmPhoneTsy->iMmPhoneExtInterface->
            GetCipheringIndicatorStatusL( &aDataPackage );

        if ( KErrNone != ret )
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            //save the handle and the request type
            iMmPhoneTsy->iReqHandleType = 
                CMmPhoneTsy::EMultimodePhoneGetCipheringIndicatorStatus;
            }
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetCipheringIndicatorStatusCancel
// Returns pointer to iMode internal attribute
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCipheringIndicatorStatusCancel()
    {
    iRetCipheringIndicator = NULL;
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetCipheringIndicatorStatus);
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle)
    	{
    	iMmPhoneTsy->ReqCompleted ( reqHandle, KErrCancel );
    	}
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteGetCipheringIndicatorStatus
// Completes an outstanding GetCipheringIndicatorStatus
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteGetCipheringIndicatorStatus(
    CMmDataPackage* aDataPackage,
    TInt aError ) 
    {
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = 
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle( 
            CMmPhoneTsy::EMultimodePhoneGetCipheringIndicatorStatus);

    //check the availability of the handler    
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown !=  reqHandle )
        {
        //unpack the data
        RMobilePhone::TMobileCallCipheringIndicator* ciphIndicator = NULL;
        aDataPackage->UnPackData ( &ciphIndicator );
    
        *iRetCipheringIndicator = *ciphIndicator;
        iRetCipheringIndicator = NULL;

        iMmPhoneTsy->ReqCompleted( reqHandle, aError );
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::ReceivedCallPrivacyInformation
// Get the call privacy information and informs all active calls
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::ReceivedCallPrivacyInformation( 
    CMmDataPackage* aDataPackage )
    {
    //unpack the data
    RMobilePhone::TMobilePhonePrivacy* privacy = NULL;
    aDataPackage->UnPackData ( &privacy );

    //Send privacy confirmation to all active call objects
    for ( TInt i = 0; i<iMmPhoneTsy->CallList()->GetNumberOfObjects(); i++ )
        {
         CMmCallTsy* mmCall = iMmPhoneTsy->CallList()->GetMmCallByIndex(i);
         if ( mmCall->MobileCallStatus() != RMobileCall::EStatusUnknown ||
              mmCall->MobileCallStatus() != RMobileCall::EStatusIdle )
            {
            mmCall->CompleteNotifyPrivacyConfirmation( *privacy );
            }                
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::IsNetworkInfoChanged
// Checks if two TMobilePhoneNetworkInfoV8 differs
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmNetTsy::IsNetworkInfoChanged( 
    const RMobilePhone::TMobilePhoneNetworkInfoV8& previousNetInfo, 
    const RMobilePhone::TMobilePhoneNetworkInfoV8& newNetInfo ) 
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_ISNETWORKINFOCHANGED_1, "TSY: CMmNetTsy::IsNetworkInfoChanged" );

    TBool ret ( ETrue );

    if ( previousNetInfo.iMode == newNetInfo.iMode &&
        previousNetInfo.iStatus == newNetInfo.iStatus &&
        previousNetInfo.iCountryCode == newNetInfo.iCountryCode &&
        previousNetInfo.iNetworkId == newNetInfo.iNetworkId &&
        previousNetInfo.iDisplayTag == newNetInfo.iDisplayTag &&
        previousNetInfo.iShortName == newNetInfo.iShortName &&
        previousNetInfo.iLongName == newNetInfo.iLongName &&
        // version 2 specific
        previousNetInfo.iAccess == newNetInfo.iAccess &&
        // version 5 specific
        previousNetInfo.iHsdpaAvailableIndicator == 
            newNetInfo.iHsdpaAvailableIndicator &&
        previousNetInfo.iEgprsAvailableIndicator == 
            newNetInfo.iEgprsAvailableIndicator &&
        // version 8 specific
        previousNetInfo.iHsupaAvailableIndicator ==
        	newNetInfo.iHsupaAvailableIndicator
        )
        {
        ret = EFalse;
        }
        
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::IsAreaInfoChanged
// Checks if two TMobilePhoneLocationAreaV1 differs
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmNetTsy::IsAreaInfoChanged(
    const RMobilePhone::TMobilePhoneLocationAreaV1& previousAreaInfo, 
    const RMobilePhone::TMobilePhoneLocationAreaV1& newAreaInfo ) 
    {
    TBool ret ( ETrue );

    if ( previousAreaInfo.iAreaKnown == newAreaInfo.iAreaKnown &&
         previousAreaInfo.iLocationAreaCode == newAreaInfo.iLocationAreaCode 
         && previousAreaInfo.iCellId == newAreaInfo.iCellId )
        {
        ret = EFalse;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::ReadVariantOptions
// Reads from ini file the values of the variant flags.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::ReadVariantOptions()
    {    
    RFs fs;
    RFile file;
    TBuf8<1> flagStatus;
    // file path. c32.exe SID is 101F7989.
    _LIT(KVariantsConfFileName, 
                              "Z:\\private\\101f7989\\operatorVariants.ini");

    //connect to the File server
    if ( KErrNone == fs.Connect() )
        {
        TInt err = file.Open( fs, KVariantsConfFileName, EFileStream | 
                                                                  EFileRead );
        if ( KErrNone == err )
            {
            TInt readStatus( KErrNone );

            //currently we need only to read the first line
            readStatus = file.Read( flagStatus );

            if ( readStatus == KErrNone && 
                 flagStatus.Length() != 0 )
                {
                //check if value is one
                if ( (flagStatus[ 0 ] - '0') == 1 )
                    {
                     //set the flag on
                    iCipheringIndicatorForcedOff = ETrue;
                    }
                }    
            //close the file
            file.Close();
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_READVARIANTOPTIONS_1, "TSY:CMmNetTsy::ReadVariantOptions:File successfully read.");
            }
        else
            {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_READVARIANTOPTIONS_2, "TSY:CMmNetTsy::ReadVariantOptions:Could not open file Z:\\private\\101f7989\\operatorVariants.ini, error=%{TSymbianErrorCodes}",err);
            }

        // close connection to file server
        fs.Close();    
        }
    else
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_READVARIANTOPTIONS_3, "TSY:CMmNetTsy::ReadVariantOptions:Could not connect to file server.");
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_READVARIANTOPTIONS_4, "TSY:CMmNetTsy::ReadVariantOptions:iCipheringIndicatorForcedOff=%d",iCipheringIndicatorForcedOff);
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::CipheringIndicatorForcedOff
// Returns ETrue if the ciphering indication is forced off.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmNetTsy::CipheringIndicatorForcedOff()
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_CIPHERINGINDICATORFORCEDOFF_1, "TSY:CMmNetTsy::CipheringIndicatorForcedOff:iCipheringIndicatorForcedOff=%d",iCipheringIndicatorForcedOff);
    return iCipheringIndicatorForcedOff;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::AuthorizationInfoPhase1
// Processes AuthorizationInfoPhase1 request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetAuthorizationInfoPhase1L(
	const TTsyReqHandle aTsyReqHandle,
	RMobilePhone::TClientId* aClient,
	TInt* aBufSize )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETAUTHORIZATIONINFOPHASE1L_1, "TSY: CMmNetTsy::AuthorizationInfoPhase1L");

    TTsyReqHandle getAuthorizationInfoPhase1Handle =
        iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetAuthorizationInfoPhase1 );

    // Check if operation is already ongoing
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown !=
         getAuthorizationInfoPhase1Handle )
        {
        // Complete request with KErrServerBusy
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }
    else
        {
        // Get the authorization data
        TInt ret = iMmPhoneTsy->MessageManager()->HandleRequestL(
            EMobilePhoneAuthorizationInfoPhase1 );

        // Check if request for authorization data was sent successfully
        if ( KErrNone != ret )
            {
            iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
            }
        else
            {
            // Save the address of the client attribute
            iAuthorizationInfoBufferSize = aBufSize;
            // Presently internal attribute, used only by this method.
            iAuthorizationInfoClientId = *aClient;

            //save the handle and the request type
            iMmPhoneTsy->iReqHandleType =
                    CMmPhoneTsy::EMultimodePhoneGetAuthorizationInfoPhase1;
            }
        }

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::ProcessAuthorizationInfoPhase1
// Completes AuthorizationInfoPhase1 request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::ProcessGetAuthorizationInfoPhase1L(
	CMmDataPackage* aDataPackage,
	TInt aError )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_PROCESSGETAUTHORIZATIONINFOPHASE1L_1, "TSY: CMmNetTsy::ProcessGetAuthorizationInfoPhase1L");

    // Fetch the request handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetAuthorizationInfoPhase1 );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // Check if authorization data response was received successfully
        if ( KErrNone != aError )
            {
            // Error occured, complete authorization info request with error
            iMmPhoneTsy->ReqCompleted( reqHandle, aError );
            }
        else
            {
            RMobilePhone::CImsAuthorizationInfoV5* imsAuthorizationInfo = 
                                                                        NULL;
            aDataPackage->UnPackData( imsAuthorizationInfo );

            // Serialize data (RMobilePhone::CImsAuthorizationInfoV5)
            // into iAuthInfoBuffer
            imsAuthorizationInfo->ExternalizeL( iAuthInfoBuffer );
			
           *iAuthorizationInfoBufferSize = iAuthInfoBuffer->Length();

           // Complete request
           iMmPhoneTsy->ReqCompleted( reqHandle, KErrNone );
           }
        }
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetAuthorizationInfoPhase2
// Completes AuthorizationInfoPhase2 request
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetAuthorizationInfoPhase2L(
	TTsyReqHandle aTsyReqHandle,
	RMobilePhone::TClientId* aClient,
	TDes8* aBuffer )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETAUTHORIZATIONINFOPHASE2L_1, "TSY: CMmNetTsy::GetAuthorizationInfoPhase2L");
	
	TInt error = KErrNone;
	
	// Check client id
	if ( (aClient->iSessionHandle != iAuthorizationInfoClientId.iSessionHandle )
		|| ( aClient->iSubSessionHandle != 
                              iAuthorizationInfoClientId.iSubSessionHandle ) )
		{
		error = KErrBadHandle;
		}
	else
		{
		if ( iAuthInfoBuffer )
			{
			// Copy data to client buffer	
			TPtr8 bufPtr = iAuthInfoBuffer->Des();
			if ( bufPtr.Length() <= aBuffer->Length() )
				{
				aBuffer->Copy( bufPtr );
				}
			else
				{
				error = KErrArgument;
				}
			
			// delete the buffer

			delete iAuthInfoBuffer;
			iAuthInfoBuffer = NULL;
			}
		else
			{
			error = KErrArgument;
			}
		}
	iMmPhoneTsy->ReqCompleted( aTsyReqHandle, error );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::GetAuthorizationInfoPhase1Cancel
// Cancels AuthorizationInfoPhase1 request
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetAuthorizationInfoPhase1Cancel()
    {
    // reset all request related pointers to client space
    iAuthorizationInfoBufferSize = NULL;

    
	// delete the buffer
	if ( iAuthInfoBuffer )
		{
		delete iAuthInfoBuffer;
		iAuthInfoBuffer = NULL;
		}
	
    // reset the req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetAuthorizationInfoPhase1 );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle)
    	{
    	iMmPhoneTsy->ReqCompleted( reqHandle, KErrCancel );
    	}

    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CMmNetTsy::GetCurrentActiveUSimApplicationL
// Gets current active USim application
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCurrentActiveUSimApplicationL(
    const TTsyReqHandle aTsyReqHandle,
    RMobilePhone::TAID* aAID )
    {  
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCURRENTACTIVEUSIMAPPLICATIONL_1, "TSY:CMmNetTsy::GetCurrentActiveUSimApplicationL");
	TTsyReqHandle getCurrentActiveUSimApplicationHandle =
	    iMmPhoneTsy->iTsyReqHandleStore->GetTsyReqHandle(
	    CMmPhoneTsy::EMultimodePhoneGetCurrentActiveUSimApplication );
	    
    // Check if operation is already ongoing
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown !=
         getCurrentActiveUSimApplicationHandle )
        {
        // Complete request with KErrServerBusy
        iMmPhoneTsy->ReqCompleted( aTsyReqHandle, KErrServerBusy );
        }	       
    else
		{
		// Current active USim application Id has to be asynchronously requested from LTSY layer
		TInt ret = iMmPhoneTsy->MessageManager()->HandleRequestL( EMobilePhoneGetCurrentActiveUSimApplication );

		if ( KErrNone == ret )
			{
			// call to LTSY layer successful, save request handle and return pointer
			// for completion
			iRetCurrentActiveUSimApplication = aAID;
            iMmPhoneTsy->iReqHandleType =
                CMmPhoneTsy::EMultimodePhoneGetCurrentActiveUSimApplication;
			}
		else
			{
			// call to LTSY layer failed, immediately complete with error
			iMmPhoneTsy->ReqCompleted( aTsyReqHandle, ret );
			}
		}

    return KErrNone;   
    }
        
// ---------------------------------------------------------------------------
// CMmNetTsy::GetCurrentActiveUSimApplicationCancel
// Cancels an outstanding GetCurrentActiveUSimApplication request
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmNetTsy::GetCurrentActiveUSimApplicationCancel()
    { 
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_GETCURRENTACTIVEUSIMAPPLICATIONCANCEL_1, "TSY: CMmNetTsy::GetCurrentActiveUSimApplicationCancel");
    //reset the req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetCurrentActiveUSimApplication );
    
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
        // reset the pointer to client data
        iRetCurrentActiveUSimApplication = NULL;        
        iMmPhoneTsy->ReqCompleted( reqHandle, KErrCancel );        
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmNetTsy::CompleteGetCurrentActiveUSimApplication
// Completes a GetCurrentActiveUSimApplication request to client side
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmNetTsy::CompleteGetCurrentActiveUSimApplication(
    CMmDataPackage* aDataPackage,
    TInt aErrorValue )
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMNETTSY_COMPLETEGETCURRENTACTIVEUSIMAPPLICATION_1, "TSY: CMmNetTsy::CompleteGetCurrentActiveUSimApplication");
    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle =
        iMmPhoneTsy->iTsyReqHandleStore->ResetTsyReqHandle(
        CMmPhoneTsy::EMultimodePhoneGetCurrentActiveUSimApplication );
        
    //check the validity of the handler
    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle )
        {
	    //unpack the data
	    RMobilePhone::TAID* aid = NULL;
	    aDataPackage->UnPackData ( &aid );
	    	    
        *iRetCurrentActiveUSimApplication = *aid;
        iRetCurrentActiveUSimApplication = NULL;
        
        //Complete the client request
        iMmPhoneTsy->ReqCompleted( reqHandle, aErrorValue );	        				                          
        }    	        
    }
    
//  End of File 



