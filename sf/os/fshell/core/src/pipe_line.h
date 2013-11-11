// pipe_line.h
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

#ifndef __PIPE_LINE_H__
#define __PIPE_LINE_H__

#include <e32base.h>
#include "command_wrappers.h"

class TError;
class CPipeLine;
class MConsole;
class CCommandFactory;
namespace IoUtils
	{
	class CEnvironment;
	}


class MPipeLineObserver
	{
public:
	virtual void HandlePipeLineComplete(CPipeLine& aPipeLine, const TError& aError) = 0;
	};


class RPipeSection
	{
public:
	RPipeSection();
	void Close();
	HBufC* GetCommandArguments() const;
public:
	class TRedirection
		{
	public:
		enum TType
			{
			ENotRedirected,
			EFile,
			EFileAppend,
			EHandle,
			ENull
			};
		enum THandle
			{
			EUnknown,
			EStdin,
			EStdout,
			EStderr
			};
	public:
		TRedirection();
		void SetFileNameL(const TDesC& aCwd, const TDesC& aName);
	public:
		TType iType;
		IoUtils::TFileName2* iFileName;
		THandle iHandle;
		};
public:
	TPtrC iFullName;
	TPtrC iCommandName;
	RArray<TPtrC> iCommandArguments;
	TRedirection iStdinRedirection;
	TRedirection iStdoutRedirection;
	TRedirection iStderrRedirection;
private:
	mutable HBufC* iCommandArgumentsBuf;
	};


class CPipeLine : public CBase, public MCommandObserver
	{
public:
	static CPipeLine* NewL(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, const RArray<RPipeSection>& aPipeSections, TBool aBackground, MPipeLineObserver* aObserver, TError& aErrorContext);
	static CPipeLine* NewLC(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, const RArray<RPipeSection>& aPipeSections, TBool aBackground, MPipeLineObserver* aObserver, TError& aErrorContext);
	~CPipeLine();
	void Kill();
	TInt Suspend();
	TInt Resume();
	TInt BringToForeground();
	void SendToBackground();
	const TDesC& Name() const;
	TInt Reattach(RIoEndPoint& aStdinEndPoint, RIoEndPoint& aStdoutEndPoint, RIoEndPoint& aStderrEndPoint);
	TBool IsDisownable() const;
	void Disown();
private:
	CPipeLine(RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MPipeLineObserver* aObserver);
	void ConstructL(const RArray<RPipeSection>& aPipeSections, TBool aBackground, TError& aErrorContext);
	static TInt CompletionCallBack(TAny* aSelf);
private:	// From MCommandObserver.
	virtual void HandleCommandComplete(MCommand& aCommand, TInt aError);
private:
	class RPipedCommand
		{
	public:
		RPipedCommand();
		void Close();
	public:
		MCommand* iCommand;
		TInt iCompletionError;
		HBufC* iCommandName;
		TBool iStdinRedirected;
		TBool iStdoutRedirected;
		TBool iStderrRedirected;
		};
private:
	TPtrC iName;
	RIoSession& iIoSession;
	RIoReadHandle& iStdin;
	RIoWriteHandle& iStdout;
	RIoWriteHandle& iStderr;
	IoUtils::CEnvironment& iEnv;
	CCommandFactory& iFactory;
	MPipeLineObserver* iObserver;
	RArray<RPipedCommand> iCommands;
	CAsyncCallBack* iCompletionCallBack;
	TError iCompletionError;
	};


#endif // __PIPE_LINE_H__
