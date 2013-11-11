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
// Source file for the api for talking to the stub protocol module.
// 
//

#include <e32base.h>
#include <e32property.h>
#include <s32mem.h>
#include <e32debug.h>
#include "lbsnetgatewayprotocoltestapi.h"
#include "lbsdevloggermacros.h"

// Hard-coded Uid of the gateway. This is the process that
// should always create the properties, if it isn't then 
// there is a problem.
const TUid KLbsNetGatewayUid = { 0x10281D46 };

//
// Listener AO
//

NONSHARABLE_CLASS(CNetProtocolMessageListener) : public CActive
	{
public:
	static CNetProtocolMessageListener* NewL(TUid aCategory, 
											 TUint aRxKey, 
											 MNetGatewayProtocolTestObserver& aObserver);
	~CNetProtocolMessageListener();
	
	void RunL();
	void DoCancel();
	
	void NotifyNextMessage();
	
private:
	CNetProtocolMessageListener(MNetGatewayProtocolTestObserver& aObserver);
	void ConstructL(TUid aCategory, TUint aRxKey);
	
private:
	RProperty iRxProperty;
	RProperty iRxAckProperty;
	MNetGatewayProtocolTestObserver& iObserver;
	RBuf8 iAgpsDataBuffer;
	RProperty iAgpsDataSetProperty;
	RLbsAssistanceDataBuilderSet iAgpsDataBuilderSet;
	TNetGatewayMsg	iNetGatMsg;
	};

//

CNetProtocolMessageListener::CNetProtocolMessageListener(MNetGatewayProtocolTestObserver& aObserver) :
	CActive(EPriorityNormal),
	iObserver(aObserver)
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::CNetProtocolMessageListener()");
	CActiveScheduler::Add(this);
	}

CNetProtocolMessageListener::~CNetProtocolMessageListener()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::~CNetProtocolMessageListener()");
	Cancel();
	iAgpsDataSetProperty.Close();
	iAgpsDataBuilderSet.Close();
	iAgpsDataBuffer.Close();
	iRxAckProperty.Close();
	iRxProperty.Close();
	}
	
CNetProtocolMessageListener* CNetProtocolMessageListener::NewL(TUid aCategory, 
										 					   TUint aRxKey, 
										 					   MNetGatewayProtocolTestObserver& aObserver)
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::NewL()");
	CNetProtocolMessageListener* self = new (ELeave) CNetProtocolMessageListener(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aCategory, aRxKey);
	CleanupStack::Pop();
	return self;
	}

void CNetProtocolMessageListener::ConstructL(TUid aCategory, TUint aRxKey)
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::ConstructL()");
	__ASSERT_DEBUG(iRxProperty.Handle() == NULL, User::Invariant());
	
	User::LeaveIfError(iRxProperty.Attach(aCategory, aRxKey));
	User::LeaveIfError(iRxAckProperty.Attach(aCategory, (aRxKey + KChannelAckOffset)));
	
	// Allocate a buffer to store the Externalize'd assistance data set data.
	iAgpsDataBuffer.CreateL(RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize());	
	iAgpsDataBuilderSet.OpenL();
	User::LeaveIfError(iAgpsDataSetProperty.Attach(KLbsNetGatewayUid, KAgpsDataSetKey));
	}

void CNetProtocolMessageListener::RunL()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::RunL()");
		
	
	User::LeaveIfError(iStatus.Int());
	
	NotifyNextMessage();
	TPckg<TNetGatewayMsg> pckg(iNetGatMsg);
	User::LeaveIfError(iRxProperty.Get(pckg));
	
	// if the message contains assistance data then 
	// we need to read it out of the separate property
	// and Internalize it into a local buffer.
	if (iNetGatMsg.iType == ENetMsgProcessAssistanceData)
		{
		User::LeaveIfError(iAgpsDataSetProperty.Get(iAgpsDataBuffer));
		RDesReadStream rstream(iAgpsDataBuffer);
		CleanupClosePushL(rstream);
		iAgpsDataBuilderSet.InternalizeL(rstream);
		CleanupStack::PopAndDestroy();
		SNetMsgProcessAssistanceData* data = reinterpret_cast<SNetMsgProcessAssistanceData*>(iNetGatMsg.Data());
		// Note: there is no operator = for RLbsAssistanceDataBuilderSet, so 
		// we force a byte-wise copy of the buffered one into the message
		Mem::Copy(&data->iData, &iAgpsDataBuilderSet, sizeof(RLbsAssistanceDataBuilderSet));
		}
	
	User::LeaveIfError(iRxAckProperty.Set(KErrNone));
	iObserver.ProcessNetProtocolMessage(iNetGatMsg);
	}
	
void CNetProtocolMessageListener::DoCancel()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::DoCancel()");
	iRxProperty.Cancel();
	}
	
