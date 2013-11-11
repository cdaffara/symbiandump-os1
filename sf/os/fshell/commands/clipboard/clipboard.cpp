// clipboard.cpp
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

#include <fshell/ioutils.h>
#include <fshell/ltkutils.h>

using namespace IoUtils;

class CCmdClipboard : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdClipboard();
private:
	CCmdClipboard();
	void CopyToClipboardL(const TDesC& aBuf);
	HBufC* GetFromClipboardL();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	HBufC* iString;
	TBool iPipe;
	};


CCommandBase* CCmdClipboard::NewLC()
	{
	CCmdClipboard* self = new(ELeave) CCmdClipboard();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdClipboard::~CCmdClipboard()
	{
	delete iString;
	}

CCmdClipboard::CCmdClipboard()
	{
	}

const TDesC& CCmdClipboard::Name() const
	{
	_LIT(KName, "clipboard");	
	return KName;
	}

void CCmdClipboard::ArgumentsL(RCommandArgumentList& aArguments)
	{
	_LIT(KArg, "text");
	aArguments.AppendStringL(iString, KArg);
	}

void CCmdClipboard::OptionsL(RCommandOptionList& aOptions)
	{
	_LIT(KOptStdin, "stdin");
	aOptions.AppendBoolL(iPipe, KOptStdin);
	}

void CCmdClipboard::DoRunL()
	{
	if (iString)
		{
		CopyToClipboardL(*iString);
		}
	else if (iPipe)
		{
		// Copy to clipboard from stdin
		CTextBuffer* buffer = CTextBuffer::NewLC(0x100);
		Stdin().SetReadMode(RIoReadHandle::EOneOrMore);
		TBuf<0x100> buf;
		while (Stdin().Read(buf) == KErrNone)
			{
			buffer->AppendL(buf);
			}

		CopyToClipboardL(buffer->Descriptor());
		CleanupStack::PopAndDestroy(buffer);
		}
	else
		{
		HBufC* res = NULL;
		TRAPD(err, res = GetFromClipboardL());
		LeaveIfErr(err, _L("Couldn't read clipboard (clipboard is empty?)"));

		TBool cons = Stdout().AttachedToConsole();
		if (cons) Printf(_L("Clipboard contents, length=%d:\r\n"), res->Length());
		Write(*res);
		if (cons) Write(_L("\r\n"));
		delete res;
		}
	}

void CCmdClipboard::CopyToClipboardL(const TDesC& aBuf)
	{
	LtkUtils::CopyToClipboardL(aBuf, &FsL());
	}

HBufC* CCmdClipboard::GetFromClipboardL()
	{
	return LtkUtils::GetFromClipboardL(&FsL());
	}

EXE_BOILER_PLATE(CCmdClipboard)

