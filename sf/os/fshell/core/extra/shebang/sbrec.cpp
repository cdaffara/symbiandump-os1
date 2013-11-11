// sbrec.cpp
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// Description:
// Recognizes files that begin with a "shebang" ("#!").

#include <e32debug.h>
#include <apmrec.h>
#include <apmstd.h>
#include <implementationproxy.h> 
#include <fshell/common.mmh>
#include "sbrec.h"


//
// Constants.
//

const TUid KShebangRecognizerUid = {FSHELL_UID_SBREC};
const TInt KShebangRecognizerImplementationUidValue = {0x102835C1};
_LIT8(KShebang, "#!");
_LIT8(KShebangDataType, "X-Symbian-Shebang");

#ifdef FSHELL_PATCHABLE_CONSTANTS_SUPPORT
EXPORT_C extern const TBool KAutoStartFshell; // from patchdata.cpp
#endif

//
// CShebangRecognizer.
//
 
CShebangRecognizer::CShebangRecognizer()
	: CApaDataRecognizerType(KShebangRecognizerUid, CApaDataRecognizerType::EHigh)
	{
	iCountDataTypes = 1;
	}

TUint CShebangRecognizer::PreferredBufSize()
	{
	return 2;
	}

TDataType CShebangRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
	{
	return TDataType(KShebangDataType);
	}

void CShebangRecognizer::DoRecognizeL(const TDesC& /*aName*/, const TDesC8& aBuffer)
	{
	if ((aBuffer.Length() >= 2) && (aBuffer.Left(2) == KShebang))
		{
		iDataType = TDataType(KShebangDataType);
		iConfidence = ECertain;
		}
	}

CApaDataRecognizerType* CShebangRecognizer::CreateRecognizerL()
	{
#ifdef FSHELL_PATCHABLE_CONSTANTS_SUPPORT
	if (KAutoStartFshell)
		{
	_LIT(KFshell, "fshell.exe");
		_LIT(KArgs, "autostart.script");
		RProcess proc;
		TInt err = proc.Create(KFshell, KArgs);
		if (err)
			{
			RDebug::Printf("Couldn't start fshell, err=%d", err);
			}
		else
			{
			proc.Resume();
			proc.Close();
			}
		}
#endif
	return new (ELeave) CShebangRecognizer();
	}


//
// ECOM boiler-plate.
//

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KShebangRecognizerImplementationUidValue, CShebangRecognizer::CreateRecognizerL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

GLDEF_C TInt E32Dll()
	{
	return KErrNone;
	}
