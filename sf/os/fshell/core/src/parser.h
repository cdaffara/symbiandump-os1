// parser.h
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

#ifndef __PARSER_H__
#define __PARSER_H__

#include <e32base.h>
#include "error.h"
#include "pipe_line.h"

class CLexer;
class CParser;


class MParserObserver
	{
public:
	virtual void HandleParserComplete(CParser& aParser, const TError& aError) = 0;
	virtual void AboutToExecuteLine(const TDesC& aOrignalLine, const TDesC& aExpandedLine);
	virtual void LineReturned(TInt aError);
	};


class CParser : public CBase, public MPipeLineObserver
	{
public:
	enum TMode
		{
		ENormal				= 0x00000000,
		EKeepGoing			= 0x00000001,
		EDebug				= 0x00000002,
		EExportLineNumbers	= 0x00000004
		};
public:
	static CParser* NewL(TUint aMode, const TDesC& aDes, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MParserObserver* aObserver, TInt aStartingLineNumber = 1);
	~CParser();
	void Start();
	void Start(TBool& aIsForeground);
	void Kill();
	TInt Suspend();
	TInt Resume();
	TInt BringToForeground();
	void SendToBackground();
	TInt Reattach(RIoEndPoint& aStdinEndPoint, RIoEndPoint& aStdoutEndPoint, RIoEndPoint& aStderrEndPoint);
	TBool IsDisownable() const;
	void Disown();
private:
	enum TCondition
		{
		ENone,
		EAnd,
		EOr,
		EAndOr
		};
private:
	CParser(TUint aMode, const TDesC& aDes, RIoSession& aIoSession, RIoReadHandle& aStdin, RIoWriteHandle& aStdout, RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv, CCommandFactory& aFactory, MParserObserver* aObserver, TInt aStartingLineNumber);
	void ConstructL();
	void CreateNextPipeLine(TBool* aIsForeground);
	void CreateNextPipeLineL(TBool* aIsForeground);
	void FindNextPipeLineL(TPtrC& aData, TCondition& aCondition, TBool& aReachedLineEnd);
	HBufC* ExpandVariablesLC(const TDesC& aData);
	TInt SkipLineRemainder();
	void SkipToEnd();
	static TInt CompletionCallBack(TAny* aSelf);
	static TInt NextCallBack(TAny* aSelf);
	static TInt ExitCallBack(TAny* aSelf);
private:	// From MPipeLineObserver.
	virtual void HandlePipeLineComplete(CPipeLine& aPipeLine, const TError& aError);
private:
	const TUint iMode;
	const TPtrC iData;
	TCondition iCondition;
	RIoSession& iIoSession;
	RIoReadHandle iStdin;
	RIoWriteHandle iStdout;
	RIoWriteHandle iStderr;
	IoUtils::CEnvironment& iEnv;
	CCommandFactory& iFactory;
	CLexer* iLexer1;	///< Used to find a "pipe-line's worth" of data in iData.
	CLexer* iLexer2;	///< Used to parse a particular pipe-line (after its variables have been expanded).
	MParserObserver* iObserver;
	CPipeLine* iForegroundPipeLine;
	RPointerArray<CPipeLine> iBackgroundPipeLines;
	TError iCompletionError;
	CAsyncCallBack* iCompletionCallBack;
	CAsyncCallBack* iNextPipeLineCallBack;
	CAsyncCallBack* iExitCallBack;
	TBool iAbort;
	TInt iNextLineNumber;
	TBool iOwnsIoHandles;
	};


#endif // __PARSER_H__
