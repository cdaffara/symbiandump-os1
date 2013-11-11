// tefcons.cpp
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

#include <e32std.h>
#include <e32cons.h>
#include <e32keys.h>
#include <test/testexecutelogger.h>
#include <fshell/consoleextensions.h>

NONSHARABLE_CLASS(CTefConsole) : public CConsoleBase
	{
public:
	CTefConsole();
	virtual ~CTefConsole();
	virtual TInt Create(const TDesC &aTitle,TSize aSize);
	virtual void Read(TRequestStatus &aStatus);
	virtual void ReadCancel();
	virtual void Write(const TDesC &aDes);
	virtual TPoint CursorPos() const;
	virtual void SetCursorPosAbs(const TPoint &aPoint);
	virtual void SetCursorPosRel(const TPoint &aPoint);
	virtual void SetCursorHeight(TInt aPercentage);
	virtual void SetTitle(const TDesC &aTitle);
	virtual void ClearScreen();
	virtual void ClearToEndOfLine();
	virtual TSize ScreenSize() const;
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);

	void WriteStdErr(const TDesC &aDes);

private:
	CTestExecuteLogger* iLogger;
	};

CTefConsole::CTefConsole()
	{
	}

TInt CTefConsole::Create(const TDesC& aTitle, TSize /*aSize*/)
	{
	iLogger = new CTestExecuteLogger();
	if (!iLogger) return KErrNoMemory;
	TInt logMode = TLoggerOptions(ELogHTMLOnly);
	TInt logLevel = RFileFlogger::TLogSeverity(ESevrAll);
	iLogger->SetLoggerOptions(logMode);
	TInt err = iLogger->Connect();
	if (err) return err;
	const TDesC& logFilePath(aTitle);
	err = iLogger->HtmlLogger().CreateLog(logFilePath, RTestExecuteLogServ::ELogModeAppend);
	if (err) return err;
	iLogger->HtmlLogger().SetLogLevel(TLogSeverity(logLevel));

	iLogger->HtmlLogger().Write(_L("<pre>\r\n")); // horrible horrible tef logging - the client is responsible for writing markup!
	return KErrNone;
	}


CTefConsole::~CTefConsole()
	{
	if (iLogger)
		{
		iLogger->HtmlLogger().Write(_L("</pre>\r\n")); // horrible horrible tef logging - the client is responsible for writing markup!
		iLogger->Close();
		delete iLogger;
		}
	}

void CTefConsole::Read(TRequestStatus &aStatus)
	{
	TRequestStatus *pS=(&aStatus);
	User::RequestComplete(pS, KErrNotSupported);
	}

void CTefConsole::ReadCancel()
	{
	}

void CTefConsole::Write(const TDesC& aDes)
	{
	//TODO sort out the file and line
	//iLogger->LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, aDes);
	iLogger->HtmlLogger().Write(aDes);
	}

void CTefConsole::WriteStdErr(const TDesC &aDes)
	{
	//iLogger->LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, aDes);
	iLogger->HtmlLogger().Write(aDes);
	}

TPoint CTefConsole::CursorPos() const
	{
	return TPoint(0,0);
	}

void CTefConsole::SetCursorPosAbs(const TPoint& /*aPoint*/)
	{
	}

void CTefConsole::SetCursorPosRel(const TPoint& /*aPoint*/)
	{
	}

void CTefConsole::SetCursorHeight(TInt /*aPercentage*/)
	{
	}

void CTefConsole::SetTitle(const TDesC& /*aTitle*/)
	{
	}

void CTefConsole::ClearScreen()
	{
	}

void CTefConsole::ClearToEndOfLine()
	{
	}

TSize CTefConsole::ScreenSize() const
	{
	return TSize(10,10);
	}

TKeyCode CTefConsole::KeyCode() const
	{
	return EKeyNull;
	}

TUint CTefConsole::KeyModifiers() const
	{
	return 0;
	}

extern "C" EXPORT_C TAny *NewConsole()
	{
	return(new CTefConsole);
	}

TInt CTefConsole::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	if (aExtensionId == ConsoleStdErr::KWriteStdErrConsoleExtension)
		{
		TDesC* des = (TDesC*)a1;
		WriteStdErr(*des);
		return KErrNone;
		}
	else
		{
		return CConsoleBase::Extension_(aExtensionId, a0, a1);
		}
	
	}
