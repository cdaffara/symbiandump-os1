// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// NetDial Serial IO Functions
// 
//

/**
 @file Sio.cpp 
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SIOTraces.h"
#endif

#include "SSCREXEC.H"
#include "SIO.H"
#include <networking/bca.h>

const TInt KChatterPriority=0;
const TInt KCommReadPriority=10;
const TInt KCommWritePriority=20;
const TInt KChatBufferSize=64;
const TInt KWriteTimeOutSec=6;
const TInt KOneSecInMicroSecs=1000000;
const TInt KPreSendPauseTimeMicroSec=200000;
const TInt KClockTick=15500;
const TReal KTRealOneSecInMicroSecs=1E6;

// CScriptIO definitions 
CScriptIO* CScriptIO::NewL(CScriptExecutor* aScriptExecutor, const TDesC& aCommsChannel)

/**
2 phased constructor for CScriptIO, first phase.

@param aScriptExecutor a pointer to script executor.
@param aCommPort a reference to COMM port.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CScriptIO object.
*/
	{
	CScriptIO* c=new(ELeave) CScriptIO(aScriptExecutor);
	CleanupStack::PushL(c);
	c->ConstructL(aCommsChannel);
	CleanupStack::Pop();
	return c;
	}

CScriptIO::CScriptIO(CScriptExecutor* aScriptExecutor)
	: iScriptExecutor(aScriptExecutor)
/**
Constructor for CSetCommand, used in the first phase of construction.

@param aScriptExecutor a pointer to script executor.
@param aCommPort a reference to COMM port.
*/
	{}

void CScriptIO::ConstructL(const TDesC& aCommsChannel)
/**
Instantiates member variables.
*/
	{
	CommConstructL(KCommReadPriority,KCommWritePriority);
	iChat=CCommChatter::NewL(this,KChatterPriority,KChatBufferSize);
	iPreSendPause=CPreSendPause::NewL(this);
	iExcessData.Set(NULL,0);

	iCommsChannel.CreateL(aCommsChannel);
	iCommsChannel.Copy(aCommsChannel);
	}

void CScriptIO::CreateChannel(TRequestStatus& aStatus)
	{
	ASSERT(iCreateAndShutdownStatus == NULL);
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CREATECHANNEL_1,"Script:\tOpening Comm Port '%S'", iCommsChannel);

	iCommClosed = EFalse;
	TInt err = CommOpen(iCommsChannel);
	if (err != KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CREATECHANNEL_2, "Script: Error %d opening Comm Port", err);
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, err);	
		}
	iCreateAndShutdownStatus = &aStatus;
	}

void CScriptIO::CancelCreateChannel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CANCELCREATECHANNEL_1, "Script:\tCancelCreateChannel()");
	CommCancel();
	}

void CScriptIO::InitializeComplete()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_INITIALIZECOMPLETE_1,"Script:\tInitializeComplete()");
	ASSERT(iCreateAndShutdownStatus);
	User::RequestComplete(iCreateAndShutdownStatus, KErrNone);
	iCreateAndShutdownStatus = NULL;
	}

void CScriptIO::ShutdownComplete(TInt aError)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_SHUTDOWNCOMPLETE_1, _"Script:\tShutdownComplete(aError %d)", aError);

	ASSERT(iCreateAndShutdownStatus);
	if (iCreateError != KErrNone) //The creation error is probably more interesting than a bad shutdown error
		{
		aError = iCreateError;
		}
	User::RequestComplete(iCreateAndShutdownStatus, aError);
	iCreateAndShutdownStatus = NULL;
	iCreateError = KErrNone;
	iCommClosed = ETrue;
	}

void CScriptIO::ConfigurePort(TRequestStatus& aStatus, const TCommConfig& aConfiguration)
/**
Configures COMM port.

@return error value from RComm::SetConfig() request.
*/
	{
	using namespace BasebandChannelAdaptation;
	iConfig() = aConfiguration;
	iBca->Ioctl(aStatus, KBcaOptLevelExtSerial, KSerialSetConfig, iConfig);
	}

void CScriptIO::CancelConfigurePort()
/**
Cancel Configuration of COMM port.
*/
	{
	iBca->CancelIoctl();
	}

void CScriptIO::ShutdownChannel(TRequestStatus& aStatus)
	{
	ASSERT(iCreateAndShutdownStatus == NULL);
	iCreateAndShutdownStatus = &aStatus;
	Stop(KErrNone);
	}

