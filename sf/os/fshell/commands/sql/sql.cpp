// sql.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/common.mmh>

#include <fshell/ioutils.h>
using namespace IoUtils;

class TResponseLex
	{
public:
	TResponseLex(const TDesC& aResponse);
	
	TInt GetIntColon(TInt& aValue);
	TInt GetStringColon(TPtrC& aValue);
	TInt GetIntEof(TInt& aValue);
	TPtrC Remainder();
private:
	TPtrC iRemainder;
	};

class CCmdSqlCli : public CClientBase
	{
public:
/*	
	static const TDesC* PowerConsumptionToString(TPositionQuality::TPowerConsumption aPower);
	static void CapabilitiesToString(TPositionModuleInfo::TCapabilities aCap, TDes &aDes);
*/	
	void PrintTime(const TTime& aTime, TBool aNewline);	
	static CCommandBase* NewLC();
	~CCmdSqlCli();
private:
	enum TLineProcessingBehaviour
		{
		EIgnoreLines,
		EProcessLines,
		};
	
	CCmdSqlCli();
	
	void ProcessLineL(const TDesC& aLine);
	TBool MatchLineStart(const TDesC& aPrefix, TPtrC& aRemainder);
	void ProcessErrorL(const TDesC& aDes);
	void ProcessFailureL(const TDesC& aDes);
	void UnrecognizedLineL(const TDesC& aDes);
	void ProcessAsyncIdL(const TDesC& aDes);
	void ProcessTimeoutL(const TDesC& aDes);
	void ProcessResultL(const TDesC& aDes);
	
	void ProcessLogL(const TDesC& aDes);


private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void OptionsL(RCommandOptionList& aOptions);
	// From CClientBase.
	virtual void HandleServerResponseL(RPointerArray<HBufC> aLines);		
	};

///////////////////////////////////////////////////////////////////////////////

_LIT(KPersistentConsoleName, "fshell_sqlsrv");
_LIT(KSrvExeName, "fshell_sqlsrv");
_LIT(KNewLine, "\r\n");
_LIT(KPrompt, "ok\r\n");
_LIT(KErrorPrefix, "error:");
_LIT(KFailurePrefix, "failure:");
_LIT(KAsyncIdPrefix, "id:");
_LIT(KTimeoutPrefix, "timedout:");
_LIT(KResultPrefix, "result:");
_LIT(KLogPrefix, "log:");
_LIT(KColon, ":");


CCommandBase* CCmdSqlCli::NewLC()
	{
	CCmdSqlCli* self = new (ELeave)CCmdSqlCli();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSqlCli::CCmdSqlCli()
	: CClientBase(0, KSrvExeName, KPersistentConsoleName, KPrompt)
	{
	//iActiveWait = new (ELeave) CActiveSchedulerWait;	
	}


CCmdSqlCli::~CCmdSqlCli()
	{
	}

//////////////////////////////////////////////////////


//////////////////////////////////////////////////////

const TDesC& CCmdSqlCli::Name() const
	{
	_LIT(KName, "sql");
	return KName;
	}

void CCmdSqlCli::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KVerbose, "verbose");
	aOptions.AppendBoolL(iVerbose, KVerbose);
	}

void CCmdSqlCli::HandleServerResponseL(RPointerArray<HBufC> aLines)
	{
	const TInt numLines = aLines.Count();
	for (TInt i = 0; i < numLines; ++i)
		{
		ProcessLineL(*aLines[i]);
		}
	}

TBool CCmdSqlCli::MatchLineStart(const TDesC& aPrefix, TPtrC& aRemainder)
	{
	if (aRemainder.Left(aPrefix.Length()).Compare(aPrefix)==0)
		{
		aRemainder.Set(aRemainder.Mid(aPrefix.Length()));
		return ETrue;
		}
	return EFalse;
	}

void CCmdSqlCli::UnrecognizedLineL(const TDesC& aDes)
	{
	if (!iVerbose)
		{
		Write(aDes);
		Write(KNewLine);
		}
	}

void CCmdSqlCli::ProcessLogL(const TDesC& aDes)
	{
	TResponseLex lex(aDes);
	TInt id;
	TInt err = lex.GetIntColon(id);
	if (err != KErrNone)
		{
		UnrecognizedLineL(aDes);
		}
	}

