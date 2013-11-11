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
// suplproxypossubsession.cpp
// 

#include <e32std.h>
#include <lbs/epos_eomasuplposerrors.h>		
#include "lbsdevloggermacros.h"
#include "suplproxypossubsession.h"
#include "EPos_PrivacyServerPanic.h"
#include "suplproxyutils.h"
#include "suplpospayload.h"

/**
Constructor.

@param aProtocol Interface to send supl proxy requests to.
*/
CSuplProxyPosSubSession::CSuplProxyPosSubSession(MSuplProxyPosProtocol& aPosProtocol, TInt aRequestId): iPosProtocol(aPosProtocol), iRequestId(aRequestId)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::CSuplProxyPosSubSession() Begin and End\n");
	}

/**
Destructor.
*/
CSuplProxyPosSubSession::~CSuplProxyPosSubSession()
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::~CSuplProxyPosSubSession() Begin \n");
	// close the assistance data builder
	iBuilderSet.Close();
	delete iSuplInfoRetriever;
	delete iRrlpFsm;

	LBSLOG(ELogP1, "CSuplProxyPosSubSession::~CSuplProxyPosSubSession() End \n");

	}


CSuplProxyPosSubSession* CSuplProxyPosSubSession::NewL(MSuplProxyPosProtocol& aPosProtocol, TInt aRequestId, TInt aRefLocSourceId)
{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::NewL() Begin \n");

    CSuplProxyPosSubSession* self = new (ELeave) CSuplProxyPosSubSession(aPosProtocol, aRequestId);
	CleanupClosePushL(*self);
	self->ConstructL(aRefLocSourceId);
	CleanupStack::Pop(self);    

	LBSLOG(ELogP1, "CSuplProxyPosSubSession::NewL() End \n");
    
    return self;
}

void CSuplProxyPosSubSession::ConstructL(TInt aRefLocSourceId)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ConstructL() Begin \n");

	iSuplInfoRetriever = CSuplProxySuplInfoRetriever::NewL(iPosProtocol, aRefLocSourceId);
	// Open up assistance data builder
	iBuilderSet.OpenL();

	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ConstructL() End \n");

	}

//iPosProtocol.SessionDisconnected(this); TODO figure out when to complete session

/**
Called when a message is received for this subsession.

@param aMessage The message from the client.
*/
void CSuplProxyPosSubSession::ServiceL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ServiceL() Begin\n");
	switch (aMessage.Function())
		{
		case ELbsSuplProxyServerPosResetRequestId:
		    {
		    HandleResetRequestId(aMessage);
		    break;
		    }
        case ELbsSuplProxyServerGetPosition:
        	{
        	HandleGetPosition(aMessage);
        	break;
        	}     
        case ELbsSuplProxyServerCancelGetPosition:   
           	{
			HandleCancelGetPosition(aMessage);
			break;
			} 
        case ELbsSuplProxyServerPosClose:
        	{
        	HandleClose(aMessage);
        	break;
        	}
        case ELbsSuplProxyServerPosNotifySessionComplete:
            {
            HandleSessionComplete(aMessage);
			break;
            }
        case ELbsSuplProxyServerPosPayload:
        	{
        	HandlePosPayloadL(aMessage);
        	break;
        	}
        case ELbsSuplProxyServerGetSuplInfo:
        	{
        	HandleGetSuplInfo(aMessage);
        	break;
        	}
        case ELbsSuplProxyServerCancelGetSuplInfo:
        	{
        	HandleCancelGetSuplInfo(aMessage);
        	break;
        	} 
        case ELbsSuplProxyServerSetPosMethod:
        	{
        	HandleSetPosMethod(aMessage);
        	break;
        	}
        case ELbsSuplProxyServerSetSlpAddress:
        	{
        	HandleSetSlpAddressL(aMessage);
        	break;
        	}
	default:
			User::Leave(KErrNotSupported);
		}

	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ServiceL() End\n");
	}

void CSuplProxyPosSubSession::HandleSessionComplete(const RMessage2& aMessage)
    {
    LBSLOG2(ELogP1, "CSuplProxyPosSubSession::HandleSessionComplete(0x%x) Begin\n", iRequestId);

    // Network has ended the SUPL session (for an MOLR / MTLR)
    TInt err = KErrNone;
    err = iPosProtocol.NotifySessionEnd(iRequestId);
    aMessage.Complete(err); 
	iSuplInfoRetriever->CancelRequest();

    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleSessionComplete() End\n");   
    }

