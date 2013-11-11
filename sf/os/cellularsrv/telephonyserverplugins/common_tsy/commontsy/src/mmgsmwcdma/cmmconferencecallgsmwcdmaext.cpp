// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cmmconferencecallgsmwcdmaextTraces.h"
#endif

#include "cmmconferencecallgsmwcdmaext.h"
#include "cmmphonetsy.h"
#include "cmmconferencecalltsy.h"
#include "cmmvoicecalltsy.h"
#include "cmmcalllist.h"
#include "cmmcallgsmwcdmaext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagemanagerbase.h"
#include <etelmmerr.h> // etel error codes

// ======== MEMBER FUNCTIONS ========

CMmConferenceCallGsmWcdmaExt::CMmConferenceCallGsmWcdmaExt()
    {
    }

CMmConferenceCallGsmWcdmaExt::~CMmConferenceCallGsmWcdmaExt()
    {
    iMmConferenceCallTsy = NULL;
    }

CMmConferenceCallGsmWcdmaExt* CMmConferenceCallGsmWcdmaExt::NewL(
    CMmConferenceCallTsy* aMmConferenceCallTsy )
    {
    CMmConferenceCallGsmWcdmaExt* const conferenceCallGsmWcdmaExt = 
        new ( ELeave ) CMmConferenceCallGsmWcdmaExt();
    
    CleanupStack::PushL( conferenceCallGsmWcdmaExt );
    conferenceCallGsmWcdmaExt->iMmConferenceCallTsy = aMmConferenceCallTsy;
    conferenceCallGsmWcdmaExt->ConstructL();
    CleanupStack::Pop();

    return conferenceCallGsmWcdmaExt;
    }

void CMmConferenceCallGsmWcdmaExt::ConstructL()
    {
    ResetAttributes();
    }

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::CreateConferenceL
// This method allows clients to create the initial conference call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::CreateConferenceL(
    RMobileConferenceCall::TMobileConferenceStatus aStatus )
    {
    TInt ret = KErrNotReady;

    //This method allows clients to create the inital 3-way conference call. 
    //The phone must be in a state where there are two single voice calls, 
    //one on hold and the other connected or alerting the remote, called party. 
    //The TSY will reflect the conference creation capability by setting 
    //the KCapsJoin flags of the individual calls and the KCapsCreate flag 
    //of the conference call sub-session.
    //
    //In GSM mode, this method will complete when the TSY has received 
    //confirmation from the network that the conference call service has been 
    //successfully invoked.

    //This check is required because Add call uses same DOS operation to 
    //as this one. Therefore we don't want to continue if status is not
    //Idle.     
    if ( RMobileConferenceCall::EConferenceIdle == aStatus )
        {
        //We have to get 2 active status indications before the conference 
        //has really gone to active state.
        iCreateConferenceCounter = 2;

#ifndef USING_CTSY_DISPATCHER
        //send request to DOS
		// no packaged parameters for DOS call
		ret = iMmConferenceCallTsy->Phone()->MessageManager()->
            HandleRequestL( EMobileConferenceCallCreateConference );
#else
		// Pass down the call IDs of the active and held calls to be
		// joined in the conference.
		TInt heldCallId = iMmConferenceCallTsy->HeldCall()->CallId();
		TInt connectedCallId = iMmConferenceCallTsy->ConnectedCall()->CallId();
		CCallDataPackage package;
		package.SetCallIdAndMode(heldCallId, iMmConferenceCallTsy->HeldCall()->CallMode());
		package.PackData(&connectedCallId);
		ret = iMmConferenceCallTsy->Phone()->MessageManager()->
			HandleRequestL( EMobileConferenceCallCreateConference, &package);
#endif   // USING_CTSY_DISPATCHER
        }

    //return request result
    return ret;

    }

#ifndef USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::AddCallL
// Addition of a new call to an existing conference call. If 
// call object is not found by the name given as input parameter KErrArgument
// will be returned.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::AddCallL(
    const TName* aCallName,
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList* aCallList )
    {
    TInt ret = KErrArgument;

    // Get the call object that should be added to the conference call
    CMmCallTsy* mmCall = aCallList->GetMmCallByName( aCallName );
    if ( mmCall ) 
        {
        // Fetch the active call extension
        CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>( mmCall->ActiveCallExtension() );
        // Get the mobile call capabilities
        RMobileCall::TMobileCallCapsV1 mobileCallCaps;
        mmCallGsmWcdmaExt->GetMobileCallCapsV1(mobileCallCaps);
        // Only calls whose call control capabilities include KCapsJoin can be added to a conference call.
        if( !( RMobileCall::KCapsJoin & mobileCallCaps.iCallControlCaps ))
        	{ // Do not add the call to the conference since the call doesn't have KCapsJoin capability.
        	if(KMaxCallsInConference <= iMmConferenceCallTsy->NumberOfCallsInConference())
        		return KErrMMEtelMaxReached;
        	return ret;
        	} 

        RMobileCall::TMobileCallStatus callStatus = 
            mmCall->MobileCallStatus();
        //check that this call is not in hold or connected  state and 
        //conference call is not idle (=no reason to try this)
        if ( ( ( callStatus == RMobileCall::EStatusHold ) ||
                ( callStatus == RMobileCall::EStatusConnected ) )
            && ( RMobileConferenceCall::EConferenceIdle != aStatus ) )
            {
            // send request to DOS
            // only packed parameter for DOS call: Id of the call to be added
            CCallDataPackage callDataPackage;
            callDataPackage.SetCallIdAndMode( mmCall->CallId(), 
                RMobilePhone::EServiceUnspecified ); // service type not needed
            ret = iMmConferenceCallTsy->Phone()->MessageManager()->
                    HandleRequestL( EMobileConferenceCallAddCall,
                    &callDataPackage );
            }
        }

    return ret;
    }