void CScriptIO::Stop(TInt  aError)
/**
Upcall from CScriptBcaControl class indicating an error was encountered.  Clean up and close BCA.

@param aError System wide error code. 
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_STOP_1,"Script:\tStop(aError %d)", aError);

	iCreateError = aError;
	delete iChat;
	iChat = NULL;
	delete iPreSendPause;
	iPreSendPause = NULL;
	CommClose();
	}

CScriptIO::~CScriptIO()
/**
Destructor.
Deletes iChat.
Deletes iPreSendPause.
Calls CommDelete().
*/
	{
	iCommsChannel.Close();
	delete iChat;
	delete iPreSendPause;
	CommDelete();
	}


void CScriptIO::Start()
/**
Starts write.
*/
	{
	CommWriteReady();
	iWritePending=ETrue;
	iChat->StartTimer(KWriteTimeOutSec*KOneSecInMicroSecs);
	}

void CScriptIO::CommReadComplete(TInt aStatus)
/**
Reads completely - stops timer and if no error checks string against the desired string
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_1,"Script:\tRead Complete");
	if(aStatus==KErrCommsLineFail)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_2,"Script:\tComms Error %d",aStatus);
		iChat->StopTimer();
		iReadPending=EFalse;
		TRAPD(ret,iScriptExecutor->CompletedReadL(KErrCommsLineFail));
		if (KErrNone != ret)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_3,"Script:\tCompleteReadL Failure");
			}
		return;
		}

	__ASSERT_ALWAYS(iReadPending,NetDialPanic(EIllegalReadComplete));
	iReadPending=EFalse;

	
	if (aStatus==KErrCommsFrame)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_4, "Script:\tComms Error %d",aStatus);
		User::After(KClockTick);		// wait for a clock tick and continue
		aStatus=KErrNone;
		}

	else if (aStatus!=KErrNone)
		{
		TRAPD(ret,iScriptExecutor->CompletedReadL(aStatus));
		if (KErrNone != ret)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_5,"Script:\tCompleteReadL Failure");
			}
		return;
		}

	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_7, "Rx:\t%s",iRxBuffer);


	if (iScriptExecutor->RequestUsePct())
		{
		TInt err=iScriptExecutor->WritePct(iRxBuffer);
		if (err!=KErrNone)
			{
			TRAPD(ret,iScriptExecutor->CompletedReadL(err));
			if (KErrNone != ret)
				{
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_8,"Script:\tCompleteReadL Failure");
				}
			return;
			}
		}

	if (!iScriptExecutor->ReadPctPending())
		{
		for (iRxBufOffset=0; iRxBufOffset<iRxBuffer.Length(); iRxBufOffset++)
			{
			iChat->AddChar(iRxBuffer[iRxBufOffset]);
			if(iStringFound!=-1)
				{
				iExcessData.Set(iRxBuffer.Right(iRxBuffer.Length()-iRxBufOffset-1));
				OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_9, "Script:\tExcess data buffer set to: %s",iExcessData);
				break;
				}
			}
		}
	else
		iStringFound=KErrNotFound;

	if(iStringFound!=KErrNotFound)
		{
		iChat->StopTimer();
		TRAPD(ret,iScriptExecutor->CompletedReadL(aStatus,iStringFound));
		if (KErrNone != ret)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMREADCOMPLETE_10, "Script:\tCompleteReadL Failure");
			}
		}
	else
		{
		iReadPending=ETrue;
		CommReadOneOrMore(iRxBuffer);
		}
	}

void CScriptIO::CommWriteComplete(TInt aStatus)
/**
Writes completely - stops timer
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMWRITECOMPLETE_1,"Script:\tWrite Complete");
	iChat->StopTimer();
	if(aStatus==KErrCommsLineFail)
 		{
 		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_COMMWRITECOMPLETE_2, "Script:\tComms Error %d",aStatus);
 		iWritePending=EFalse;
 		iScriptExecutor->CompletedWrite(KErrCommsLineFail);
 		return;
 		}
	__ASSERT_ALWAYS(iWritePending,NetDialPanic(EIllegalWriteComplete));
	iWritePending=EFalse;
	if(aStatus==KErrCommsFrame)		// ignore Comms Frame Error
		aStatus=KErrNone;
	iScriptExecutor->CompletedWrite(aStatus);
	}

void CScriptIO::ChatStringMatch(TInt aIndex)
/**
Logs matching string found and sets iStringFound to aIndex.
*/
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CHATSTRINGMATCH_1, "Script:\tMatching String Found %d",aIndex);
	iStringFound=aIndex;
	}

