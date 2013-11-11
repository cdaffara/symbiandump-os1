// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <lbs/epos_eomasuplposerrors.h>		
#include "suplproxysuplinforetriever.h"
#include "suplproxyprotocol.h"
#include "suplpospayload.h"
#include "suplrrlpstatemachine.h"
#include "LbsInternalInterface.h"
#include "lbsdevloggermacros.h"

CSuplProxySuplInfoRetriever* CSuplProxySuplInfoRetriever::NewL(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId)
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::NewL() Begin\n");

    CSuplProxySuplInfoRetriever* self = new(ELeave) CSuplProxySuplInfoRetriever(aPosProtocol, aRefLocSourceId);

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::NewL() End\n");

    return self;
    }

CSuplProxySuplInfoRetriever::~CSuplProxySuplInfoRetriever()
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::~CSuplProxySuplInfoRetriever() Begin\n");

    if(!iMessage.IsNull())
    	{
    	iMessage.Complete(KErrSessionClosed);
		if(iRefLocationSrc)
			{
			iRefLocationSrc->CancelRefPositionInfoRequest();
			}
    	}
    if(iRefLocationSrc)
    	{
		delete iRefLocationSrc;
    	}
    if (iPosPayloadBuffer)
        {
        delete iPosPayloadBuffer;
        iPosPayloadBuffer = NULL;
        }

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::~CSuplProxySuplInfoRetriever() End\n");

    }

void CSuplProxySuplInfoRetriever::NewRequest(const RMessage2& aMessage, TInt aRequestId)
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::NewRequest() Begin\n");

    if(!iMessage.IsNull())
    	{
    	aMessage.Complete(KErrInUse);
    	}
    else
    	{
		iMessage = aMessage;
		// read the requested data and store locally for writing back later 
		TPckg<TSuplInfoRequestData> requestedDataPckg(iSuplInfoData);
		iMessage.Read(0, requestedDataPckg, 0); 
		
		iOutstandingItemMask = iSuplInfoData.RequestMask();
 		LBSLOG2(ELogP1, "CSuplProxySuplInfoRetriever iOutstandingItemMask = 0x%d\n", iOutstandingItemMask);
		
 	     if(iOutstandingItemMask & TSuplInfoRequestData::ERequestItemPosPayload)
 	            {
 	            //Immediately clear the outstanding PosPayload mask if it has already been received
 	            if (iPosPayloadAvailable)
 	                {
 	                //HandlePosPayloadL() has arrived before the HandleGetSuplInfo() 
 	                if (iPositioningError == KErrNone)
 	                    {
 	                    if (iLastPosPayload)
 	                        {
 	                        // this will be the last pospayload for this rrlp session, let SUPL FW know
 	                        iSuplInfoData.SetIsLastPosMsg(ETrue);
 	                        //reset the variable
 	                        iLastPosPayload = EFalse;
 	                        }
 	                    __ASSERT_DEBUG(iPosPayloadBuffer,User::Invariant());
 	                    TPtr8 payloadBufferPtr = iPosPayloadBuffer->Des();
 	                    //Write encoded buffer to slot in stored msg
 	                    TInt err = iMessage.Write(1, payloadBufferPtr, 0);
 	                    if (err != KErrNone)
 	                        {
 	                        iPositioningError = err;
 	                        }
 	                    delete iPosPayloadBuffer;
 	                    iPosPayloadBuffer = NULL;
 	                    }//if (iPositioningError == KErrNone)
 	                iSuplInfoData.SetPosPayloadError(iPositioningError);
 	                iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemPosPayload;
 	                //reset the variables
 	                iPosPayloadAvailable = EFalse;
 	                iPositioningError = KErrNone;
 	                }
 	            }
		if(iOutstandingItemMask & TSuplInfoRequestData::ERequestItemCapabilities)
			{
			TLbsNetPosCapabilities caps;
			iPosProtocol.GetCurrentPosCapabilities(caps);
			iSuplInfoData.SetCapabilities(caps);
			iSuplInfoData.SetCapabilitiesError(KErrNone);
			iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemCapabilities;
			}
		if(iOutstandingItemMask & TSuplInfoRequestData::ERequestItemAssistance)
			{
			//Request the CSuplProxyProtocol for the assistance data mask. If it is 
			//not available CSuplProxyProtocol would inject a ProcessLocationRequest
            //to LBS. The required assistance data mask is updated by CSuplProxyProtocol
            //through the UpdatePendingDataReqMask() callback.
			TInt err = iPosProtocol.GetAssistanceDataMask(aRequestId);
			if (err != KErrNone)
			    {
			    iSuplInfoData.SetAssistanceMaskError(err);
		        iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemAssistance;
			    }
			}
		if(iOutstandingItemMask & TSuplInfoRequestData::ERequestItemPosition)
			{
			if(iRefLocationSrc)
				{
				iRefLocationSrc->RequestRefPositionInfo();
				}
			else
				{
				iSuplInfoData.SetPositionError(KErrBadHandle);
		        iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemPosition;				
				}
			}
		CompleteMessageIfReady();
    	}

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::NewRequest() End\n");
    }

