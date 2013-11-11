// consoleextensions.inl
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

// Dummy class used to allow calls into protected Extension_ in a CBase derived class.
class CBaseExtensionDummy : public CBase
	{
public:
	using CBase::Extension_;
	};

TInt ConsoleMode::Set(CBase* aConsole, ConsoleMode::TMode aMode)
	{
	TAny* ignore;
	return ((CBaseExtensionDummy*)aConsole)->Extension_(KSetConsoleModeExtension, ignore, reinterpret_cast<TAny*>(aMode));
	}

TInt UnderlyingConsole::Set(CBase* aConsole, CConsoleBase* aUnderlyingConsole)
	{
	TAny* ignore;
	return ((CBaseExtensionDummy*)aConsole)->Extension_(KSetUnderlyingConsoleExtension, ignore, aUnderlyingConsole);
	}

ConsoleAttributes::TAttributes::TAttributes(TUint aAttributes, TColor aForegroundColor, TColor aBackgroundColor)
	: iAttributes(aAttributes), iForegroundColor(aForegroundColor), iBackgroundColor(aBackgroundColor)
	{
	}

TInt ConsoleAttributes::Set(CBase* aConsole, const TAttributes& aAttributes)
	{
	TAny* ignore;
	return ((CBaseExtensionDummy*)aConsole)->Extension_(KSetConsoleAttributesExtension, ignore, (TAny*)&aAttributes);
	}

TBool LazyConsole::IsLazy(CBase* aConsole)
	{
	TBool constructed = EFalse;
	TAny* ignore;
	TInt err = ((CBaseExtensionDummy*)aConsole)->Extension_(KLazyConsoleExtension, ignore, (TAny*)&constructed);
	return (err==KErrNone);
	}

TBool LazyConsole::IsConstructed(CBase* aConsole)
	{
	TBool constructed = EFalse;
	TAny* ignore;
	TInt err = ((CBaseExtensionDummy*)aConsole)->Extension_(KLazyConsoleExtension, ignore, (TAny*)&constructed);
	return (err==KErrNone) && (constructed);
	}

TInt ConsoleStdErr::Write(CBase* aConsole, const TDesC& aDes)
	{
	TAny* ignore;
	return ((CBaseExtensionDummy*)aConsole)->Extension_(KWriteStdErrConsoleExtension, ignore, const_cast<TAny*>((const TAny*)&aDes));
	}
