// smsevent.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "smsevent.h"
#include "sms.h"

_LIT(KInternationlDialDirectIndicator,	"+");

//
// CSmsEventBase
//
CSmsEventBase::CSmsEventBase(CCmdSms& aObserver):
CActive(CActive::EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

//
// CSmsEventDial
//
CSmsEventDial* CSmsEventDial::NewL(CCmdSms& aObserver, RCall::TTelNumberC& aTelNumber)
	{
	CSmsEventDial* self = new (ELeave) CSmsEventDial(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aTelNumber);
	CleanupStack::Pop(self);
	return self;
	}

CSmsEventDial::CSmsEventDial(CCmdSms& aObserver):
CSmsEventBase(aObserver), iState(ESmsEventNetworkIdle)
	{
	}

CSmsEventDial::~CSmsEventDial()
	{
	Cancel();
	iTimeout.Cancel();
	iTimeout.Close();
	if (iCall.SubSessionHandle() > 0)
		iCall.Close(); // should also force a hangup on the call object if it's active
	if (iLine.SubSessionHandle() > 0)
		iLine.Close();
	if (iPhone.SubSessionHandle() > 0)
		iPhone.Close();
	if (iTelServer.Handle() > 0)
		iTelServer.Close();
	}

void CSmsEventDial::ConstructL(RCall::TTelNumberC& aTelNumber)
	{
	// timeout timer 
	iTimeout.CreateLocal();
	
	// capture the number to dial
	iNumber.iTypeOfNumber = RMobilePhone::ENationalNumber;
	iNumber.iNumberPlan = RMobilePhone::EIsdnNumberPlan;
	iNumber.iTelNumber.Copy(aTelNumber);
	
	// check if there's a '+' at the front of the number
	if (aTelNumber.MatchC(KInternationlDialDirectIndicator) == 0)
		iNumber.iTypeOfNumber = RMobilePhone::EInternationalNumber;
	
	// connect to ETel. Note we're assuming the TSY has been loaded already
	RTelServer::TPhoneInfo phoneInfo;
	User::LeaveIfError(iTelServer.Connect());
	User::LeaveIfError(iTelServer.GetPhoneInfo(0, phoneInfo));
	
	// open the first phone object
	User::LeaveIfError(iPhone.Open(iTelServer, phoneInfo.iName));
	
	// find the voice line & open it
	TInt lines = 0;
	RPhone::TLineInfo lineInfo;
	User::LeaveIfError(iPhone.EnumerateLines(lines));
	if (lines <= 0)
		User::Leave(KErrNotSupported);
	for (TInt ii = 0 ; ii < lines ; ii++)
		{
		User::LeaveIfError(iPhone.GetLineInfo(ii, lineInfo));
		if (lineInfo.iLineCapsFlags & RLine::KCapsVoice)
			{
			// found our voice line
			if (lineInfo.iStatus > RCall::EStatusIdle)
				User::Leave(KErrInUse);
			User::LeaveIfError(iLine.Open(iPhone, lineInfo.iName));
			break;
			}
		}
	if (iLine.SubSessionHandle() <= 0)
		User::Leave(KErrNotFound);
	
	// create a voice call (note it's not currently active)
	User::LeaveIfError(iCall.OpenNewCall(iLine, iCallName));
	}

//
// CSmsEventDial::StartL
// start the dial-out procedure, beginning
// with a check of the network, line/call status
//
void CSmsEventDial::StartL()
	{
	User::LeaveIfError(iLine.GetMobileLineStatus(iCallStatus));
	if (iCallStatus > RMobileCall::EStatusIdle)
		User::Leave(KErrInUse);
	
	// some product tsy implementations can't handle a dial request immediately following receiving an sms. so we wait a little bit..5 seconds in fact
	iTimeout.After(iStatus, 5000000);
	SetActive();
	iState = ESmsEventDial;
	}

void CSmsEventDial::RunL()
	{
	User::LeaveIfError(iStatus.Int()); // process the error elsewhere
	switch (iState)
		{
		case ESmsEventDial:
			{
			// dial the number
			iCall.Dial(iStatus, iNumber.iTelNumber);
			SetActive();
			iState = ESmsEventStatusChange;
			}
		break;
				
		case ESmsEventWatchStatus:
		case ESmsEventStatusChange:
			{
			User::LeaveIfError(iCall.GetMobileCallStatus(iCallStatus));
			if (iCallStatus > RMobileCall::EStatusIdle)
				{
				// keep the call going until it drops
				iCall.NotifyMobileCallStatusChange(iStatus, iCallStatus);
				SetActive();
				iState = ESmsEventWatchStatus;
				return;
				}
			}
		break;
		
		default:
		break;
		};
	}

//
// CSmsEventDial::RunError
// inform the observer we hit a prob. with execution of the dial state machine
//
TInt CSmsEventDial::RunError(TInt /*aError*/)
	{
	//TODO iObserver.EventComplete(aError); 
	return KErrNone;
	}

void CSmsEventDial::DoCancel()
	{
	switch (iState)
		{
		case ESmsEventDial:
			iTimeout.Cancel();
		break;
		
		case ESmsEventStatusChange:
			iCall.DialCancel();
		break;
		
		case ESmsEventWatchStatus:
			iCall.NotifyStatusChangeCancel();			
		break;
		
		default:
		break;
		};
	}

CSmsShellCommand* CSmsShellCommand::NewL(CCmdSms& aObserver, const TDesC& aCommandText, const TDesC& aSenderAddress)
	{
	CSmsShellCommand* self = new(ELeave) CSmsShellCommand(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aCommandText, aSenderAddress);
	CleanupStack::Pop(self);
	return self;
	}

CSmsShellCommand::CSmsShellCommand(CCmdSms& aObserver)
	: CSmsEventBase(aObserver)
	{
	}

void CSmsShellCommand::ConstructL(const TDesC& aCommandText, const TDesC& aSenderAddress)
	{
	iSmsSender.CreateL(aSenderAddress);
	// Set up environment for script
	iScriptEnv = CEnvironment::NewL(iObserver.Env());
	_LIT(KSmsSenderEnvironmentVariable, "SMS_SENDER");
	iScriptEnv->SetL(KSmsSenderEnvironmentVariable, iSmsSender);

	const TDesC* scriptName = NULL;
	_LIT(KFshellName, "C:\\shared\\fshellscript%d.script");
	_LIT(KPerlName, "C:\\shared\\perlscript%d.pl");
	if (aCommandText.MatchF(_L("#!fshell*")) == 0)
		{
		iExeName = _L("fshell.exe");
		scriptName = &KFshellName;
		}
	else if (aCommandText.MatchF(_L("#!perl*")) == 0)
		{
		iExeName = _L("perl.exe");
		scriptName = &KPerlName;
		}
	else
		{
		User::Leave(KErrArgument);
		}	
	
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.CreateL(aCommandText.Length());
	buf.Copy(aCommandText);

	RFile scriptFile;
	CleanupClosePushL(scriptFile);
	TInt nonce = 0;
	iObserver.Fs().MkDirAll(*scriptName);
	TInt err = KErrNone;
	do {
		iScriptFile.Format(*scriptName, nonce);
		nonce++;
		err = scriptFile.Create(iObserver.Fs(), iScriptFile, EFileShareAny|EFileWrite);
		} while (err == KErrAlreadyExists);
	User::LeaveIfError(scriptFile.Write(buf));
	CleanupStack::PopAndDestroy(2, &buf);
	}

void CSmsShellCommand::StartL()
	{
	iProcess.CreateL(iExeName, iScriptFile, iObserver.IoSession(), iObserver.Stdin(), iObserver.Stdout(), iObserver.Stderr(), *iScriptEnv);
	iProcess.Run(iStatus);
	SetActive();
	}

void CSmsShellCommand::DoCancel()
	{
	iProcess.Process().LogonCancel(iStatus);
	}

CSmsShellCommand::~CSmsShellCommand()
	{
	Cancel();
	iProcess.Close();
	if (!iObserver.Debug())
		{
		iObserver.Fs().Delete(iScriptFile);
		}
	delete iScriptEnv;
	iSmsSender.Close();
	}

void CSmsShellCommand::RunL()
	{
	TExitType exitType = iProcess.Process().ExitType();
	TInt exitReason = iProcess.Process().ExitReason();
	TBool ok = exitType == EExitKill && exitReason == 0;
	if (ok)
		{
		iObserver.Printf(_L("Script from %S completed ok.\r\n"), &iSmsSender);
		}
	else
		{
		TInt err = exitReason;
		if (err >= 0) err = KErrGeneral;
		switch (iProcess.Process().ExitType())
			{
			case EExitKill:
				iObserver.PrintError(err, _L("Script %S failed, returned %d"), &iScriptFile, exitReason);
				break;
			case EExitTerminate:
				iObserver.PrintError(err, _L("Script %S terminated with %d"), &iScriptFile, exitReason);
				break;
			case EExitPanic:
				{
				TExitCategoryName cat = iProcess.Process().ExitCategory();
				iObserver.PrintError(err, _L("Script %S panicked with %S %d"), &iScriptFile, &cat, exitReason);
				break;
				}
			default:
				break;
			}
		}
	delete this;
	}