void CSuplProxyPosSubSession::HandleClose(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleClose() Begin\n");

	TInt err = iPosProtocol.NotifySubSessionClose(iRequestId);
	aMessage.Complete(err); 

    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleClose() End\n");
	}

void CSuplProxyPosSubSession::HandleResetRequestId(const RMessage2& aMessage)
    {
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleResetRequestId() Begin\n");

    iRequestId = aMessage.Int0();
    iPosProtocol.NotifySubSessionReInit(this,iRequestId);
    iSuplInfoRetriever->ResetState();
    aMessage.Complete(KErrNone); 

    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleResetRequestId() End\n"); 
    }

void CSuplProxyPosSubSession::HandleGetPosition(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleGetPosition() Begin\n");

	TInt err = iPosProtocol.NetworkGetPosition(aMessage, iRequestId);
	if(err)
		{
		aMessage.Complete(err);
		}
	
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleGetPosition() End\n");    
	}


void CSuplProxyPosSubSession::HandleCancelGetPosition(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleCancelGetPosition() Begin\n");

	TInt err = iPosProtocol.CancelNetworkGetPosition(aMessage, iRequestId);
	
	aMessage.Complete(err);
	
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleCancelGetPosition() End\n");    
	}

void CSuplProxyPosSubSession::HandleGetSuplInfo(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleGetSuplInfo() Begin\n");

	iSuplInfoRetriever->NewRequest(aMessage, iRequestId);

    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleGetSuplInfo() End\n");    
	}

void CSuplProxyPosSubSession::HandleCancelGetSuplInfo(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "HandleCancelGetSuplInfo() Begin\n");

	iSuplInfoRetriever->CancelRequest(aMessage);

    LBSLOG(ELogP1, "HandleCancelGetSuplInfo() End\n");    
	}

/*
 * 
 */
void CSuplProxyPosSubSession::HandleSetPosMethod(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleSetPosMethod() Begin\n");
	
	// read the method(s) requested:
	TLbsNetPosRequestMethod method;
	TPckg<TLbsNetPosRequestMethod> posMethodPckg(method);
	aMessage.Read(0, posMethodPckg, 0);		
	
	// tell pos protocol about pos method for this subsession
	iPosProtocol.SetPosMethod(method, iRequestId);
	
	aMessage.Complete(KErrNone);
	
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleSetPosMethod() End\n");    
	}

/*
 * 
 */
void CSuplProxyPosSubSession::HandleSetSlpAddressL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "HandleSetSlpAddressL() Begin\n");
	
	const TInt KMaxServerNameLength = 256;
	// read the slp address:
	__ASSERT_DEBUG(aMessage.GetDesLengthL(0) < KMaxServerNameLength, User::Invariant()); 
	TBuf<KMaxServerNameLength> address;							// avoid constant by using HBuf?
	
	User::LeaveIfError(aMessage.Read(0, address, 0));		
	
	// tell pos protocol about slp address for this subsession
	iSuplInfoRetriever->SetSlpAddress(address);	

	aMessage.Complete(KErrNone);
	
    LBSLOG(ELogP1, "HandleSetSlpAddressL() End\n");    
	}

/**
 * Decodes a POS payload (RRLP message) and passes it to the RRLP state machine.
 * State machine will call it's observer functions as data has been processed and  
 */
void CSuplProxyPosSubSession::HandlePosPayloadL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandlePosPayloadL() Begin\n");
	TInt err = KErrNone;
	// pull out the buffer to be decoded and pass to handler:
	
	HBufC8* encodedPosPayload = HBufC8::NewLC(aMessage.GetDesLengthL(0));	
	TPtr8	descriptor = encodedPosPayload->Des();
	
    User::LeaveIfError(SuplProxyUtils::Read(aMessage, 0, descriptor));
	
    // if this is the first rrlp message, create the rrlp state machine
	if(!iRrlpFsm)	
		{
		iRrlpFsm = CSuplRrlpFsm::NewL(*this, iBuilderSet);
		}      	
	
	//  decode the payload then pass to the rrlp state machine
	CSuplPosPayload* decodedPayload = CSuplPosPayload::DecodePosPayloadL(const_cast<const HBufC8*>(encodedPosPayload), err);	

	// process only if no errors decoding the pos payload.
	if (KErrNone==err)
	    {
	    iRrlpFsm->ProcessPositioningMessage(decodedPayload);
	    }
	else
	    {
	    // complete the GetSuplInfo indicating an error has occurred
	    iSuplInfoRetriever->ReportPosPayloadError(err);
	    }

	CleanupStack::PopAndDestroy(encodedPosPayload);	

	// synchronous completion.
    aMessage.Complete(err);

    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandlePosPayloadL() End\n");
	}

