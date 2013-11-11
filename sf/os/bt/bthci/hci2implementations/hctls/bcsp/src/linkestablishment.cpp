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

/**
 @file
 @internalComponent
*/

#include "linkestablishment.h"

#include "hctlbcsplinkstate.h"
#include "hctlbcspconsts.h"
#include "hctlbcsp.h"
#include "debug.h"

#include <bluetooth/hci/delay.h>

CLinkEstablishment::CLinkEstablishment(CHCTLBcsp& aHCTLBcsp)
	:iHctlBcsp(aHCTLBcsp)
	{
	LOG_FUNC
	}

CLinkEstablishment* CLinkEstablishment::NewL(CHCTLBcsp& aHCTLbcsp)
    {
	LOG_STATIC_FUNC

    CLinkEstablishment* self=new(ELeave) CLinkEstablishment(aHCTLbcsp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CLinkEstablishment::ConstructL()
    {
	LOG_FUNC
	TCallBack cb = TCallBack(CLinkEstablishment::ShyTimeout, this);
	iShyTimer = CDelay::NewL(cb, CActive::EPriorityLow);
	
	cb = TCallBack(CLinkEstablishment::ConfTimeout, this);
	iConfTimer = CDelay::NewL(cb, CActive::EPriorityLow);
	
	iLinkStateFactory = CLinkStateFactory::NewL(*this);
	
	//Set first state to EShy however do not call the Exit function
	iLinkState = &(iLinkStateFactory->State(CLinkStateFactory::EShy));	 
	
	//Call Entry point to first state i.e. TLinkStateShy
	iLinkState->Entry();	

    }

CLinkEstablishment::~CLinkEstablishment()
	{
	LOG_FUNC

	delete iShyTimer;
	delete iConfTimer;
	delete iLinkStateFactory;
	}

/**
	Simple stub method that passes a received @param aLinkMsg()
	from the receiver and forwards it to the CLinkEstblishment::LinkStateMachine() 
	method
*/
void CLinkEstablishment::ProcessLinkMsg(const TDesC8& aLinkMsg)
	{
	LOG_FUNC

	LinkStateMachine(aLinkMsg);
	}

/**
	LinkStateMachine method filters @param aLinkMsg by comparing with the 4 BCSP link message
	constants and then calling the appropriate handling methods which then call in to the State Pattern
	link state machine
*/
void CLinkEstablishment::LinkStateMachine(const TDesC8& aLinkMsg)
	{
	LOG_FUNC

	if ( aLinkMsg.Compare(KBcspLinkMsg_Sync) == 0 )
		{
		LOG(_L8("CLinkEstablishment::LinkStateMachine() KBcspLinkMsgSync RECVD"));
		HandleLinkMsgSync();
		}
	else if ( aLinkMsg.Compare(KBcspLinkMsg_SyncResp) == 0 )
		{
		LOG(_L8("CLinkEstablishment::LinkStateMachine() KBcspLinkMsgSyncResp RECVD"));
		HandleLinkMsgSyncResp();
		}
	else if ( aLinkMsg.Compare(KBcspLinkMsg_Conf) == 0 )
		{
		LOG(_L8("CLinkEstablishment::LinkStateMachine() KBcspLinkMsgConf RECVD"));
		HandleLinkMsgConf();
		}
	else if ( aLinkMsg.Compare(KBcspLinkMsg_ConfResp) == 0 )
		{
		LOG(_L8("CLinkEstablishment::LinkStateMachine() KBcspLinkMsgConfResp RECVD"));
		HandleLinkMsgConfResp();
		}
	else
		{
		LOG(_L8("CLinkEstablishment::LinkStateMachine() UNRECOGNIZED"));
		}
	}
	

/**
	Start the shy timer
*/
void CLinkEstablishment::StartShyTimer()
	{
	LOG_FUNC
	iShyTimer->After(KTShyTimeout);	
	}

/**
	Simple stub method to allow state machine to make a call to Start the ConfTimer
*/
void CLinkEstablishment::StartConfTimer()
	{		
	LOG_FUNC
	//reset the confirmation counter
	iConfCnt = 0;
	//start the timer
	iConfTimer->After(KTConfTimeout);
	}

/**
	State Pattern method to switch state
	@param aLinkState
*/
void CLinkEstablishment::SetState(const TLinkState& aLinkState)
	{
	LOG_FUNC

	iLinkState->Exit();
	iLinkState = &aLinkState;
	iLinkState->Entry();
	}

TBool CLinkEstablishment::Muzzled()
	{
	return (iHctlBcsp.Muzzled());
	}

void CLinkEstablishment::ResetMuzzled()
	{
	iHctlBcsp.ResetMuzzled();
	}

void CLinkEstablishment::HandleLinkMsgConf()
	{
	LOG_FUNC

	iLinkState->HandleLinkMsgConf();
	}

void CLinkEstablishment::HandleLinkMsgConfResp()
	{
	LOG_FUNC

	iLinkState->HandleLinkMsgConfResp();
	}

void CLinkEstablishment::HandleLinkMsgSync()
	{
	LOG_FUNC

	ResetMuzzled();
	iLinkState->HandleLinkMsgSync();
	}

void CLinkEstablishment::HandleLinkMsgSyncResp()
	{
	LOG_FUNC

	iLinkState->HandleLinkMsgSyncResp();
	}

void CLinkEstablishment::StopShyTimer()
	{
	LOG_FUNC

	iShyTimer->Cancel();
	}

void CLinkEstablishment::StopConfTimer()
	{
	LOG_FUNC

	iConfTimer->Cancel();
	}

void CLinkEstablishment::UnChoke()
	{
	LOG_FUNC

	iHctlBcsp.UnChoke();
	}

void CLinkEstablishment::Choke()
	{
	LOG_FUNC

	iHctlBcsp.Choke();
	}

void CLinkEstablishment::QueueReadForNextFrame()
	{
	LOG_FUNC

    iHctlBcsp.QueueReadForNextFrame();
	}

void CLinkEstablishment::SendLinkMsg(const TDesC8& aLinkMsg)
	{
	LOG_FUNC

	iHctlBcsp.TxLinkMsg(aLinkMsg);
	}
	
/**
  	Handle a peer reset detection by passing it up the stack
*/
void CLinkEstablishment::HandlePeerReset()
 	{
	LOG_FUNC

 	iHctlBcsp.HandlePeerReset();
 	}
 
/**
  	Reset the link establishment state machine to its default state
*/
void CLinkEstablishment::Reset()
 	{
	LOG_FUNC

 	SetState(iLinkStateFactory->State(CLinkStateFactory::EShy));
 	}
 	
void CLinkEstablishment::HandleConfTimeout()
	{
	LOG_FUNC
	
	if(iConfCnt < KBcspConfCntMax)
		{
		SendLinkMsg(KBcspLinkMsg_Conf);
		iConfTimer->After(KTConfTimeout);
		iConfCnt++;
		}
	else 
		{
		iConfTimer->Cancel();
		}
	}

void CLinkEstablishment::HandleShyTimeout()
	{
	LOG_FUNC
	
	SendLinkMsg(KBcspLinkMsg_Sync);
	StartShyTimer();
	}

/**
	Confirmation timeout, we will try send another conf message if the max number
	of confirmation is not reached. Otherwise we stop doing so and just wait silently 
	for the confirmation response.
*/
/*static*/ TInt CLinkEstablishment::ConfTimeout(TAny* aLink)
	{
	reinterpret_cast<CLinkEstablishment*>(aLink)->HandleConfTimeout();
	return KErrNone;
	}

/**
	Shy timer expired, we just keep sending more sync message until we get response
*/	
/*static*/ TInt CLinkEstablishment::ShyTimeout(TAny* aLink)
	{
	reinterpret_cast<CLinkEstablishment*>(aLink)->HandleShyTimeout();
	return KErrNone;
	}
