// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>

// LBS-specific
#include <lbs.h>
#include <lbs/lbslocerrors.h>
#include "lbsdevloggermacros.h"

#include "lbsprocessuiddefs.h"
#include "ngmessageswitch.h"
#include "lbsnetinternalapi.h"
#include "nrhpanic.h"


//
// CNGMessageSendBufferBase
//

CNGMessageSendBufferBase::CNGMessageSendBufferBase(RLbsNetChannel& aGatewayChannel, CLbsAdmin& aAdmin) :
	CActive(EPriorityStandard),
	iAdmin(aAdmin),
	iNetGatewayChannel(aGatewayChannel)
	
	{
	CActiveScheduler::Add(this);
	}
	
CNGMessageSendBufferBase::~CNGMessageSendBufferBase()
	{
	Cancel();
	}

void CNGMessageSendBufferBase::RunL()
	{
	// This must be implemented by the child class, so leave here
	User::Leave(KErrNotSupported);
	}

void CNGMessageSendBufferBase::DoCancel()
	{
	iNetGatewayChannel.CancelSendMessageNotification();
	}

void CNGMessageSendBufferBase::SendMessageToNetwork(const TLbsNetInternalMsgBase& aMessage)
	{
	iNetGatewayChannel.SendMessage(aMessage, iStatus);
	SetActive();
	}
	
//
// CNGMessageSendBuffer
//

// Dummy params used for initialising member variables
const TLbsNetSessionIdInt KInvalidSessionId;
const TPositionInfo KInvalidPosInfo;
const TTime KInvalidTTime(0);


//
// CNGMessageSendBuffer
//

	
CNGMessageSendBuffer::CNGMessageSendBuffer(RLbsNetChannel& aGatewayChannel, CLbsAdmin& aAdmin) :
	CNGMessageSendBufferBase(aGatewayChannel, aAdmin),
	iEmergencyBuffer(2), // two spaces allocated - all we need to process an emergency
	iEmergencyPrivacyResponseData(KInvalidSessionId, TLbsNetworkEnumInt::EPrivacyResponseUnknown, KErrNone),
	iEmergencyLocationResponseData(KInvalidSessionId, KErrNone, KInvalidPosInfo, KInvalidTTime)
	{
	}
	
CNGMessageSendBuffer::~CNGMessageSendBuffer()
	{
	iEmergencyBuffer.Reset();
	iBuffer.ResetAndDestroy();
	}

CNGMessageSendBuffer* CNGMessageSendBuffer::NewL(RLbsNetChannel& aGatewayChannel, CLbsAdmin& aAdmin)
	{
	CNGMessageSendBuffer* self = new (ELeave) CNGMessageSendBuffer(aGatewayChannel, aAdmin);
	return self;
	}
	
void CNGMessageSendBuffer::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	// Send the next message in the buffer,
	// if there is one there.
	if (iEmergencyBuffer.Count() > 0)
		{
		// Send the emergency related message to the Network Gateway
		TLbsNetInternalMsgBase* msg = iEmergencyBuffer[0];
		SendMessageToNetwork(*msg);
		iEmergencyBuffer.Remove(0);
		}
	else if (iBuffer.Count() > 0)
		{
		// Send the message to the Network Gateway
		TLbsNetInternalMsgBase* msg = iBuffer[0];
		SendMessageToNetwork(*msg);
		
		// Remove and delete the item from the buffer if it was a "newed" one
		iBuffer.Remove(0);
		delete msg;
		}
	}

/* Error function that catches the leave from RunL.

For this class, it is not a fatal error if the property
returns an error, so only log the error with LBS devlogger.
*/
TInt CNGMessageSendBuffer::RunError(TInt aError)
	{
	LBSLOG_ERR2(ELogP3, "CNGMessageSendBuffer::RunError : %d", aError);
	return KErrNone;
	}

void CNGMessageSendBuffer::BufferData(TLbsNetInternalMsgBase* aData)
	{	
	// Add the data to the buffer
	TInt err = iBuffer.Append(aData);
	if( err != KErrNone )
		{
		LBSLOG_ERR2(ELogP3, "CNGMessageSendBuffer::BufferData Append failed: %d", err);
		delete aData;
		}
	}

