	// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <c32comm.h>
#include "receiver.h"
#include "receiveobserver.h"
#include <bluetooth/logger.h>
 
#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_REF_SER_BEARER);
#endif
 
CReceiver* CReceiver::NewL(RComm& aComm, MReceiveObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CReceiver* self = new(ELeave) CReceiver(aComm, aObserver);
	return self;
	}

CReceiver::~CReceiver()
	{
	LOG_FUNC
	Cancel();
	}

CReceiver::CReceiver(RComm& aComm, MReceiveObserver& aObserver)
:	CActive(CActive::EPriorityStandard),
	iComm(aComm),
	iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CReceiver::Receive(TDes8& aBuf)
	{
	iComm.Read(iStatus, aBuf);
	SetActive();
	}

void CReceiver::RunL()
	{
	iObserver.MroReceiveComplete(iStatus.Int());
	}

void CReceiver::DoCancel()
	{
	iComm.ReadCancel();
	}
