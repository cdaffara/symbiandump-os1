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



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmvoicelinetsyTraces.h"
#endif

#include "cmmvoicelinetsy.h"
#include "cmmphonetsy.h"
#include "cmmvoicecalltsy.h"
#include "cmmcallextinterface.h"
#include "cmmlinelist.h"
#include "cmmcalllist.h"
#include "cmmtsyreqhandlestore.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include <ctsy/pluginapi/mtelephonyaudiocontrol.h>

// ======== MEMBER FUNCTIONS ========

CMmVoiceLineTsy::CMmVoiceLineTsy()
    {
    }

void CMmVoiceLineTsy::ConstructL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_CONSTRUCTL_1, "TSY: CMmVoiceLineTsy::ConstructL");
    CMmLineTsy::ConstructL();
    }

CMmVoiceLineTsy* CMmVoiceLineTsy::NewL(
    CMmPhoneTsy* aMmPhone,
    RMobilePhone::TMobileService aMode,
    const TDesC& aName,
    CMmMessageManagerBase* aMessageManager,
    MTelephonyAudioControl* aTelephonyAudioControl )
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_NEWL_1, "TSY: CMmVoiceLineTsy::NewL, Voice line %S created", aName);
    CMmVoiceLineTsy* mmLineTsy = NULL;

    if ( aMmPhone != NULL && ( aMode == RMobilePhone::EVoiceService 
        || aMode == RMobilePhone::EAuxVoiceService ) )
        {
        mmLineTsy = new ( ELeave ) CMmVoiceLineTsy();
        CleanupClosePushL( *mmLineTsy );
        mmLineTsy->iMmPhone  = aMmPhone;    
        mmLineTsy->iLineName = aName;
        mmLineTsy->iLineMode = aMode;
        mmLineTsy->iMessageManager = aMessageManager;
        mmLineTsy->iTelephonyAudioControl = aTelephonyAudioControl;
        mmLineTsy->ConstructL();
        CleanupStack::Pop();
        }
    return mmLineTsy;
    }

CMmVoiceLineTsy::~CMmVoiceLineTsy()
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_DTOR_1, "TSY: CMmVoiceLineTsy::~CMmVoiceLineTsy. Line name: %S", iLineName);
    }