void CNetProtocolMessageListener::NotifyNextMessage()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageListener::NotifyNextMessage()");
	iRxProperty.Subscribe(iStatus);
	SetActive();	
	}

//
// Transmitter AO
//

NONSHARABLE_CLASS(CNetProtocolMessageTransmitter) : public CActive
	{
public:
	static CNetProtocolMessageTransmitter* NewL(TUid aCategory, 
												TUint aTxKey);
	~CNetProtocolMessageTransmitter();
	
	void RunL();
	void DoCancel();
	
	void NotifyMessageRead();
	
	void SendNetProtocolMessageL(const TNetGatewayMsg& aMessage);
	
private:
	CNetProtocolMessageTransmitter();
	void ConstructL(TUid aCategory, TUint aTxKey);
	
private:
	RProperty iTxProperty;
	RProperty iTxAckProperty;
	RBuf8 iAgpsDataBuffer;
	RProperty iAgpsDataSetProperty;
	TNetGatewayMsg iNetGatMsg;
	CActiveSchedulerWait iActiveWait;
	};
	
//

CNetProtocolMessageTransmitter::CNetProtocolMessageTransmitter() :
	CActive(EPriorityHigh)
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::CNetProtocolMessageTransmitter()");
	CActiveScheduler::Add(this);
	}

CNetProtocolMessageTransmitter::~CNetProtocolMessageTransmitter()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::~CNetProtocolMessageTransmitter()");
	Cancel();
	iAgpsDataSetProperty.Close();
	iAgpsDataBuffer.Close();
	iTxAckProperty.Close();
	iTxProperty.Close();
	}
	
CNetProtocolMessageTransmitter* CNetProtocolMessageTransmitter::NewL(TUid aCategory, 
										 						     TUint aTxKey)
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::NewL()");
	CNetProtocolMessageTransmitter* self = new (ELeave) CNetProtocolMessageTransmitter();
	CleanupStack::PushL(self);
	self->ConstructL(aCategory, aTxKey);
	CleanupStack::Pop();
	return self;
	}

void CNetProtocolMessageTransmitter::ConstructL(TUid aCategory, TUint aTxKey)
	{
	LBSLOG2(ELogP1, "CNetProtocolMessageTransmitter::ConstructL(0x%x)", aTxKey);
	__ASSERT_DEBUG(iTxProperty.Handle() == NULL, User::Invariant());
	
	User::LeaveIfError(iTxProperty.Attach(aCategory, aTxKey));
	User::LeaveIfError(iTxAckProperty.Attach(aCategory, (aTxKey + KChannelAckOffset)));
	
	// Allocate a buffer to store the Externalize'd assistance data set data.
	iAgpsDataBuffer.CreateL(RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize());	
	User::LeaveIfError(iAgpsDataSetProperty.Attach(KLbsNetGatewayUid, KAgpsDataSetKey));
	}

void CNetProtocolMessageTransmitter::RunL()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::RunL()");
	
	User::LeaveIfError	(iStatus.Int());
	if (iActiveWait.IsStarted())
		{
		iActiveWait.AsyncStop();
		}
	}
	
void CNetProtocolMessageTransmitter::DoCancel()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::DoCancel()");
	iTxAckProperty.Cancel();
	}
	
void CNetProtocolMessageTransmitter::NotifyMessageRead()
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::NotifyMessageRead()");
	iTxAckProperty.Subscribe(iStatus);
	SetActive();	
	}

void CNetProtocolMessageTransmitter::SendNetProtocolMessageL(const TNetGatewayMsg& aMessage)
	{
	LBSLOG(ELogP1, "CNetProtocolMessageTransmitter::SendNetProtocolMessageL()");
	__ASSERT_DEBUG(EFalse == IsActive(), User::Invariant());

	// If the message to be sent includes assistance data, that data
	// needs to be sent separately to the actual message. It is then 
	// put back together again when received.
	if (aMessage.iType == ENetMsgProcessAssistanceData)
		{
		RDesWriteStream wstream(iAgpsDataBuffer);
		CleanupClosePushL(wstream);
		iNetGatMsg = aMessage;
		SNetMsgProcessAssistanceData* data = reinterpret_cast<SNetMsgProcessAssistanceData*>(iNetGatMsg.Data());
		data->iData.ExternalizeL(wstream);
		CleanupStack::PopAndDestroy();
		User::LeaveIfError(iAgpsDataSetProperty.Set(iAgpsDataBuffer));
		}
	
	NotifyMessageRead();
	TPckg<TNetGatewayMsg> pckg(aMessage);
	TInt err = iTxProperty.Set(pckg);
	if(err!=KErrNone)
		{
		LBSLOG2(ELogP1, "err = 0x%x", err);
		Cancel();
		User::Leave(err);
		}
	iActiveWait.Start();
	}
	
