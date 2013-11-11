// mainfshell.cpp
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
#include "fed.h"
#include <e32cons.h>
#include <BADESCA.H>
#include <fshell/descriptorutils.h>

using namespace IoUtils;

class CCmdFed : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdFed();
	CConsoleBase* Console() { return iConsole; }
	TInt TabWidth() const { return iTabWidth; }
	void ShowHelpL();
private:
	CCmdFed();

private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void StdinChange(TUint aChange);
private:
	RArray<TFileName2> iFiles;
	CColorConsoleBase* iConsole;
	CFed* iFed;
	TInt iTabWidth;
	};

CCmdFed* gCmd = NULL; // Only used for the assertion handler and tab fns

CCommandBase* CCmdFed::NewLC()
	{
	CCmdFed* self = new(ELeave) CCmdFed();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFed::~CCmdFed()
	{
	iFiles.Close();
	delete iFed;
	delete iConsole;
	}

CCmdFed::CCmdFed()
	: CCommandBase(EManualComplete | ENotifyStdinChanges), iTabWidth(4)
	{
	}

const TDesC& CCmdFed::Name() const
	{
	_LIT(KName, "fed");	
	return KName;
	}

void CCmdFed::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendFileNameL(iFiles, _L("file"));
	}

void CCmdFed::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iTabWidth, _L("tab-width"));
	}

EXE_BOILER_PLATE(CCmdFed)

void CCmdFed::DoRunL()
	{
	iConsole = GetColorConsoleBaseL();
	gCmd = this;

	// Set the console title - nice for when using win32cons
	// Not so nice - fshell doesn't reset it when fed exits... -TomS
	/*
	_LIT(KFed, "fed ");
	RBuf cmdLine;
	cmdLine.CreateL(User::CommandLineLength() + KFed().Length());
	User::CommandLine(cmdLine);
	cmdLine.Insert(0, KFed);
	iConsole->SetTitle(cmdLine);
	cmdLine.Close();
	*/

	// Capture CTRL-C so we can (a) use it and (b) prevent users from accidentally nuking their work
	User::LeaveIfError(Stdin().CaptureKey(CTRL('c'), 0, 0));

	iFed = CFed::NewL(*iConsole, FsL());
	CDesC16ArrayFlat* argsBuf = new(ELeave) CDesC16ArrayFlat(Max(1, iFiles.Count()));
	CleanupStack::PushL(argsBuf);
	for (TInt i = 0; i < iFiles.Count(); i++)
		{
		argsBuf->AppendL(iFiles[i]);
		}
	CleanupStack::Pop(argsBuf);
	iFed->StartL(argsBuf);
	}

void CCmdFed::StdinChange(TUint aChange)
	{
	if (aChange & RIoReadHandle::EGainedForeground)
		{
		iFed->RedrawEverythingL();
		}
	}

void AssertionFail(const char* aAssertion, const char* aFile, TInt aLine)
	{
	__DEBUGGER();
	TBuf8<256> buf;
	TOverflowTruncate8 o;
	buf.AppendFormat(_L8("Assertion failed: \"%s\" in %s:%i\n"), &o, aAssertion, aFile, aLine);
	if (buf.Length()*2>buf.MaxLength()) buf.SetLength(buf.MaxLength()/2);
	gCmd->Console()->Write(buf.Expand());
	User::Panic(_L("Fed"), aLine);
	}

TInt KConsoleWidthCorrection = 0;
TInt KConsoleHeightCorrection = 0;

TInt TabWidth()
	{
	return gCmd->TabWidth();
	}

void ShowHelpL()
	{
	gCmd->ShowHelpL();
	}

void CCmdFed::ShowHelpL()
	{
	LtkUtils::RLtkBuf helpText;
	CleanupClosePushL(helpText);
	helpText.AppendL(_L("\
Fed Help\r\n\
========\
"));
	_LIT(KStart, "Console text editor.");
	CTextBuffer* helpBuf = const_cast<CTextBuffer*>(GetHelpTextL()); // nasty cast... GetHelpTextL shouldn't return a const obj...
	CleanupStack::PushL(helpBuf);
	TInt found = helpBuf->Descriptor().Find(KStart);
	helpText.AppendL(helpBuf->Descriptor().Mid(found + KStart().Length()));
	iFed->ShowHelpL(helpText.ToHBuf());
	CleanupStack::PopAndDestroy(2, &helpText); // helpBuf, helpText
	}
