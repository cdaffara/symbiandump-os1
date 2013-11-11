// consoleextensions.h
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

#ifndef __CONSOLEEXTENSIONS_H__
#define __CONSOLEEXTENSIONS_H__

#include <e32base.h>

class ConsoleMode
	{
public:
	enum TMode
		{
		EBinary,
		EText,
		};
	static const TUint KSetConsoleModeExtension = 0x10285e15;
public:
	/**
	A console implementation that does not support different mode or a CBase derived class
	that does not derive from CConsoleBase will return KErrExtensionNotSupported if passed
	into this function.
	*/
	static inline TInt Set(CBase* aConsole, TMode aMode);
	};

class UnderlyingConsole
	{
public:
	static const TUint KSetUnderlyingConsoleExtension = 0x10287199;
public:
	/**
	Set the underlying console of another console. The underlying console may be used during construction
	of the actual console to display information or error messages, such as the IP address to connect to
	in the case of a TCP console implementation.
	
	Note, for consoles loaded within the IO server, the underlying console is write only. Attempting to
	read from it will cause a panic.
	*/
	static inline TInt Set(CBase* aConsole, CConsoleBase* aUnderlyingConsole);
	};

class ConsoleAttributes
	{
public:
	// Note these are also defined in win32cons.h
	enum TAttribute
		{
		ENone			= 0x0001,
		EBold			= 0x0002,
		EUnderscore		= 0x0004,
		EBlink			= 0x0008,
		EInverse		= 0x0010,
		EConceal		= 0x0020
		};
	enum TColor
		{
		EBlack,
		ERed,
		EGreen,
		EYellow,
		EBlue,
		EMagenta,
		ECyan,
		EWhite,
		EReset,
		EUnchanged
		};
	class TAttributes
		{
	public:
		inline TAttributes(TUint aAttributes, TColor aForegroundColor = EUnchanged, TColor aBackgroundColor = EUnchanged);
	public:
		TUint iAttributes;
		TColor iForegroundColor;
		TColor iBackgroundColor;
		};
	static const TUint KSetConsoleAttributesExtension = 0x10286B6E;
public:
	/**
	A console implementation that does not support different mode or a CBase derived class
	that does not derive from CConsoleBase will return KErrExtensionNotSupported if passed
	into this function.
	*/
	static inline TInt Set(CBase* aConsole, const TAttributes& aAttributes);
	};
	
class LazyConsole
	{
public:
	static const TUint KLazyConsoleExtension = 0x1028719e;
public:
	/**
	Returns true if the console is lazy.
	*/
	static inline TBool IsLazy(CBase* aConsole);
	/**
	Returns true if he console is lazy, AND it's been constructed.
	*/
	static inline TBool IsConstructed(CBase* aConsole);
	};

class ConsoleStdErr
	{
public:
	static const TInt KWriteStdErrConsoleExtension = 0x10286aaa;
public:
	/**
	Write to StdErr of the underlying console if the console supports this.
	*/
	static inline TInt Write(CBase* aConsole, const TDesC& aDes);
	};

// BC on this interface is not guaranteed - only for use by things inside /fshell/plugins/consoles
class MIosrvConsoleHelper
	{
protected:
	enum TVerbosity
		{
		EInformation,
		EError,
		EDebug,
		};
	IMPORT_C void Message(TVerbosity aVerbosity, TRefByValue<const TDesC> aFmt, ...);
	IMPORT_C TBool Debug() const;
	IMPORT_C void SetDebug(TBool aDebug);
	IMPORT_C void HandleConsoleCreationError(const TDesC& aConsoleName, TInt aError);
	IMPORT_C void CleanupUnderlyingConsole();
	IMPORT_C CConsoleBase* UnderlyingConsole() const;

	IMPORT_C TInt MIosrvConsoleHelper_Extension(TUint aExtensionId, TAny*& a0, TAny* a1);
	
	IMPORT_C virtual TInt WriteStdErr(const TDesC& aDes); // Default returns KErrExtensionNotSupported
	
private:
	TBool iDebug;
	CConsoleBase* iUnderlyingConsole;
	};

#include <fshell/consoleextensions.inl>
	
#endif //__CONSOLEEXTENSIONS_H__