void CSuplProxySuplInfoRetriever::CancelRequest(const RMessage2& aMessage)
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CancelRequest() Begin\n");

	TInt err = CancelRequest();
    aMessage.Complete(err);

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CancelRequest() End\n");

    }

TInt CSuplProxySuplInfoRetriever::CancelRequest()
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CancelRequest() Begin\n");

    TInt ret(KErrNone);

    if(iRefLocationSrc)
    	{
    	iRefLocationSrc->CancelRefPositionInfoRequest();
    	}
	
    // cancel any outstanding GetSuplInfoL() message with KErrCancel
    if(!iMessage.IsNull())
        {
        iMessage.Complete(KErrCancel);
        iOutstandingItemMask = TSuplInfoRequestData::ERequestItemNone; 
        }
    else
        {
        ret = KErrNotFound;
        }

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CancelRequest() End\n");

	return ret;
    }


void CSuplProxySuplInfoRetriever::SetSlpAddress(const TDesC& aAddress)
	{
    if(iRefLocationSrc)
    	{
    	iRefLocationSrc->SetRequestorAddress(aAddress);
    	}	
	}

void CSuplProxySuplInfoRetriever::ResetState()
	{
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::ResetState() Begin\n");

	// TODO: Add things to be done when the Reset message is sent by the SUPL FW
	iPositioningError = KErrNone;

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::ResetState() End\n");

	}

CSuplProxySuplInfoRetriever::CSuplProxySuplInfoRetriever(MSuplProxyPosProtocol& aPosProtocol, TInt aRefLocSourceId)
	: iPosProtocol(aPosProtocol),iPosPayloadAvailable(EFalse), iLastPosPayload(EFalse)
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CSuplProxySuplInfoRetriever() Begin\n");

    TRAP_IGNORE(LoadRefLocSrcL(aRefLocSourceId));

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CSuplProxySuplInfoRetriever() End\n");
    }

void CSuplProxySuplInfoRetriever::LoadRefLocSrcL(TInt aRefLocSourceId)
	{
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::LoadRefLocSrcL() Begin\n");

	if(0 != aRefLocSourceId)
		{
		iRefLocationSrc = CLbsRefLocationSourceBase::NewL(*this, TUid::Uid(aRefLocSourceId));
		}

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::LoadRefLocSrcL() End\n");
	}


/** Called when there is some positioning payload information 
*   for eg. with an assistance data ACK while processing a PosPayload containing assistance data
*            or a request for more assistance data
*            or a position response while processing a PosPayload containing a measurement position request
*@param aPositioningPayload Positioning Protocol (RRLP) message that will be payload of a SUPL POS message 
*/
void CSuplProxySuplInfoRetriever::PositioningPayloadToNetwork(TBool aSessionEnded, const CSuplPosPayload& aPositioningPayload)
	{  
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::PositioningPayloadToNetwork() Begin\n");
	if (aSessionEnded)
       {
       // this will be the last pospayload for this rrlp session
       iLastPosPayload = ETrue;
       }
	// Encode the data from the rrlp state machine (before passing to the SUPL FW)
	TRAPD(encodeErr,EncodePosPayloadL(aPositioningPayload));
    iPositioningError = encodeErr;
	UpdatePosPayloadInfo();
    LBSLOG2(ELogP1, "CSuplProxySuplInfoRetriever iOutstandingItemMask = 0x%d\n", iOutstandingItemMask);
    CompleteMessageIfReady();
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::PositioningPayloadToNetwork() End\n");
	}

