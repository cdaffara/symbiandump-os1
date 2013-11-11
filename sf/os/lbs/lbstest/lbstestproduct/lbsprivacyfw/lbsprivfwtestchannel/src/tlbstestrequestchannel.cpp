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
// Definition of network request channel component.
// 
//


#include <e32base.h>
#include <e32debug.h>
#include <lbs/lbslocerrors.h>
#include "lbsdevloggermacros.h"
#include <lbs/test/tlbstestrequestchannel.h>

//
// CTestRequestChannel
//

/* The maximum number of messages that may be buffered at any one time.
*/
const TInt KPrivFwQSize = 20;

CTestRequestChannel::CTestRequestChannel(MLbsTestChannelRequestObserver* aObserver) : 
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iQ(KPrivFwQSize)
	{
	}
	
CTestRequestChannel::~CTestRequestChannel()
	{
	Cancel();
	iPrivFwRequestChannel.Close();
	iQ.Close();
	}

EXPORT_C CTestRequestChannel* CTestRequestChannel::NewL(MLbsTestChannelRequestObserver* aObserver, RLbsTestChannel::TLbsTestChannelId aChannelId)
	{
	CTestRequestChannel* self = new (ELeave) CTestRequestChannel(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aChannelId);
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestRequestChannel::ConstructL(RLbsTestChannel::TLbsTestChannelId aChannelId)
	{
	iPrivFwRequestChannel.OpenL(aChannelId, *this);
	CActiveScheduler::Add(this);
	iQ.OpenL();
	}
	
void CTestRequestChannel::RunL()
	{
	LBSLOG2(ELogP1, "CTestRequestChannel::RunL() Begin. Status: %d\n", iStatus.Int());
	__ASSERT_DEBUG(iStatus.Int() == KErrNone, User::Panic(KTLbsPrivFwTestChannelPanic, iStatus.Int()));

	User::LeaveIfError(iStatus.Int());
	
	// Check if more queued messages await sending
	if (!iQ.IsEmpty())
		{
 		iPrivFwRequestChannel.SendMessage(iQ.Read(), iStatus);
 		SetActive();
		}
	LBSLOG(ELogP1, "CTestRequestChannel::RunL() End\n");

	}
	
void CTestRequestChannel::DoCancel()
	{
	iPrivFwRequestChannel.CancelSendMessageNotification();

    if (iStatus == KRequestPending) 
    	{
    	TRequestStatus* status = &iStatus;
 		User::RequestComplete(status,KErrCancel);    	
    	}
	}
	
TInt CTestRequestChannel::RunError(TInt aError)
	{
	return aError;
	}

EXPORT_C void CTestRequestChannel::SendPrivFwRequestMessage(const TLbsTestChannelMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CTestRequestChannel::SendNetRequestMessage:");
	
	if (!IsActive())
		{
		iPrivFwRequestChannel.SendMessage(aMessage, iStatus);
		SetActive();
		}
	else
		{
		// Still waiting for acknowledgement that a previous message
		// was read, so buffer this new message.
		BufferMessage(aMessage);
		}
	}
	
EXPORT_C void CTestRequestChannel::ProcessTestChannelMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
	iObserver->ProcessTestChannelRequestMessage(aChannelId, aMessage);
	}

	
void CTestRequestChannel::BufferMessage(const TLbsTestChannelMsgBase& aMessage)
	{
	LBSLOG(ELogP1, "CTestRequestChannel::BufferMessage()\n");
	iQ.Write(aMessage);
	}