void CScriptIO::ChatTimeout()
/**
Timeout has occurred without while read/write pending.  Calls executor with error.
*/
	{
	CommCancel();
	if(iWritePending)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CHATTIMEOUT_1, "Script:\tWrite Chat Time Out");
		iWritePending=EFalse;
		iScriptExecutor->CompletedWrite(KErrTimedOut);
		}
	else if(iReadPending)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CHATTIMEOUT_2,"Script:\tRead Chat Time Out");
		iReadPending=EFalse;
		TRAPD(ret,iScriptExecutor->CompletedReadL(KErrTimedOut));
		if (KErrNone != ret)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_CHATTIMEOUT_3,"Script:\tCompleteReadL Failure");
			}
		}
	else
		NetDialPanic(EIllegalTimeOutComplete);
	}

void CScriptIO::Read(CLabelSearchArray* aSearchArray, const TReal& aTimeOut)
/**
Read from port.
*/
	{
	__ASSERT_ALWAYS(aSearchArray!=NULL, NetDialPanic(ENullSearchArray));

	iExcessData.Set(NULL,0);	// clear excess data buffer

	for(TInt i=0; i<aSearchArray->Count(); i++)
		{
		iChat->AddString((*aSearchArray)[i]->ChatString());
		}
	iReadPending=ETrue;
	iStringFound=KErrNotFound;
//	
	TReal realTimeInterval=aTimeOut*KTRealOneSecInMicroSecs;
	TInt timeInterval=TInt(realTimeInterval);
	if (realTimeInterval>TReal(timeInterval))
		timeInterval++;
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_READ_1,"Script:\tRead Pending In %d Microseconds",timeInterval);
	
//
	iChat->StartTimer(timeInterval);
	CommReadOneOrMore(iRxBuffer);
	iRxBufOffset=0;
	}

void CScriptIO::ReadEcho()
/**
Reads echo.
*/
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_READECHO_1,"Script:\tRead Echo");
	iExcessData.Set(NULL,0);	// clear excess data buffer

	iReadPending=ETrue;
	iChat->StartTimer(KMaxTInt);
	CommReadOneOrMore(iRxBuffer);
	iRxBufOffset=0;
	}

void CScriptIO::Write(const TDesC8& aString)
/**
Writes to port - start pre-send pause, when it completes, will start write.
*/
	{
	if (aString.Length()>KTxBufferSize)
		iTxBuffer.Copy(aString.Left(KTxBufferSize));
	else
		iTxBuffer.Copy(aString);
	__ASSERT_ALWAYS(!iWritePending, NetDialPanic(EIllegalWritePending));
	iWritePending=ETrue;
	iPreSendPause->Start();
	}

void CScriptIO::PreSendPauseCompleted()
/**
PreSend pause is finished, can now do write.
*/
	{
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSCRIPTIO_PRESENDPAUSECOMPLETED_1, "Tx:\t%s",iTxBuffer);
	CommWrite(iTxBuffer);
	iChat->StartTimer(KWriteTimeOutSec*KOneSecInMicroSecs);
	}

TBool CScriptIO::RWPending()
/**
Returns true if a read or write is pending.
*/
	{
	return (iWritePending)||(iReadPending);
	}

TInt CScriptIO::GetExcessData(TDes8& aBuffer)
/**
Gets excess data.
*/
	{
	TInt len=aBuffer.Length();
	if(iExcessData.Length()>len)
		aBuffer.Copy(iExcessData.Right(len));
	else
		aBuffer.Copy(iExcessData);
	return KErrNone;
	}

void CScriptIO::Disconnect()
/**
Disconnection - resets handshaking and closes comm port.
*/
	{
	Cancel();
/*	TCommConfig cbuf;
	TCommConfigV01 &cfg=cbuf();
	iCommPort.Config(cbuf);
	cfg.iHandshake = KConfigFreeRTS	| KConfigFreeDTR;
	iCommPort.SetConfig(cbuf);		// ignore return value
	iCommPort.SetSignalsToSpace(KSignalRTS | KSignalDTR);
*/
	// Don't issue a CommClose() if we have already issued it before.
	if (!iCommClosed)
		{
		CommClose();
		}
	}