void CSuplProxySuplInfoRetriever::ReportPosPayloadError(TInt aError)
	{
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::ReportPosPayloadError() Begin\n");

	// RRLP FSM is indicating that the session 'has no future'. called, eg when Out of Memory or when there's a protocol error 
	// or when FSM times out waiting for LBS-side or Network-side event.
	// Note: in theroy this could be a positive error code of type TRrlpErrorCode, but in practice those aren't being used
	// We may or may not have an outstanding GetSuplInfoL(pospayload) call to use to pass this error code back to SUPL FW.
	// Respond to the outstanding GetSuplInfoL(pospayload) with an appropriate error code, or if non outstanding, 
	// store the error code and respond to next one with it.
	iPositioningError = aError;
	UpdatePosPayloadInfo();
	LBSLOG2(ELogP1, "CSuplProxySuplInfoRetriever iOutstandingItemMask = 0x%d\n", iOutstandingItemMask);
	CompleteMessageIfReady();
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::ReportPosPayloadError() End\n");
	}

/** An utility function used to hold calls to leaving methods [called from PositioningPayloadToNetwork within a trap statement.
 *Encodes the PosPayload passed from the RRLP FSM into an ASN1 RRLP payload that would be passed on
 *GetSuplInfoL() request completion.
 */
void CSuplProxySuplInfoRetriever::EncodePosPayloadL(const CSuplPosPayload& aPositioningPayload)
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::EncodePosPayloadL() Begin\n");
    const CRrlpMessageBase& constMsgBase = static_cast<const CRrlpMessageBase&>(aPositioningPayload);
    CRrlpMessageBase& msgBase = const_cast<CRrlpMessageBase&>(constMsgBase);
    //Required to store the payload incase the SUPL FW has not sent a GetSuplInfoL() yet.
	iPosPayloadBuffer = HBufC8::NewL(KMaxOutgoingPosPayloadLength);
    TPtr8 payloadBufferPtr = iPosPayloadBuffer->Des();
    
    // take the CSuplPosPayload* and turn it into a ASN1T_PosPayLoad_rrlpPayload before passing it to the network:  
    User::LeaveIfError(msgBase.EncodeToL(payloadBufferPtr));

    //if GetSuplInfoL() is outstanding - write the encoded buffer
    if (SuplInfoPayloadReqPending())
        {
        // write the encoded buffer into slot in stored msg
        User::LeaveIfError(iMessage.Write(1, payloadBufferPtr, 0));
        delete iPosPayloadBuffer;
        iPosPayloadBuffer = NULL;
        }
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::EncodePosPayloadL() End\n");
    }
	
	
/**Method that ensures that irrespective of the order of arrival of GetSuplInfoL() and HandleSuplPosMessageL() , 
*  the necessary info are stored and hence used while completing the GetSuplInfoL().
*/
void  CSuplProxySuplInfoRetriever::UpdatePosPayloadInfo()
{
   LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::UpdatePosPayloadInfo() Begin\n");
    if (iOutstandingItemMask == TSuplInfoRequestData::ERequestItemNone)
        {
        //GetSuplInfoL() request has not been received yet
        iPosPayloadAvailable = ETrue;
        }
    else
        {
        //GetSuplInfoL() is outstanding
        if (SuplInfoPayloadReqPending())
            {
            //Outstanding GetSuplInfoL() requires pospayload info
            if (iLastPosPayload)
                {
                // this will be the last pospayload for this rrlp session, let SUPL FW know
                iSuplInfoData.SetIsLastPosMsg(ETrue);
                //reset the variable
                iLastPosPayload = EFalse;
                }
            iSuplInfoData.SetPosPayloadError(iPositioningError);
            iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemPosPayload;
            iPositioningError = KErrNone; //reset the variable
            }
        }
    LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::UpdatePosPayloadInfo() End\n");
}
	