// ---------------------------------------------------------------------------
// CMmVoiceLineTsy::OpenNewObjectL
// This method creates a new name for a call and opens a new call. This method 
// is called when the client uses RCall::OpenNewCall method. Currently the 
// maximum number of opened calls from tsy is 20.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmVoiceLineTsy::OpenNewObjectL(
    TDes& aNewName )
    {
    CMmVoiceCallTsy* mmCall = NULL;

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
        mmCall = CMmVoiceCallTsy::NewL( iMmPhone, this, iLineMode, aNewName, 
            iMessageManager, iTelephonyAudioControl );

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
// CMmVoiceLineTsy::InitInternalAttributesL
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceLineTsy::InitInternalAttributesL()
    {
    CMmLineTsy::InitInternalAttributesL();

    // The line supports voice connections.
    iLineCaps.iFlags = RLine::KCapsVoice | RLine::KCapsEventIncomingCall;

	if( RMobilePhone::EAuxVoiceService == iLineMode )
    	{
    	iLineCaps.iFlags |= RMobileLine::KCapsAuxVoice;
    	}
    
    //flag for swap handling
    iSwapStatus = EStatusSwapNotRequested;
    }

// ---------------------------------------------------------------------------
// CMmVoiceLineTsy::CreateCallObjectForIncomingCall
// This function creates and stores a Call object that is used for a incoming 
// call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmVoiceLineTsy::CreateCallObjectForIncomingCall()
    {
    TInt trapError( KErrNone );
    TInt result( CMmPhoneTsy::EMultimodePhoneOutOfMemory );

    if ( !iCallForIncomingCall )
        {
        TName newName;
        CreateNewCallName( newName );

        TRAP( trapError,
            iCallForIncomingCall = CMmVoiceCallTsy::NewL(
                iMmPhone, this, iLineMode, newName, iMessageManager, 
                iTelephonyAudioControl );
            );

        if ( trapError == KErrNone ) 
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
// CMmVoiceLineTsy::CreateGhostCallObject
// This method creates a Call object for the Ghost call. A ghost call is a 
// call that has been initiated bypassing the ETel.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmVoiceLineTsy::CreateGhostCallObject(
    TInt aCallId,
    RMobilePhone::TMobileService,
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    TInt trapError( KErrNone );

    TName newName;
    CreateNewCallName( newName );
    CMmVoiceCallTsy* mmCall = NULL;

    TRAP( trapError, ( mmCall = CMmVoiceCallTsy::NewL( iMmPhone, 
                    this, iLineMode, newName, iMessageManager, 
                    iTelephonyAudioControl ) ) );

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
        mmCall->SetCallCaps( RCall::KCapsVoice | RCall::KCapsHangUp );
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
// CMmVoiceLineTsy::CompleteNotifyIncomingCall
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
void CMmVoiceLineTsy::CompleteNotifyIncomingCall(
    CMmDataPackage* aDataPackage )
    {
    TInt callId( -1 );
    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );

    CCallDataPackage* callDataPackage = 
        reinterpret_cast<CCallDataPackage*>(aDataPackage);

    callDataPackage->GetCallIdAndMode( callId, callMode ); 

    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_COMPLETENOTIFYINCOMINGCALL_1, "TSY: CMmVoiceLineTsy::CompleteNotifyIncomingCall - Line \name: %S, Call id: %d", iLineName, callId );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle( 
        EMultimodeLineNotifyIncomingCall );

    CMmVoiceCallTsy* mmCall = NULL;

    // Check if notification is requested
    if ( EMultimodeLineReqHandleUnknown != reqHandle )
        {
        mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
            CallObjectForIncomingCall() );

        if ( mmCall )
            {
            // Add the new call to the list of calls
            TInt ret = iMmPhone->CallList()->AddObject( mmCall );

            if ( KErrNone == ret )
                {
                //set flag indicating that this is a unowned incoming call
                mmCall->SetUnownedCallObjectFlag( ETrue );
                //set call ID
                mmCall->SetCallId( callId );
                // Set call direction to EMobileTerminated
                mmCall->SetCallDirection( RMobileCall::EMobileTerminated );

                // Add answer capability and remove dial capability to
                // existing call capabilities
                TUint32 callCaps = mmCall->CallCaps();
                callCaps |= RCall::KCapsAnswer; 
                callCaps &= ~RCall::KCapsDial;
                mmCall->SetCallCaps( callCaps );

                //set data to gsm extension
                //( direction added because not uppdated in messagehandler)
                mmCall->ActiveCallExtension()->SetMobileCallData(
                    aDataPackage, RMobileCall::EMobileTerminated );

                iNameOfCallForAnswering = mmCall->CallName();
				iNumCalls++;

                *iRetIncomingCallName = mmCall->CallName(); 
                iCallForIncomingCall = NULL;
                ReqCompleted( reqHandle, KErrNone );

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
// CMmVoiceLineTsy::CompleteNotifyDiallingStatus
// This method notifies line that a call has entered Dialling state. It means
// that Call ID should be saved and in case of ghost call a new call object
// should be created.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceLineTsy::CompleteNotifyDiallingStatus(
    CMmDataPackage* aDataPackage )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_COMPLETENOTIFYDIALLINGSTATUS_1, "TSY: CMmVoiceLineTsy::CompleteNotifyDiallingStatus");
    TInt callId( -1 );
    TBool ghostCall( EFalse );
    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );

    CCallDataPackage* callData = 
        reinterpret_cast<CCallDataPackage*>(aDataPackage);

    callData->GetCallIdAndMode( callId, callMode );

    CMmVoiceCallTsy* mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
        iMmPhone->CallList()->GetMmCallByMode( callMode ) );

    if ( NULL == mmCall )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_COMPLETENOTIFYDIALLINGSTATUS_2, "TSY: CMmVoiceLineTsy::CompleteNotifyDiallingStatus - \GhostCall");
        ghostCall = ETrue;
        }
    else
        {
        TBool result( EFalse );
        TBool resultNoFdnCheck( EFalse );
        TBool resultISV( EFalse );

        //now we have to check, if this is a ghost call
        result = mmCall->IsServiceLocallyRequested( 
            CMmCallTsy::EMultimodeCallDial );
            
        //now we have to check, if this is a ghost call
        resultNoFdnCheck = mmCall->IsServiceLocallyRequested( 
            CMmCallTsy::EMultimodeCallDialNoFdnCheck );
         //now we have to check, if this is a ghost call (3rd party )
        resultISV = mmCall->IsServiceLocallyRequested( 
            CMmCallTsy::EMultimodeCallDialISV );

        if ( ( !result ) && ( !resultISV ) && ( !resultNoFdnCheck ) )
            {
            result = mmCall->IsServiceLocallyRequested( 
                CMmCallTsy::EMultimodeMobileCallDialEmergencyCall );
            }        

        if ( result || resultNoFdnCheck || resultISV )
            {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_COMPLETENOTIFYDIALLINGSTATUS_3, "TSY: CMmVoiceLineTsy::CompleteNotifyDiallingStatus - Not Emergency call");
            for ( TInt i = 0; i < iMmPhone->CallList()->GetNumberOfObjects(); 
                      i++ )
                {    
                mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
                    iMmPhone->CallList()->GetMmCallByIndex(i) );
                if ( ( ( mmCall->CallId() == 0 ) || ( mmCall->CallId() == -1 ))
                    && ( mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeCallDial ) 
                    ||  mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeMobileCallDialEmergencyCall )
                    || ( mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeCallDialISV ) ) 
                    || ( mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeCallDialNoFdnCheck ) ) ) )
                    {
                    mmCall->SetCallId( callId );
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_COMPLETENOTIFYDIALLINGSTATUS_4, "TSY: CMmVoiceLineTsy::CompleteNotifyDiallingStatus - mmCall SetCallId: %d", callId);
                    mmCall->CompleteNotifyMobileCallInfoChange( 
                                aDataPackage );
                    break;   
                    }
                }
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMVOICELINETSY_COMPLETENOTIFYDIALLINGSTATUS_5, "TSY: CMmVoiceLineTsy::CompleteNotifyDiallingStatus - \Dial not found");
            ghostCall = ETrue;
            }
        }
    //Ghost call handling starts here
    if ( ghostCall )
        {
        //complete call added notification directly from here
        CompleteNotifyAddBypassingCall( aDataPackage ); 
        }
    }

