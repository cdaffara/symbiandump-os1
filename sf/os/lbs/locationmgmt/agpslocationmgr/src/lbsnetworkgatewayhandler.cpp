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


#include "lbsnetworkgatewayhandler.h"
#include "lbsassistancedatacacheapi.h"
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistancebadsatlist.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsassistancenavigationmodel.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceionosphericmodel.h>
#include <lbs/lbsassistanceutcmodel.h>
#include <lbs/lbsassistancereferencelocation.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsagpsngmsgs.h"
#include "lbsdevloggermacros.h"
#include <lbs/lbslocerrors.h>


// We can queue up to 4 messages
const TInt KNetworkGatewayHandlerQSize = 4;


CNetworkGatewayHandler * CNetworkGatewayHandler::NewL
	(
	MNetworkGatewayHandlerObserver* aObserver
	)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::NewL()");
	CNetworkGatewayHandler * self = new(ELeave) CNetworkGatewayHandler (aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
CNetworkGatewayHandler::CNetworkGatewayHandler(MNetworkGatewayHandlerObserver* aObserver) : 
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iQ(KNetworkGatewayHandlerQSize)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::CNetworkGatewayHandler()");
	}

void CNetworkGatewayHandler::ConstructL()
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::ConstructL()");
	CActiveScheduler::Add(this);
	iAssistanceDataCache.OpenL();
	iNGChannel.OpenL(RLbsNetChannel::EChannelAGPS2NG, *this);
	iQ.OpenL();
	}

//------------------------------------------------------------------------------------------------------------
CNetworkGatewayHandler::~CNetworkGatewayHandler()
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::~CNetworkGatewayHandler()");
	iAssistanceDataCache.Close(); 
	iNGChannel.Close();
	iQ.Close();
	}

 // request from integration module to read a particular  assistance data item
TInt CNetworkGatewayHandler::GetAssistanceDataItem(TLbsAssistanceDataItem aItem,RDataReaderRootBase& aDataRoot,	TTime& aTimeStamp)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::GetAssistanceDataItem()");
	TInt error = iAssistanceDataCache.GetAssistDataItem(aItem,aDataRoot);
	if (KErrNone == error)
		{
		TBool dataAvailable = EFalse;
		switch(aItem)
        	{
	        case EAssistanceDataAquisitionAssistance:
	            dataAvailable = reinterpret_cast<RUEPositioningGpsAcquisitionAssistanceReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataBadSatList:
	        	dataAvailable = reinterpret_cast<RBadSatListReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataNavigationModel:
	        	dataAvailable = reinterpret_cast<RUEPositioningGpsNavigationModelReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataReferenceTime:
	        	dataAvailable = reinterpret_cast<RUEPositioningGpsReferenceTimeReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataIonosphericModel:
	        	dataAvailable = reinterpret_cast<RUEPositioningGpsIonosphericModelReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataReferenceLocation:
	        	dataAvailable = reinterpret_cast<RReferenceLocationReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataAlmanac:
	        	dataAvailable = reinterpret_cast<RUEPositioningGpsAlmanacReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        case EAssistanceDataPositioningGpsUtcModel:
	        	dataAvailable = reinterpret_cast<RUEPositioningGpsUtcModelReader*>(&aDataRoot)->IsDataAvailable();
	            break;
	        default:
	            //Intentionally empty. dataAvailable remains EFalse.
	            break;
	        }
	        
		if (!dataAvailable)
			{
			error = KErrNotFound;
			}
		else
			{
			aTimeStamp = aDataRoot.TimeStamp();	
			}
		}
	return error;
	}

// request from integration module to read a particular  assistance data item time stamp only
TInt CNetworkGatewayHandler::GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem aItem, TTime& aTimeStamp)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::GetAssistanceDataItemTimeStamp()");
	TInt error = KErrNone;
	TTime timeStamp;
	
	switch(aItem)
    	{
        case EAssistanceDataAquisitionAssistance:
			{
			RUEPositioningGpsAcquisitionAssistanceReader reader;
			TRAP(error, reader.OpenL());
			if (KErrNone == error)
			    {
	            error = GetAssistanceDataItem(aItem, reader, timeStamp);
	            reader.Close();
			    }
			break;
			}
        case EAssistanceDataBadSatList:
			{
			RBadSatListReader reader;
			TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        case EAssistanceDataNavigationModel:
			{
			RUEPositioningGpsNavigationModelReader reader;
			TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        case EAssistanceDataReferenceTime:
			{
			RUEPositioningGpsReferenceTimeReader reader;
            TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        case EAssistanceDataIonosphericModel:
			{
			RUEPositioningGpsIonosphericModelReader reader;
            TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        case EAssistanceDataReferenceLocation:
			{
			RReferenceLocationReader reader;
            TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        case EAssistanceDataAlmanac:
			{
			RUEPositioningGpsAlmanacReader reader;
            TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        case EAssistanceDataPositioningGpsUtcModel:
			{
			RUEPositioningGpsUtcModelReader reader;
            TRAP(error, reader.OpenL());
            if (KErrNone == error)
                {
                error = GetAssistanceDataItem(aItem, reader, timeStamp);
                reader.Close();
                }
			break;
			}
        default:
			{
			error = KErrNotFound;
			break;
			}
        }
        
    if (KErrNone == error)
    	{
    	aTimeStamp = timeStamp;
    	}

	return error;
	}


// request from integration module to request a new set of assistance data from network
void CNetworkGatewayHandler::SendAssistanceDataRequest(TLbsAsistanceDataGroup aDataItemMask)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::SendAssistanceDataRequest()");
 	TLbsNetAssistanceDataRequestMsg msg(aDataItemMask);
 	SendOrQueueMessage(msg);
 	}
	
void CNetworkGatewayHandler::SendSelfLocationRequest(const TLbsNetSessionIdInt& aSessionId, 
													 const TLbsNetPosRequestOptionsAssistanceInt& aOptions)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::SendSelfLocationRequest()");
 	TLbsNetSelfLocationRequestMsg msg(aSessionId, aOptions);
 	SendOrQueueMessage(msg);
	}
	
void CNetworkGatewayHandler::SendSelfLocationCancel(const TLbsNetSessionIdInt& aSessionId, TInt aReason)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::SendSelfLocationCancel()");
 	TLbsNetSelfLocationCancelMsg msg(aSessionId, aReason);
 	SendOrQueueMessage(msg);
	}

void CNetworkGatewayHandler::SendSystemStatusAdvice(TBool aTracking)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::SendSystemStatusAdvice()");
 	TLbsNetSystemStatusAdviceMsg msg(aTracking);
 	SendOrQueueMessage(msg);
	}
	
void CNetworkGatewayHandler::RunL()
	{
	LBSLOG2(ELogP1, "CNetworkGatewayHandler::RunL() Begin. Status: %d\n", iStatus.Int());
	__ASSERT_DEBUG(iStatus.Int() == KErrNone, User::Panic(KLbsAGPSManFault, iStatus.Int()));
	
	// Check if more queued messages await sending
	if (!iQ.IsEmpty())
		{
 		iNGChannel.SendMessage(iQ.Read(), iStatus);
 		SetActive();
		}
	LBSLOG(ELogP1, "CNetworkGatewayHandler::RunL() End\n");
	}
	
void CNetworkGatewayHandler::DoCancel()
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::DoCancel()");
	iNGChannel.CancelSendMessageNotification();
	}
	