/**Callback from CSuplProxyProtocol when the details regarding the assistance data required
by the module are available. Update the mask only if this session has an outstanding GetSuplInfoL
requiring assistance data mask details[Note: CSuplProxyProtocol calls this method for all the sessions ].
@param aDataRequestMask     The assistance data mask to be passed to the SUPL FW
@param aError               The error code indicating the status of data in aDataRequestMask
*/
void  CSuplProxySuplInfoRetriever::UpdatePendindDataReqMask(TInt aError,
                TLbsAsistanceDataGroup aDataRequestMask)
    {
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::UpdatePendindDataReqMask() Begin\n");

    //Update the mask only if the assistance data info is required to complete
    //an outstanding GetSuplInfoL
    if (iOutstandingItemMask & TSuplInfoRequestData::ERequestItemAssistance)
        {
        if (aError == KErrNone)
            {
            iSuplInfoData.SetAssistanceDataMask(aDataRequestMask);
            }
        iSuplInfoData.SetAssistanceMaskError(aError);
        iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemAssistance;
        LBSLOG2(ELogP1, "CSuplProxySuplInfoRetriever iOutstandingItemMask = 0x%d\n", iOutstandingItemMask);
        CompleteMessageIfReady();
        }

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::UpdatePendindDataReqMask() End\n");
    }

void CSuplProxySuplInfoRetriever::HandleRefPositionInformation(TInt aError, 
			const TPositionInfoBase* aFirstPosition, const TPositionAreaInfoBase* aFistCellId, 
            const TPositionInfoBase* aSecondPosition, const TPositionAreaInfoBase* aSecondCellId)
	{
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::HandleRefPositionInformation() Begin\n");

	if(aError == KErrNone && aFirstPosition)
		{
		iSuplInfoData.SetPositionError(KErrNone);
		iSuplInfoData.SetPosition(*aFirstPosition);
		}
	else
		{
		iSuplInfoData.SetPositionError(aError);
		}
    iOutstandingItemMask &= ~TSuplInfoRequestData::ERequestItemPosition;	
    
    LBSLOG2(ELogP1, "CSuplProxySuplInfoRetriever iOutstandingItemMask = 0x%d\n", iOutstandingItemMask);
	CompleteMessageIfReady();
	if(aFirstPosition)
		{
		delete aFirstPosition;
		}
	if(aFistCellId)
		{
		delete aFistCellId;
		}
	if(aSecondPosition)
		{
		delete aSecondPosition;
		}
	if(aSecondCellId)
		{
		delete aSecondCellId;
		}
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::HandleRefPositionInformation() ++\n");
	}

/** A utility function to complete the outstanding GetSuplInfoL() request if all the required information are available.
*/
void CSuplProxySuplInfoRetriever::CompleteMessageIfReady()
	{
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CompleteMessageIfReady() Begin\n");

	if(!iMessage.IsNull() && iOutstandingItemMask == TSuplInfoRequestData::ERequestItemNone)
		{
		LBSLOG(ELogP1, "All GetSuplInfoL() data is ready, complete request\n");
		TPckg<TSuplInfoRequestData> requestedDataPckg(iSuplInfoData);
		iMessage.Write(0, requestedDataPckg, 0);	
		iMessage.Complete(KErrNone);
		}

	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::CompleteMessageIfReady() End\n");
	}

/** Utility function that returns True if a GetSuplInfoL() request requiring PosPayload information
 *  is outstanding. 
 * 
 */
TBool CSuplProxySuplInfoRetriever::SuplInfoPayloadReqPending()
	{
	LBSLOG(ELogP1, "CSuplProxySuplInfoRetriever::IsPosPayloadRequested() Begin and End\n");
	return (!iMessage.IsNull() && (iOutstandingItemMask & TSuplInfoRequestData::ERequestItemPosPayload));
	}
	
// END OF FILE
