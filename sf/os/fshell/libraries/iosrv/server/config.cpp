// config.cpp
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "config.h"
#include <fshell/settings.h>


// Default values (used if the config file can't be read).
_LIT(KDefaultConsole, "defcons.dll");
const TSize KDefaultSizeAdjustment(0, 0);

// Config file details.
_LIT(KConfigFileName, "\\system\\console\\iosrv.ini");
_LIT(KConfigDescriptionFile, "\\resource\\iosrv.idf");

// Configuration attributes.
_LIT(KAttConsole, "console");
_LIT(KAttConsoleSizeAdjustment, "console_size_adjustment");
_LIT(KAttConsoleSizeDetect, "console_size_detect");


TIoConfig::TIoConfig()
	: iConsoleImplementation(KDefaultConsole), iConsoleSizeAdjustment(KDefaultSizeAdjustment), iConsoleSizeDetect(ETrue)
	{
	}

TInt TIoConfig::Init()
	{
	LtkUtils::CIniFile* iniFile = NULL;
	TRAPD(ret, iniFile = LtkUtils::CIniFile::NewL(KConfigFileName, KConfigDescriptionFile));
	if (ret == KErrNone) ret = DoInit(iniFile);
	delete iniFile;
	return ret;
	}

TInt TIoConfig::DoInit(LtkUtils::CIniFile* iniFile)
	{
	TInt ret = KErrNone;
	iConsoleImplementation.Copy(iniFile->GetString(KAttConsole).Left(iConsoleImplementation.MaxLength()));
	iConsoleSizeDetect = iniFile->GetBool(KAttConsoleSizeDetect);
	const TDesC& sizeAdjust(iniFile->GetString(KAttConsoleSizeAdjustment));
	if (sizeAdjust.Length())
		{
		TLex lex(sizeAdjust);
		ret = lex.Val(iConsoleSizeAdjustment.iWidth);
		if (ret) return ret;
		lex.Get(); // Skip delimiter.
		ret = lex.Val(iConsoleSizeAdjustment.iHeight);
		if (ret) return ret;
		iConsoleSizeDetect = EFalse;
		}
	return ret;
	}

const TDesC& TIoConfig::ConsoleImplementation() const
	{
	return iConsoleImplementation;
	}

const TSize& TIoConfig::ConsoleSizeAdjustment() const
	{
	return iConsoleSizeAdjustment;
	}

TBool TIoConfig::ConsoleSizeDetect() const
	{
	return iConsoleSizeDetect;
	}
