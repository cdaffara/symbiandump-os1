// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of the CSimNtRas class.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimNtRasTraces.h"
#endif

#include "CSimNtras.h"

_LIT8(KNtRasWriteText,"CLIENT");			// < Solicitation text, send to start the NTRas server
_LIT8(KNtRasReceiveText,"SERVER");			// < NTRas server response, indicating the PPP session can commence.
const TInt KNtRasServerResponseTimeout=5;	// < Connection attempt time out value in seconds.

const TDataBits KDefaultDataBits=EData8;	// < Default configuration is 8N1
const TParity KDefaultParity=EParityNone;	// < Default configuration is 8N1
const TStopBits KDefaultStopBits=EStop1;	// < Default configuration is 8N1
const TText8 KDefaultXon=17;				// < Default XON character is <ctrl>Q
const TText8 KDefaultXoff=19;				// < Default XOFF character is <ctrl>S
const TInt KTxMaxRetries=3;					// < Maximum number of transmit retries before giving up

CSimNtRas* CSimNtRas::NewL(CSimPhone* aPhone)
/**
 * Standard two phase constructor.
 */
	{
	CSimNtRas* self=new(ELeave) CSimNtRas(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimNtRas::CSimNtRas(CSimPhone* aPhone)
	: iPhone(aPhone), iPortOpen(EFalse)
/**
 * Trivial first phase constructor.
 */
	{
	}

void CSimNtRas::ConstructL()
/**
 * Second phase constructor.  Create heap-based objects, in this instance the timer
 * and the Comm Port classes.
 */
	{
	iTimer=CSimTimer::NewL(iPhone);
	CommConstructL(CActive::EPriorityStandard,CActive::EPriorityStandard);
	}

CSimNtRas::~CSimNtRas()
/**
 * Standard destructor.  Close Comm Port and destroy heap-based objects.
 */
	{
	if(iPortOpen)
		{
		CommCancel();
		CommClose();
		}
	delete iTimer;
	}

TInt CSimNtRas::Connect(const TDesC& aCsyName, const TDesC& aPort,
						const TCommConfigV01& aConfig, MNTRasCallBack* aCallback)
/**
 * Attempt to connect to an NT Ras "Cable Connection" service.
 * @param aCsyName	The CSY to be used for the connection.
 * @param aPort		The Port to be used for the connection.
 * @param aConfig	The port configuration to be used for the connection.
 * @param aCallback	The callback class that must be signalled when the connection completes.
 * @return TInt		Standard error value.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNTRAS_CONNECT_1, "Starting to attempt a NT RAS Connection");
	iReadBuf.Zero();
	iInputBuf.Zero();

// Open and configuration the port
	TInt ret=CommOpen(aCsyName,aPort,ECommShared);
	if(ret!=KErrNone)
		return ret;
	iPortOpen=ETrue;

	TCommConfig configPckg;
	TCommConfigV01& config=configPckg();
	iCommPort.Config(configPckg);

	config.iDataBits=KDefaultDataBits;
	config.iParity=KDefaultParity;
	config.iStopBits=KDefaultStopBits;
	config.iXonChar=KDefaultXon;
	config.iXoffChar=KDefaultXoff;
	config.iTerminatorCount=0;
	config.iSpecialRate=0;

	config.iRate=aConfig.iRate;
	config.iHandshake=aConfig.iHandshake;
	ret=iCommPort.SetConfig(configPckg);
	if(ret!=KErrNone)
		return ret;

// Power up the port ready for receives and transmits
	CommReadReady();
	CommWriteReady();

	iAttemptCnt=0;
	iCallback=aCallback;
	CommReadOneOrMore(iReadBuf);
	AttemptConnect();
	return KErrNone;
	}

void CSimNtRas::AttemptConnect()
/**
 * Transmit "CLIENT" and attempt a connection
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNTRAS_ATTEMPTCONNECT_1, "NTRAS Writing CLIENT...");
	iAttemptCnt++;
	// Send the soliciting message, and await the NT Ras server's response.
	CommWrite(KNtRasWriteText);
	iTimer->Start(KNtRasServerResponseTimeout,this);
	}

void CSimNtRas::Cancel()
/**
 * Cancel any outstanding comm port reads, writes or timer requests.
 */
	{
	iTimer->Cancel();
	CommCancel();
	}

void CSimNtRas::Terminate()
/**
 * Terminate the NTRas connection.  If opened, cancel any outstanding requests and close
 * the port.  Also, cancel any outstanding timer request.
 */
	{
	if(iPortOpen)
		{
		CommCancel();
		CommClose();
		}
	iTimer->Cancel();
	}

void CSimNtRas::CommReadComplete(TInt aStatus)
/**
 * Serial port read completion callback.  If there's no read error, append the read
 * data to the input buffer and scan for the server's response.
 * If no match is found, re-post the read.
 * @param aStatus	Standard error value, indicating the result of the read request.
 */
	{
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNTRAS_COMMREADCOMPLETE_1, "NTRAS Rx Completion: %s",iInputBuf);
	if(aStatus!=KErrNone)
		{
		iTimer->Cancel();
		iCallback->NTRasCallBack(aStatus);
		return;
		}

	if((iInputBuf.Length()+iReadBuf.Length())>KInputBufferSize)
		{
		iTimer->Cancel();
		iCallback->NTRasCallBack(KErrNotFound);
		return;
		}

	iInputBuf.Append(iReadBuf);
	TInt pos=iInputBuf.FindF(KNtRasReceiveText);
	if(pos>=0)
		{
		iTimer->Cancel();
		iCallback->NTRasCallBack(KErrNone);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNTRAS_COMMREADCOMPLETE_2, "NTRAS Received SERVER!");
		return;
		}

	CommReadOneOrMore(iReadBuf);
	}

void CSimNtRas::CommWriteComplete(TInt aStatus)
/**
 * Serial port write completion callback.  Terminate the connection attempt if an error
 * has occurred.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNTRAS_COMMWRITECOMPLETE_1, "NTRAS Write Completion");
	if(aStatus!=KErrNone)
		{
		iTimer->Cancel();
		iCallback->NTRasCallBack(aStatus);
		}
	}

void CSimNtRas::TimerCallBack(TInt /*aId*/)
/**
 * Timer expired callback.  Give up the attempted connection with a time-out error.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMNTRAS_TIMERCALLBACK_1, "NTRAS Timeout Occurred");
	CommWriteCancel();
	if(iAttemptCnt<KTxMaxRetries)
		AttemptConnect();
	else
		{
		CommReadCancel();
		iCallback->NTRasCallBack(KErrTimedOut);
		}
	}