TInt CNetworkGatewayHandler::RunError(TInt aError)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::RunError()\n");
	LBSLOG_ERR2(ELogP2, " %d \n", aError);
 	return aError;
	}

void CNetworkGatewayHandler::SendOrQueueMessage(TLbsNetInternalMsgBase& aMsg)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::SendOrQueueMessage()\n");
	if (!IsActive())
		{
 		iNGChannel.SendMessage(aMsg, iStatus);
 		SetActive();
		}
	else
		{
		QueueMessage(aMsg);
		}
	}
/**
Queues messages.
Performs implicit cancels if the queue not empty.
Supports only one self location session at a time.
*/	
void CNetworkGatewayHandler::QueueMessage(TLbsNetInternalMsgBase& aMsg)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::QueueMessage()\n");
	TBool removed;
	
	switch (aMsg.Type())
		{
		case TLbsNetInternalMsgBase::ESelfLocationRequest:
			// A new request implicitly cancels a previous request and/or cancel
			iQ.FindAndRemove(TLbsNetInternalMsgBase::ESelfLocationRequest);
			iQ.FindAndRemove(TLbsNetInternalMsgBase::ESelfLocationCancel);
			iQ.Write(aMsg);
			break;
			
		case TLbsNetInternalMsgBase::ESelfLocationCancel:
			// The AGPS manager should not send a Cancel twice
			removed = iQ.FindAndRemove(TLbsNetInternalMsgBase::ESelfLocationCancel);
			__ASSERT_DEBUG(removed == EFalse, User::Panic(KLbsAGPSManFault, KErrAlreadyExists));
			removed = removed == removed; //dummy for UREL
			iQ.Write(aMsg);
			break;
			
		case TLbsNetInternalMsgBase::EAssistanceDataRequest:
			// It is an implicit cancel of a previous Assistance Data Request
			iQ.FindAndRemove(TLbsNetInternalMsgBase::EAssistanceDataRequest);
			iQ.Write(aMsg);
			break;
			
		case TLbsNetInternalMsgBase::ESystemStatusAdvice:
			// It is an implicit cancel of a previous System Status Advice
			iQ.FindAndRemove(TLbsNetInternalMsgBase::ESystemStatusAdvice);
			iQ.Write(aMsg);
			break;
			
		default:
			break;
			
		}
	}

//---------------------------------------------------------------------------------------------------
void CNetworkGatewayHandler::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, 
													  const TLbsNetInternalMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CNetworkGatewayHandler::ProcessNetChannelMessage()\n");
	__ASSERT_DEBUG(aChannelId == RLbsNetChannel::EChannelAGPS2NG, User::Panic(KLbsAGPSManFault, EAGPSManUnexpectedNetChannelId));
	(void) aChannelId;
		
	const TLbsNetSessionCompleteAgpsMsg& msgComp = static_cast<const TLbsNetSessionCompleteAgpsMsg&>(aMessage);
	const TLbsNetAssistanceDataResponseMsg& msgAss = static_cast<const TLbsNetAssistanceDataResponseMsg&>(aMessage);

	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ESessionComplete:
			iObserver->OnSessionComplete(msgComp.Reason(), msgComp.SessionId());
			break;
			
		case TLbsNetInternalMsgBase::EAssistanceDataResponse:
			iObserver->OnAssistanceDataResponse(msgAss.Reason(), msgAss.DataResponseMask());
			break;
			
		default:
			LBSLOG_ERR2(ELogP2, "Unexpected message type: %d \n", aMessage.Type());
			__ASSERT_DEBUG(EFalse, User::Panic(KLbsAGPSManFault, KErrArgument));
			break;
		}
	}