// from MSuplPositioningProtocolFsmObserver:
 
/** Handle a notification of availability of new assistance data from
	the Positioning Protocol (RRLP) state machine.
*/
void CSuplProxyPosSubSession::ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ProcessAssistanceData() ++\n");
	// Pass the assistance data now in iBuilderSet to the CSuplProxyProtocol which in
	// turn retrieves the reference position(if present) and sends the reference position,
	// assistance data to the LBS subsystem
	iPosProtocol.ProcessAssistanceData(aGroupMask, iBuilderSet, aReason,iRequestId);	
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ProcessAssistanceData() --\n");
	}


/** The RRLP wants to inject a location request into LBS.
@param aQuality  quality parameters for the location request.
@param aPosMethod positioning method requested by the Positioning Protocol
*/
void CSuplProxyPosSubSession::ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality,
									   const TLbsNetPosRequestMethod& aPosMethod)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ProcessPositioningRequest() ++\n");
	// Location Request has been received at Rrlp level, pass it to LBS
	TInt err = iPosProtocol.RrlplLocationRequest(aQuality, aPosMethod, iRequestId);
	if(err != KErrNone)
		{
		iSuplInfoRetriever->ReportPosPayloadError(err);
		iRrlpFsm->CancelMachine(CSuplPositioningProtocolFsm::ECancelNone, err);
		}
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::ProcessPositioningRequest()--\n");
	}

	
/** Handle a request from the Positioning Protocol state machine to send
a positioning protocol message to the network as the payload in a SUPL POS
@param aPositioningPayload Positioning Protocol (RRLP) message that will be payload of a SUPL POS message
       [NOTE: CSuplRrlpFsm passes on the ownership of CSuplPosPayload to this class]
@see CSuplRrlpFsm::LocationResp()     
@see CSuplRrlpFsm::AssistanceDataRequest()
@see CSuplRrlpFsm::HandleAssistanceDataMessage()
*/
void CSuplProxyPosSubSession::PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload )
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::PositioningPayloadToNetwork() ++\n");
	iSuplInfoRetriever->PositioningPayloadToNetwork(iSessionEnded, *aPositioningPayload);

    // CSuplRrlpFsm doesn't hold ownership of aPositioningPayload. So it has to be deleted here.
    delete aPositioningPayload;
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::PositioningPayloadToNetwork() --\n");
	}

/** Handle error notification from Positioning Protocol
This is an unrecoverable error. 
@param aError Error code reported by Postioning Protocol State Machine.
*/
void CSuplProxyPosSubSession::PositioningProtocolError(const TInt& aError)
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::PositioningProtocolError() ++\n");
	iSuplInfoRetriever->ReportPosPayloadError(aError);
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::PositioningProtocolError() --\n");
	}

/** Inform SUPL FW that the Positioning session has finished **/
void CSuplProxyPosSubSession::PositioningSessionEnded()
	{
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::PositioningSessionEnded() ++\n");
	// RRLP FSM is indicating that the next pospayload (about to be passed to us) will be the last.
	iSessionEnded = ETrue;	
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::PositioningSessionEnded() --\n");
	}
/**
 * A fresh assistance data request has been requested by LBS. Inform the SuplInfoRetriever
 * of this so that it updates the assistance data request field in the subsequent
 * completion of GetSuplInfoL 
 * @see CSuplProxySuplInfoRetriever::UpdatePendindDataReqMask
 */
void CSuplProxyPosSubSession::HandleSetAssistanceDataMask(TInt aReason, TLbsAsistanceDataGroup aDataRequestMask)
    {
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleSetAssistanceDataMask() ++\n");
    //Update the object about the assistance data requested by LBS.
    iSuplInfoRetriever->UpdatePendindDataReqMask(aReason, aDataRequestMask);
    LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleSetAssistanceDataMask() --\n");
    }

/**
 * A Location update has been received from LBS as a consequence of us passing a decoded RRLP request
 * @see CSuplProxyPosSubSession::ProcessPositioningRequest() above
 */
void CSuplProxyPosSubSession::HandleRrlpLocationResponse(TInt aReason, const TPositionInfoBase& aPosInfo)
	{	
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleRrlpLocationResponse() ++\n");
    __ASSERT_DEBUG(iRrlpFsm != NULL, User::Invariant());
	// pass this back to the RRLP State machine
	iRrlpFsm->LocationResp(aReason, aPosInfo);
	LBSLOG(ELogP1, "CSuplProxyPosSubSession::HandleRrlpLocationResponse() --\n");
	}

