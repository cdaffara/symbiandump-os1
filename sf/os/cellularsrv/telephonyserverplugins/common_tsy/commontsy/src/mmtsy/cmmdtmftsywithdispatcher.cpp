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

/**
 * This file contains additional function definitions for the CMmDtmfTsy class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher.
 */


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmdtmftsywithdispatcherTraces.h"
#endif

#include "cmmdtmftsy.h"
#include "cmmphonetsy.h"
#include "cmmtsyreqhandlestore.h"
#include "cmmcalllist.h"
#include "cmmcalltsy.h"

/**
 * Starts sending a single DTMF digit.
 *
 * This version of StartDTMFToneL sends the call ID of the active call through
 * which the DTMF digit will be sent and hence relieves the LTSY of the
 * responsibility to track the call state and to locate the active call.
 *
 * @param aTsyReqHandle The TSY request handle.
 * @param aTone The tone to send.
 * @return Return value to the ETel Server.
 */
TInt CMmDtmfTsy::StartDTMFToneL(const TTsyReqHandle aTsyReqHandle, TChar* aTone)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_STARTDTMFTONEL_1, "TSY: CMmDtmfTsy::StartDTMFToneL");

    TInt ret = KErrServerBusy;

    // Check if request handle already exists
    TTsyReqHandle startDtmftoneHandle =
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle(
            CMmPhoneTsy::EMultimodePhoneStartDTMFTone );

    if (CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != startDtmftoneHandle)
        {
        iMmPhone->ReqCompleted(aTsyReqHandle, ret);
        }
    else
        {
        // DTMF can only be sent if there is a connected call
        // The call ID for the first connected call found is returned. If there
        // isn't an ongoing conference call, there will only be one active call.
        // If there is a conference call going, the DTMF will be sent using the
        // call ID of one of the active calls in the conference and will be
        // sent through to the conference call.
        TInt callId = -1;
        if (GetConnectedCallId(callId) != KErrNone)
        	{
        	iMmPhone->ReqCompleted(aTsyReqHandle, KErrEtelCallNotActive);
        	}
        else
        	{
	        CMmDataPackage dataPackage;
	        dataPackage.PackData(&callId, aTone);
	        ret = iMmPhone->MessageManager()->HandleRequestL(
	            EMobilePhoneStartDTMFTone,
	            &dataPackage
	            );

	        if (ret != KErrNone)
	        	{
	        	iMmPhone->ReqCompleted(aTsyReqHandle, ret);
	        	}
	        else
	            {
	            // Save the type of the DTMF
	            iDtmfType = EDtmfTypeDigit;

	            iMmPhone->iReqHandleType =
	                            CMmPhoneTsy::EMultimodePhoneStartDTMFTone;
	            }
        	}
        }

    return KErrNone;
	} // CMmDtmfTsy::StartDTMFToneL

/**
 * Stops sending a single DTMF digit.
 *
 * @param aTsyReqHandle The TSY request handle.
 * @return Return value to the ETel Server.
 */
TInt CMmDtmfTsy::StopDTMFToneL(const TTsyReqHandle aTsyReqHandle)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_STOPDTMFTONEL_1, "TSY: CMmDtmfTsy::StopDTMFToneL");

    TInt ret = KErrServerBusy;

    // Check if request handle already exists
    TTsyReqHandle stopDtmftoneHandle =
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle(
            CMmPhoneTsy::EMultimodePhoneStopDTMFTone );

    if ( CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != stopDtmftoneHandle )
        {
        iMmPhone->ReqCompleted( aTsyReqHandle, ret );
        }
    else
        {
        // DTMF can only be stopped if there is a connected call
        TInt callId = -1;
        if (GetConnectedCallId(callId) != KErrNone)
        	{
        	iMmPhone->ReqCompleted(aTsyReqHandle, KErrEtelCallNotActive);
        	}
        else
        	{
	        CMmDataPackage dataPackage;
	        dataPackage.PackData(&callId);
	        ret = iMmPhone->MessageManager()->HandleRequestL(
	            EMobilePhoneStopDTMFTone,
	            &dataPackage
	            );

	        if (ret != KErrNone)
	        	{
	        	iMmPhone->ReqCompleted(aTsyReqHandle, ret);
	        	}
	        else
	            {
	            // Save the type of the DTMF
	            iDtmfType = EDtmfTypeDigit;

	            iMmPhone->iReqHandleType =
	                            CMmPhoneTsy::EMultimodePhoneStopDTMFTone;
	            }
        	}
        }

    return KErrNone;
	} // CMmDtmfTsy::StopDTMFToneL

/**
 * Complete the Start DTMF Tone request.
 *
 * @param aResult Error code from LTSY.
 */
void CMmDtmfTsy::CompleteStartDTMFTone(TInt aResult)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_COMPLETESTARTDTMFTONE_1, "TSY: CMmDtmfTsy::CompleteStartDTMFTone aResult=%d", aResult);

    TTsyReqHandle reqHandle
        = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        		CMmPhoneTsy::EMultimodePhoneStartDTMFTone);

    if (CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle)
        {
        iMmPhone->ReqCompleted(reqHandle, aResult);
        }

	} // CMmDtmfTsy::CompleteStartDTMFTone

/**
 * Complete the Stop DTMF Tone request.
 *
 * @param aResult Error code from LTSY.
 */
