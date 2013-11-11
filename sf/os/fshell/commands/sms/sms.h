// sms.h
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

#ifndef __SMS_H__
#define __SMS_H__

#include <fshell/ioutils.h>
#include "smswatch.h"
#include "smssend.h"

using namespace IoUtils;

NONSHARABLE_CLASS(CCmdSms) : public CCommandBase, public MSmsSendObserver
	{
public:
	static CCmdSms* NewLC();
	~CCmdSms();

	// Functions for helper objects
	using CCommandBase::Fs;
	using CCommandBase::Printf;
	using CCommandBase::PrintError;
	using CCommandBase::IoSession;
	using CCommandBase::Stdin;
	using CCommandBase::Stdout;
	using CCommandBase::Stderr;
	TBool Debug() const { return iDebug; }
	RPointerArray<HBufC>& MatchStrings() { return iMatchStrings; }
		

private:
	CCmdSms();
	static TInt CompletionCallBack(TAny* aEvent);
	void Cleanup();
	
	// From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	
	// From MSmsListenerObserver
	virtual void EventsFinished(const TInt aError);
	
	// From MSmsSendObserver
	virtual void SendComplete(const TInt aError);
	
private:
	CAsyncCallBack* iCompletionCallBack;
	CSmsWatcher* iWatch;
	CSmsSender* iSendMsg;
	TInt iCompletionError;
	enum 
		{
		ESend, EReceive
		} iOperation;
	TBool iAutometricSms;
	HBufC* iMsgToSend;
	HBufC* iDestination;
	RPointerArray<HBufC> iMatchStrings;
	TBool iDebug;
	CTextBuffer* iMessageFromStdin;
	};

#endif // __SMS_H__
