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
// Gateway channel class implementation
// 
//


#include "gatewaychannel.h"


/**
 * Constructor
 */
CGatewayChannel::CGatewayChannel() :
	CActive(EPriorityHigh)
	{
	CActiveScheduler::Add(this);
	}
/**
 * Destructor
 */	
CGatewayChannel::~CGatewayChannel()
	{
	Cancel();
	iNetGatewayChannel.Close();
	}
	
/**
 * Standard Symbian OS two-phase constructor.
 * @return A new instance of this class.
 */
CGatewayChannel* CGatewayChannel::NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId)
	{
	CGatewayChannel* self = new (ELeave) CGatewayChannel();
	CleanupStack::PushL(self);
	self->ConstructL(aObserver, aChannelId);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
 * Standard ConstructL
 */
void CGatewayChannel::ConstructL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId)
	{
	iNetGatewayChannel.OpenL(aChannelId, aObserver);
	}
		
/**
  * From CActive. Called when messages sent succesfully to NG
  */
void CGatewayChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	CActiveScheduler::Stop();
	}

/**
* From CActive. Cancels the outstanding request.
*/	
void CGatewayChannel::DoCancel()
	{
	iNetGatewayChannel.CancelSendMessageNotification();
	}
/**
*  send message to NG and wait for complete
*/
void CGatewayChannel::SendMessageAndWaitForResponse(const TLbsNetInternalMsgBase& aMessage)
	{
	iNetGatewayChannel.SendMessage(aMessage, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}
