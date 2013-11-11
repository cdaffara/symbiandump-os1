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
#include "cmmdatalinetsyTraces.h"
#endif

#include "cmmdatalinetsy.h"
#include "cmmphonetsy.h"
#include "cmmdatacalltsy.h"
#include "cmmcallextinterface.h"
#include "cmmlinelist.h"
#include "cmmcalllist.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmmessagemanagerbase.h"
#include "CMmCommonStaticUtility.h"
#include <ctsy/pluginapi/cmmdatapackage.h>


// ======== MEMBER FUNCTIONS ========

CMmDataLineTsy::CMmDataLineTsy()
    {
    }


void CMmDataLineTsy::ConstructL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_CONSTRUCTL_1, "TSY: CMmDataLineTsy::ConstructL");

    CMmLineTsy::ConstructL();
    }

CMmDataLineTsy* CMmDataLineTsy::NewL(
    CMmPhoneTsy* aMmPhone,
    RMobilePhone::TMobileService aMode,
    const TDesC& aName,
    CMmMessageManagerBase* aMessageManager )
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_NEWL_1, "TSY: CMmDataLineTsy::NewL - Data line %S created", aName);
    CMmDataLineTsy* mmLineTsy = NULL;

    if ( NULL != aMmPhone && RMobilePhone::ECircuitDataService == aMode )
        {
        mmLineTsy = new ( ELeave ) CMmDataLineTsy();
 
        CleanupClosePushL( *mmLineTsy );
        mmLineTsy->iMmPhone  = aMmPhone;    
        mmLineTsy->iLineName = aName;
        mmLineTsy->iLineMode = aMode;
        mmLineTsy->iMessageManager = aMessageManager;
        mmLineTsy->ConstructL();
        CleanupStack::Pop();
        }

    return mmLineTsy;
    }

CMmDataLineTsy::~CMmDataLineTsy()
    {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_DTOR_1, "TSY: CMmDataLineTsy::~CMmDataLineTsy - Line name: %S",iLineName);

    }

