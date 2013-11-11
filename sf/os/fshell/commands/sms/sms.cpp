// sms.cpp
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

#include "sms.h"

CCmdSms* CCmdSms::NewLC()
	{
	CCmdSms* self = new (ELeave) CCmdSms();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSms::~CCmdSms()
	{
	delete iCompletionCallBack;
	delete iWatch;
	delete iSendMsg;
	delete iMsgToSend;
	delete iDestination;
	delete iMessageFromStdin;
	iMatchStrings.ResetAndDestroy();
	}

CCmdSms::CCmdSms():
CCommandBase(CCommandBase::EManualComplete)
	{
	}

const TDesC& CCmdSms::Name() const
	{
	_LIT(KName, "sms");	
	return KName;
	}

//
// CCmdSms::DoRunL
//
void CCmdSms::DoRunL()
	{
	iCompletionCallBack = new(ELeave) CAsyncCallBack(TCallBack(CompletionCallBack, this), CActive::EPriorityStandard);

	switch (iOperation)
		{
	case ESend:
			{
			if (!iDestination)
				{
				LeaveIfErr(KErrArgument, _L("Option --destination not specified for send."));
				}
			if (!iMsgToSend)
				{
				iMessageFromStdin = CTextBuffer::NewL(160);
				Stdin().SetReadMode(RIoReadHandle::EFull);
				TBuf<0x100> buf;
				while (Stdin().Read(buf) == KErrNone)
					{
					iMessageFromStdin->AppendL(buf);
					}
				
				iSendMsg = CSmsSender::NewL(FsL(), *this, iMessageFromStdin->Descriptor(), *iDestination);
				}
			else
				{
				iSendMsg = CSmsSender::NewL(FsL(), *this, *iMsgToSend, *iDestination);
				}
			iSendMsg->StartL();
			}
		break;
	case EReceive:
			{
			FsL(); // So that CSmsWatcher can call Fs() if it needs to
			iWatch = CSmsWatcher::NewL(*this);
			iWatch->WaitForMessage();
			}
		break;
		};
	}

void CCmdSms::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iOperation, _L("command"));
	aArguments.AppendStringL(iMsgToSend, _L("message"));
	}

//
// CCmdSms::OptionsL
//
void CCmdSms::OptionsL(RCommandOptionList& aOptions)
	{
	// send sms options
	_LIT(KOptDestination, "destination");
	aOptions.AppendStringL(iDestination, KOptDestination);
	
	// receive sms options
	_LIT(KOptDebug, "no-delete");
	aOptions.AppendBoolL(iDebug, KOptDebug);
	
	_LIT(KOptMatch, "match");
	aOptions.AppendStringL(iMatchStrings, KOptMatch);
	}

//
// CCmdSms::SendComplete
// up-call from the underlying smssender to indicate
// it has finished its task
//
void CCmdSms::SendComplete(TInt aError)
	{
	EventsFinished(aError);
	}

//
// CCmdSms::EventsFinished
// up-call from the underlying smswatcher to indicate
// it has finished its task(s)
//
void CCmdSms::EventsFinished(const TInt aError)
	{
	iCompletionError = aError;
	iCompletionCallBack->CallBack();
	}

//
// CCmdSms::CompletionCallBack
// static function called in the event the cmd has finished executing
// 
TInt CCmdSms::CompletionCallBack(TAny* aEvent)
	{
	CCmdSms* self = static_cast<CCmdSms*>(aEvent);
	self->Cleanup();
	return KErrNone;
	}

//
// CCmdSms::Cleanup
// 
void CCmdSms::Cleanup()
	{
	if (iCompletionError)
		PrintError(iCompletionError, _L("amsms.exe failure"));
	else
		{
		if (iWatch)
			{
			// dump any underlying messages (if any!)
			const TDesC& msg = iWatch->Message();
			if (msg.Length() > 0)
				Printf(_L("Message received: \r\n"), &msg); // dump the message to stdout
			}
		}
	// complete the fshell command
	Complete(iCompletionError);
	}

EXE_BOILER_PLATE(CCmdSms);