void CCmdSqlCli::ProcessLineL(const TDesC& aLine)
	{
	TPtrC remainder(aLine);
	if (MatchLineStart(KErrorPrefix, remainder))
		{
		ProcessErrorL(remainder);
		}
	else if (MatchLineStart(KFailurePrefix, remainder))
		{
		ProcessFailureL(remainder);
		}
	else if (MatchLineStart(KAsyncIdPrefix, remainder))
		{
		ProcessAsyncIdL(remainder);
		}
	else if (MatchLineStart(KTimeoutPrefix, remainder))
		{
		ProcessTimeoutL(remainder);
		}
	else if (MatchLineStart(KResultPrefix, remainder))
		{
		ProcessResultL(remainder);
		}
	else if (MatchLineStart(KLogPrefix, remainder))
		{
		ProcessLogL(remainder);
		}
	else
		{
		UnrecognizedLineL(aLine);
		}
	}

void CCmdSqlCli::ProcessErrorL(const TDesC& aDes)
	{
	TResponseLex lex(aDes);
	TInt commandId;
	TInt errorFromServer;
	TInt err = lex.GetIntColon(errorFromServer);
	if (err == KErrNone)
		{
		err = lex.GetIntColon(commandId);
		}
		
	if (err == KErrNone)
		{
		TPtrC remander = lex.Remainder();
		LeaveIfErr(errorFromServer, remander);
		}
	else
		{
		UnrecognizedLineL(aDes);
		}
	}

void CCmdSqlCli::ProcessFailureL(const TDesC&)
	{
	LeaveIfErr(KErrGeneral, _L("Failed"));
	}
	
void CCmdSqlCli::ProcessAsyncIdL(const TDesC& aDes)
	{
	TResponseLex lex(aDes);
	TInt id;
	TInt err = lex.GetIntEof(id);
	if (err != KErrNone)
		{
		UnrecognizedLineL(aDes);
		}
	}

void CCmdSqlCli::ProcessTimeoutL(const TDesC& aDes)
	{
	TResponseLex lex(aDes);
	TInt id;
	TInt err = lex.GetIntEof(id);
	if (err == KErrNone)
		{
		LeaveIfErr(KErrTimedOut, _L("Command %d timed-out"), id);
		}
	else
		{
		UnrecognizedLineL(aDes);
		}
	}
	
void CCmdSqlCli::ProcessResultL(const TDesC& aDes)
	{
	TResponseLex lex(aDes);
	TInt id;
	TInt err = lex.GetIntColon(id);
	if (err == KErrNone)
		{
		if (id == 0)
			{
			TPtrC name;
			err = lex.GetStringColon(name);
			if (err == KErrNone)
				{
				Write(lex.Remainder());
				Write(KNewLine);
				}
			}
		}
	if (err != KErrNone)
		{
		UnrecognizedLineL(aDes);
		}
	}



EXE_BOILER_PLATE(CCmdSqlCli)

#define CASE_MODELIT(x) case x: { _LIT(KName, #x); pString = &KName; break; }

void CCmdSqlCli::PrintTime(const TTime& aTime, TBool aNewline)
	{	
	TTime NullTime = Time::NullTTime();
	if (aTime == NullTime) 
		{
		Printf(_L("(NullTime)"));
		}
	else
		{
		_LIT8(KDateTimeFormat, "%d-%02d-%02d %02d:%02d:%02d");
		TDateTime dt = aTime.DateTime();
		Printf(KDateTimeFormat, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
		}
	
	if (aNewline) Printf(_L("\r\n"));
	}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TResponseLex::TResponseLex(const TDesC& aResponse)
	: iRemainder(aResponse)
	{
	}

TInt TResponseLex::GetIntColon(TInt& aValue)
	{
	TInt pos = iRemainder.Find(KColon);
	if (pos == KErrNotFound)
		{
		return pos;
		}
	TPtrC intStr(iRemainder.Left(pos));
	TLex lex(intStr);
	TInt err = lex.Val(aValue);
	if (err!=KErrNone) return err;
	if (lex.Remainder().Length()) return KErrArgument;
	iRemainder.Set(iRemainder.Mid(pos+1));
	return KErrNone;
	}

TInt TResponseLex::GetStringColon(TPtrC& aValue)
	{
	TInt pos = iRemainder.Find(KColon);
	if (pos == KErrNotFound)
		{
		return pos;
		}
	aValue.Set(iRemainder.Left(pos));
	iRemainder.Set(iRemainder.Mid(pos+1));
	return KErrNone;
	}
	
TInt TResponseLex::GetIntEof(TInt& aValue)
	{
	TLex lex(iRemainder);
	TInt err = lex.Val(aValue);
	if (err==KErrNone)
		{
		if (lex.Remainder().Length()) err = KErrArgument;
		}
	if (err==KErrNone)
		{
		iRemainder.Set(lex.Remainder());
		}
	return err;
	}

TPtrC TResponseLex::Remainder()
	{
	return iRemainder;
	}
