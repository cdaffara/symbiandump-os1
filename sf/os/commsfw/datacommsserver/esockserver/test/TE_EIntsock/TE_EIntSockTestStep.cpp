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

//#include<c32root.h>
#include <e32base.h>
#include <es_sock.h>
//#include <ss_std.h>
//#include <e32std.h>
#include <comms-infras/eintsock.h>

#include "TE_EIntSockTestStep.h"
#include "es_ptestinternalsocket.h"
//#include <e32des8.h>

// constructor
CEIntSockTestStep::CEIntSockTestStep()
	{
	}

// destructor
CEIntSockTestStep::~CEIntSockTestStep()
	{
	iESockProxyThread.Close();
	}

enum TVerdict CEIntSockTestStep::doTestStepPreambleL( void )
	{
	return EPass;
	}

enum TVerdict CEIntSockTestStep::doTestStepL()
	{
	
	TRAPD(trapRet,enum TVerdict stepRet = InternalDoTestStepL();\
		if (stepRet !=EPass)\
			{\
			SetTestStepResult(stepRet);\
			}\
			)
			
			if (trapRet != KErrNone)
				{
				SetTestStepResult(EFail);
				return EFail;
				}
			return EPass;
	};

//This method ensures that any
enum TVerdict CEIntSockTestStep::doTestStepPostambleL( void )
	{
	return EPass;
	}
	
enum TVerdict CEIntSockTestStep::InternalDoTestStepL()
	{
	RSocketServ ss1;
	TInt ret;
	ret = ss1.Connect();
	if (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Connect to socket server failed with %S"),&EpocErrorToText(ret));
		}
	CleanupClosePushL(ss1);
	TESTL(ret == KErrNone);
	RSocket sock1;
	ret = sock1.Open(ss1,KProxyAddrFamily,KSockDatagram, KProxy);
	//ret = sock1.Open(ss1,KProxyName);
	if (ret != KErrNone)
		{
		Logger().WriteFormat(_L("Opening socket to the Test Internal Socket Protocol failed with %S"),&EpocErrorToText(ret));
		}
	TESTL(ret == KErrNone);
	Logger().Close();
	TRequestStatus stat;
		
	TProxyProtocolRequest request;
	request.name.Copy(TestStepName());
	
	TPckg<TProxyProtocolRequest> requestPckg(request);
	
	_LIT(PanicStep,"Test21");
	if (TestStepName() == PanicStep)
		{
		doPanicStepTestPreambleL();
		}

	sock1.Ioctl(KProxyProtocolTestRequest,stat,&requestPckg);
	User::WaitForRequest(stat);
	CleanupStack::Pop(&ss1);
	ss1.Close();

	if (TestStepName() == PanicStep)
		{
		doPanicStepTestPostambleL(stat);
		}
	else
		{
		TESTL(stat == KErrNone);
		SetTestStepResult(request.result);
		}
	return TestStepResult();
	}

void CEIntSockTestStep::doPanicStepTestPreambleL()
	{
	// Search for *ESock_Proxy* thread
	TFindThread findThread(_L("*ESock_Proxy*"));
	TFullName threadName;
	TESTL(findThread.Next(threadName) == KErrNone);
		
	TESTL(iESockProxyThread.Open(findThread) == KErrNone);

	iESockProxyThread.Logon(iESockProxyThreadStatus);
	}

void CEIntSockTestStep::doPanicStepTestPostambleL( TRequestStatus& aStat )
	{
	//ESock is expected to have a panic. The Kernel will therefore return KErrAbort
	TESTL(aStat  == KErrAbort);

	User::WaitForRequest(iESockProxyThreadStatus);

	//Test that ESock_Proxy Paniced
	TESTL(iESockProxyThread.ExitType() == EExitPanic);
	
	// Test that the Panic Category was "InternalSocket"
	_LIT(KInternalSocketPanic, "InternalSocket");
	TESTL(iESockProxyThread.ExitCategory() == KInternalSocketPanic);

	// Test that the Panic Reason was EInternalSocketNotNullSocket (12)
	TESTL(iESockProxyThread.ExitReason() == ENotNullSocket);

	SetTestStepResult(EPass);	
	}
	
