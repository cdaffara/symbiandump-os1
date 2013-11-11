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

/**
 @file lbsnrhngchannel.cpp
*/

#include <test/testexecutelogger.h>
#include <test/testexecutelogext.h>
#include "lbsnrhngchannel.h"


CNgChannel::CNgChannel(CTestExecuteLogger& aLogger) :
	CActive(EPriorityHigh),
	iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}
	
CNgChannel::~CNgChannel()
	{
	Cancel();
	iFinalPosUpdates.Close();
	iRefPosUpdates.Close();
	iNrhChannel.Close();
	}
	
CNgChannel* CNgChannel::NewL(CTestExecuteLogger& aLogger)
	{
	CNgChannel* self = new (ELeave) CNgChannel(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CNgChannel::ConstructL()
	{
	iNrhChannel.OpenL(RLbsNetChannel::EChannelNG2NRH, *this);
	iRefPosUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	iFinalPosUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);
	}
		
void CNgChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if (iSendWait.IsStarted())
		{
		iSendWait.AsyncStop();
		}
	}
	
void CNgChannel::DoCancel()
	{
	iNrhChannel.CancelSendMessageNotification();
	}

void CNgChannel::SendMessageAndWait(
		const TLbsNetInternalMsgBase& aMessage,
		TTimeIntervalSeconds /*aTimeout*/)
	{
	LogOutgoingMessage(aMessage);
	
	// The reference and final network positions
	// are sent over the NG channel

		{
		iNrhChannel.SendMessage(aMessage, iStatus);
		SetActive();
		iSendWait.Start();
		}
	}

void CNgChannel::WaitForResponseL(
		TTimeIntervalSeconds /*aTimeout*/)
	{
	if (iResponseBuffer.Count() == 0)
		{
		// Start the active scheduler - this should be stopped when
		// ProcessNetChannelMessage is called
		iReceiveWait.Start();
		}
	}

TLbsNetInternalMsgBase::TLbsNetInternalMsgType CNgChannel::GetNextResponseType()
	{
	if (iResponseBuffer.Count() == 0)
		{
		return TLbsNetInternalMsgBase::EUnknown;
		}
	else
		{
		TLbsNetInternalMsgBase::TLbsNetInternalMsgType type = iResponseBuffer[0]->Type();
		return (type);
		}
	}

void CNgChannel::GetNextResponseL(
		TLbsNetInternalMsgBase& aMessage)
	{
	if (iResponseBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (aMessage.Type() != iResponseBuffer[0]->Type()
		&& aMessage.Size() == iResponseBuffer[0]->Size())
		{
		User::Leave(KErrArgument);
		}
	
	// Message type should match what is in the buffer,
	// so copy the data then delete the message in the buffer.
	TLbsNetInternalMsgBase* msg = iResponseBuffer[0];
	iResponseBuffer.Remove(0);
	Mem::Copy(&aMessage,
			  msg,
			  aMessage.Size());
	delete msg;
	}

void CNgChannel::ProcessNetChannelMessage(
		RLbsNetChannel::TLbsNetChannelId aChannelId, 
		const TLbsNetInternalMsgBase& aMessage)
	{
	__ASSERT_ALWAYS(aChannelId == RLbsNetChannel::EChannelNG2NRH,
					User::Invariant());

	LogIncomingMessage(aMessage);
	
	// Store the message so that it can be read later
	TRAP_IGNORE(BufferMessageL(aMessage));
	
	// Jump out of the AS loop
	if (iReceiveWait.IsStarted())
		{
		iReceiveWait.AsyncStop();
		}
	}

void CNgChannel::BufferMessageL(
		const TLbsNetInternalMsgBase& aMessage)
	{
	TAny* msg = User::AllocLC(aMessage.Size());
	Mem::Copy(msg, &aMessage, aMessage.Size());
	iResponseBuffer.AppendL(reinterpret_cast<TLbsNetInternalMsgBase*>(msg));
	CleanupStack::Pop(msg);
	}

void CNgChannel::LogIncomingMessage(
		const TLbsNetInternalMsgBase& aMessage)
	{
	//INFO_PRINTF3(_L("-> SessionId : 0x%x, %d"), aMessage.SessionId().SessionOwner(), 
	//											aMessage.SessionId().SessionNum());
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Location Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ELocationResponse:
			{
			INFO_PRINTF1(_L(">>>> NG Channel: Received Location Reponse."));
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationRequest:
			{
			INFO_PRINTF1(_L(">>>> NG Channel: Received Transmit Location Request."));
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationCancel:
			{
			INFO_PRINTF1(_L(">>>> NG Channel: Received Transmit Location Cancel."));
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyRequest:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Privacy Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyResponse:
			{
			INFO_PRINTF1(_L(">>>> NG Channel: Received Privacy Response."));
			break;
			}
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Session Complete!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ESelfLocationRequest:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Self Locate Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ESelfLocationCancel:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Self Locate Cancel!!"));
			break;
			}
		case TLbsNetInternalMsgBase::EAssistanceDataRequest:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Assistance Data Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::EAssistanceDataResponse:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Assistance Data Response!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ESystemStatusAdvice:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received System Status Advice!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ENetworkLocationUpdate:
			{
			ERR_PRINTF1(_L(">>>> NG Channel: Received Network Location Update!!"));
			break;
			}
		default:
			{
			break;
			}
		}
	}

void CNgChannel::LogOutgoingMessage(const TLbsNetInternalMsgBase& aMessage)
	{
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::ELocationRequest:
			{
			INFO_PRINTF1(_L("<<<< NG Channel: Sending Location Request..."));
			break;
			}
		case TLbsNetInternalMsgBase::ELocationResponse:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Location Reponse!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationRequest:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Transmit Location Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ETransmitLocationCancel:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Transmit Location Cancel!!"));
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyRequest:
			{
			INFO_PRINTF1(_L("<<<< NG Channel: Sending Privacy Request..."));
			break;
			}
		case TLbsNetInternalMsgBase::EPrivacyResponse:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Privacy Response!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ESessionComplete:
			{
			INFO_PRINTF1(_L("<<<< NG Channel: Sending Session Complete..."));
			break;
			}
		case TLbsNetInternalMsgBase::ESelfLocationRequest:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Self Locate Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ESelfLocationCancel:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Self Locate Cancel!!"));
			break;
			}
		case TLbsNetInternalMsgBase::EAssistanceDataRequest:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Assistance Data Request!!"));
			break;
			}
		case TLbsNetInternalMsgBase::EAssistanceDataResponse:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending Assistance Data Response!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ESystemStatusAdvice:
			{
			ERR_PRINTF1(_L("<<<< NG Channel: Sending System Status Advice!!"));
			break;
			}
		case TLbsNetInternalMsgBase::ENetworkLocationUpdate:
			{
			INFO_PRINTF1(_L("<<<< NG Channel: Sending Network Location Update..."));
			break;
			}
		default:
			{
			break;
			}
		}	
	}
