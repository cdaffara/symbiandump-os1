// consoleextensions.cpp
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

#include <fshell/consoleextensions.h>
#include <e32debug.h>
#include <e32cons.h>

class TOverflowTruncate : public TDes16Overflow
	{
public:
	virtual void Overflow(TDes16&) {}
	};
_LIT(KNewLine, "\r\n");


EXPORT_C void MIosrvConsoleHelper::Message(TVerbosity aVerbosity, TRefByValue<const TDesC> aFmt, ...)
	{
	if (Debug() || (aVerbosity == EInformation) || (aVerbosity == EError))
		{
		TOverflowTruncate overflow;
		VA_LIST list;
		VA_START(list, aFmt);
		TBuf<0x100> buf;
		buf.AppendFormatList(aFmt, list, &overflow);
		
		if (iUnderlyingConsole)
			{
			iUnderlyingConsole->Write(buf);
			iUnderlyingConsole->Write(KNewLine);
			}
		else
			{
			// Cover all the bases as best we can
			User::InfoPrint(buf);
			RDebug::Print(buf);
			}
		}
	}

EXPORT_C TBool MIosrvConsoleHelper::Debug() const
	{
	return iDebug;
	}

EXPORT_C void MIosrvConsoleHelper::SetDebug(TBool aDebug)
	{
	iDebug = aDebug;
	}

EXPORT_C TInt MIosrvConsoleHelper::WriteStdErr(const TDesC& /*aDes*/)
	{
	return KErrExtensionNotSupported;
	}

EXPORT_C TInt MIosrvConsoleHelper::MIosrvConsoleHelper_Extension(TUint aExtensionId, TAny*& /*a0*/, TAny* a1)
	{
	if (aExtensionId == UnderlyingConsole::KSetUnderlyingConsoleExtension)
		{
		iUnderlyingConsole = (CConsoleBase*)a1;
		return KErrNone;
		}
	else if (aExtensionId == ConsoleStdErr::KWriteStdErrConsoleExtension)
		{
		const TDesC* des = (const TDesC*)a1;
		return WriteStdErr(*des);
		}
	else
		{
		return KErrExtensionNotSupported;
		}
	}

EXPORT_C void MIosrvConsoleHelper::HandleConsoleCreationError(const TDesC& aConsoleName, TInt aError)
	{
	if (aError)
		{
		TBuf<512> message;
		message.Format(_L("Failed to create console %S (%d)."), &aConsoleName, aError);
			
		if (iUnderlyingConsole && (LazyConsole::IsConstructed(iUnderlyingConsole) || !LazyConsole::IsLazy(iUnderlyingConsole)))
		// if we have an underlyconsole, which is either not lazy or is lazy but already constructed, then print the error to it.
			{
			iUnderlyingConsole->Write(message);
			iUnderlyingConsole->Write(KNewLine);
			}
		else
		// else display a dialog
			{
			RNotifier notifier;
			if (notifier.Connect() == KErrNone)
				{
				TInt buttonVal;
				TRequestStatus notifierStatus;
				notifier.Notify(aConsoleName, message, _L("OK"), KNullDesC, buttonVal, notifierStatus);
				User::WaitForRequest(notifierStatus);
				notifier.Close();
				}
			}
		}
	Message(EDebug, _L("%S console create completed with err=%d"), &aConsoleName, aError);
	}

EXPORT_C void MIosrvConsoleHelper::CleanupUnderlyingConsole()
	{
	delete iUnderlyingConsole; // In case of leave during construction, this might still be non-null
	iUnderlyingConsole = NULL;
	}

EXPORT_C CConsoleBase* MIosrvConsoleHelper::UnderlyingConsole() const
	{
	return iUnderlyingConsole;
	}
