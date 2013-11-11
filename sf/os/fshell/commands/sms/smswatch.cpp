// smswatch.cpp
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

#include "smswatch.h"
#include <gsmumsg.h>
#include <gsmubuf.h>
#include <smsustrm.h>
#include "sms.h"

// __TEST_MODE__ provides a dummy autometric command
//#define __TEST_MODE__

#undef ASSERT
#define ASSERT(x) if (!(x)) { iParent.Printf(_L8("Assertion failed at line %d: " #x "\r\n"), __LINE__); PanicMe(__LINE__); }

// local static method definitions
static void PanicMe(TInt aError)
	 {
	 _LIT(KSmsWatcherPanic, "AmSms");
	 User::Panic(KSmsWatcherPanic, aError);	 
	 }

//
// CSmsWatcher
//
CSmsWatcher* CSmsWatcher::NewL(CCmdSms& aParent)
	{
	CSmsWatcher* self = new (ELeave) CSmsWatcher(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSmsWatcher::CSmsWatcher(CCmdSms& aParent)
	: CActive(CActive::EPriorityStandard), iParent(aParent), iState(ESmsListenIdle)
	{
	CActiveScheduler::Add(this);
	}

CSmsWatcher::~CSmsWatcher()
	{
	Cancel();
	delete iMessage;
	iCommand.Close();
	if (iSocket.SubSessionHandle() > 0)
		iSocket.Close();
	if (iSocketServer.Handle() > 0)
		iSocketServer.Close();
	}

void CSmsWatcher::ConstructL()
	{
	// connect to the sms socket
	User::LeaveIfError(iSocketServer.Connect());
	User::LeaveIfError(iSocket.Open(iSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol));
	iSmsAddr.SetSmsAddrFamily(ESmsAddrMatchText);
	_LIT8(KShebang, "#!");
	iSmsAddr.SetTextMatch(KShebang());
	User::LeaveIfError(iSocket.Bind(iSmsAddr));
	}

void CSmsWatcher::DoCancel()
	{
	switch(iState)
		{
		case ESmsListenReadSms:
		case ESmsListenProcessSms:
		case ESmsListenIdle: // This could happen when signalling failure via KIoctlReadMessageFailed
			iSocket.CancelIoctl();
			break;
		default:
			PanicMe(__LINE__);
		}
	}


void CSmsWatcher::RunL()
	{
	iErrorMessage.Zero();
	if (iStatus.Int() < 0)
		{
		iErrorMessage.AppendFormat(_L("Error reported from CSmsWatcher::RunL"));
		User::Leave(iStatus.Int()); // process any errors elsewhere
		}
	
	switch(iState)
		{
		case ESmsListenReadSms:
			ReadSmsAndRespondToStackL(); // inform the stack we've got the sms
			break;
		
		case ESmsListenProcessSms:
			ProcessCommandL(); // process the contents of the sms
			WaitForMessage(); // Start listening for another message (RunError will call it if ProcessCommandL leaves)
			break;
#ifdef __TEST_MODE__
		case ESmsListenTestMode:
			{
			// a test mode skipping reception & decoding of an autometric sms
			//_LIT(KTestMsg, "!#AUTOMETRIC;d:0243261943;?");
			_LIT(KTestMsg, "#!fshell\ninfoprint \"hello world!\"\n");
			ParseBufferL(KTestMsg, _L("012345678"));
			}
			break;
#endif
		case ESmsListenIdle:
			//CompleteWatch(KErrCompletion); // indicate an error with a prior event
			break;
		
		default:
			PanicMe(KErrUnknown); // design fault
			break;
		}
	}

//
// CSmsWatcher::RunError
// handle any probs with the state machine 
//
TInt CSmsWatcher::RunError(TInt aError)
	{
	switch (iState)
		{
		case ESmsListenProcessSms:
			// Problem such as bad message - log it and carry on
			if (iErrorMessage.Length())
				{
				iParent.PrintError(aError, iErrorMessage);
				}
			else
				{
				iParent.PrintError(aError, _L("Error when processing SMS"));
				}
			WaitForMessage(); // Start listening for another message
		break;
		
		case ESmsListenReadSms:
			{
			// inform the stack we've failed to retrieve the sms. Likely due to OOM when calling PrepBuffersL
			iSocket.Ioctl(KIoctlReadMessageFailed, iStatus, &iIoctlResult, KSolSmsProv);
			SetActive();
			iState = ESmsListenIdle;
			}
		break;
		
		default:
			PanicMe(__LINE__); // design fault
		break;
		};
	return KErrNone;
	}

//
// CSmsWatcher::WaitForMessage
// posts an async. read against the sms socket waiting for an incoming sms
//
void CSmsWatcher::WaitForMessage()
	{
	ASSERT(!IsActive());

#ifndef __TEST_MODE__
	iIoctlResult = KSockSelectRead;
	ASSERT(iSocket.SubSessionHandle() > 0);
	iSocket.Ioctl(KIOctlSelect, iStatus, &iIoctlResult, KSOLSocket);
	iState = ESmsListenReadSms;
#else
	TRequestStatus* status = &iStatus;
	*status = KRequestPending;
	User::RequestComplete(status, KErrNone);
	iState = ESmsListenTestMode;
#endif
	SetActive();
	}

//
// CSmsWatcher::ReadSmsAndRespondToStackL
// retrieve the sms and inform the stack whether we've got it
//
void CSmsWatcher::ReadSmsAndRespondToStackL()
	{
	// prep. buffers used to hold the message
	PrepBuffersL();
	
	// retrieve the sms from the stack
	RSmsSocketReadStream readstream1(iSocket);
	CleanupClosePushL(readstream1);
	iMessage->InternalizeL(readstream1);
	CleanupStack::PopAndDestroy();
	
	// inform the stack we've got the sms
	iSocket.Ioctl(KIoctlReadMessageSucceeded, iStatus, &iIoctlResult, KSolSmsProv);
	iState = ESmsListenProcessSms;
	SetActive();
	}

//
// CSmsWatcher::ProcessCommandL
// having captured the sms that contains the command, slurp it out & process any instructions contained therein
//
void CSmsWatcher::ProcessCommandL()
	{
	ASSERT(iMessage);
	ASSERT(iCommand.Length() == 0);
	const TInt length = iMessage->Buffer().Length();
	iCommand.CreateL(length);
	iMessage->Buffer().Extract(iCommand, 0, length);
	TPtrC address = iMessage->ToFromAddress();
	ParseBufferL(iCommand, address);
	}

//
// CSmsWatcher::PrepBuffersL
// trash the old buffer data, replace with virgin ones
//
void CSmsWatcher::PrepBuffersL()
	{
	delete iMessage;
	iMessage = NULL;
	iCommand.Close();
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CleanupStack::PushL(buffer);
	iMessage = CSmsMessage::NewL(iParent.Fs(), CSmsPDU::ESmsDeliver, buffer);
	CleanupStack::Pop(buffer); // iMessage takes ownership
	}

//
// CSmsWatcher::ParseInstructionsL
// interpret the instructions received OTA
// eg: "#!fshell\necho Hello!\n"
//
void CSmsWatcher::ParseBufferL(const TDesC& aInstruction, const TDesC& aSenderAddress)
	{
	TBool accept = EFalse;
	RPointerArray<HBufC>& matchStrings = iParent.MatchStrings();
	if (matchStrings.Count()==0)
		{
		accept = ETrue;
		}
	else
		{
		for (TInt i=0; i<matchStrings.Count(); ++i)
			{
			Printf(_L("Sender %S matches %S: "), &aSenderAddress, matchStrings[i]);
			if (aSenderAddress.Match(*matchStrings[i])!=KErrNotFound)
				{
				accept = ETrue;
				}
			}
		}
		
	if (!accept)
		{
		iErrorMessage.Zero();
		iErrorMessage.AppendFormat(_L("Rejecting SMS from %S: did not match authorised sender"), &aSenderAddress);
		User::Leave(KErrAccessDenied);
		}
	
	
	if (aInstruction.MatchF(_L("#!fshell*")) == 0 || aInstruction.MatchF(_L("#!perl*")) == 0)
		{
		CSmsShellCommand* command = CSmsShellCommand::NewL(iParent, aInstruction, aSenderAddress);
		command->StartL();		
		}
	else
		{
		TPtrC firstLine(aInstruction);
		TInt pos = firstLine.Find(_L("\n"));
		if (pos!=KErrNotFound)
			{
			firstLine.Set(firstLine.Left(pos));
			if ((firstLine.Length()>0)&&(firstLine[firstLine.Length()-1]=='\r'))
				{
				firstLine.Set(firstLine.Left(firstLine.Length()-1));
				}
			}
		iErrorMessage.Zero();
		iErrorMessage.AppendFormat(_L("Unhandled shebang command from %S: %S"), &aSenderAddress, &firstLine);
		User::Leave(KErrArgument);
		}
	}

//
// CSmsWatcher::Message
// 
const TDesC& CSmsWatcher::Message()
	{
	return iCommand;
	}
