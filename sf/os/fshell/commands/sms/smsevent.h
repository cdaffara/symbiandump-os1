// smsevent.h
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

#ifndef __SMS_EVENT_H__
#define __SMS_EVENT_H__

#include <e32base.h>
#include <etelmm.h>
#include <fshell/ioutils.h>

using namespace IoUtils;

class CCmdSms;

//
// CSmsEventBase
// abstrace base class providing functionality
// common to all sms events
//
class CSmsEventBase : public CActive
	{
public:
	// pure virtuals 
	virtual void StartL() = 0;

protected:
	CSmsEventBase(CCmdSms& aObserver);

protected:
	CCmdSms& iObserver;
	};

//
// CSmsEventDial
// functionality specific to a dial event
// 
NONSHARABLE_CLASS(CSmsEventDial) : public CSmsEventBase
	{
public:
	static CSmsEventDial* NewL(CCmdSms& aObserver, RCall::TTelNumberC& aTelNumber);
	virtual ~CSmsEventDial();

	// from CSmsEventBase 
	virtual void StartL();
	
	// from CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
private:
	CSmsEventDial(CCmdSms& aObserver);
	void ConstructL(RCall::TTelNumberC& aTelNumber);
private:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMobileLine iLine;
	RMobileCall iCall;
	RMobilePhone::TMobileAddress iNumber;
	TName iCallName;
	RMobileCall::TMobileCallStatus iCallStatus;
	RTimer iTimeout;
	
	enum TSmsEventState
		{
		ESmsEventNetworkIdle,
		ESmsEventDial,
		ESmsEventStatusChange,
		ESmsEventWatchStatus
		};
	TSmsEventState iState;
	};

NONSHARABLE_CLASS(CSmsShellCommand) : public CSmsEventBase
	{
public:
	static CSmsShellCommand* NewL(CCmdSms& aObserver, const TDesC& aCommandText, const TDesC& aSenderAddress);
	~CSmsShellCommand();
	void StartL();

private:
	CSmsShellCommand(CCmdSms& aObserver);
	void ConstructL(const TDesC& aCommandText, const TDesC& aSenderAddress);
	void RunL();
	void DoCancel();

private:
	RChildProcess iProcess;
	TFileName iScriptFile;
	TBuf<32> iExeName;
	CEnvironment* iScriptEnv;
	RBuf iSmsSender;
	};

#endif // __SMS_EVENT_H__