// extra logging methods
const TPtrC& CEIntSockTestStep::EpocErrorToText(const TInt aErrCode)
/**
Convert a Error code to text.

  @param aError error code to display as text.
  @return Text describing the error.
  */
	{
	//	TPtr errText;
	switch (aErrCode)
		{
		case KErrNone:
			iErrText.Set(_L("KErrNone"));
			break;
		case KErrNotFound:
			iErrText.Set(_L("KErrNotFound"));
			break;
		case KErrGeneral:
			iErrText.Set(_L("KErrGeneral"));
			break;
		case KErrCancel:
			iErrText.Set(_L("KErrCancel"));
			break;
		case KErrNoMemory:
			iErrText.Set(_L("KErrNoMemory"));
			break;
		case KErrNotSupported:
			iErrText.Set(_L("KErrNotSupported"));
			break;
		case KErrArgument:
			iErrText.Set(_L("KErrArgument"));
			break;
		case KErrTotalLossOfPrecision:
			iErrText.Set(_L("KErrTotalLossOfPrecision"));
			break;
		case KErrBadHandle:
			iErrText.Set(_L("KErrBadHandle"));
			break;
		case KErrOverflow:
			iErrText.Set(_L("KErrOverflow"));
			break;
		case KErrUnderflow:
			iErrText.Set(_L("KErrUnderflow"));
			break;
		case KErrAlreadyExists:
			iErrText.Set(_L("KErrAlreadyExists"));
			break;
		case KErrPathNotFound:
			iErrText.Set(_L("KErrPathNotFound"));
			break;
		case KErrDied:
			iErrText.Set(_L("KErrDied"));
			break;
		case KErrInUse:
			iErrText.Set(_L("KErrInUse"));
			break;
		case KErrServerTerminated:
			iErrText.Set(_L("KErrServerTerminated"));
			break;
		case KErrServerBusy:
			iErrText.Set(_L("KErrServerBusy"));
			break;
		case KErrCompletion:
			iErrText.Set(_L("KErrCompletion"));
			break;
		case KErrNotReady:
			iErrText.Set(_L("KErrNotReady"));
			break;
		case KErrUnknown:
			iErrText.Set(_L("KErrUnknown"));
			break;
		case KErrCorrupt:
			iErrText.Set(_L("KErrCorrupt"));
			break;
		case KErrAccessDenied:
			iErrText.Set(_L("KErrAccessDenied"));
			break;
		case KErrLocked:
			iErrText.Set(_L("KErrLocked"));
			break;
		case KErrWrite:
			iErrText.Set(_L("KErrWrite"));
			break;
		case KErrDisMounted:
			iErrText.Set(_L("KErrDisMounted"));
			break;
		case KErrEof:
			iErrText.Set(_L("KErrEof"));
			break;
		case KErrDiskFull:
			iErrText.Set(_L("KErrDiskFull"));
			break;
		case KErrBadDriver:
			iErrText.Set(_L("KErrBadDriver"));
			break;
		case KErrBadName:
			iErrText.Set(_L("KErrBadName"));
			break;
		case KErrCommsLineFail:
			iErrText.Set(_L("KErrCommsLineFail"));
			break;
		case KErrCommsFrame:
			iErrText.Set(_L("KErrCommsFrame"));
			break;
		case KErrCommsOverrun:
			iErrText.Set(_L("KErrCommsOverrun"));
			break;
		case KErrCommsParity:
			iErrText.Set(_L("KErrCommsParity"));
			break;
		case KErrTimedOut:
			iErrText.Set(_L("KErrTimedOut"));
			break;
		case KErrCouldNotConnect:
			iErrText.Set(_L("KErrCouldNotConnect"));
			break;
		case KErrCouldNotDisconnect:
			iErrText.Set(_L("KErrCouldNotDisconnect"));
			break;
		case KErrDisconnected:
			iErrText.Set(_L("KErrDisconnected"));
			break;
		case KErrBadLibraryEntryPoint:
			iErrText.Set(_L("KErrBadLibraryEntryPoint"));
			break;
		case KErrBadDescriptor:
			iErrText.Set(_L("KErrBadDescriptor"));
			break;
		case KErrAbort:
			iErrText.Set(_L("KErrAbort"));
			break;
		case KErrTooBig:
			iErrText.Set(_L("KErrTooBig"));
			break;
		case KRequestPending:
			iErrText.Set(_L("KRequestPending"));
			break;
		default:
			iErrBuff.Format(_L(" %d"),aErrCode);
			iErrText.Set(iErrBuff.Ptr());
			break;
		}
		return iErrText;
	}	


