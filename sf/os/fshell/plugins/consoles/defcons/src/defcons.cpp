// defcons.cpp
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

#include <e32std.h>
#include <e32cons.h>
#include <e32keys.h>
#include <e32uid.h>
#ifdef __WINS__
#include <e32wins.h>
#endif
#include <fshell/descriptorutils.h>


//
// Constants.
//

const LtkUtils::SLitC KConsoleImplementations[] =
	{
	DESC("guicons"),
	DESC("econseik"),
	DESC("econs"),
	DESC("nullcons")
	};
const TInt KNumConsoleImplementations = sizeof(KConsoleImplementations) / sizeof(LtkUtils::SLitC);


/**

  The default console implementation, used by iosrv. This console implentation has
  two reasons to exist:

  1) To hunt for a suitable real console implementation to be used by default. On
     GUI configurations this will either be guicons.dll or econseik.dll. On text
	 configurations this will be econs.dll.

  2) To delay the creation of the real console implementation until it is known
     to be needed. This is useful because CCommandBase creates a default console
	 even if --console has been specified on the command line. The default console
	 may be used by the specified console during its construction to interact with
	 the user (e.g. vt100tcpcons uses it to tell the user which TCP port and IP
	 address it is listening on). However, if it weren't to be used, the user
	 would see the default console briefly appear and then disappear when the
	 actual console is created. Delaying the creation of the console underlying
	 the default console avoids this.

*/
NONSHARABLE_CLASS(CDefaultConsole) : public CConsoleBase
	{
public:
	enum TPanicReason
		{
		EDoubleRead = 0,
		EUnableToCreateConsole = 1
		};
public:
	CDefaultConsole();
	virtual ~CDefaultConsole();
	virtual TInt Create(const TDesC &aTitle, TSize aSize);
	virtual void Read(TRequestStatus& aStatus);
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
private:
	void CreateIfRequired();
	void CreateIfRequired() const;
	TInt TryCreateConsole(const TDesC& aImplementation);
	void Panic(TPanicReason aReason);
private:
	HBufC* iTitle;
	TSize iSize;
	RLibrary iConsoleLibrary;
	CConsoleBase* iUnderlyingConsole;
	};

CDefaultConsole::CDefaultConsole()
	{
	}

CDefaultConsole::~CDefaultConsole()
	{
	delete iTitle;
	delete iUnderlyingConsole;
	iConsoleLibrary.Close();
	}

TInt CDefaultConsole::Create(const TDesC& aTitle, TSize aSize)
	{
	iTitle = aTitle.Alloc();
	if (iTitle)
		{
		iSize = aSize;
		return KErrNone;
		}
	return KErrNoMemory;
	}

void CDefaultConsole::Read(TRequestStatus& aStatus)
	{
	CreateIfRequired();
	iUnderlyingConsole->Read(aStatus);
	}

void CDefaultConsole::ReadCancel()
	{
	if (iUnderlyingConsole)
		{
		iUnderlyingConsole->ReadCancel();
		}
	}

void CDefaultConsole::Write(const TDesC& aDes)
	{
	CreateIfRequired();
	iUnderlyingConsole->Write(aDes);
	}

TPoint CDefaultConsole::CursorPos() const
	{
	CreateIfRequired();
	return iUnderlyingConsole->CursorPos();
	}

void CDefaultConsole::SetCursorPosAbs(const TPoint& aPoint)
	{
	CreateIfRequired();
	iUnderlyingConsole->SetCursorPosAbs(aPoint);
	}

void CDefaultConsole::SetCursorPosRel(const TPoint& aPoint)
	{
	CreateIfRequired();
	iUnderlyingConsole->SetCursorPosRel(aPoint);
	}

void CDefaultConsole::SetCursorHeight(TInt aPercentage)
	{
	CreateIfRequired();
	iUnderlyingConsole->SetCursorHeight(aPercentage);
	}

void CDefaultConsole::SetTitle(const TDesC& aTitle)
	{
	CreateIfRequired();
	iUnderlyingConsole->SetTitle(aTitle);
	}

void CDefaultConsole::ClearScreen()
	{
	CreateIfRequired();
	iUnderlyingConsole->ClearScreen();
	}

void CDefaultConsole::ClearToEndOfLine()
	{
	CreateIfRequired();
	iUnderlyingConsole->ClearToEndOfLine();
	}

TSize CDefaultConsole::ScreenSize() const
	{
	CreateIfRequired();
	return iUnderlyingConsole->ScreenSize();
	}

TKeyCode CDefaultConsole::KeyCode() const
	{
	CreateIfRequired();
	return iUnderlyingConsole->KeyCode();
	}

TUint CDefaultConsole::KeyModifiers() const
	{
	CreateIfRequired();
	return iUnderlyingConsole->KeyModifiers();
	}

TInt CDefaultConsole::Extension_(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	CreateIfRequired();
	return ((CDefaultConsole*)iUnderlyingConsole)->Extension_(aExtensionId, a0, a1); // Evil cast to work around the fact that Extension_ is protected in CConsoleBase.
	}

void CDefaultConsole::CreateIfRequired()
	{
	if (iUnderlyingConsole == NULL)
		{
		TInt err = KErrGeneral;
#ifdef __WINS__
		if (EmulatorNoGui())
			{
			err = TryCreateConsole(_L("nullcons"));
			}
		else if (EmulatorTextShell())
			{
			err = TryCreateConsole(_L("econs"));
			}
		else
			{
#endif
		for (TInt i = 0; i < KNumConsoleImplementations; ++i)
			{
			err = TryCreateConsole(KConsoleImplementations[i]);
			if (err == KErrNone)
				{
				break;
				}
			}

#ifdef __WINS__
			}
#endif

		__ASSERT_ALWAYS(err == KErrNone, Panic(EUnableToCreateConsole));
		}
	}

void CDefaultConsole::CreateIfRequired() const
	{
	const_cast<CDefaultConsole*>(this)->CreateIfRequired();
	}

TInt CDefaultConsole::TryCreateConsole(const TDesC& aImplementation)
	{
	TInt err = iConsoleLibrary.Load(aImplementation);
	if ((err == KErrNone) && (iConsoleLibrary.Type()[1] == KSharedLibraryUid) && (iConsoleLibrary.Type()[2] == KConsoleDllUid))
		{
		TLibraryFunction entry = iConsoleLibrary.Lookup(1);
		CConsoleBase* console = (CConsoleBase*)entry();
		if (console)
			{
			err = console->Create(*iTitle, iSize);
			}
		if (err == KErrNone)
			{
			iUnderlyingConsole = console;
			}
		else
			{
			delete console;
			iConsoleLibrary.Close();
			}
		}
	return err;
	}

void CDefaultConsole::Panic(TPanicReason aReason)
	{
	_LIT(KCategory, "defcons");
	User::Panic(KCategory, aReason);
	}

extern "C" EXPORT_C TAny *NewConsole()
	{
	return(new CDefaultConsole);
	}