#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::SwapL
// This method can be used to swap the ongoing conference call
// between active and held status. 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::SwapL(
	RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList* aCallList )
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_SWAPL_1, "TSY: CMmConferenceCallGsmWcdmaExt::SwapL");
	TInt ret = KErrArgument;

	// conference must be active or held
    if ( aStatus == RMobileConferenceCall::EConferenceIdle )
        {
        ret = KErrNotReady;
        }
    else
        {
        // Init counter
        iNumberOfCallsToBeSwapped = 0;
        CMmCallTsy* mmCall = NULL;
        CMmCallList* callList = iMmConferenceCallTsy->Phone()->CallList();

        // Check from call list the number of calls to be swapped
        for ( TInt i = 0; i < callList->GetNumberOfObjects(); i++ )
            {
            mmCall = callList->GetMmCallByIndex( i );

            if ( mmCall->IsPartOfConference() 
                && ( mmCall->MobileCallStatus() == RMobileCall::EStatusHold 
                || mmCall->MobileCallStatus() == RMobileCall::EStatusConnected ) )
                {
                // Call found, increase the counter.
                iNumberOfCallsToBeSwapped++;
                }
            }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_SWAPL_2, "TSY: CMmConferenceCallGsmWcdmaExt::SwapL - Number of calls in conference: %d", iNumberOfCallsToBeSwapped);

        // Find active and held call objects:
        CMmCallTsy* mmActiveCall = 
            aCallList->GetMmCallByStatus( RMobileCall::EStatusConnected );
        CMmCallTsy* mmHeldCall = 
            aCallList->GetMmCallByStatus( RMobileCall::EStatusHold ); 

        if ( RMobileConferenceCall::EConferenceActive == aStatus )
            {
            // conference is active
            // if held call is found, swap operation is required
            if ( mmHeldCall )
                {
#ifndef USING_CTSY_DISPATCHER
                // need to swap
                // no packed parameters for DOS call
                ret = iMmConferenceCallTsy->Phone()->MessageManager()->
                    HandleRequestL( EMobileConferenceCallSwap );
#else
                if (mmActiveCall && mmCall)
                	{
	                CCallDataPackage callDataPackage;
	                callDataPackage.SetCallIdAndMode( mmCall->CallId(),
	                		RMobilePhone::EServiceUnspecified ); // service type not needed
	                TInt heldCallId = mmHeldCall->CallId();
	                TInt connectedCallId = mmActiveCall->CallId();
	                callDataPackage.PackData(&heldCallId, &connectedCallId);
	                ret = iMmConferenceCallTsy->Phone()->MessageManager()->
	                		HandleRequestL(EMobileConferenceCallSwap, &callDataPackage);
                	}
                else
                	{
                	ret = KErrNotReady; // Shouldn't happen
                	}
#endif // USING_CTSY_DISPATCHER
                }
            // if held call is not found, there should be an active call
	        // and hold operation is required
            else if ( mmActiveCall )
                {
                // need to hold the active (conference) call

                // create package; set call id and call mode
                CCallDataPackage package;
                package.SetCallIdAndMode( mmActiveCall->CallId(),
                    mmActiveCall->CallMode() );

                // send to DOS
                ret = iMmConferenceCallTsy->Phone()->MessageManager()->
                    HandleRequestL( EMobileCallHold, &package );
                }
            // we have neither an active nor a held call
            else
                {
                ret = KErrNotReady; // should never happen
                }
            }
        else if ( RMobileConferenceCall::EConferenceHold == aStatus )
            {
            // conference is held
            //if active call is found, swap operation is required
            if ( mmActiveCall )
                {
#ifndef USING_CTSY_DISPATCHER
                // need to swap
                // no packed parameters for DOS call
                ret = iMmConferenceCallTsy->Phone()->MessageManager()->
                    HandleRequestL( EMobileConferenceCallSwap );
#else
                if (mmHeldCall && mmCall)
                	{
	                CCallDataPackage callDataPackage;
	                callDataPackage.SetCallIdAndMode( mmCall->CallId(),
	                		RMobilePhone::EServiceUnspecified ); // service type not needed
	                TInt heldCallId = mmHeldCall->CallId();
	                TInt connectedCallId = mmActiveCall->CallId();
	                callDataPackage.PackData(&heldCallId, &connectedCallId);
	                ret = iMmConferenceCallTsy->Phone()->MessageManager()->
	                		HandleRequestL(EMobileConferenceCallSwap, &callDataPackage);
                	}
                else
                	{
                	ret = KErrNotReady; // Shouldn't happen
                	}
#endif // USING_CTSY_DISPATCHER
                }
            // if active call is not found, there should be a held call
            // and resume operation is required
            else if ( mmHeldCall )
                {
                // need to retrieve (unhold) the held call

                // create package; set call id and call mode
                CCallDataPackage package;
                package.SetCallIdAndMode( mmHeldCall->CallId(),
                    mmHeldCall->CallMode() );

                // send to DOS
                ret = iMmConferenceCallTsy->Phone()->MessageManager()->
                    HandleRequestL( EMobileCallResume, &package );
                }
            // we have neither an active nor a held call
            else
                {
                ret = KErrNotReady; // should never happen
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::CallStatusChanged
// After the status of a single call has changed, this method decides if
// the status of the conference call should be changed as well.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::CallStatusChanged(
    RMobileCall::TMobileCallStatus aNewStatus,
    TInt aCallId )
    {
OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_CALLSTATUSCHANGED_1, "TSY: CMmConferenceCallGsmWcdmaExt::CallStatusChanged, aNewStatus:%d, aCallId:%d", aNewStatus, aCallId );

    TBool statusChanged( EFalse );

    //get current conference call status
    RMobileConferenceCall::TMobileConferenceStatus status 
            = iMmConferenceCallTsy->Status();

OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_CALLSTATUSCHANGED_2, "TSY: CMmConferenceCallGsmWcdmaExt::CallStatusChanged, Current conf call iMmConferenceCallTsy->Status:%d ", status);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_CALLSTATUSCHANGED_3, "TSY: CMmConferenceCallGsmWcdmaExt::CallStatusChanged, Current conference extension iNewStatus:%d ", iNewStatus);
    //get call list
    CMmCallList* callList = iMmConferenceCallTsy->Phone()->CallList();

    switch( aNewStatus )
        {
        //call has gone to idle state
        case RMobileCall::EStatusIdle:
            //If conference call is in active or held state 
            if ( ( RMobileConferenceCall::EConferenceActive == status ||
                   RMobileConferenceCall::EConferenceHold == status ) )           
                {
                //handle call releasing
                statusChanged = HandleCallRelease(
                    status, *callList, aNewStatus, aCallId );
                }
            break;
        //call has gone to held state
        case RMobileCall::EStatusHold:
            //if conference call is in active state
            if ( RMobileConferenceCall::EConferenceActive == status )
                {
                //check if swap request is pending
                if ( iMmConferenceCallTsy->ServiceRequested(
                        CMmConferenceCallTsy::EMultimodeConferenceCallSwap )
                     || iCurrentGhostOperation ==
                        RMobileConferenceCall::EConferenceSwapped )
                    {
                    //handle swapping
                    statusChanged = HandleSwap( status, *callList );
                    }
                //is GoOneToOne going on
                else if ( iGoOneToOneHandlingStarted )
                    {
                    statusChanged = HandleGoOneToOne( *callList, aCallId );
                    }
                //check that add call request is not pending
                else if ( !iMmConferenceCallTsy->ServiceRequested( 
                    CMmConferenceCallTsy::EMultimodeConferenceCallAddCall ) )
                    {
                    //Client has requested hold for a single call. If the call is part
                    //of the conference, all calls participating in the conference
                    //call will be held.
                    CMmCallTsy* call = callList->GetMmCallById(aCallId);
                    if(call != NULL && call->IsPartOfConference())
                    	{
                    	statusChanged = HandleSingleCallControl(status, aNewStatus);
                    	}
                    }
                }
            break;
        //call has gone to active state
        case RMobileCall::EStatusConnected:
            //check if CreateConference request is pending
            if ( ( iMmConferenceCallTsy->ServiceRequested(
                    CMmConferenceCallTsy::EMultimodeConferenceCallCreateConference )
                 || iCurrentGhostOperation == RMobileConferenceCall::EConferenceBuilt ) 
                 && RMobileConferenceCall::EConferenceIdle == status )
                {
                //handle create conference
                statusChanged = HandleCreateConference(
                    *callList, aCallId );
                }
            //check if AddCall request is pending
            else if ( ( iMmConferenceCallTsy->ServiceRequested(
                    CMmConferenceCallTsy::EMultimodeConferenceCallAddCall ) ||
                iCurrentGhostOperation ==  RMobileConferenceCall::EConferenceBuilt ||
                iCurrentGhostOperation ==  RMobileConferenceCall::EConferenceCallAdded ) &&
                ( RMobileConferenceCall::EConferenceActive == status ||
                RMobileConferenceCall::EConferenceHold == status ) )
                {
                //handle add call...
                statusChanged = HandleAddCall( status, *callList,
                    aNewStatus );
                }
            //check if swap request is pending
            else if ( iMmConferenceCallTsy->ServiceRequested(
                    CMmConferenceCallTsy::EMultimodeConferenceCallSwap ) ||
                iCurrentGhostOperation ==  RMobileConferenceCall::EConferenceSwapped )
                {
                //is conference status hold
                if ( RMobileConferenceCall::EConferenceHold == status )
                    {
                    //handle swap...
                    statusChanged = HandleSwap( status, *callList );
                    }
                }
            // call status might have changed because client has requested it
            // directly from a call object which is part of the conference
            // (not from the conference call object).
            else if ( RMobileConferenceCall::EConferenceHold == status )
                {
                CMmCallTsy* mmCall = callList->GetMmCallById( aCallId );
                if ( mmCall )
                    {
                    if ( mmCall->IsPartOfConference() )
                        {
                        // if this call is part of the conference, conference
                        // status is Hold and new call status is Connected, we
                        // have to get the whole conference into active state
                        statusChanged = HandleSingleCallControl(
                            status, aNewStatus );
                        }
                    }
                }
            break;
        case RMobileCall::EStatusUnknown:
        case RMobileCall::EStatusDialling:
        case RMobileCall::EStatusRinging:
        case RMobileCall::EStatusAnswering:
        case RMobileCall::EStatusConnecting:
        case RMobileCall::EStatusDisconnecting:
        case RMobileCall::EStatusDisconnectingWithInband:
        case RMobileCall::EStatusReconnectPending:
        case RMobileCall::EStatusWaitingAlternatingCallSwitch:
        case RMobileCall::EStatusTransferring:
        case RMobileCall::EStatusTransferAlerting:
        default:
            break;
        }

    if ( statusChanged )
        {
        // -1 indicates that there is no unhandled ghost operation anymore
        iCurrentGhostOperation = -1;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_CALLSTATUSCHANGED_4, "TSY: CMmConferenceCallGsmWcdmaExt::CallStatusChanged, statusChanged!! ");
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_CALLSTATUSCHANGED_5, "TSY: CMmConferenceCallGsmWcdmaExt::CallStatusChanged END, current conference extension iNewStatus:%d ", iNewStatus);

    return statusChanged;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::SetGoOneToOneHandlingStarted
