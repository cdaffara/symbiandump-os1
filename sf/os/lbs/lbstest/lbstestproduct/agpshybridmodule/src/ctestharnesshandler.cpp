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
// Implementation of Test A_Gps module request handler component.
// 
//

#include "ctestharnesshandler.h"

const TInt KBufferedMsgMaxCount = 6;

CT_TestHarnessHandler::CT_TestHarnessHandler(MT_RequestObserver* aObserver) : 
	CActive(EPriorityHigh),
	iObserver(aObserver)
	{
	}
	
CT_TestHarnessHandler::~CT_TestHarnessHandler()
	{
	Cancel();
	iTHChannel.Close();
	iMsgBuffer.ResetAndDestroy();
	}

CT_TestHarnessHandler* CT_TestHarnessHandler::NewL(MT_RequestObserver* aObserver)
	{
	CT_TestHarnessHandler* self = new (ELeave) CT_TestHarnessHandler(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CT_TestHarnessHandler::ConstructL()
	{
    RT_LbsChannel::InitializeL(RT_LbsChannel::EChannelTAGPS2TH);
	iMsgBuffer.ReserveL(KBufferedMsgMaxCount);
	iTHChannel.OpenL(RT_LbsChannel::EChannelTAGPS2TH, *this);
	CActiveScheduler::Add(this);
	}

void CT_TestHarnessHandler::SendDataOutResponse(TT_LbsAGpsResponseMsg::TModuleResponseType aResponse)
	{
 	TT_LbsAGpsResponseMsg* aMsg = new TT_LbsAGpsResponseMsg(aResponse);

 	BufferMessage(aMsg);	
	}
	
void CT_TestHarnessHandler::RunL()
	{
	User::LeaveIfError(iStatus.Int());
		
	if (iMsgBuffer.Count() > 0)
		{	
		SendResponseMessage(iMsgBuffer[0]);
		iMsgBuffer.Remove(0);
		}
	}
	
void CT_TestHarnessHandler::DoCancel()
	{
	iTHChannel.CancelSendMessageNotification();
	}
	
TInt CT_TestHarnessHandler::RunError(TInt aError)
	{
	return aError;
	}

void CT_TestHarnessHandler::SendResponseMessage(const TT_LbsMsgBase* aMessage)
	{
	iTHChannel.SendMessage(*aMessage, iStatus);
	// The msg has now been sent to the channel. We can now delete it.
	delete(aMessage);
	SetActive();
	}

void CT_TestHarnessHandler::BufferMessage(const TT_LbsMsgBase* aMessage)
	{	
	if (!IsActive())
		{
		// The AO is not active, so there should be no buffered messages.
		__ASSERT_DEBUG(iMsgBuffer.Count() == 0, User::Invariant());
		
		// Immediately send the new message.
		SendResponseMessage(aMessage);
		}
	else
		{
		__ASSERT_ALWAYS(iMsgBuffer.Count() < KBufferedMsgMaxCount, User::Invariant());

		// Still waiting for acknowledgement that a previous message
		// was read, so buffer this new message.
		iMsgBuffer.Append(aMessage);
		}
	}

/** Process requests from the test step. Test data is passed to the test module via the input
	data bus.
*/
void CT_TestHarnessHandler::ProcessChannelMessage(RT_LbsChannel::TT_LbsChannelId /*aChannelId*/, 
												 const TT_LbsMsgBase& aMessage)
	{
	TInt err = KErrNone;
	
	switch (aMessage.Type())
		{
		case TT_LbsMsgBase::EModuleRequestUpdateInit:
			{
			const TT_LbsAGpsRequestUpdateInitMsg& msg = static_cast<const TT_LbsAGpsRequestUpdateInitMsg&>(aMessage);
			err = iObserver->ProcessRequestUpdateInit(msg.ConfigFileName(), msg.ConfigSection());
			break;				
			}
		case TT_LbsMsgBase::EModuleRequestTimeOut:
			{
			const TT_LbsAGpsRequestTimeOut& msg = static_cast<const TT_LbsAGpsRequestTimeOut&>(aMessage);
			iObserver->ProcessRequestTimeOut(msg.TimeOut());
			break;				
			}
		case TT_LbsMsgBase::EModuleRequestOptions:
			{
			const TT_LbsAGpsRequestOptions& msg = static_cast<const TT_LbsAGpsRequestOptions&>(aMessage);
			err = iObserver->ProcessRequestModuleOptions(msg.ModuleOption(), msg.ModuleValue());		
			break;				
			}
		case TT_LbsMsgBase::EModuleRequestError:
			{
			const TT_LbsAGpsRequestError& msg = static_cast<const TT_LbsAGpsRequestError&>(aMessage);
			iObserver->ProcessRequestError(msg.Error());
			break;
			}
		case TT_LbsMsgBase::EModuleRequestForcedUpdate:
			{
			iObserver->ProcessRequestForcedUpdate();
			break;
			}
		case TT_LbsMsgBase::EModuleImmediateMeasurements:
			{
			const TT_LbsAGpsImmediateMeasurements& msg = static_cast<const TT_LbsAGpsImmediateMeasurements&>(aMessage);
			iObserver->ProcessImmediateMeasurements(msg.ImmediateMeasurements());
			break;
			}
		default:
			{
			err = KErrNotSupported;									
			break;										
			}
		}		

	// Publish a response to the in coming data, now that we have handled it.
	if (err == KErrNone)
		{
		SendDataOutResponse(TT_LbsAGpsResponseMsg::EModuleResponseOk);
		}
	else
		{
		SendDataOutResponse(TT_LbsAGpsResponseMsg::EModuleErr);
		}

	}

