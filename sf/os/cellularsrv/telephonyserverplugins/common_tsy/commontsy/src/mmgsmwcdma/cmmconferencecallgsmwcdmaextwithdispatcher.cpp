// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef USING_CTSY_DISPATCHER

/**
 * This file contains additional function definitions for the CMmConferenceCallGsmWcdmaExt class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher. 
 */

//  INCLUDE FILES
#include "cmmconferencecallgsmwcdmaext.h"
#include "cmmphonetsy.h"
#include "cmmconferencecalltsy.h"
#include "cmmvoicecalltsy.h"
#include "cmmcalllist.h"
#include "cmmcallgsmwcdmaext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmmessagemanagerbase.h"
#include <etelmmerr.h> // etel error codes

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

            CMmCallTsy* mmExistingCall = NULL;
            TInt existingCallId;
            __DEBUG_ONLY(TBool callFound = EFalse;)
            TInt numOfObjects = aCallList->GetNumberOfObjects();            
            for ( TInt i = 0; i < numOfObjects; i++ )
            	{
            	mmExistingCall = aCallList->GetMmCallByIndex(i);
            	if ( mmExistingCall->IsPartOfConference() )
            		{
            		__DEBUG_ONLY(callFound = ETrue;)
            		existingCallId = mmExistingCall->CallId();
            		break;
            		}            		
            	}
            __ASSERT_DEBUG(callFound,User::Invariant());
            
            callDataPackage.PackData(&existingCallId);
            
            ret = iMmConferenceCallTsy->Phone()->MessageManager()->
                    HandleRequestL( EMobileConferenceCallAddCall,
                    &callDataPackage );
            }
        }

    return ret;
    }

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
        RArray<TInt> callIds;
        CleanupClosePushL( callIds );
                
        CMmCallList* callList = iMmConferenceCallTsy->Phone()->CallList();
        CMmCallTsy* mmCall = NULL;
        TInt numOfObjects = callList->GetNumberOfObjects();
        
        for ( TInt i = 0; i < numOfObjects; i++ )
        	{
        	mmCall = callList->GetMmCallByIndex(i);
        	if ( mmCall->IsPartOfConference() )
        		{
        		callIds.AppendL( mmCall->CallId() );
        		}
        	}
        
        CMmDataPackage package;
        package.PackData(&callIds);
        
        ret = iMmConferenceCallTsy->Phone()->MessageManager()->
            HandleRequestL( EMobileConferenceCallHangUp, &package );
                
        CleanupStack::PopAndDestroy(&callIds);         
        }

    return ret;
    }

#endif //USING_CTSY_DISPATCHER