void CNGMessageSendBuffer::BufferEmergencyData(TLbsNetInternalMsgBase* aData)
	{	
	// Add the data to the emergency buffer
	TInt err = iEmergencyBuffer.Append(aData);
	// there should always be enough space in this array of pointers because we pre-alloated
	__ASSERT_DEBUG(KErrNone == err, Panic(ENrhPanicOOMWhenProcessingEmergency));

	}
	
	
void CNGMessageSendBuffer::SendNetLocResponseToNetwork(
		const TLbsNetSessionIdInt& aSessionId, 
		TInt aCompletionCode,
		const TPositionInfoBase& aPosInfo,
		const TTime& aTimeStamp,
		TBool aEmergency)
	{
	TLbsNetLocationResponseMsg msg(aSessionId,
								   aCompletionCode,
								   aPosInfo,
								   aTimeStamp);

	if (!IsActive())
		{
		SendMessageToNetwork(msg);
		}
	else
		{
		TLbsNetLocationResponseMsg* bufMsg = NULL;
		if (aEmergency)
			{
			iEmergencyLocationResponseData = msg;
			bufMsg = &iEmergencyLocationResponseData;
			BufferEmergencyData(bufMsg);	
			}
		else
			{
			bufMsg = new TLbsNetLocationResponseMsg(msg);
			if (bufMsg)
				{
				BufferData(bufMsg);	
				}
			else
				{
				LBSLOG_ERR(ELogP3, "CNGMessageSendBuffer::SendNetLocResponseToNetwork: Failed to allocate memory.");
				}
			}
				
		// coverity [memory_leak]
		// we do not delete bufMsg here as it is now owned by iBuffer
		}
	}
	
void CNGMessageSendBuffer::SendMtlrResponseToNetwork(
		const TLbsNetSessionIdInt& aSessionId, 
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResult,
		TInt /* aReason */,
		TBool aEmergency)
	{
	
	TLbsNetMtLrReponseMsg* bufMsg = NULL;
	
	TLbsNetMtLrReponseMsg msg(aSessionId, aResult, KErrNone);
	
	if (!IsActive())
		{
		SendMessageToNetwork(msg);
		}
	else
		{

		if (aEmergency)
			{
			iEmergencyPrivacyResponseData = msg;
			bufMsg = &iEmergencyPrivacyResponseData;
			BufferEmergencyData(bufMsg);	
			}
		else
			{
			bufMsg = new TLbsNetMtLrReponseMsg(msg);
			if (bufMsg)
				{
				BufferData(bufMsg);	
				}
			else
				{
				LBSLOG_ERR(ELogP3, "CNGMessageSendBuffer::SendMtlrResponseToNetwork: Failed to allocate memory.");
				}
			}
	
		// coverity [memory_leak]
		// we do not delete bufMsg here as it is now owned by iBuffer
		}	
	}
	
void CNGMessageSendBuffer::SendX3pRequestToNetwork(
		const TLbsNetSessionIdInt& aSessionId,
		const TDesC& aDestId,
		TUint aTransmitPriority,
		const TLbsNetPosRequestOptionsTechnologyInt& aOptions)
	{
	TLbsNetTransmitLocationRequestMsg msg(aSessionId, 
										  aDestId, 
										  aTransmitPriority,
										  aOptions);	
	

	if (!IsActive())
		{
		SendMessageToNetwork(msg);
		}
	else
		{
		TLbsNetTransmitLocationRequestMsg* bufMsg = new TLbsNetTransmitLocationRequestMsg(msg);
		if (bufMsg)
			{
			BufferData(bufMsg);	
			}
		else
			{
			LBSLOG_ERR(ELogP3, "CNGMessageSendBuffer::SendX3pRequestToNetwork: Failed to allocate memory.");
			}
		
		// coverity [memory_leak]
		// we do not delete bufMsg here as it is now owned by iBuffer
		}	
	}
	