// ---------------------------------------------------------------------------
// CMmVoiceLineTsy::IsSwapReady
// Contains the logic for determining if a Swap request is ready
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmVoiceLineTsy::IsSwapReady(
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    TBool ret( EFalse );
    CMmVoiceLineTsy* otherVoiceLine = NULL;

    CMmLineList* lineList = iMmPhone->LineList();
    TInt numberOfObjectsInLineList = lineList->GetNumberOfObjects();
    RMobilePhone::TMobileService lineMode( 
                                      RMobilePhone::EServiceUnspecified );

    //Required by ALS. We have to check what is the status of calls at the 
    //other line. This for loop find the other voice line (primary or aux).
    for ( TInt i = 0; i < numberOfObjectsInLineList; i++ )
        {
        CMmVoiceLineTsy* line = reinterpret_cast<CMmVoiceLineTsy*>(
            lineList->GetMmLineByIndex( i ) );
            
        if ( line != this && line != NULL )
            {
			lineMode = line->LineMode();
            if ( ( lineMode == RMobilePhone::EVoiceService ) ||
                ( lineMode == RMobilePhone::EAuxVoiceService ) )
                {
                otherVoiceLine = line;
                break;
                }
            }
        }

	CMmCallList* callList = iMmPhone->CallList();
	TInt numberOfObjectsInCallList = callList->GetNumberOfObjects();
    CMmVoiceCallTsy* mmCall = NULL;
		
    //is TSY still waiting both hold and connected status indications
    if ( EWaitingHoldAndResumeOk == iSwapStatus )
        {
        //TSY is still waiting both hold and connected status indications
        if ( RMobileCall::EStatusHold == aCallStatus )
            {
            //hold status indication received
            //-> still waiting connected status indication
            iSwapStatus = EWaitingResumeOk;
            }
        else if ( RMobileCall::EStatusConnected == aCallStatus )
            {
            //connected status indication received
            //-> still waiting hold status indication
            iSwapStatus = EWaitingHoldOk;
            }
        }
    //swap handling is going on. 
    else if ( ( EWaitingResumeOk == iSwapStatus &&
        RMobileCall::EStatusConnected == aCallStatus ) ||
              ( EWaitingHoldOk == iSwapStatus &&
        RMobileCall::EStatusHold == aCallStatus ) )
        {
        //find the call object from which the swap was requested.
        for ( TInt i = 0; i < numberOfObjectsInCallList; i++ )
            {
            mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
                callList->GetMmCallByIndex(i) );
            if ( mmCall->ServiceRequested( 
                    CMmCallTsy::EMultimodeMobileCallSwap ) )
                {
                //complete swap request when the correct object has been found
                mmCall->CompleteSwap( KErrNone );
                break;
                }
            }
        //clear swap status.
        iSwapStatus = EStatusSwapNotRequested;
        //return value indicating that Swap was completed.
        ret = ETrue;
        }
    //swap handling is going on. This the case where the calls to be swapped 
    //are opened different lines (primary and aux).
    else if ( otherVoiceLine )
        {
        if ( ( EWaitingResumeOk == otherVoiceLine->SwapStatus() &&
            RMobileCall::EStatusConnected == aCallStatus ) ||
            ( EWaitingHoldOk == otherVoiceLine->SwapStatus() &&
            RMobileCall::EStatusHold == aCallStatus ) )
            {
            CMmCallList* callList = iMmPhone->CallList();
            TInt numberOfObjectsInCallList = callList->GetNumberOfObjects();
            CMmVoiceCallTsy* mmCall = NULL;
            //find the call object from which the swap was requested.
            for ( TInt i = 0; i < numberOfObjectsInCallList; i++ )
                {
                mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
                    callList->GetMmCallByIndex(i) );
                if ( mmCall->ServiceRequested( 
                        CMmCallTsy::EMultimodeMobileCallSwap ) )
                    {
                    //complete swap request when the correct object has found
                    mmCall->CompleteSwap( KErrNone );
                    break;
                    }
                }
            //clear swap status.
            iSwapStatus = EStatusSwapNotRequested;
            //clear swap status also from the aux line
            otherVoiceLine->SetSwapStatus( EStatusSwapNotRequested );

            //return value indicating that Swap was completed.
            ret = ETrue;
            }
        }

    if ( numberOfObjectsInCallList==1 && !otherVoiceLine )
    	{

    	// Swapping a single call. 
        mmCall = reinterpret_cast<CMmVoiceCallTsy*>(
            callList->GetMmCallByIndex(0) );
        if ( mmCall->ServiceRequested( 
                CMmCallTsy::EMultimodeMobileCallSwap ) )
            {
            // Complete swap request when the correct object has been found
            mmCall->CompleteSwap( KErrNone );
            }    	
    	ret = ETrue;
    	}
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmVoiceLineTsy::SetSwapStatus
// Sets status of Swap. This can be used by call objects opened from this line
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmVoiceLineTsy::SetSwapStatus(
    CMmVoiceLineTsy::TSwapStatus aSwapStatus )
    {
    iSwapStatus = aSwapStatus;
    }

// ---------------------------------------------------------------------------
// CMmVoiceLineTsy::SwapStatus
// Return status of Swap for this Line. This can be used by call objects  
// opened from this line.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmVoiceLineTsy::TSwapStatus CMmVoiceLineTsy::SwapStatus() const
    {
    return iSwapStatus;
    }

//  End of File 
