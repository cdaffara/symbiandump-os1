// wslog.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <bautils.h>
#include <W32STD.H>
#include <fshell/ltkutils.h>

using namespace IoUtils;

class CCmdWslog : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdWslog();
private:
	CCmdWslog();
	void CmdL(const TDesC& aCommand, const TDesC& aArgs);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	RWsSession iWs;
	enum 
		{
		EStatus,
		EEnable,
		EDisable,
		EMessage,
		EDump,
		} iOperation;
	HBufC* iMessage;
	};

CCommandBase* CCmdWslog::NewLC()
	{
	CCmdWslog* self = new(ELeave) CCmdWslog();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdWslog::~CCmdWslog()
	{
	iWs.Close();
	delete iMessage;
	}

CCmdWslog::CCmdWslog()
	{
	}

const TDesC& CCmdWslog::Name() const
	{
	_LIT(KName, "wslog");
	return KName;
	}

void CCmdWslog::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KOperation, "operation");
	aArguments.AppendEnumL((TInt&)iOperation, KOperation);
	_LIT(KMessage, "message");
	aArguments.AppendStringL(iMessage, KMessage);
	}

void CCmdWslog::OptionsL(RCommandOptionList&)
	{
	}

EXE_BOILER_PLATE(CCmdWslog)

void CCmdWslog::DoRunL()
	{
	if (iMessage && iOperation != EMessage)
		{
		LeaveIfErr(KErrArgument, _L("Message can only be specified if operation is 'message'"));
		}
	
	LeaveIfErr(iWs.Connect(FsL()), _L("Couldn't connect to wserv"));

	switch (iOperation)
		{
		case EEnable:
			iWs.LogCommand(RWsSession::ELoggingEnable);
			break;
		case EDisable:
			iWs.LogCommand(RWsSession::ELoggingDisable);
			break;
		case EMessage:
			{
			if (!iMessage)
				{
				LeaveIfErr(KErrArgument, _L("A message must be provided for operation 'message'"));
				}
			TLogMessageText message = *iMessage;
			iWs.LogMessage(message);
			}
			break;
		case EStatus:
			{
			Printf(_L("LtkUtils::W32CrackIsEnabled reports: %d\r\n"), LtkUtils::W32CrackIsEnabled());

			//_LIT(KWsIniFile, "z:\\system\\data\\wsini.ini");
			_LIT(KCWsIniFile, "c:\\system\\data\\wsini.ini");

			if (BaflUtils::FileExists(FsL(), KCWsIniFile))
				{
				Printf(_L("C drive wsini log lines:\r\n"));
				CmdL(_L("fshell.exe"), _L("-e 'cat c:\\system\\data\\wsini.ini | grep LOG*'"));
				}
			else
				{
				Printf(_L("(No C drive wsini)\r\n"));
				}
			Printf(_L("Z drive wsini log lines:\r\n"));
			CmdL(_L("fshell.exe"), _L("-e 'cat z:\\system\\data\\wsini.ini | grep LOG*'"));

			Printf(_L("Possible logging DLLs (none means logging probably not enabled):\r\n"));
			CmdL(_L("kerninfo.exe"), _L("codeseg -m DLOG*"));
			}
			break;
		case EDump:
			//__DEBUGGER();
			iWs.LogCommand(RWsSession::ELoggingStatusDump);
			break;
		};
	iWs.Flush();
	}

void CCmdWslog::CmdL(const TDesC& aCommand, const TDesC& aArgs)
	{
	RChildProcess proc;
	TRequestStatus stat;
	proc.CreateL(aCommand, aArgs, IoSession(), Stdin(), Stdout(), Stderr());
	proc.Run(stat);

	User::WaitForRequest(stat);
	proc.Close();
	}