void CNGMessageSendBuffer::SendX3pCancelToNetwork(
		const TLbsNetSessionIdInt& aSessionId,
		TInt aReason)
	{
	TLbsNetTransmitLocationCancelMsg msg(aSessionId, aReason);

	if (!IsActive())
		{
		SendMessageToNetwork(msg);
		}
	else
		{
		TLbsNetTransmitLocationCancelMsg* bufMsg = new TLbsNetTransmitLocationCancelMsg(msg);
		if (bufMsg)
			{
			BufferData(bufMsg);	
			}
		else
			{
			LBSLOG_ERR(ELogP3, "CNGMessageSendBuffer::SendX3pCancelToNetwork: Failed to allocate memory.");
			}
		
		// coverity [memory_leak]
		// we do not delete bufMsg here as it is now owned by iBuffer
		}	
	}

void CNGMessageSendBuffer::SendExternalLocateCancelToNetwork(const TLbsNetSessionIdInt& aSessionId,
												  TInt aReason)
	{
	TLbsNetCancelFromPrivacyControllerMsg msg(aSessionId, 
											 aReason);

	if (!IsActive())
		{
		SendMessageToNetwork(msg);
		}
	else
		{
		TLbsNetCancelFromPrivacyControllerMsg* bufMsg = new TLbsNetCancelFromPrivacyControllerMsg(msg);
		if (bufMsg)
			{
			BufferData(bufMsg);	
			}
		else
			{
			LBSLOG_ERR(ELogP3, "CNGMessageSendBuffer::SendExternalLocateCancelToNetwor: Failed to allocate memory.");
			}
        
		// coverity [memory_leak]
        // we do not delete bufMsg here as it is now owned by iBuffer
		}	
	}


//
// CNGMessageSwitch
//	

CNGMessageSwitch::CNGMessageSwitch(CLbsAdmin& aAdmin):	
	iAdmin(aAdmin)
    {	
    }
    
CNGMessageSwitch* CNGMessageSwitch::NewL(CLbsAdmin& aAdmin)
    {
	CNGMessageSwitch* self = new (ELeave) CNGMessageSwitch(aAdmin);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);	
	return(self);
    }
        
void CNGMessageSwitch::ConstructL()
    {
    iObservers.ReserveL(2);
    iNetworkGateway.OpenL(RLbsNetChannel::EChannelNRH2NG, *this);
    	
    iNetworkGatewaySendBuffer = CNGMessageSendBuffer::NewL(iNetworkGateway, iAdmin);

    }
    
CNGMessageSwitch::~CNGMessageSwitch()
    {

    iRefPosBuffer.Close();
    
    iFNPPosBuffer.ResetAndDestroy();

    delete iNetworkGatewaySendBuffer;

    iNetworkGateway.Close();
    
    iObservers.Close();
    }
    
TInt CNGMessageSwitch::RegisterObserver(MNGMessageSwitchObserver* aObserver)
    {
	return iObservers.Append(aObserver);
    }

void CNGMessageSwitch::SendX3pRequest(const TLbsNetSessionIdInt& aSessionId,
									  const TDesC& aDestId,
									  TUint aTransmitPriority,
									  const TLbsNetPosRequestOptionsTechnologyInt& aOptions)
    {
    iNetworkGatewaySendBuffer->SendX3pRequestToNetwork(aSessionId, aDestId, aTransmitPriority, aOptions);
    }

void CNGMessageSwitch::SendX3pCancel(const TLbsNetSessionIdInt& aSessionId,
									 TInt aReason)
    {
    iNetworkGatewaySendBuffer->SendX3pCancelToNetwork(aSessionId, aReason);
    }
        
void CNGMessageSwitch::SendMTLRResponse(const TLbsNetSessionIdInt& aSessionId, 
										TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResult,
										TInt aReason,
										TBool aEmergency)
    {
    iNetworkGatewaySendBuffer->SendMtlrResponseToNetwork(aSessionId, aResult, aReason, aEmergency);
    }
void CNGMessageSwitch::SendExternalLocateCancel(const TLbsNetSessionIdInt& aSessionId, TInt aReason)
	{
	iNetworkGatewaySendBuffer->SendExternalLocateCancelToNetwork(aSessionId, aReason);
	}