//
// Channel Interface
//


EXPORT_C RNetGatewayProtocolTestChannel::RNetGatewayProtocolTestChannel(TUint aModuleIndex) :
iModuleIndex(aModuleIndex)
	{
	LBSLOG2(ELogP1, "RNetGatewayProtocolTestChannel::RNetGatewayProtocolTestChannel(0x%x)", aModuleIndex);
	}

EXPORT_C RNetGatewayProtocolTestChannel::RNetGatewayProtocolTestChannel() :
iModuleIndex(0)
	{
	LBSLOG(ELogP1, "RNetGatewayProtocolTestChannel::RNetGatewayProtocolTestChannel()");
	}

EXPORT_C void RNetGatewayProtocolTestChannel::InitialiseL(TUint aModuleIndex)
	{
	LBSLOG2(ELogP1, "RNetGatewayProtocolTestChannel::InitialiseL(0x%x)", aModuleIndex);
	__ASSERT_DEBUG(KLbsNetGatewayUid  == RProcess().SecureId(), User::Invariant());
	
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);

	TInt err = RProperty::Define(KNetProtocolChannelKey + aModuleIndex, 
								 RProperty::ELargeByteArray, 
								 readPolicy,
								 writePolicy,
								 sizeof(TNetGatewayMsg));
	if (KErrAlreadyExists != err)
		{
		User::LeaveIfError(err);
		}

	err = RProperty::Define(KNetProtocolChannelAckKey + aModuleIndex, 
							 RProperty::EInt, 
							 readPolicy,
							 writePolicy);
	if (KErrAlreadyExists != err)
		{
		User::LeaveIfError(err);
		}
						 
	err = RProperty::Define(KTestAppChannelKey + aModuleIndex, 
							RProperty::ELargeByteArray, 
							readPolicy,
							writePolicy,
							sizeof(TNetGatewayMsg));
	if (KErrAlreadyExists != err)
		{
		User::LeaveIfError(err);
		}
		
	err = RProperty::Define(KTestAppChannelAckKey + aModuleIndex, 
							 RProperty::EInt, 
							 readPolicy,
							 writePolicy);
	if (KErrAlreadyExists != err)
		{
		User::LeaveIfError(err);
		}

	err = RProperty::Define(KAgpsDataSetKey + aModuleIndex, 
							 RProperty::ELargeByteArray, 
							 readPolicy,
							 writePolicy,
							 RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize());
	if (KErrAlreadyExists != err)
		{
		User::LeaveIfError(err);
		}
	}
	
EXPORT_C void RNetGatewayProtocolTestChannel::Shutdown(TUint aModuleIndex)
	{
	LBSLOG(ELogP1, "RNetGatewayProtocolTestChannel::Shutdown()");
	RProperty::Delete(KNetProtocolChannelKey + aModuleIndex);
	RProperty::Delete(KNetProtocolChannelAckKey + aModuleIndex);
	RProperty::Delete(KTestAppChannelKey + aModuleIndex);
	RProperty::Delete(KTestAppChannelAckKey + aModuleIndex);
	RProperty::Delete(KTestAppChannelKey + aModuleIndex);
	}

EXPORT_C void RNetGatewayProtocolTestChannel::OpenL(TUint aTxKey, 
												    TUint aRxKey, 
												    MNetGatewayProtocolTestObserver& aRxObserver)
	{	
	LBSLOG(ELogP1, "RNetGatewayProtocolTestChannel::OpenL()");
	iTransmitter = CNetProtocolMessageTransmitter::NewL(KLbsNetGatewayUid, aTxKey + iModuleIndex);
	
	iListener = CNetProtocolMessageListener::NewL(KLbsNetGatewayUid, aRxKey + iModuleIndex, aRxObserver);
	iListener->NotifyNextMessage();
	}

EXPORT_C void RNetGatewayProtocolTestChannel::OpenL(TUint aTxKey, 
												    TUint aRxKey, 
												    MNetGatewayProtocolTestObserver& aRxObserver,
												    TUint aModuleIndex)
	{	
	LBSLOG(ELogP1, "RNetGatewayProtocolTestChannel::OpenL()");

	iModuleIndex = aModuleIndex;
	OpenL(aTxKey, aRxKey, aRxObserver);
	}

EXPORT_C void RNetGatewayProtocolTestChannel::Close()
	{
	LBSLOG(ELogP1, "RNetGatewayProtocolTestChannel::Close()");

	delete iTransmitter;
	delete iListener;

	}

EXPORT_C void RNetGatewayProtocolTestChannel::SendNetProtocolMessageL(const TNetGatewayMsg& aMessage)
	{
	LBSLOG(ELogP1, "RNetGatewayProtocolTestChannel::SendNetProtocolMessageL()");
	iTransmitter->SendNetProtocolMessageL(aMessage);
	}
