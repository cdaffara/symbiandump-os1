// paste.cpp
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
#include <fshell/ltkutils.h>
#include <e32keys.h>
#include <txtetext.h>
#include <baclipb.h>

using namespace IoUtils;

class CCmdPaste : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPaste();
private:
	CCmdPaste();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	HBufC* iString;
	TBool iStdin;
	};


CCommandBase* CCmdPaste::NewLC()
	{
	CCmdPaste* self = new(ELeave) CCmdPaste();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdPaste::~CCmdPaste()
	{
	delete iString;
	}

CCmdPaste::CCmdPaste()
	{
	}

const TDesC& CCmdPaste::Name() const
	{
	_LIT(KName, "paste");	
	return KName;
	}

void CCmdPaste::DoRunL()
	{
	if (iString && iStdin)
		{
		LeaveIfErr(KErrArgument, _L("Cannot read text from both command line and stdin."));
		}

	if (iString)
		{
		LtkUtils::CopyToClipboardL(*iString, &FsL());
		}
	else if (iStdin)
		{
		// Copy to clipboard from stdin
		CTextBuffer* buffer = CTextBuffer::NewLC(0x100);
		Stdin().SetReadMode(RIoReadHandle::EOneOrMore);
		TBuf<0x100> buf;
		while (Stdin().Read(buf) == KErrNone)
			{
			buffer->AppendL(buf);
			}

		LtkUtils::CopyToClipboardL(buffer->Descriptor(), &FsL());
		CleanupStack::PopAndDestroy(buffer);
		}

	TInt err = LtkUtils::InjectRawKeyEvent('V', EModifierCtrl, 0); // Send a CTRL-V to the frontmost app.
	LeaveIfErr(err, _L("Failed to inject ctrl-v event"));
	}

void CCmdPaste::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendStringL(iString, _L("text"));
	}

void CCmdPaste::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iStdin, _L("stdin"));
	}


EXE_BOILER_PLATE(CCmdPaste)