// Informs the Gsm extension if Go-One-To-One handling
// has started for any call.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::SetGoOneToOneHandlingStarted(
    TBool aVal,
    TInt aCallId )
    {
    iGoOneToOneHandlingStarted = aVal;
    iGoOneToOneCallId = aCallId;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::CheckConferenceCapability
// Check if conference call creation is possible.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::CheckConferenceCapability(
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList* aCallList )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_CHECKCONFERENCECAPABILITY_1, "TSY: CMmConferenceCallGsmWcdmaExt::CheckConferenceCapability. aStatus:%d ", aStatus);
    //Check if conference creation is possible. CC has to be in Idle state
    if ( RMobileConferenceCall::EConferenceIdle == aStatus  )
        {
        TUint8 connectedCount( 0 ); //how many calls are in active state
        TUint8 heldCount( 0 ); //how many calls are in hold state
        //number of calls in CallList
        TInt numberOfCallObjects = aCallList->GetNumberOfObjects();

        //check how many calls are in connected and held state
        for ( TInt i = 0; i < numberOfCallObjects; i++ )
            {
            CMmCallTsy* aMmCall = aCallList->GetMmCallByIndex( i );
            if ( aMmCall->MobileCallStatus() == 
                    RMobileCall::EStatusConnected )
                {
                //increment connected count
                connectedCount++;
                }
            else if ( aMmCall->MobileCallStatus() ==
                        RMobileCall::EStatusHold )
                {
                //increment held count
                heldCount++;
                }
            }

        //If caps were KCapsCreate and conference call creation is not anymore
        //possible, clear caps.
        TBool tryToClearConference( EFalse );

        if ( iMmConferenceCallTsy->Caps() == 
                RMobileConferenceCall::KCapsCreate )
            {
            tryToClearConference = ETrue;
            }

        //Check that exactly one call is on hold and one is active
        if ( ( 1 == connectedCount && 1 == heldCount ) && 
            !tryToClearConference )
            {
            //Conference call creation is possible -> inform Common part.
            iMmConferenceCallTsy->
                CompleteNotifyCapsChange( RMobileConferenceCall::KCapsCreate );
            }
        else if ( ( ( 1 == connectedCount && 0 == heldCount ) ||
            ( 0 == connectedCount && 1 == heldCount ) ) && 
            tryToClearConference ) 
            {
            //Conference call creation is not possible -> inform Common part.
            iMmConferenceCallTsy->CompleteNotifyCapsChange( KErrNone );
            }
        }
    }

#ifndef USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HangUpL
// This method terminates the whole conference call. All calls 
// participating in the conference call will be disconnected.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::HangUpL(
    RMobileConferenceCall::TMobileConferenceStatus aStatus )
    {
    TInt ret( KErrAccessDenied );

    if ( aStatus != RMobileConferenceCall::EConferenceIdle )
        {
        //Close all calls that are part of the conference call.
        //Held call is not released if there is one...
        // no packed parameters for DOS call
        ret = iMmConferenceCallTsy->Phone()->MessageManager()->
            HandleRequestL( EMobileConferenceCallHangUp );
        }

    return ret;
    }