void CMmDtmfTsy::CompleteStopDTMFTone(TInt aResult)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_COMPLETESTOPDTMFTONE_1, "TSY: CMmDtmfTsy::CompleteStopDTMFTone aResult=%d", aResult);

    TTsyReqHandle reqHandle
        = iMmPhone->iTsyReqHandleStore->ResetTsyReqHandle(
        		CMmPhoneTsy::EMultimodePhoneStopDTMFTone);

    if (CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != reqHandle)
        {
        iMmPhone->ReqCompleted(reqHandle, aResult);
        }

	} // CMmDtmfTsy::CompleteStopDTMFTone

/**
 * Send a DTMF string passing down the call ID of the connected call to LTSY.
 * @param aTsyReqHandle The TSY request handle.
 * @param aTones
 * @return
 */
TInt CMmDtmfTsy::SendDTMFTonesL(const TTsyReqHandle aTsyReqHandle, const TDesC* aTones)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_SENDDTMFTONESL_1, "TSY: CMmDtmfTsy::SendDTMFTonesL");

    TTsyReqHandle sendDTMFTonesHandle = iMmPhone->iTsyReqHandleStore
        ->GetTsyReqHandle(CMmPhoneTsy::EMultimodePhoneSendDTMFTones);

    if (CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == sendDTMFTonesHandle)
        {
        // DTMF can only be sent if there is a connected call
        TInt callId = -1;
        if (GetConnectedCallId(callId) != KErrNone)
        	{
        	iMmPhone->ReqCompleted(aTsyReqHandle, KErrEtelCallNotActive);
        	}
        else
        	{
	        CMmDataPackage dataPackage;
	        dataPackage.PackData(&callId, const_cast<TDesC*>(aTones));
	        TInt ret = iMmPhone->MessageManager()->HandleRequestL(
	            EMobilePhoneSendDTMFTones,
	            &dataPackage
	            );

	        if (ret != KErrNone)
	            {
	            // DTMF string sending has failed
	            iMmPhone->ReqCompleted(aTsyReqHandle, ret);
	            }
	        else
	            {
    	        iDtmfType = EDtmfTypeString;
	            iMmPhone->iReqHandleType = CMmPhoneTsy::EMultimodePhoneSendDTMFTones;
	            }
        	}
        } // if (CMmPhoneTsy::EMultimodePhoneReqHandleUnknown == sendDTMFTonesHandle)
    else
    	{
    	iMmPhone->ReqCompleted(aTsyReqHandle, KErrServerBusy);
    	}

    return KErrNone;
	} // CMmDtmfTsy::SendDTMFTonesL

/**
 * Locate an active voice call.
 *
 * @param aCallId The call ID of an active call, if there is one.
 * @return KErrNone if it finds an active call, KErrNotFound otherwise.
 */
TInt CMmDtmfTsy::GetConnectedCallId(TInt& aCallId)
	{
	CMmCallTsy* mmCall = iMmPhone->CallList()->GetMmCallByStatus(RMobileCall::EStatusConnected);

	if (mmCall)
		{
		aCallId = mmCall->CallId();
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}

	} // CMmDtmfTsy::GetConnectedCallId

// ---------------------------------------------------------------------------
// CMmDtmfTsy::Complete
// Completes the request due timer expiration. Adds a timeout for start
// and stop of DTMF.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmDtmfTsy::Complete(TInt aReqHandleType, TInt)
    {
    switch (aReqHandleType)
        {
        case CMmPhoneTsy::EMultimodePhoneSendDTMFTones:
            CompleteSendDTMFTones(KErrTimedOut);
            break;
        case CMmPhoneTsy::EMultimodePhoneStartDTMFTone:
        	CompleteStartDTMFTone(KErrTimedOut);
        	break;
        case CMmPhoneTsy::EMultimodePhoneStopDTMFTone:
        	CompleteStopDTMFTone(KErrTimedOut);
        	break;
        default:
            // Ignore
            break;
        }
    } // CMmDtmfTsy::Complete

// ---------------------------------------------------------------------------
// CMmDtmfTsy::SendDTMFTonesCancelL
// Cancels DTMF tone sending passing down the call ID.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmDtmfTsy::SendDTMFTonesCancelL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMDTMFTSY_SENDDTMFTONESCANCELL1_1, "TSY: CMmDtmfTsy::SendDTMFTonesCancel");

    TTsyReqHandle sendDTMFTonesHandle =
        iMmPhone->iTsyReqHandleStore->GetTsyReqHandle(
            CMmPhoneTsy::EMultimodePhoneSendDTMFTones );

    // There must be a pending Send to cancel
    if (CMmPhoneTsy::EMultimodePhoneReqHandleUnknown != sendDTMFTonesHandle)
        {
		TInt callId = -1;

		// There needs to be a connected call to send the cancel through
		// If there isn't do nothing
		if (GetConnectedCallId(callId) == KErrNone)
			{
			// no packed parameter for DOS call
			CMmDataPackage dataPackage;
			dataPackage.PackData(&callId);
			TInt ret = iMmPhone->MessageManager()->HandleRequestL(
					EMobilePhoneSendDTMFTonesCancel,
					&dataPackage);

			if ( KErrNone == ret )
				{
				// Set the DTMFTonesCancelFlag
				iSendDTMFTonesCancelFlag = ETrue;
				}
			}
        }

    return KErrNone;
	}

#endif // USING_CTSY_DISPATCHER

