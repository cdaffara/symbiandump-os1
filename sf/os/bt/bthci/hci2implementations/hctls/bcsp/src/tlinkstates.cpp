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

#include "hctlbcsplinkstate.h"
#include "hctlbcspconsts.h"
#include "linkestablishment.h"
#include "bcsputils.h"
#include "debug.h"

TLinkState::TLinkState(CLinkStateFactory& aFactory, CLinkEstablishment& aLink) 
	: iFactory(aFactory), 
	  iLink(aLink)
	{
	LOG_FUNC
	}

TLinkState::~TLinkState()
	{
	LOG_FUNC
	}


void TLinkState::Entry() const
	{
	LOG_FUNC

	PANIC(KBcspPanicCat, EUnexpectedLinkEvent);
	}

void TLinkState::Exit() const
	{
	LOG_FUNC

	PANIC(KBcspPanicCat, EUnexpectedLinkEvent);
	}

void TLinkState::HandleLinkMsgConf() const
	{
	LOG_FUNC

	PANIC(KBcspPanicCat, EUnexpectedLinkEvent);
	}

void TLinkState::HandleLinkMsgConfResp() const
	{
	LOG_FUNC

	PANIC(KBcspPanicCat, EUnexpectedLinkEvent);
	}

void TLinkState::HandleLinkMsgSync() const
	{
	LOG_FUNC

	PANIC(KBcspPanicCat, EUnexpectedLinkEvent);
	}

void TLinkState::HandleLinkMsgSyncResp() const
	{
	LOG_FUNC

	PANIC(KBcspPanicCat, EUnexpectedLinkEvent);
	}


TLinkStateShy::TLinkStateShy (CLinkStateFactory& aFactory, CLinkEstablishment& aLink) 
	: TLinkState(aFactory, aLink)
	{
	LOG_FUNC
	}

TLinkStateShy::~TLinkStateShy()
	{
	LOG_FUNC
	}


void TLinkStateShy::Entry() const
	{
	LOG_FUNC

	iLink.Choke();
	if(iLink.Muzzled() == EFalse)
		{
		iLink.StartShyTimer();
		}
	iLink.QueueReadForNextFrame();
	}

void TLinkStateShy::Exit() const
	{
	LOG_FUNC

	iLink.StopShyTimer();
	}

void TLinkStateShy::HandleLinkMsgSync() const
	{
	LOG_FUNC

	iLink.SendLinkMsg(KBcspLinkMsg_SyncResp);
	}

void TLinkStateShy::HandleLinkMsgSyncResp() const
	{
	LOG_FUNC

	iLink.SetState(iFactory.State(CLinkStateFactory::ECurious));
	}

void TLinkStateShy::HandleLinkMsgConf() const
	{
	LOG_FUNC

	// Ignore message
	}



TLinkStateCurious::TLinkStateCurious (CLinkStateFactory& aFactory, CLinkEstablishment& aLink)
	: TLinkState(aFactory, aLink)
	{
	LOG_FUNC
	}

TLinkStateCurious::~TLinkStateCurious()
	{
	LOG_FUNC
	}


void TLinkStateCurious::Entry() const
	{
	LOG_FUNC

	iLink.StartConfTimer();
	}

void TLinkStateCurious::Exit() const
	{
	LOG_FUNC

	iLink.StopConfTimer();
	}

void TLinkStateCurious::HandleLinkMsgConf() const
	{
	LOG_FUNC

	iLink.SendLinkMsg(KBcspLinkMsg_ConfResp);
	}

void TLinkStateCurious::HandleLinkMsgConfResp() const
	{
	LOG_FUNC

	iLink.SetState(iFactory.State(CLinkStateFactory::EGarrulous));
	}

void TLinkStateCurious::HandleLinkMsgSync() const
	{
	LOG_FUNC

	iLink.SendLinkMsg(KBcspLinkMsg_SyncResp);
	}


void TLinkStateCurious::HandleLinkMsgSyncResp() const
	{
	LOG_FUNC
	}



TLinkStateGarrulous::TLinkStateGarrulous (CLinkStateFactory& aFactory, CLinkEstablishment& aLink) 
	: TLinkState(aFactory, aLink)
	{
	LOG_FUNC
	}

TLinkStateGarrulous::~TLinkStateGarrulous()
	{
	LOG_FUNC
	}


void TLinkStateGarrulous::Entry() const
	{
	LOG_FUNC

	iLink.UnChoke();	//Connected
	}

void TLinkStateGarrulous::Exit() const
	{
	LOG_FUNC
	}

void TLinkStateGarrulous::HandleLinkMsgConf() const
	{
	LOG_FUNC

	iLink.SendLinkMsg(KBcspLinkMsg_ConfResp);
	}

void TLinkStateGarrulous::HandleLinkMsgConfResp() const
	{
	LOG_FUNC
	}

void TLinkStateGarrulous::HandleLinkMsgSync() const
	{
	LOG_FUNC

	//Peer Reset Detection SetState EShy
	LOG(_L("HCI CONNECTION RESET BY PEER"));
	//Purge all buffers Datagram Queue and Sequence Queue
	//Reset Sequence layer
	//Signal Stack that HCI link is down 
 	iLink.HandlePeerReset();	
	}