#endif //USING_CTSY_DISPATCHER

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::GetMobileCallInfo
// This method returns a current snapshot of the call information 
// associated with the call specified by the aIndex parameter. This works in 
// the following way: depending on the conference call status active/held the
// call object with the same status is searched from the call list. The index 
// defines which object's information is searched, for example if index is 2,
// second call object's information with the same status will be returned.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::GetMobileCallInfo(
    TInt* aIndex,
    TDes8* aCallInfo,
    CMmCallList* aCallList,
    RMobileConferenceCall::TMobileConferenceStatus aConferenceStatus )
    {
    TInt ret( KErrNotFound );
    RMobileCall::TMobileCallStatus searchedStatus( RMobileCall::EStatusIdle );
    
    //check if we are trying to find active or held calls
    if ( aConferenceStatus == RMobileConferenceCall::EConferenceActive )
        {
        //active
        searchedStatus = RMobileCall::EStatusConnected;
        }
    else if ( aConferenceStatus == RMobileConferenceCall::EConferenceHold )
        {
        //held
        searchedStatus = RMobileCall::EStatusHold;
        }

    if ( searchedStatus != RMobileCall::EStatusIdle )
        {
        TInt counter( 0 );
        CMmCallTsy* mobileCall = NULL;

        //search through all objects in call list
        for ( TInt i = 0; i < aCallList->GetNumberOfObjects(); i++)
            {
            //get mobile call
            mobileCall = static_cast<CMmCallTsy*>(
                aCallList->GetObjectByIndex( i ) );
            //if call's status matches with searched status
            if ( mobileCall->MobileCallStatus() == searchedStatus && 
                    mobileCall->IsPartOfConference() )
                {
                //if index matches
                if ( counter == *aIndex )
                    {
                    //get mobile call info
                    ret = mobileCall->FillMobileCallInfo( aCallInfo );
                    break;
                    }
                else
                    {
                    //else loop to next item in CallList with same status.
                    counter++;
                    }
                }
            }
        }
    
    return ret;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::IsGoOneToOneSupported
// GoOneToOne is only valid if the MS is in GSM mode, the 
// selected call is a voice call and it is a member of an ongoing conference 
// call and there are no calls in Hold state.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::IsGoOneToOneSupported(
    TInt  aCallId,
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList* aCallList )
    {
    TInt ret( KErrNotSupported );

    //The action is only valid if the MS is in GSM mode, the selected call 
    //is a voice call and it is a member of an ongoing conference call.
    if ( aStatus == RMobileConferenceCall::EConferenceActive )
        {
        CMmCallTsy* mobileCall = aCallList->GetMmCallById( aCallId );
        
        if (mobileCall != NULL)
            {
            if ( ( RMobilePhone::EVoiceService == mobileCall->CallMode() ) // is a voice call
                && ( RMobileCall::EStatusConnected ==  mobileCall->MobileCallStatus()) // is an active call
                && mobileCall->IsPartOfConference() ) // is a member of an ongoing conference call
                {
                ret = KErrNone;
	
                //Check that there are no held calls
                mobileCall = aCallList->GetMmCallByStatus( 
                                RMobileCall::EStatusHold );
	
                if ( mobileCall )
                    {
                    //held call found -> GoOneToOne not possible
                    ret = KErrNotSupported;
                    }
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HandleSwap
// This method checks when Swap is ready and can be completed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::HandleSwap(
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList& aCallList )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLESWAP_1, "TSY: CMmConferenceCallGsmWcdmaExt::HandleSwap. aStatus:%d ", aStatus);
    TBool statusChanged( EFalse );

    //increment counter telling how many calls will still change state
    iNumberOfCallsToBeSwapped--;

    //if all have changed state
    if ( 0 == iNumberOfCallsToBeSwapped )
        {
        //if status was active
        if ( RMobileConferenceCall::EConferenceActive == aStatus )
            {
            //new status is hold
            iNewStatus = RMobileConferenceCall::EConferenceHold;
            statusChanged = ETrue;            
            }
        //else if status was hold
        else if ( RMobileConferenceCall::EConferenceHold == aStatus )
            {
            //new status id active
            iNewStatus = RMobileConferenceCall::EConferenceActive;
            statusChanged = ETrue;
            }
        //update caps
        UpdateCapsAfterSwap( aStatus, aCallList );
        }

    return statusChanged;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HandleAddCall
// This method checks when AddCall is ready and can be completed.
// It also updates the capability flags of the calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::HandleAddCall(
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList& aCallList,
    RMobileCall::TMobileCallStatus aNewStatus )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLEADDCALL_1, "TSY: CMmConferenceCallGsmWcdmaExt::HandleAddCall. aStatus:%d ", aStatus);
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLEADDCALL_2, "TSY: CMmConferenceCallGsmWcdmaExt::HandleAddCall. aNewStatus:%d ", aNewStatus);
    TBool statusChanged( EFalse );

    //Handle adding a call to conference call
    if ( RMobileCall::EStatusConnected == aNewStatus ||
         RMobileCall::EStatusIdle == aNewStatus )
        {
        TUint8 heldCount( 0 );

        //calculate how many calls are in hold state.
        for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
            {
            if ( aCallList.GetMmCallByIndex(i)->MobileCallStatus() 
                    == RMobileCall::EStatusHold )
                {
                //if a held call is found get out. There will be more active
                //status indications coming...
                heldCount++;
                break;
                }
            }
        //All calls have gone to active state
        if ( heldCount == 0 )
            {
            //This means that the conference call has gone from hold state
            //to active state in the add call process. AddCall can now be
            //completed.
            if ( RMobileConferenceCall::EConferenceHold == aStatus )
                {
                iNewStatus = RMobileConferenceCall::EConferenceActive;
                statusChanged = ETrue;
                }
            //Conference call status has not changed but we still have to 
            //complete the AddCall operation. 
            else 
                { 
                iMmConferenceCallTsy->CompleteAddCall( KErrNone ); 
                } 

            if ( RMobileConferenceCall::EConferenceBuilt == iCurrentGhostOperation ||
                 RMobileConferenceCall::EConferenceCallAdded == iCurrentGhostOperation )
                {
                TInt callIndex( 0 );
                CMmCallTsy* mmCall = aCallList.GetMmCallByIndex( callIndex );

                while ( mmCall ) 
                    { 
                    if ( !mmCall->IsPartOfConference() &&
                         ( mmCall->MobileCallStatus() == RMobileCall::EStatusConnected ||
                         mmCall->MobileCallStatus() == RMobileCall::EStatusHold) )
                        {
                        iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
                            mmCall->CallName(), 
                            RMobileConferenceCall::EConferenceCallAdded );
                        break;
                        }
                    callIndex++;
                    mmCall = aCallList.GetMmCallByIndex( callIndex );
                    }
                }

            //Call method that updates the caps of the joined calls
            UpdateCapsAfterJoin( aCallList );
            } 
        }
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLEADDCALL_3, "TSY: CMmConferenceCallGsmWcdmaExt::HandleAddCall. statusChanged:%d ", statusChanged);
    
    return statusChanged;
    }


// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HandleCreateConference
// This method checks when CreateConference is ready and can be completed.
// This method checks that 2 active status indications are received
// as a response to the CreateConference request and when this is true, 
// CreateConference request can be completed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::HandleCreateConference(
    CMmCallList& aCallList,
    TInt aCallId )
    {
    TBool statusChanged ( EFalse );
    CMmCallTsy* mmCall = NULL;

    //get mmcall
    mmCall = aCallList.GetMmCallById( aCallId );

    if ( mmCall )
        {
        //send call added event...
        iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
            mmCall->CallName(), RMobileConferenceCall::EConferenceCallAdded );

        //decrement counter..
        iCreateConferenceCounter--;

        //if 2 status indications are received
        if ( 0 == iCreateConferenceCounter )
            {
            //CreateConferece can be completed.
            iNewStatus = RMobileConferenceCall::EConferenceActive;
            statusChanged = ETrue;

            //Call method that updates the caps of the joined calls
            UpdateCapsAfterJoin( aCallList );
            } 
        }

    return statusChanged;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HandleCallRelease
// This method is called when status indications are received
// after HangUp is requested from Conference call object. First this method
// finds out which call object has gone to idle state. Then if there are
// less than 2 calls left in the remaining conference call, the rest of
// the conference call will be put to idle state as well (1 call can't be
// conference call from TSY's point of view).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::HandleCallRelease(
		RMobileConferenceCall::TMobileConferenceStatus aStatus,
		CMmCallList& aCallList,
		RMobileCall::TMobileCallStatus aNewStatus,
		TInt aCallId )
	{
	TBool statusChanged( EFalse );
	TBool callFound( EFalse );
	CMmCallTsy* mmCall = NULL;

	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLECALLRELEASE_1, "TSY: CMmConferenceCallGsmWcdmaExt::HandleCallRelease aNewStatus %d, aCallId %d",aNewStatus , aCallId );
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLECALLRELEASE_2, "TSY: CMmConferenceCallGsmWcdmaExt::HandleCallRelease Calls in CC %d",iMmConferenceCallTsy->NumberOfCallsInConference());

	//first find the call object which has gone to idle state
	for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
		{
		mmCall = aCallList.GetMmCallByIndex( i );

		if(mmCall)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLECALLRELEASE_3, "TSY: Previous Call ID: %d", mmCall->PreviousCallId() );
			if ( mmCall->PreviousCallId() == aCallId )
				{
				if ( 2 == iMmConferenceCallTsy->NumberOfCallsInConference() && mmCall->IsPartOfConference() &&
						iMmConferenceCallTsy->ServiceRequested( 
								CMmConferenceCallTsy::EMultimodeConferenceCallAddCall ) )
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLECALLRELEASE_4, "TSY: STOP handling this case");
					return EFalse;
					}
				mmCall->ResetPreviousCallId();
				callFound = ETrue;
				break;
				}
			}
		}

	//if call object has been found
	if ( callFound )
		{
		//add Join caps to the call that is not part of conference
		if ( KMaxCallsInConference == 
		iMmConferenceCallTsy->NumberOfCallsInConference() )
			{
			CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = NULL;
			RMobileCall::TMobileCallStatus status( RMobileCall::EStatusIdle );

			if ( RMobileConferenceCall::EConferenceActive == aStatus )
				{
				//get the held call
				status = RMobileCall::EStatusHold;
				}
			else if ( RMobileConferenceCall::EConferenceHold == aStatus )
				{
				//get the connected call
				status = RMobileCall::EStatusConnected;
				}

			if ( status != RMobileCall::EStatusIdle )
				{
				CMmCallTsy* nonIdleCall =
				aCallList.GetMmCallByStatus( status );

				if ( nonIdleCall )
					{
					// Fetch the active call extension
					mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>(
							nonIdleCall->ActiveCallExtension() );
					//add Join caps
					mmCallGsmWcdmaExt->AddGSMCallCaps( 
							RMobileCall::KCapsJoin );
					//complete caps change notification
					nonIdleCall->CompleteNotifyMobileCallCapsChange( 
							KErrNone );
					}
				}
			}

		if ( mmCall && mmCall->IsPartOfConference() )
			{    
			// if the released call was in the conference, send 
			// removed from conference indication
			iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
					mmCall->CallName(),
					RMobileConferenceCall::EConferenceCallRemoved );
			}

		//If there is only 1 call left in the CC, remove it. 1 call can't be
		//a conference call from MM API's point of view.
		if ( ( 1 == iMmConferenceCallTsy->NumberOfCallsInConference() ) &&
				!iMmConferenceCallTsy->ServiceRequested( 
						CMmConferenceCallTsy::EMultimodeConferenceCallHangUp ) )
			{
			RMobileCall::TMobileCallStatus callStatus( 
					RMobileCall::EStatusIdle );

			//send removed from conference also to the last call in the 
			// conference. Single call cannot be a conference call.
			if ( RMobileConferenceCall::EConferenceActive == aStatus )
				{
				//we are trying to find connected call
				callStatus = RMobileCall::EStatusConnected;

				//Special case when last member is on hold and CC Status is active at this 
				//point(2 members in CC and one is dropped and MO call is made at the same time)
				for ( TInt d = 0; d < aCallList.GetNumberOfObjects(); d++ )
					{
					CMmCallTsy* remainingCall = aCallList.GetMmCallByIndex(d);

					if( remainingCall->MobileCallStatus() == RMobileCall::EStatusHold )
						{
						callStatus = RMobileCall::EStatusHold;
						OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_HANDLECALLRELEASE_5, "TSY: CMmConferenceCallGsmWcdmaExt::HandleCallRelease Call with Hold status found" );
						break;
						}	
					}

				}
			else if ( RMobileConferenceCall::EConferenceHold == aStatus )
				{
				//we are trying to find a held call
				callStatus = RMobileCall::EStatusHold;
				}
			//now search that call
			for ( TInt c = 0; c < aCallList.GetNumberOfObjects(); c++ )
				{
				CMmCallTsy* remainingCall = aCallList.GetMmCallByIndex(c);
				if ( remainingCall->MobileCallStatus() == callStatus )
					{
					iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
							remainingCall->CallName(), 
							RMobileConferenceCall::EConferenceCallRemoved );
					break;
					}                
				}

			UpdateCapsAfterCallRelease( aCallList );
			}

		//If there are no calls anymore in the CC, notify it...
		if ( 0 == iMmConferenceCallTsy->NumberOfCallsInConference() )
			{
			iNewStatus = RMobileConferenceCall::EConferenceIdle;
			statusChanged = ETrue;
			TName dummyName; //No calls left. Send EConferenceTerminate event. Use an empty placeholder name
			iMmConferenceCallTsy->CompleteNotifyConferenceEvent(dummyName,RMobileConferenceCall::EConferenceTerminated);
			}
		//if add call was going on and there was a call release
		else if ( iMmConferenceCallTsy->ServiceRequested( 
				CMmConferenceCallTsy::EMultimodeConferenceCallAddCall ) )
			{
			//conference call has not gone to idle state
			HandleAddCall( aStatus, aCallList, aNewStatus );
			}
		//if swap was going on and there was a call release
		else if ( iMmConferenceCallTsy->ServiceRequested( 
				CMmConferenceCallTsy::EMultimodeConferenceCallSwap ) )
			{
			//conference call has not gone to idle state
			HandleSwap( aStatus, aCallList );
			}
		}

	return statusChanged;
	}

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HandleSingleCallControl
// This method is called when status indication is received
// and Swap, AddCall, CreateConference or HangUp is not requested from
// Conference call object. This means that client has requested hold or
// resume directly from single call object. If Hold has been requested 
// from single call participating conference call -> all calls in conference
// call will go to Hold state. Same applies also to the Resume command.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::HandleSingleCallControl(
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    RMobileCall::TMobileCallStatus aNewStatus )
    {
    TBool statusChanged( EFalse );

    if ( !iCallControlStarted )
        {   
        //save count, how many indications should be received
        iCallsToBeControlled = iMmConferenceCallTsy->
            NumberOfCallsInConference();
        //Don't come here before all calls have changed their status
        iCallControlStarted = ETrue;
        }

    //if CC is in Hold status and we get Active status indication
    if ( RMobileCall::EStatusConnected == aNewStatus &&
        RMobileConferenceCall::EConferenceHold == aStatus )
        {
        //one call in Conference call has changed its status
        iCallsToBeControlled--;
        }
    //if CC is in Active status and we get Hold status indication
    else if ( RMobileCall::EStatusHold == aNewStatus &&
        RMobileConferenceCall::EConferenceActive == aStatus )
        {
        //one call in Conference call has changed its status
        iCallsToBeControlled--;
        }

    //Have all calls in Conference call changed their status?
    if ( 0 == iCallsToBeControlled )
        {
        //if CC was originally in Hold status we have changed to active state
        if ( RMobileConferenceCall::EConferenceHold == aStatus )
            {
            iNewStatus = RMobileConferenceCall::EConferenceActive;
            }
        //if CC was originally in Active status we have changed to hold state
        else if ( RMobileConferenceCall::EConferenceActive == aStatus )
            {
            iNewStatus = RMobileConferenceCall::EConferenceHold;
            }
        //reset flag
        iCallControlStarted = EFalse;
   
        statusChanged = ETrue;
        }   

    return statusChanged;
    }
    
// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::HandleGoOneToOne
// This method checks when GoOneToOne is ready and can be completed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CMmConferenceCallGsmWcdmaExt::HandleGoOneToOne(
    CMmCallList& aCallList,
    TInt aCallId )
    {
    TBool statusChanged( EFalse );
    CMmCallTsy* mmCall = NULL;

    // check if the call placed on hold is the one to go one to one with
    // if so, cancel one to one
    if (aCallId == iGoOneToOneCallId)
        {
        mmCall = aCallList.GetMmCallById( iGoOneToOneCallId );
        if ( mmCall )
            {
            // failure of the Go-One-To-One operation
            reinterpret_cast<CMmVoiceCallTsy*>( mmCall )->CompleteGoOneToOne(
                KErrNotReady );
            }
        
        iGoOneToOneHandlingStarted = EFalse;
        iGoOneToOneCallId = -1;
        iCallControlStarted = EFalse;
        return statusChanged;
        }

    if ( !iCallControlStarted )
        {   
        //save count, how many indications should be received. All but one 
        //call (the one that was splitted) should go to hold state.
        iCallsToBeControlled = 
            iMmConferenceCallTsy->NumberOfCallsInConference() - 1;

        //Conference call will go to Idle state
        if ( 1 == iCallsToBeControlled )
            {
            for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
                {
                mmCall = aCallList.GetMmCallByIndex( i );

                if ( RMobileCall::EStatusConnected == mmCall->MobileCallStatus() 
                    || RMobileCall::EStatusHold == mmCall->MobileCallStatus() )
                    {
                    // The call removed notification event for the call which has
                    // been split gets completed later when the split event is
                    // completed. This if stops extra call removed event from being completed.
                    if (mmCall->CallId() != iGoOneToOneCallId)
                    	{
#ifdef USING_CTSY_DISPATCHER
						iMmConferenceCallTsy->CompleteNotifyConferenceEvent(
							mmCall->CallName(),
							RMobileConferenceCall::EConferenceCallRemoved );
#else // USING_CTSY_DISPATCHER
                    iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
                        mmCall->CallName(), 
                        RMobileConferenceCall::EConferenceCallRemoved );
#endif // USING_CTSY_DISPATCHER
                    	}
   
                    CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = 
                        static_cast<CMmCallGsmWcdmaExt*>( 
                            mmCall->ActiveCallExtension() );

                    TUint32 caps = RCall::KCapsHangUp;
                    caps |= RMobileCall::KCapsJoin |
                        RMobileCall::KCapsSwap |
                        RMobileCall::KCapsTransfer;

                    mmCallGsmWcdmaExt->AddGSMCallCaps( caps );
                    mmCallGsmWcdmaExt->RemoveGSMCallCaps( 
                        RMobileCall::KCapsOneToOne );

                    mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );  
                    }
                }

            //new status is idle (there is no conference anymore)
            iNewStatus = RMobileConferenceCall::EConferenceIdle;
            statusChanged = ETrue;
            }

        // Call control is going on, i.e. we are waiting for other calls'
        // status indications
        iCallControlStarted = ETrue;
        }      

    //decrement counter telling how many calls will still change state
    iCallsToBeControlled--;

    //All conference related call object have changed their state. Remove 
    //go-one-to-one call object from conference call.
    if ( iCallsToBeControlled == 0 && iCallControlStarted )
        {
        if ( !statusChanged )
            {
            // If conference is created with two other parties, conference
            // is destructed (status is set to EConferenceIdle, see above)
            iNewStatus = RMobileConferenceCall::EConferenceHold;
            statusChanged = ETrue;
            }
        iCallControlStarted = EFalse;
        iGoOneToOneHandlingStarted = EFalse;

        mmCall = aCallList.GetMmCallById( iGoOneToOneCallId );
        
        iGoOneToOneCallId = -1;

        if ( mmCall )
            {
            iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
                    mmCall->CallName(), 
                    RMobileConferenceCall::EConferenceCallRemoved );  
                    
            iMmConferenceCallTsy->CompleteNotifyConferenceEvent( 
                    mmCall->CallName(), 
                    RMobileConferenceCall::EConferenceSplit );  

            CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = 
                static_cast<CMmCallGsmWcdmaExt*>( 
                    mmCall->ActiveCallExtension() );
                    
            mmCallGsmWcdmaExt->AddGSMCallCaps( RMobileCall::KCapsJoin );
            mmCallGsmWcdmaExt->AddGSMCallCaps( RMobileCall::KCapsHangUp );
            mmCallGsmWcdmaExt->RemoveGSMCallCaps( RMobileCall::KCapsOneToOne );  
            
            mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );        

            // successful complete of the Go-One-To-One operation
            reinterpret_cast<CMmVoiceCallTsy*>( mmCall )->CompleteGoOneToOne(
                KErrNone );
            }
        }
    //Update caps only if call is still part of conference call
    else if ( iCallControlStarted && iGoOneToOneCallId != aCallId )
        {        
        mmCall = aCallList.GetMmCallById( aCallId );

        if ( NULL != mmCall )
            {
            CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = 
                static_cast<CMmCallGsmWcdmaExt*>(
                    mmCall->ActiveCallExtension() );

            mmCallGsmWcdmaExt->AddGSMCallCaps( RMobileCall::KCapsHangUp );
            mmCallGsmWcdmaExt->RemoveGSMCallCaps( 
                RMobileCall::KCapsOneToOne );

            mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
            }
        }

    return statusChanged;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::GetNewStatus
