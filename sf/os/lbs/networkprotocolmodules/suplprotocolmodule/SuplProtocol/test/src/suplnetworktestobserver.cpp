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
// csuplnetworkobserver.cpp
// Protocol Module network observer class implementation
// 
//

#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include "lbstimer.h"
#include "suplnetworktestobserver.h"
#include "rrlpmeasureposresponse.h"
#include "suplpos.h"

const TInt KNetworkTimerId = 0x20202022;
const TInt KNetworkTimerDurationInMicroSec = 1000000;

/** Static constructor
*/
CSuplNetworkTestObserver* CSuplNetworkTestObserver::NewL()
	{
	CSuplNetworkTestObserver* self = new (ELeave) CSuplNetworkTestObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Constructor
*/
CSuplNetworkTestObserver::CSuplNetworkTestObserver()
	{

	}


/** Destructor
*/
CSuplNetworkTestObserver::~CSuplNetworkTestObserver()
	{
	delete iTimer;
	delete iScheduler;
	delete iReceivedMessage;
	// Some tests don't use the source code that instantiates the 
	// Connection Manager, so this class creates it. Delete it from
	// here using the static destructor in case the production code
	// has destroyed it first.
	iConnectionManager->Destroy();
	iConnectionManager = NULL;
	}


/** Second phase Constructor
*/
void CSuplNetworkTestObserver::ConstructL()
	{
	iScheduler = (CActiveSchedulerWait*) new CActiveSchedulerWait;
	iTimer = CLbsCallbackTimer::NewL(*this);
	iConnectionManager = CSuplConnectionManager::NewL(); // This should return the existing single instance.
	iConnectionManager->SetTestObserver(this);
	}


/** Reset the observer's called status
*/
void CSuplNetworkTestObserver::ResetWasObserverCalled()
	{
	delete iReceivedMessage;
	iReceivedMessage = NULL;
	iWasObserverCalled = EFalse;
	iCallBackResult = ENone;
	}


/** Wait for an observer callback
*/
void CSuplNetworkTestObserver::WaitForCallBack()
	{
	if (!iWasObserverCalled)
		{
		iScheduler->Start();
		}
	}


/** Signal that observer has received a callback
@param aResult Enumeration showing which observer method was called.
*/
void CSuplNetworkTestObserver::SignalObserverCallBack(const TCallBackResult aResult)
	{
	iTimer->Cancel();
	iWasObserverCalled = ETrue;
	iCallBackResult = aResult;
	if (iScheduler->IsStarted())
		{
		iScheduler->AsyncStop();
		}
	}


/** Check if the observer was called
*/
TBool CSuplNetworkTestObserver::WasObserverCalled() const
	{
	return iWasObserverCalled;
	}


/** Retrieve callback result
*/
TInt CSuplNetworkTestObserver::CallBackResult() const
	{
	return iCallBackResult;
	}


/** Mobile location position
*/
const TPosition& CSuplNetworkTestObserver::MobilePos() const
	{
	return iMobilePos;
	}
	
TSuplLocalSessionId CSuplNetworkTestObserver::SessionId() const
	{
	return iSessionId;		
	}

CSuplMessageBase* CSuplNetworkTestObserver::ReceivedMessage()
	{ 
	return iReceivedMessage;	
	}

TLbsAssistanceDataGroup CSuplNetworkTestObserver::AssistDataReq()
{
	return iRxAssistData;
}

CSuplPosPayload::TSuplPosPayloadType CSuplNetworkTestObserver::RrlpMessageType()
{
	return iRrlpMessageType;
}

void CSuplNetworkTestObserver::ConnectRequestReceived(const TSuplLocalSessionId& aSessionId, const CSuplSessionRecord::TServiceType& aServiceType, const TLbsHostSettingsId& aSettingsId)
	{
	iSessionId = aSessionId;
	iSettingsId = aSettingsId;
	(void)aServiceType;
	SignalObserverCallBack(EConnectReq);	
	}

void CSuplNetworkTestObserver::DisconnectRequestReceived(const TSuplLocalSessionId& aSessionId)
	{
	(void)aSessionId;// Can be ignored when testing a single location request
	SignalObserverCallBack(EDisconnectReq);	
	}

void CSuplNetworkTestObserver::SendMessageRequestReceived(const CSuplMessageBase* aMessage, const TSuplLocalSessionId& aSessionId)
	{
	(void)aSessionId;
	TCallBackResult result = ENone;
	
 	if (aMessage != NULL)
	 	{
		iReceivedMessage = const_cast <CSuplMessageBase*> (aMessage);
		// Identify the message received
		switch(iReceivedMessage->MessageType())
			{
			case CSuplMessageBase::ESuplStart:
				result = ESuplStartSendReq;
				break;
			
			case CSuplMessageBase::ESuplPosInit:
				result = ESendSuplPosInitSendReq;
				break;	

			case CSuplMessageBase::ESuplPos:
				{
				result = ESendSuplPosSendReq;
								
				CSuplPos* posMessage = static_cast<CSuplPos*>(iReceivedMessage);
				CSuplPosPayload* posPayload = posMessage->PosPayload();
				if (posPayload)
					{
					TInt rrplMessage = posPayload->MessageType();
					switch (rrplMessage)
						{
						case CSuplPosPayload::ERrlpAssistanceDataAck:
							{
							//CRrlpAssistanceData* pAssistData = static_cast<CRrlpAssistanceData*>(posPayload);
							iRrlpMessageType = CSuplPosPayload::ERrlpAssistanceDataAck;
							break;
							}
						case CSuplPosPayload::ERrlpMeasurePositionResp:
							{
							CRrlpMeasurePositionResponse* pResp = static_cast<CRrlpMeasurePositionResponse*>(posPayload);
							iRrlpMessageType = CSuplPosPayload::ERrlpMeasurePositionResp;
							
							// Extract assistance data request mask if present
							if (ERrlpLocErrorGpsAssDataMissing == pResp->GetLocationError())
								{
								iRxAssistData = pResp->GetDataRequestMask();
								}
							else
								{
								iRxAssistData = 0;
								}		
							
							break;
							}
						case CSuplPosPayload::ERrlpProtocolError:
							{
							//CRrlpProtocolError* pErr = static_cast<CRrlpProtocolError*>(posPayload);
							iRrlpMessageType = CSuplPosPayload::ERrlpProtocolError;
							break;
							}
						default:
							{
							iRrlpMessageType = CSuplPosPayload::EPosPayloadMessageTypeLimit;
							}
						} // switch
					}
					delete posPayload;
				}
				break;
				
			case CSuplMessageBase::ESuplEnd:
				result = ESendSuplEndSendReq;
				break;

			default:
				result = ENone;	
				break;
			}
	 	}
	 
	SignalObserverCallBack(result);	
	}

/** Send to Connection Manager an instruction to communicate to the state
machine identified by aSessionId that the connection has been stablished
*/
void CSuplNetworkTestObserver::InjectConnectedIndication(const TSuplLocalSessionId& aSessionId)
	{
	iConnectionManager->InjectConnectedInd(aSessionId);
	}

/** Send to Connection Manager an instruction to communicate to the state
machine identified by aSessionId that there has been an error in the connection
*/
void CSuplNetworkTestObserver::InjectConnectionErrorIndication(const TSuplLocalSessionId& aSessionId, const MSuplConnectionManagerObserver::TSuplConnectionError& aError)
	{
	iConnectionManager->InjectConnectionErrorInd(aSessionId, aError);
	}

/** Send to Connection Manager an instruction to communicate to the state
machine identified by aSessionId that it has received a message from the network
*/
void CSuplNetworkTestObserver::InjectSuplMessage(const TSuplLocalSessionId& aSessionId, CSuplMessageBase* aSuplMessage)
	{
	iConnectionManager->InjectMessageReceivedInd(aSessionId, aSuplMessage);
	}

/** Handle a timeout
*/
void CSuplNetworkTestObserver::OnTimerEventL(TInt aTimerId)
	{
	if (KNetworkTimerId == aTimerId)
		{
		SignalObserverCallBack(ETimeout);
		}	
	}

/** Handle a timer error
*/
TInt CSuplNetworkTestObserver::OnTimerError(TInt aTimerId, TInt aError)
{
	if (KNetworkTimerId == aTimerId)
		{
		SignalObserverCallBack(ETimerError);
		}
		
	return aError;
}

/** Check if more observer activity takes place
*/
TBool CSuplNetworkTestObserver::IsMoreObserverActivity()
	{
	TBool ret;
	ResetWasObserverCalled();
	iTimer->EventAfter(TTimeIntervalMicroSeconds32(KNetworkTimerDurationInMicroSec), KNetworkTimerId);
	WaitForCallBack();
	ret = ETimeout == CallBackResult()? EFalse : ETrue;
	ResetWasObserverCalled();
	return ret;
	}


/** Retrieve host settings Id that should be used for
the session
*/
TLbsHostSettingsId CSuplNetworkTestObserver::HostSettingsId()
{
	return iSettingsId;
}
