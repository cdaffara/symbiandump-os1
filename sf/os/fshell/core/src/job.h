// job.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#ifndef __JOB_H__
#define __JOB_H__

#include <e32base.h>
#include "parser.h"

class CJob;
class RIoSession;
class MConsole;
class CParser;
class CCommandFactory;

class MJobObserver
	{
public:
	virtual void HandleJobComplete(CJob& aJob, const TError& aError) = 0;
	};


class CJob : public CBase, public MParserObserver
	{
public:
	enum TStatus
		{
		EPending,
		ERunning,
		EStopped,
		EComplete
		};
public:
	static CJob* NewL(TInt aId, const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MJobObserver* aObserver);
	~CJob();
	void Start(TBool& aIsForeground);
	void Kill();
	void Suspend();
	void Resume();
	TInt BringToForeground();
	void SendToBackground();
	TInt Id() const;
	const TDesC* Name() const;
	TStatus Status() const;
	TInt Reattach(RIoEndPoint& aStdinEndPoint, RIoEndPoint& aStdoutEndPoint, RIoEndPoint& aStderrEndPoint);
	TBool IsDisownable() const;
	void Disown();
private:
	CJob(TInt aId, MJobObserver* aObserver, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv);
	void ConstructL(const TDesC& aCommandLine, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory);
	static TInt CompletionCallBack(TAny* aSelf);
private:	// From MParserObserver.
	virtual void HandleParserComplete(CParser& aParser, const TError& aError);
private:
	TInt iId;
	TStatus iStatus;
	MJobObserver* iObserver;
	HBufC* iCommandLine;
	TError iCompletionError;
	CAsyncCallBack* iCompletionCallBack;
	CParser* iParser;
	};

#endif // __JOB_H__