// Called by common Conference call object to get the current
// conference call status. It will be copied to aNewStatus.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmConferenceCallGsmWcdmaExt::GetNewStatus(
    RMobileConferenceCall::TMobileConferenceStatus* aNewStatus )
    {
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_GETNEWSTATUS_1, "TSY: CMmConferenceCallGsmWcdmaExt::GetNewStatus. iNewStatus:%d ", iNewStatus );
    //copy new status
    *aNewStatus = iNewStatus;
    //delete new status from GSM extension
    iNewStatus = RMobileConferenceCall::EConferenceIdle;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::ResetAttributes
// Resets internal attributes.
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::ResetAttributes()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_RESETATTRIBUTES_1, "TSY: CMmConferenceCallGsmWcdmaExt::ResetAttributes. " );
    //Flag telling if this is not the first hold/active status indication
    //caused by client asking hold/resume for a single call.
    iCallControlStarted = EFalse;
    //Counter telling how many calls should still change their state before
    //Conference call changes its' state.
    iCallsToBeControlled = 0;
    //Counter telling when Create conference is ready to be completed 
    iCreateConferenceCounter = 0;
    //New conference call status
    iNewStatus = RMobileConferenceCall::EConferenceIdle;
    //Count, how many calls are to be swapped
    iNumberOfCallsToBeSwapped = 0;
    //Flag telling if GoOneToOne handling is going on.
    iGoOneToOneHandlingStarted = EFalse;
    //Call id for which GoOneToOne handling is going on.
    iGoOneToOneCallId = -1;
    //Operation initiated bypassing the ETel (ghost operetion).
    iCurrentGhostOperation = -1;
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::UpdateCapsAfterJoin
// Updates call capabilities of all calls after a Join operation
// has been executed (CreateConference or AddCall).
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::UpdateCapsAfterJoin(
    CMmCallList& aCallList ) const
    {
    //update flags of all joined calls
    for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
        {
        //Get call object
        CMmCallTsy* mmCall = aCallList.GetMmCallByIndex(i);

        if ( mmCall )
            {
            if ( mmCall->MobileCallStatus() == RMobileCall::EStatusConnected )
                {
                // Fetch the active call extension
                CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = 
                    static_cast<CMmCallGsmWcdmaExt*>(
                        mmCall->ActiveCallExtension() );

                //set caps to be removed;
                TUint32 caps =  
                    RMobileCall::KCapsJoin 
                    | RMobileCall::KCapsSwap;
                //remove Join and Swap caps
                mmCallGsmWcdmaExt->RemoveGSMCallCaps( caps );
                //Add GoOneToOne caps
                mmCallGsmWcdmaExt->AddGSMCallCaps( 
                    RMobileCall::KCapsOneToOne );
                //complete caps change notification
                mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::UpdateCapsAfterCallRelease
// Updates call capabilities of all calls after a call participating in
// the conference call has gone to idle state.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::UpdateCapsAfterCallRelease(
    CMmCallList& aCallList ) const
    {
    TUint32 caps( 0 );

    //try to find connected call
    CMmCallTsy* connectedCall = aCallList.GetMmCallByStatus( 
        RMobileCall::EStatusConnected );
    //try to find a held call
    CMmCallTsy* heldCall = aCallList.GetMmCallByStatus( 
        RMobileCall::EStatusHold );

    CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = NULL;

    if ( connectedCall )
        {
        // Fetch the active call extension
        mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>(
            connectedCall->ActiveCallExtension() );

        //add HangUp caps
        mmCallGsmWcdmaExt->AddGSMCallCaps( 
            RMobileCall::KCapsHangUp );

        //update caps of connected call
        if ( heldCall )
            {
            //set caps to be added;
            caps = RMobileCall::KCapsJoin 
                | RMobileCall::KCapsTransfer
                | RMobileCall::KCapsSwap;
            //add Join, Transfer and Swap caps
            mmCallGsmWcdmaExt->AddGSMCallCaps( caps );
            }
        else
            {
            //set caps to be removed;
            caps = RMobileCall::KCapsJoin 
                | RMobileCall::KCapsTransfer
                | RMobileCall::KCapsSwap;
            //remove Join, Transfer and Swap caps
            mmCallGsmWcdmaExt->RemoveGSMCallCaps( caps );
            }

        //complete caps change notification
        connectedCall->CompleteNotifyMobileCallCapsChange( KErrNone );
        }

    if ( heldCall )
        {
        // Fetch the active call extension
        mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>(
            heldCall->ActiveCallExtension() );

        //add HangUp caps
        mmCallGsmWcdmaExt->AddGSMCallCaps( 
            RMobileCall::KCapsHangUp );

        //update caps of connected call
        if ( connectedCall )
            {
            //set caps to be added;
            caps = RMobileCall::KCapsJoin 
                | RMobileCall::KCapsTransfer
                | RMobileCall::KCapsSwap;
            //add Join, Transfer and Swap caps
            mmCallGsmWcdmaExt->AddGSMCallCaps( caps );
            }
        else
            {
            //set caps to be removed;
            caps = RMobileCall::KCapsJoin 
                | RMobileCall::KCapsTransfer
                | RMobileCall::KCapsSwap;
            //remove Join, Transfer and Swap caps
            mmCallGsmWcdmaExt->RemoveGSMCallCaps( caps );
            }

        //complete caps change notification
        heldCall->CompleteNotifyMobileCallCapsChange( KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::UpdateCapsAfterSwap
// Updates call capabilities of all calls after a Swap operation has
// been executed.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::UpdateCapsAfterSwap(
    RMobileConferenceCall::TMobileConferenceStatus aStatus,
    CMmCallList& aCallList ) const
    {
    CMmCallTsy* mmCall = NULL;
    CMmCallGsmWcdmaExt* mmCallGsmWcdmaExt = NULL;

    //if status was active
    if ( aStatus == RMobileConferenceCall::EConferenceActive )
        {
        //update caps of all calls participating conference
        //loop through all call objects searching the held calls
        for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
            {
            mmCall = aCallList.GetMmCallByIndex( i );

            if ( mmCall->MobileCallStatus() == RMobileCall::EStatusHold )
                {
                // Fetch the active call extension
                mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>(
                    mmCall->ActiveCallExtension() );

                //remove GoOneToOne caps
                mmCallGsmWcdmaExt->RemoveGSMCallCaps( 
                    RMobileCall::KCapsOneToOne );
                //complete caps change notification
                mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
                }
            }
        }
    //else if status was hold
    else if ( aStatus == RMobileConferenceCall::EConferenceHold )
        {
        TInt numberOfNonIdleCalls( 0 );

        //find out the number of non idle calls
        for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
            {
            mmCall = aCallList.GetMmCallByIndex( i );
            if ( mmCall->MobileCallStatus() != RMobileCall::EStatusIdle )
                {
                numberOfNonIdleCalls++;
                }
            }

        //check that all non idle calls belong to conference call
        //-> GoOneToOne can be activated.
        if ( numberOfNonIdleCalls <=
            iMmConferenceCallTsy->NumberOfCallsInConference() )
            {
            //update caps of all calls participating conference
            //loop through all call objects searching the held calls
            for ( TInt i = 0; i < aCallList.GetNumberOfObjects(); i++ )
                {
                mmCall = aCallList.GetMmCallByIndex( i );

                if ( mmCall->MobileCallStatus() 
                    == RMobileCall::EStatusConnected )
                    {
                    // Fetch the active call extension
                    mmCallGsmWcdmaExt = static_cast<CMmCallGsmWcdmaExt*>(
                        mmCall->ActiveCallExtension() );

                    //add GoOneToOne caps
                    mmCallGsmWcdmaExt->AddGSMCallCaps( 
                        RMobileCall::KCapsOneToOne );
                    //complete caps change notification
                    mmCall->CompleteNotifyMobileCallCapsChange( KErrNone );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent
// Informs the Gsm extension if there is conference call
// operation performed that has been initiated bypassing the ETel.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent(
    CMmDataPackage* aDataPackage )
    {
    TInt callId( -1 );
    RMobilePhone::TMobileService callMode(
        RMobilePhone::EServiceUnspecified );
    TInt confEvent = -1;

    CCallDataPackage* callDataPackage = 
        reinterpret_cast<CCallDataPackage*>(aDataPackage);

    callDataPackage->GetCallIdAndMode( callId, callMode );
    callDataPackage->UnPackData( confEvent );

    //get current conference call status
    RMobileConferenceCall::TMobileConferenceStatus status 
            = iMmConferenceCallTsy->Status();
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_1, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent. iMmConferenceCallTsy->Status():%d ", status );

    switch( confEvent )
        {
        case RMobileConferenceCall::EConferenceBuilt:
            //if createConference has not been requested using ETel API
            //and conference status is idle
            if ( RMobileConferenceCall::EConferenceIdle == status && 
                !iMmConferenceCallTsy->ServiceRequested( CMmConferenceCallTsy::
                    EMultimodeConferenceCallCreateConference ) )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_2, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent - Build");
                //We have to get 2 active status indications before the 
                //conference has really gone to active state.
                iCreateConferenceCounter = 2;
                iCurrentGhostOperation = confEvent;
                }
            //if addCall has not been requested using ETel API
            //and conference status is Hold or Active
            else if ( RMobileConferenceCall::EConferenceIdle != status &&
                !iMmConferenceCallTsy->ServiceRequested( CMmConferenceCallTsy::
                    EMultimodeConferenceCallAddCall ) )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_3, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent - Build/Add");
                iCurrentGhostOperation = confEvent;
                }
            break;
        case RMobileConferenceCall::EConferenceCallAdded:
            //if addCall has not been requested using ETel API
            //and conference status is Hold or Active
            if ( RMobileConferenceCall::EConferenceIdle != status &&
                !iMmConferenceCallTsy->ServiceRequested( CMmConferenceCallTsy::
                    EMultimodeConferenceCallAddCall ) )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_4, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent - Add");
                iCurrentGhostOperation = confEvent;
                }
            break;
        case RMobileConferenceCall::EConferenceSwapped:
            //if swap has not been requested using ETel API
            if ( RMobileConferenceCall::EConferenceIdle != status &&
                !iMmConferenceCallTsy->ServiceRequested( 
                CMmConferenceCallTsy::EMultimodeConferenceCallSwap ) )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_5, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent - Swap");
                iNumberOfCallsToBeSwapped 
                    = iMmConferenceCallTsy->NumberOfCallsInConference();

                iCurrentGhostOperation = confEvent;
                }
            break;
        //GoOneToOne has been requested
        case RMobileConferenceCall::EConferenceSplit:
            if ( !iGoOneToOneHandlingStarted )
                {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_6, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent - Split");
                iGoOneToOneHandlingStarted = ETrue;
                iCurrentGhostOperation = confEvent;
                iGoOneToOneCallId = callId;
                }
            break;
        default:
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCONFERENCECALLGSMWCDMAEXT_COMPLETENOTIFYCONFERENCEEVENT_7, "TSY: CMmConferenceCallGsmWcdmaExt::CompleteNotifyConferenceEvent - No Actions");
            break;
        }
    }


//  End of File 