void CNGMessageSwitch::SendNetLocResponse(
		const TLbsNetSessionIdInt& aSessionId, 
		TInt aCompletionCode,
		const TLbsNetPosRequestQualityInt& aRequestQuality,
		const TPositionInfoBase& aPosInfo,
		const TTime& aTimeStamp,
		TBool aEmergency)
	{
	// Send a Network Location Response to the network
	iNetworkGatewaySendBuffer->SendNetLocResponseToNetwork(aSessionId, 
														aCompletionCode,
														aPosInfo, 
														aTimeStamp,
														aEmergency);
	
	// Let the observers know what the quality criteria were, so that if
	// the network modifies teht final position the client can see if it
	// matches the orignal criteria.
	const TInt count = iObservers.Count();
	for (TInt i = 0; i < count; i++)
		{
		iObservers[i]->OnNetLocResponse(aSessionId,aRequestQuality);
		}
	}

/**
 From MLbsNetChannelObserver.
 Called when a message is sent by the Network Gateway.
 */    
void CNGMessageSwitch::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	__ASSERT_DEBUG(aChannelId == RLbsNetChannel::EChannelNRH2NG, Panic(ENrhPanicUnexpectedNetChannelId));
	(void) aChannelId;
    
	switch(aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			const TLbsNetLocationRequestMsg* msg = 
							static_cast<const TLbsNetLocationRequestMsg*>(&aMessage);
			const TInt count = iObservers.Count();
			// for an NI the REF location may arrive before the location request
			TPositionInfo posInfo;
			TInt err = GetNetworkReferencePosition(msg->SessionId(), posInfo);

			for (TInt i = 0; i < count; i++)
				{
				iObservers[i]->OnNetLocRequest(msg->SessionId(), 
														msg->Method(),
														msg->Service(), 
														msg->IsEmergency(), 
														msg->Quality());
				if (KErrNone == err) // if no error then there is a reference position so pass it on
					{
					iObservers[i]->OnNetLocReferenceUpdate(msg->SessionId(), posInfo);
					}
				}
			break;
			}
		   
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			const TLbsNetSessionCompleteMsg* msg = 
						static_cast<const TLbsNetSessionCompleteMsg*>(&aMessage);

			const TInt count = iObservers.Count();
			for (TInt i = 0; i < count; i++)
				{
				iObservers[i]->OnSessionComplete(msg->SessionId(), 
												 msg->Reason());
				}

			// We know the request session has finished now, so remove
			// any buffered reference or final network positions.
			RemoveBufferedNetworkPositions(msg->SessionId());
			break;
			}
		   
		case TLbsNetInternalMsgBase::EPrivacyRequest: 
			{
			const TLbsNetMtLrRequestMsg* msg = 
						static_cast<const TLbsNetMtLrRequestMsg*>(&aMessage);

			const TInt count = iObservers.Count();
			for (TInt i = 0; i < count; i++)
				{
				iObservers[i]->OnMTLRRequest(msg->SessionId(), 
										 	 TLbsNetworkEnumInt::EServiceMobileTerminated, 
										 	 msg->IsEmergency(),
											 msg->RequestInfo(),
											 msg->Privacy());
				}
			break;
			}

		case TLbsNetInternalMsgBase::ENetworkLocationUpdate:
				{
				const TLbsNetLocationUpdateMsg * msg = 
							static_cast<const TLbsNetLocationUpdateMsg*>(&aMessage);
				// its either a REF location or a FNP
				const TPositionInfoBase& positionInfo =  msg->PositionInfo();
				
				TPositionModuleInfo::TTechnologyType techType = positionInfo.PositionMode();
			
				const TInt count = iObservers.Count();
				
						
				if (TPositionModuleInfo::ETechnologyNetwork == techType)
					{ // reference position add to ref pos list 
					TNetPosItem item;
					item.iSessionId = msg->SessionId();
					Mem::Copy(&item.iPosInfo, &positionInfo, positionInfo.PositionClassSize());
					
					// OOM  - do nothing here - just ignore the error which results in the REF pos 
					// not being saved. Note we do enure that the mobiles position
					// does get sent out to the network - its juts means that the callback
					// that delivers the REF pos to the privacy system does not happen!
					TInt err = iRefPosBuffer.Append(item);
					if( KErrNone != err )
						{
						LBSLOG_ERR(ELogP3, "CNGMessageSwitch::ProcessNetChannelMessage: iRefPosBuffer.Append Failed!!");
						}
					for (TInt i = 0; i < count; i++)
						{
						iObservers[i]->OnNetLocReferenceUpdate(msg->SessionId(), positionInfo);
						}
					}
				else
					{ 
					// save  FNP here - for processing 
					// completion of X3Ps when session completes
					TNetFNPPosItem* item = new TNetFNPPosItem();
					if (item == NULL)
						{
						// OOM  - do nothing here - just discard the FNP.. 
						// Note thats OK becuase we don't need to save the FNP for tp process 
						// emergencies just X3Ps. 
						}
					else
						{
						item->iSessionId = msg->SessionId();
					
						Mem::Copy(&item->iPosInfo, &positionInfo, positionInfo.PositionClassSize());
						TInt err = iFNPPosBuffer.Append(item); // here, ownership passes to pointer array!
						if( KErrNone != err )
							{
							delete item;
							LBSLOG_ERR(ELogP3, "CNGMessageSwitch::ProcessNetChannelMessage: iFNPPosBuffer.Append Failed!!");
							}
						}
					for (TInt i = 0; i < count; i++)
						{
						iObservers[i]->OnNetLocFinalUpdate(msg->SessionId(), positionInfo);
						}
					}
				break;
				}
		case TLbsNetInternalMsgBase::ETransmitLocationRequest:// outgoing only
		case TLbsNetInternalMsgBase::ELocationResponse:       // outgoing only
		case TLbsNetInternalMsgBase::EPrivacyResponse:        // outgoing only
		default:
			{
			break;
			}
		}
    }


