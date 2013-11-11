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

#include "hctlbcspsender.h"

#include "hctlbcspsequencer.h"
#include "debug.h"

#include <d32comm.h>


/**
Implementation of Class CHCTLBcspSender
*/
CHCTLBcspSender::CHCTLBcspSender(CHCTLBcspSequencer& aSequencer, RBusDevComm& aPort) : 
	CActive(EPriorityStandard),
	iSequencer(aSequencer),
	iPort(aPort)
    {
	LOG_FUNC
	CActiveScheduler::Add(this);
    }

CHCTLBcspSender::~CHCTLBcspSender()
    {
	LOG_FUNC

	Cancel();
    }

CHCTLBcspSender* CHCTLBcspSender::NewL(CHCTLBcspSequencer& aSequencer, RBusDevComm& aPort)
    {
	LOG_STATIC_FUNC

    CHCTLBcspSender* self=new(ELeave)CHCTLBcspSender(aSequencer, aPort);
    return self;
    }

TInt CHCTLBcspSender::Write(const TDesC8& aData)
    {
	LOG_FUNC
#ifdef __DEBUG_FLOG_RAW_UART_	
	LOG(_L8("HCTLBCSP: UART TX..."));
	LOGHEXDESC(aData);
#endif

	iPort.Write(iStatus, aData);
 	SetActive();

    return KErrNone;
    }

void CHCTLBcspSender::RunL()
    {
	LOG_LINE
	LOG_FUNC
	LOG1(_L8("\tiStatus = %d"), iStatus.Int());

	iSequencer.SendNextPacket();
    }

void CHCTLBcspSender::DoCancel()
	{
	LOG_FUNC
	
	iPort.WriteCancel(); 
	}