void CScriptIO::ReConfigureAndCancelPort(TRequestStatus& aStatus)
/**
Cancels and reconfigures Comm Port to allow RTS and DTR to be subsequently dropped (see also DropSignals()).

@param aStatus TRequestStatus to complete once Comm Port is configured.
*/
	{
	using namespace BasebandChannelAdaptation;

	Cancel();

	// Someone has (presumably accidentally) defined Ioctls KSerialConfig and KSerialSetConfig as
	// requiring a package buffer within a package buffer, hence the use of "()()".
	iConfig()().iHandshake = KConfigFreeRTS | KConfigFreeDTR;

	iBca->Ioctl(aStatus, KBcaOptLevelExtSerial, KSerialSetConfig, iConfig);
	}

void CScriptIO::DropSignals(TRequestStatus& aStatus)
/**
Drop RTS and DTR.  Occurs once ReConfigureAndCancelPort() has released the signals.

@param aStatus TRequestStatus to complete once signals have been dropped.
*/
	{
	SetControlLines(&aStatus, 0, KSignalRTS | KSignalDTR);
	}

void CScriptIO::Cancel()
/**
Cancel - cancels timers and read/write
*/
	{
	
	CommCancel();
	if (iPreSendPause)
		{
		iPreSendPause->Cancel();
		}
	iReadPending=EFalse;
	iWritePending=EFalse;
	iExcessData.Set(NULL,0);
	if (iChat)
		{
		iChat->StopTimer();
		iChat->DeleteAllAndStop();
		}
	}


void CScriptIO::ReadEchoCancel()
/**
Cancel read - cancels timers and read
*/
	{
	
	CommReadCancel();
	iReadPending=EFalse;
	iExcessData.Set(NULL,0);
	iChat->StopTimer();
	}

void CScriptIO::DropDTR(TRequestStatus* aStatusPtr)
/**
Drop DTR.
*/
	{
	Cancel();
	SetControlLines(aStatusPtr, 0, KSignalDTR);
	}


void CScriptIO::RaiseDTR(TRequestStatus* aStatusPtr)
/**
Raide DTR.
*/
	{
	Cancel();
	SetControlLines(aStatusPtr, KSignalDTR, 0);
	}

void CScriptIO::SetControlLines(TRequestStatus* aStatusPtr, TUint aSetMask, TUint aClearMask)
/**
Issue request to BCA to alter control lines.
*/
	{
	using namespace BasebandChannelAdaptation;
	TPckgBuf<TSerialSetControlLines> lines;
	lines().iSetMask = aSetMask;
	lines().iClearMask = aClearMask;
	if (aStatusPtr == NULL)
		{
		// During conversion from RComm to BCA, synchronous behaviour was allowed in this particular circumstance.
		// This is to avoid wholesale changes to the CSD Agent to accomodate rarely (if ever) traversed
		// code paths.  The original RComm based code would previously issue a synchronous RComm::SetSignals()
		// call at this point, so the synchronous behaviour here is no worse than before.
		TRequestStatus status;
		iBca->Ioctl(status, KBcaOptLevelExtSerial, KSerialSetControlLines, lines);
		User::WaitForRequest(status);
		}
	else
		{
		iBca->Ioctl(*aStatusPtr, KBcaOptLevelExtSerial, KSerialSetControlLines, lines);
		}
	}

const TDesC& CScriptIO::BcaStack()
	{
	ASSERT(iScriptExecutor);
	return iScriptExecutor->BcaStack();
	}

TInt CScriptIO::IapId()
	{
	ASSERT(iScriptExecutor);
	return iScriptExecutor->IapId();
	}

// CPreSendPause definitions

CPreSendPause* CPreSendPause::NewL(CScriptIO* aNotifier)
/**
2 phased constructor for CPreSendPause, first phase.

@param aNotifier a pointer to script IO notifier.
@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CPreSendPause object.
*/
	{
	CPreSendPause* p=new(ELeave) CPreSendPause(aNotifier);
	CleanupStack::PushL(p);
	p->ConstructL(); // CTimer::ConstructL()
	CleanupStack::Pop();
	return p;
	}

CPreSendPause::CPreSendPause(CScriptIO* aNotifier) 
	: CTimer(EPriorityStandard), iNotifier(aNotifier)
/**
Constructor for CScriptCharacterConverter, used in the first phase of construction.
*/
	{
	CActiveScheduler::Add(this);
	}

void CPreSendPause::Start()
/**
Starts timer.
*/
	{
	After(KPreSendPauseTimeMicroSec);
	}

void CPreSendPause::RunL()
/**
Notifies script IO that pause complete.
*/
	{
	iNotifier->PreSendPauseCompleted();
	}
