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
#include "cmmfaxlinetsyTraces.h"
#endif

#include "cmmfaxlinetsy.h"
#include "cmmphonetsy.h"
#include "cmmfaxcalltsy.h"
#include "cmmcallextinterface.h"
#include "cmmlinelist.h"
#include "cmmcalllist.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmmessagemanagerbase.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmFaxLineTsy::CMmFaxLineTsy()
    {
    }


CMmFaxLineTsy* CMmFaxLineTsy::NewL(
    CMmPhoneTsy* aMmPhone,
    RMobilePhone::TMobileService aMode,
    const TDesC& aName,
    CMmMessageManagerBase* aMessageManager )
    {
    CMmFaxLineTsy* mmLineTsy = NULL;

    if ( aMmPhone != NULL && aMode == RMobilePhone::EFaxService )
        {
        mmLineTsy = new ( ELeave ) CMmFaxLineTsy();
 
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

CMmFaxLineTsy::~CMmFaxLineTsy()
    {
    OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXLINETSY_DTOR_1, "TSY: CMmFaxLineTsy::~CMmFaxLineTsy. Line name: %S",iLineName);

    iLastIncomingFaxCall = NULL;
    }
    

// ---------------------------------------------------------------------------
// CMmFaxLineTsy::OpenNewObjectL
// This method creates a new name for a call and opens a new  call. This  
// methodis called when the client uses RCall::OpenNewCall method. Currently 
// the maximum number of opened calls from tsy is 20.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CTelObject* CMmFaxLineTsy::OpenNewObjectL(
    TDes& aNewName )
    {
    CMmFaxCallTsy* mmCall = NULL;

    // mmfax: Fax client expects that it get's the existing call object,
    // not the new one..
    if ( iLastIncomingFaxCall )
        {
        aNewName.Copy(iLastIncomingFaxCall->CallName());
        iLastIncomingFaxCall = NULL;
        mmCall = reinterpret_cast<CMmFaxCallTsy*>(
            OpenNewObjectByNameL(aNewName) );
        return mmCall;
        }

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
        mmCall = CMmFaxCallTsy::NewL(
            iMmPhone, this, iLineMode, aNewName, iMessageManager );
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
// CMmFaxLineTsy::InitInternalAttributesL
// Initialises miscellaneous internal attributes.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxLineTsy::InitInternalAttributesL()
    {
    CMmLineTsy::InitInternalAttributesL();

    // The line supports data connections.
    iLineCaps.iFlags = RLine::KCapsFax | RLine::KCapsEventIncomingCall;
    }

// ---------------------------------------------------------------------------
// CMmFaxLineTsy::CreateCallObjectForIncomingCall
// This function creates and stores a Call object that is used for a incoming 
// call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmFaxLineTsy::CreateCallObjectForIncomingCall()
    {
    TInt trapError( KErrNone );
    TInt result = CMmPhoneTsy::EMultimodePhoneOutOfMemory;

    if ( !iCallForIncomingCall )
        {
        TName newName;
        CreateNewCallName( newName );

        TRAP( trapError,
            iCallForIncomingCall = CMmFaxCallTsy::NewL(
                iMmPhone, this, iLineMode, newName, iMessageManager );
            );

        if ( trapError == KErrNone ) 
            {
            iNameOfLastCallAdded = newName;
            // Don't add the new call to the list of calls yet. Will be
            // added when incoming call has been received...
            result = KErrNone;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CMmFaxLineTsy::CreateGhostCallObject
// This method creates a Call object for the Ghost call. A ghost call is a  
// call that has been initiated bypassing the ETel.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CMmCallTsy* CMmFaxLineTsy::CreateGhostCallObject(
    TInt aCallId,
    RMobilePhone::TMobileService,
    RMobileCall::TMobileCallStatus aCallStatus )
    {
    TInt trapError( KErrNone );

    TName newName;
    CreateNewCallName( newName );
    CMmFaxCallTsy* mmCall = NULL;

    TRAP( trapError,
        mmCall = CMmFaxCallTsy::NewL(
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
        mmCall->SetCallCaps( RCall::KCapsFax | RCall::KCapsHangUp );
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
// CMmFaxLineTsy::CompleteNotifyIncomingCall
// This method is used to notify to the client about the incoming call. Basic 
// information: one Call object is allocated in the boot-up for incoming call. 
// When an incoming call occurs the call object will be given to the client 
// that answers the call. Therefore we have to create a new call object for 
// the next incoming call. In case that we are out of memory, the next 
// incoming call cannot be received! General error message about OutOfMemory  
// situation is sent to the client. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxLineTsy::CompleteNotifyIncomingCall(
    CMmDataPackage* aDataPackage )
    {
    TInt callId( -1 );
    RMobilePhone::TMobileService callMode( RMobilePhone::EVoiceService );

    CCallDataPackage* callDataPackage =
        reinterpret_cast<CCallDataPackage*>(aDataPackage);

    callDataPackage->GetCallIdAndMode( callId, callMode ); 

    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXLINETSY_COMPLETENOTIFYINCOMINGCALL_1, "TSY: CMmFaxLineTsy::CompleteNotifyIncomingCall - \Line name: %S, Call id: %d", iLineName, callId );

    //reset req handle. Returns the deleted req handle
    TTsyReqHandle reqHandle = iTsyReqHandleStore->ResetTsyReqHandle(
        EMultimodeLineNotifyIncomingCall );

    CMmFaxCallTsy* mmCall = NULL;

    // Check if notification is requested
    if ( EMultimodeLineReqHandleUnknown != reqHandle )
        {
        mmCall = reinterpret_cast<CMmFaxCallTsy*>(
            CallObjectForIncomingCall() );

        if ( mmCall )
            {
            // Add the new call to the list of calls
            TInt ret = iMmPhone->CallList()->AddObject( mmCall );

            if ( KErrNone == ret )
                {
                //set flag indicating that this is a unowned incoming call
                mmCall->SetUnownedCallObjectFlag( ETrue );
                // Set call ID
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

                // mmfax: Save ptr to last incoming fax call, so that
                // it can be used to open right call object for fax.
                iLastIncomingFaxCall = mmCall;

                iNameOfCallForAnswering = mmCall->CallName();

                *iRetIncomingCallName = mmCall->CallName(); 
                iCallForIncomingCall = NULL;
                iNumCalls++;
                
                ReqCompleted( reqHandle, KErrNone );

                // Create and store a new Call Object for incoming call.
                CreateCallObjectForIncomingCall();

                RMobileCall::TMobileCallStatus status(
                    RMobileCall::EStatusRinging );
                CCallDataPackage dataPackage;
                dataPackage.SetCallIdAndMode( callId, callMode );
                dataPackage.PackData( &status );
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
// CMmFaxLineTsy::CompleteNotifyDiallingStatus
// This method notifies line that a call has entered Dialling state. It means
// that Call ID should be saved and in case of ghost call a new call object
// should be created.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmFaxLineTsy::CompleteNotifyDiallingStatus(
    CMmDataPackage* aDataPackage )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXLINETSY_COMPLETENOTIFYDIALLINGSTATUS_1, "TSY: CMmFaxLineTsy::CompleteNotifyDiallingStatus");
    TInt callId( -1 );
    TBool ghostCall( EFalse );
    RMobilePhone::TMobileService callMode( RMobilePhone::EFaxService );
    
    CCallDataPackage* callData 
        = reinterpret_cast<CCallDataPackage*>(aDataPackage);

    callData->GetCallIdAndMode( callId, callMode );

    CMmFaxCallTsy* mmCall = reinterpret_cast<CMmFaxCallTsy*>(
        iMmPhone->CallList()->GetMmCallByMode( callMode ) );

    if ( NULL == mmCall )
        {
        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXLINETSY_COMPLETENOTIFYDIALLINGSTATUS_2, "TSY: CMmFaxLineTsy::CompleteNotifyDiallingStatus - \GhostCall");
        ghostCall = ETrue;
        }
    else
        {
        TBool result( EFalse );

        //now we have to check, if this is a ghost call
        result = mmCall->IsServiceLocallyRequested(
            CMmCallTsy::EMultimodeCallDial );

        if ( result )
            {
            for ( TInt i = 0; i < iMmPhone->CallList()->GetNumberOfObjects(); 
                i++ )
                {    
                mmCall = reinterpret_cast<CMmFaxCallTsy*>(
                    iMmPhone->CallList()->GetMmCallByIndex(i) );
                if ( mmCall->ServiceRequested(
                        CMmCallTsy::EMultimodeCallDial ) )
                    {
                    mmCall->SetCallId( callId );
                    mmCall->CompleteNotifyMobileCallInfoChange( 
                        aDataPackage );
                    break;   
                    }
                }
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXLINETSY_COMPLETENOTIFYDIALLINGSTATUS_3, "TSY: CMmFaxLineTsy::CompleteNotifyDiallingStatus - \Dial not found");
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
// CMmFaxLineTsy::ResetLastIncomingCall
// This method sets iLastIncomingFaxCall to NULL
// ---------------------------------------------------------------------------
//
void CMmFaxLineTsy::ResetLastIncomingCall()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMFAXLINETSY_RESETLASTINCOMINGCALL_1, "TSY: CMmFaxLineTsy::ResetLastIncomingCall");
    iLastIncomingFaxCall = NULL;
    }
    
//  End of File 
