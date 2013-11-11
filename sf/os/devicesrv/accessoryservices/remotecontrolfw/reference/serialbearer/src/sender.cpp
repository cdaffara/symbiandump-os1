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
#include "sender.h"
#include "sendobserver.h"
#include <bluetooth/logger.h>
 
#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_REF_SER_BEARER);
#endif
 
CSender* CSender::NewL(RComm& aComm, MSendObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CSender* self = new(ELeave) CSender(aComm, aObserver);
	return self;
	}

CSender::~CSender()
	{
	LOG_FUNC
	Cancel();
	}

CSender::CSender(RComm& aComm, MSendObserver& aObserver)
:	CActive(CActive::EPriorityStandard),
	iComm(aComm),
	iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CSender::Send(const TDesC8& aBuf)
	{
	iComm.Write(iStatus, aBuf);
	SetActive();
	}

void CSender::RunL()
	{
	iObserver.MsoSendComplete(iStatus.Int());
	}

void CSender::DoCancel()
	{
	iComm.WriteCancel();
	}
