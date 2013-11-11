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
// Implementation of Test Harness request handler component.
// 
//


#include <e32base.h>
#include "ctlbsagpshandler.h"

const TInt KBufferedMsgMaxCount = 6;

CT_LbsAGpsHandler::CT_LbsAGpsHandler(MT_ResponseObserver* aObserver) : 
	CActive(EPriorityStandard),
	iObserver(aObserver)
	{
	}
	
CT_LbsAGpsHandler::~CT_LbsAGpsHandler()
	{
	Cancel();
	iTHChannel.Close();
	iMsgBuffer.ResetAndDestroy();
	}

EXPORT_C CT_LbsAGpsHandler* CT_LbsAGpsHandler::NewL(MT_ResponseObserver* aObserver)
	{
	CT_LbsAGpsHandler* self = new (ELeave) CT_LbsAGpsHandler(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CT_LbsAGpsHandler::ConstructL()
	{
    RT_LbsChannel::InitializeL(RT_LbsChannel::EChannelTH2TAGPS);
	iMsgBuffer.ReserveL(KBufferedMsgMaxCount);
	iTHChannel.OpenL(RT_LbsChannel::EChannelTH2TAGPS, *this);
	CActiveScheduler::Add(this);
	}

EXPORT_C void CT_LbsAGpsHandler::SendRequestUpdateInitMsg(const TDesC& aConfigFileName, 
												 		  const TDesC& aConfigSection)
	{
  	TT_LbsAGpsRequestUpdateInitMsg* aMsg = new TT_LbsAGpsRequestUpdateInitMsg(aConfigFileName, aConfigSection);
 	 	
 	BufferMessage(aMsg);		
	}

EXPORT_C void CT_LbsAGpsHandler::SendRequestTimeOutMsg(const TTimeIntervalMicroSeconds& aTimeOut) 
	{
 	TT_LbsAGpsRequestTimeOut* aMsg = new TT_LbsAGpsRequestTimeOut(aTimeOut);
 	 	
 	BufferMessage(aMsg);		
	}

EXPORT_C void CT_LbsAGpsHandler::SendImmediateMeasurementsMsg(const TInt& aImmediateMeasurements) 
	{
	TT_LbsAGpsImmediateMeasurements* aMsg = new TT_LbsAGpsImmediateMeasurements(aImmediateMeasurements);
 	 	
 	BufferMessage(aMsg);		
	}


EXPORT_C void CT_LbsAGpsHandler::SendRequestSetAllModuleOptions()
	{
 	TT_LbsAGpsRequestOptions* aMsg = new TT_LbsAGpsRequestOptions(ELbsHybridModuleOptions_SetAll);
	 	
 	BufferMessage(aMsg);		
	}	

EXPORT_C void CT_LbsAGpsHandler::SendRequestClearAllModuleOptions()
	{
 	TT_LbsAGpsRequestOptions* aMsg = new TT_LbsAGpsRequestOptions(EHybridModuleOptions_ClearAll);
	 	
 	BufferMessage(aMsg);		
	}
	
EXPORT_C void CT_LbsAGpsHandler::SendRequestModuleOption(TLbsHybridModuleOptions aOption, TBool aValue)
	{
 	TT_LbsAGpsRequestOptions* aMsg = new TT_LbsAGpsRequestOptions(aOption, aValue);
 	 	
 	BufferMessage(aMsg);	
	}

EXPORT_C void CT_LbsAGpsHandler::SendRequestForcedUpdate()
	{
	TT_LbsAGpsRequestForcedUpdate* msg = new TT_LbsAGpsRequestForcedUpdate();
 	 	
 	BufferMessage(msg);
	}
	
EXPORT_C void CT_LbsAGpsHandler::SendRequestError(TInt aError)
	{
 	TT_LbsAGpsRequestError* aMsg = new TT_LbsAGpsRequestError(aError);
 	 	
 	BufferMessage(aMsg);		
	}


void CT_LbsAGpsHandler::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	if (iMsgBuffer.Count() > 0)
		{	
		SendRequestMessage(iMsgBuffer[0]);
		iMsgBuffer.Remove(0);
		}
	}

void CT_LbsAGpsHandler::DoCancel()
	{
	iTHChannel.CancelSendMessageNotification();
	}
	
TInt CT_LbsAGpsHandler::RunError(TInt aError)
	{
	return aError;
	}

void CT_LbsAGpsHandler::SendRequestMessage(const TT_LbsMsgBase* aMessage)
	{
	iTHChannel.SendMessage(*aMessage, iStatus);
	// The msg has now been sent to the channel. We can now delete it.
	delete(aMessage);
	SetActive();
	}
	
void CT_LbsAGpsHandler::BufferMessage(const TT_LbsMsgBase* aMessage)
	{	
	if (!IsActive())
		{
		// The AO is not active, so there should be no buffered messages.
		__ASSERT_DEBUG(iMsgBuffer.Count() == 0, User::Invariant());
		
		// Immediately send the new message.
		SendRequestMessage(aMessage);
		}
	else
		{
		__ASSERT_ALWAYS(iMsgBuffer.Count() < KBufferedMsgMaxCount, User::Invariant());

		// Still waiting for acknowledgement that a previous message
		// was read, so buffer this new message.
		iMsgBuffer.Append(aMessage);
		}
	}				

void CT_LbsAGpsHandler::ProcessChannelMessage(RT_LbsChannel::TT_LbsChannelId /*aChannelId*/, 
												 const TT_LbsMsgBase& aMessage)
	{

	switch (aMessage.Type())
		{
		case TT_LbsMsgBase::EModuleResponse:
			{
			const TT_LbsAGpsResponseMsg& msg = static_cast<const TT_LbsAGpsResponseMsg&>(aMessage);
			iObserver->ProcessAGpsResponseMessage(msg.ResponseType());
			break;
			}
		default:
			//LBSLOG_ERR2(ELogP2, "Unexpected message type: %d \n", aMessage.Type());
			break;
		}
	}

