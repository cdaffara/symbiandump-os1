// config.h
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


#include <e32std.h>
#include <f32file.h>

namespace LtkUtils
	{
	class CIniFile;
	}

class TIoConfig
	{
public:
	TIoConfig();
	TInt Init();
	const TDesC& ConsoleImplementation() const;
	const TSize& ConsoleSizeAdjustment() const;
	TBool ConsoleSizeDetect() const;
private:
	TInt DoInit(LtkUtils::CIniFile* iniFile);

private:
	TFileName iConsoleImplementation;
	TSize iConsoleSizeAdjustment;
	TBool iConsoleSizeDetect;
	};

