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

#ifdef USING_CTSY_DISPATCHER

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmcallgsmwcdmaextwithdispatcherTraces.h"
#endif

/**
 * This file contains additional function definitions for the CMmCallGsmWcdmaExt class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher. 
 */

#include "cmmcallgsmwcdmaext.h"
#include "cmmphonetsy.h"
#include "cmmcalltsy.h"
#include "cmmcalllist.h"

// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::SwapL
// This method can be used to swap between active and held calls.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::SwapL(TInt aCallId)
	{
    TInt ret = KErrNone;
	const TInt KInvalidCallId = -1;
	
    // CTSY supports RMobileCall::Swap when there are one or two calls. When Swap() is called
	// on a single call, it's state is switched (active to held or held to active), when on two 
	// calls(one held, one active) their states are swapped. 
    CMmPhoneTsy& mmPhone = *iMmCallTsy->Phone();
    CMmCallList& callList = *mmPhone.CallList();
    
	CCallDataPackage package;
	
	if ( callList.GetNumberOfObjects() > 2 )
		{
		// Swapping between calls only makes sense with one or two opened calls.
		return KErrNotSupported;
		}
    CMmCallTsy* call = callList.GetMmCallById(aCallId);
	package.SetCallIdAndMode(aCallId, iSymbianCallMode);
    
    if (!call)
    	{
    	ret = KErrGeneral;
    	}
    else
    	{
    	const RMobileCall::TMobileCallStatus callStatus = call->MobileCallStatus();
    	
    	switch(callStatus)
    		{
    		case RMobileCall::EStatusHold:
    			{
    			// Pack the following to the dispatcher:
    			// Call status of held call in iCallId in CCallDataPackage
    		    CCallDataPackage package;
        		package.SetCallIdAndMode(aCallId, iSymbianCallMode);
        		// Get connected call and pack its call ID
        		CMmCallTsy* connectedCall = callList.GetMmCallByStatus(RMobileCall::EStatusConnected);
				TInt connectedCallId = KInvalidCallId;
        		if (connectedCall)
        			{
        			connectedCallId = connectedCall->CallId();
        			}
        		package.PackData(&connectedCallId);
        		ret = iMessageManager->HandleRequestL(EMobileCallSwap, &package);
        		
    			break;
    			}
    			
    		case RMobileCall::EStatusConnected:
    			{
    		    // Pack the following to the dispatcher:
    		    // Call status of connected call in additional parameter slot in CCallDataPackage
    		    CCallDataPackage package;
				
    			package.PackData(&aCallId);
        		// Get held call and pack its call ID
        		CMmCallTsy* heldCall = callList.GetMmCallByStatus(RMobileCall::EStatusHold);
         		if (heldCall)
        			{
        			package.SetCallIdAndMode(heldCall->CallId(), heldCall->CallMode());
        			}
         		ret = iMessageManager->HandleRequestL(EMobileCallSwap, &package);
    			break;
    			}
    		
    		default:
    			{
    	 		ret = KErrNotReady; // Call is neither held nor in EStatusConnected state.
    								// Cannot swap it.
    			break;
    			}
    		}
    	}

    return ret;
    }


// ---------------------------------------------------------------------------
// CMmCallGsmWcdmaExt::TransferL
// This method transfers call to another destination.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmCallGsmWcdmaExt::TransferL()
    {
    TInt ret( KErrNone );

    // Retrieve possible held call
    CMmCallTsy* mmCall = iMmCallTsy->Phone()->CallList()->GetMmCallByStatus( 
        RMobileCall::EStatusHold );

    if ( mmCall )
        {
        // Held call Id
OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMCALLGSMWCDMAEXT_TRANSFERL1_1, "TSY: CMmCallGsmWcdmaExt::Transfer, CallId: %d", mmCall->CallId() );

		// With the CTSY dispatcher, there are two call IDs sent from the CTSY
		// The first (packed in iCallId of the data package is the call ID
		// of the held call, the second is the call ID of the ringing or
		// connected call.
		CMmCallTsy* mmCall2 = iMmCallTsy->Phone()->CallList()->GetMmCallByStatus(
		                RMobileCall::EStatusConnected );
		if (!mmCall2)
			{
			// Can't find a connected call, find a ringing call
			mmCall2 = iMmCallTsy->Phone()->CallList()->GetMmCallByStatus(
					                RMobileCall::EStatusRinging );
			}

		if (mmCall2)
			{
			CCallDataPackage package;
			// Set call id (of held call) and call mode (call mode not used but
			// something needs to be packed there).
			package.SetCallIdAndMode( mmCall->CallId(), iSymbianCallMode );
			TInt callId2 = mmCall2->CallId();
			package.PackData(&callId2);
			ret = iMessageManager->HandleRequestL(EMobileCallTransfer, &package);
			}
		else
			{
			ret = KErrNotReady;
			}
        }
    else
        {
        ret = KErrNotReady;
        }

    return ret;
    }

#endif //USING_CTSY_DISPATCHER