/** SessionId comparison for RArray<TNetPosItem>::Find().
*/
TBool CNGMessageSwitch::IsSessionIdEqual(
		const TLbsNetSessionIdInt* aSessionId,
    	const TNetPosItem& aItem)
	{
	return (*aSessionId == aItem.iSessionId);
	}
/** SessionId comparison for RPointerArray<TNetFNPPosItem>::Find().
*/
TBool CNGMessageSwitch::IsFNPSessionIdEqual(
		const TLbsNetSessionIdInt* aSessionId,
    	const TNetFNPPosItem& aItem)
	{
	return (*aSessionId == aItem.iSessionId);
	}
 
/**
*/
TInt CNGMessageSwitch::GetNetworkReferencePosition(
		const TLbsNetSessionIdInt& aSessionId,
		TPositionInfoBase& aPosInfo)
	{
	// Try to find the update if it has already happened.
	TInt index = iRefPosBuffer.Find(aSessionId, IsSessionIdEqual);
	if (index >= 0)
		{
		const TNetPosItem& item = iRefPosBuffer[index];
		Mem::Copy(&aPosInfo, &item.iPosInfo, item.iPosInfo.PositionClassSize());
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

/**
*/
TInt CNGMessageSwitch::GetNetworkFinalPosition(
		const TLbsNetSessionIdInt& aSessionId,
		TPositionInfoBase& aPosInfo)
	{
	// Try to find the update if it has already happened.
	TInt index = iFNPPosBuffer.Find(aSessionId, IsFNPSessionIdEqual);
	if (index >= 0)
		{
		const TNetFNPPosItem* item = iFNPPosBuffer[index];
		Mem::Copy(&aPosInfo, &item->iPosInfo, item->iPosInfo.PositionClassSize());
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

void CNGMessageSwitch::RemoveBufferedNetworkPositions(const TLbsNetSessionIdInt& aSessionId)
	{
	// Remove any reference positions
	TInt index = iRefPosBuffer.Find(aSessionId, IsSessionIdEqual);
	while (index >= 0)
		{
		iRefPosBuffer.Remove(index);
		index = iRefPosBuffer.Find(aSessionId, IsSessionIdEqual);
		}
	
	index = iFNPPosBuffer.Find(aSessionId, IsFNPSessionIdEqual);
	while (index >= 0)
		{
		TNetFNPPosItem* fnpPsItem = iFNPPosBuffer[index];
		delete fnpPsItem;
		iFNPPosBuffer.Remove(index);
		index = iFNPPosBuffer.Find(aSessionId, IsFNPSessionIdEqual);
		}
	}