// ---------------------------------------------------------------------------
// CMmDataLineTsy::OpenNewObjectL
// This method creates a new name for a call and opens a new  call. This  
// method is called when the client uses RCall::OpenNewCall method. Currently 
// the maximum number of opened calls from tsy is 20.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmDataLineTsy::OpenNewObjectL(
    TDes& aNewName )
    {
    CMmDataCallTsy* mmCall = NULL;

    //Logic here to prevent creation of unlimited amount of call objects
    //by clients. The proposed maximum amount of call objects is 20:
    //
    //  - 2 voice call object (1 for each line for incoming calls
    //  - 1 data call object (for incoming data call)
    //  - 1 emergency call object
    //  - 1+7 call objects for Confence call (max number of CC participants)
    //  - phonebook 1
    //  - javaphone 1
    //  - WTA 1
    //  - voice mail box 1
    //  - csd 1
    //  - fax 1
    //  - 3rd party 2
    //

    //Incoming calls can not be created if the maximum number is exceeded!
    if ( KMaxNumberOfCallObjectsFromTsy 
            > iMmPhone->CallList()->GetNumberOfObjects() )
        {
        CreateNewCallName( aNewName );
        mmCall = CMmDataCallTsy::NewL( iMmPhone, this, iLineMode, aNewName,
            iMessageManager );
        iNameOfLastCallAdded = aNewName;

        // Add the new call to the list of calls
        TInt ret = iMmPhone->CallList()->AddObject( mmCall );
        if ( KErrNone != ret )
            {
            //If call cannot be added to CallList -> leave with KErrNoMemory
            //CallList has to have information about every call.
            mmCall->Close();
            mmCall = NULL;
            User::Leave( KErrNoMemory );
            }
        else
            {
            CompleteNotifyCallAdded( aNewName );
            }
        }
    else
        {
        User::Leave( KErrAccessDenied );
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmDataLineTsy::InitInternalAttributesL
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataLineTsy::InitInternalAttributesL()
    {
    CMmLineTsy::InitInternalAttributesL();

    // The line supports data connections.
    iLineCaps.iFlags = RLine::KCapsData | RLine::KCapsEventIncomingCall;
    }

// ---------------------------------------------------------------------------
// CMmDataLineTsy::CreateCallObjectForIncomingCall
// This function creates and stores a Call object that is used for a incoming 
// call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDataLineTsy::CreateCallObjectForIncomingCall()
    {
    TInt trapError( KErrNone );
    TInt result( CMmPhoneTsy::EMultimodePhoneOutOfMemory );

    if ( !iCallForIncomingCall )
        {
        TName newName;
        CreateNewCallName( newName );

        TRAP( trapError,
            iCallForIncomingCall = CMmDataCallTsy::NewL(
                iMmPhone, this, iLineMode, newName, iMessageManager );
            );

        if ( KErrNone == trapError ) 
            {
            iNameOfLastCallAdded = newName;
            // Don't add the new call to the list of calls yet. Will be
            // Added when incoming call has been received...
            result = KErrNone;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CMmDataLineTsy::CreateGhostCallObject
// This method creates a Call object for the Ghost call. A ghost call is a  
// call that has been initiated bypassing the ETel.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmDataLineTsy::CreateGhostCallObject(
    TInt aCallId,
    RMobilePhone::TMobileService,
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    TInt trapError( KErrNone );
    TName newName;
    CreateNewCallName( newName );
    CMmDataCallTsy* mmCall = NULL;

    TRAP( trapError,
        mmCall = CMmDataCallTsy::NewL(
            iMmPhone, this, iLineMode, newName, iMessageManager );
        );

    if ( KErrNone == trapError && NULL != mmCall )
        {
        //set call ID
        mmCall->SetCallId( aCallId );
        //set call status
        if( aCallStatus >= RMobileCall::EStatusDialling )
            {
            //Status is allways set to dialing. If call is alredy
            //e.g. connecting, missed status changes are simulated.
            mmCall->SetCallStatus(
                RCall::EStatusDialling, RMobileCall::EStatusDialling );
            }
        //set call caps
        mmCall->SetCallCaps( RCall::KCapsData | RCall::KCapsHangUp );
        //set call direction to MO call
        mmCall->SetCallDirection( RMobileCall::EMobileOriginated );
        //set flag indicating that this is a ghost call
        mmCall->SetGhostCall( ETrue );
        //save name of the last added call
        iNameOfLastCallAdded = mmCall->CallName();
        }

    return mmCall;
    }

// ---------------------------------------------------------------------------
// CMmDataLineTsy::CompleteNotifyIncomingCall
// This method notifies clients about incoming call. Basic information: one 
// Call object is allocated in the boot-up for incoming call. When an incoming 
// call occurs the call object will be given to the client that answers the
// call. Therefore TSY creates a new call object for the next incoming call.
// In case of out of memory situation, the next incoming call cannot be
// received! General error message about OutOfMemory situation is sent to the
// client. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataLineTsy::CompleteNotifyIncomingCall(
    CMmDataPackage* aDataPackage )
    {
    TInt callId( -1 );
    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );

    CCallDataPackage* callDataPackage =
        reinterpret_cast<CCallDataPackage*>(aDataPackage);
    callDataPackage->GetCallIdAndMode( callId, callMode ); 

OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYINCOMINGCALL_1, "TSY: CMmDataLineTsy::CompleteNotifyIncomingCall - \Line name: %S, Call id: %d", iLineName, callId );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeLineNotifyIncomingCall );

    CMmDataCallTsy* mmCall = NULL;
    // First check if the incoming call is for Call Back functionality.
    // E.g. client has dialled server, which calls back to save user
    // the data call charging.
    mmCall = reinterpret_cast<CMmDataCallTsy*>(
        iMmPhone->WaitingCallForData() );
    if ( mmCall )
        {
        // If call object is valid, then there is already a call object and
        // client waiting for this incoming call!
        // Set new call id
        mmCall->SetCallId( callId );
        // Set call status
        mmCall->SetCallStatus(
            RCall::EStatusRinging, RMobileCall::EStatusRinging );        
        // Set call direction to EMobileTerminated
        mmCall->SetCallDirection( RMobileCall::EMobileTerminated );
        // Remove dial capability from existing call capabilities
        TUint32 callCaps = mmCall->CallCaps();
        callCaps &= ~RCall::KCapsDial;
        mmCall->SetCallCaps( callCaps );

        mmCall->ActiveCallExtension()->RemoveGSMCallCaps(
            RCall::KCapsDial );
        mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );

        iNameOfCallForAnswering = mmCall->CallName();
        
        *iRetIncomingCallName = mmCall->CallName(); 
        iCallForIncomingCall = NULL;

        // Used to pass information for call object's GetMobileCallInfo.
        // Set data to gsm extension
        //( direction added because not uppdated in messagehandler)
        mmCall->ActiveCallExtension()->SetMobileCallData(
            aDataPackage, RMobileCall::EMobileTerminated );
        // Data caps are changed
        RMobileCall::TMobileCallDataCapsV1 caps;

        mmCall->ActiveCallExtension()->GetMobileDataCallCapsV1( caps );
           
        caps.iSpeedCaps = RMobileCall::KCapsSpeedAutobauding;
        caps.iProtocolCaps = 0x00;
        caps.iServiceCaps = 0x00;
        caps.iQoSCaps = 0x00;
        caps.iHscsdSupport = EFalse;
        caps.iMaxRxTimeSlots = 0x00;
        caps.iMaxTxTimeSlots = 0x00;
        caps.iCodingCaps = 0x00;
        caps.iAsymmetryCaps = 0x00;
        caps.iUserInitUpgrade = EFalse;
        caps.iRLPVersionCaps = 0x00;
        caps.iV42bisCaps = 0x00;
        mmCall->ActiveCallExtension()->ReceivedMobileDataCallCaps( &caps );
        mmCall->CompleteNotifyMobileDataCallCapsChange();

        // If notification is requested, complete it here!
        if ( EMultimodeLineReqHandleUnknown != reqHandle )
            {
            ReqCompleted( reqHandle, KErrNone );
            }
        mmCall = NULL;
        }
    // Check if notification is requested
    else if ( EMultimodeLineReqHandleUnknown != reqHandle )
        {
        mmCall = reinterpret_cast<CMmDataCallTsy*>(
            CallObjectForIncomingCall() );

        if ( mmCall )
            {
            // Add the new call to the list of calls
            TInt ret = iMmPhone->CallList()->AddObject( mmCall );

            if ( KErrNone == ret )
                {
                // Set flag indicating that this is a unowned incoming call
                mmCall->SetUnownedCallObjectFlag( ETrue );
                // Set call ID
                mmCall->SetCallId( callId );
                // Set call status
                mmCall->SetCallStatus(
                    RCall::EStatusRinging, RMobileCall::EStatusRinging );                
                // Set call direction to EMobileTerminated
                mmCall->SetCallDirection( RMobileCall::EMobileTerminated );

                // Remove dial capability from existing call capabilities
                TUint32 callCaps = mmCall->CallCaps();
                callCaps &= ~RCall::KCapsDial;
                mmCall->SetCallCaps( callCaps );

                mmCall->ActiveCallExtension()->RemoveGSMCallCaps(
                    RCall::KCapsDial );
                mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );

                // Set data to gsm extension
                //( direction added because not uppdated in messagehandler)
                mmCall->ActiveCallExtension()->SetMobileCallData(
                    aDataPackage, RMobileCall::EMobileTerminated );

                iNameOfCallForAnswering = mmCall->CallName();

                *iRetIncomingCallName = mmCall->CallName(); 
                iCallForIncomingCall = NULL;
				iNumCalls++;

                ReqCompleted( reqHandle, KErrNone );

                // Data caps are changed
                RMobileCall::TMobileCallDataCapsV1 caps;
      
                mmCall->ActiveCallExtension()->GetMobileDataCallCapsV1( caps );                   
          
                caps.iSpeedCaps = RMobileCall::KCapsSpeedAutobauding;
                caps.iProtocolCaps = 0x00;
                caps.iServiceCaps = 0x00;
                caps.iQoSCaps = 0x00;
                caps.iHscsdSupport = EFalse;
                caps.iMaxRxTimeSlots = 0x00;
                caps.iMaxTxTimeSlots = 0x00;
                caps.iCodingCaps = 0x00;
                caps.iAsymmetryCaps = 0x00;
                caps.iUserInitUpgrade = EFalse;
                caps.iRLPVersionCaps = 0x00;
                caps.iV42bisCaps = 0x00;
                mmCall->ActiveCallExtension()->
                    ReceivedMobileDataCallCaps( &caps );
                mmCall->CompleteNotifyMobileDataCallCapsChange();

                // Create and store a new Call Object for incoming call.
                CreateCallObjectForIncomingCall();
                }
            else
                {
                //call object could not be added to the call list. Incoming 
                //call cannot be received.
                ReqCompleted( reqHandle, KErrNoMemory );
                }
            }
        else
            {
            //call object could not be get. Incoming call cannot be received.
            //General error message about OutOfMemory situation has been sent
            //to the client.
            ReqCompleted( reqHandle, KErrNoMemory );
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmDataLineTsy::CompleteNotifyDiallingStatus
// This method notifies line that a call has entered Dialling state. It means
// that Call ID should be saved and in case of ghost call a new call object
// should be created.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDataLineTsy::CompleteNotifyDiallingStatus(
    CMmDataPackage* aDataPackage )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_1, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus");

    TInt callId( -1 );
    TBool ghostCall( EFalse );
    RMobilePhone::TMobileService callMode( 
                                      RMobilePhone::ECircuitDataService );

    CCallDataPackage* callData =
        reinterpret_cast<CCallDataPackage*>( aDataPackage );

    callData->GetCallIdAndMode( callId, callMode );

    CMmDataCallTsy* mmCall = reinterpret_cast<CMmDataCallTsy*>(
        iMmPhone->CallList()->GetMmCallByMode( callMode ) );

    if ( NULL == mmCall )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_2, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus - \GhostCall");
        ghostCall = ETrue;
        }
    else
        {
        TBool result( EFalse );
        TBool resultNoFdnCheck( EFalse );

        //now we have to check, if this is a ghost call
        result = mmCall->IsServiceLocallyRequested(
            CMmCallTsy::EMultimodeCallDial );

        //now we have to check, if this is a ghost call
        resultNoFdnCheck = mmCall->IsServiceLocallyRequested( 
            CMmCallTsy::EMultimodeCallDialNoFdnCheck );

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_3, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus - IsDial: %d", result );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_4, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus - IsDialNoFdnCheck: %d", resultNoFdnCheck );

        if ( result || resultNoFdnCheck )
            {
            for ( TInt i = 0; i < iMmPhone->CallList()->
                                                GetNumberOfObjects(); i++ )
                {    
                mmCall = reinterpret_cast<CMmDataCallTsy*>(
                    iMmPhone->CallList()->GetMmCallByIndex(i) );
                    
                if ( ( mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeCallDial ) )
                    || ( mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeCallDialNoFdnCheck ) ) )
                    {
                    mmCall->SetCallId( callId );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_5, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus - mmCall SetCallId: %d", callId);
                    mmCall->CompleteNotifyMobileCallInfoChange( 
                                aDataPackage );
                    break;
                    }
                }
            }
        else
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_6, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus - Dial not found");
            ghostCall = ETrue;
            }
        }
    //Ghost call handling starts here
    if ( ghostCall )
        {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDATALINETSY_COMPLETENOTIFYDIALLINGSTATUS_7, "TSY: CMmDataLineTsy::CompleteNotifyDiallingStatus -> CompleteNotifyAddBypassingCall");
        //complete call added notification directly from here
        CompleteNotifyAddBypassingCall( aDataPackage ); 
        }
    }



//  End of File
