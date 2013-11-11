// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef HCIUTIL_H
#define HCIUTIL_H

#include <e32base.h>
#include <bluetooth/hci/hcierrors.h>

// Note that KMaxHciUtilComponentName + ini-file path + ".ini" must not
// exceed the length of a TName.
const static TUint KMaxHciUtilComponentName = 60;
_LIT(KDefaultIniFilePath, "\\private\\101F7989\\Bluetooth\\");

_LIT(KHciUtilPanic, "HCI Util Panic");

enum THciUtilPanic
	{
	EAlreadyAnIniFileOpen = 0,
	ESectionNameTooBig,
	EKeyNameTooBig,
	};

/**
Common features used throughout the HCI
*/
class CIniFileData;
class CHciUtil : public CBase
	{
public:
	IMPORT_C static CHciUtil* NewL(const TDesC& aComponentName);
	IMPORT_C ~CHciUtil();

	IMPORT_C void OpenIniFileL();
	IMPORT_C void OpenIniFileL(const TDesC& aPath, const TDesC& aFile);
	IMPORT_C void CloseIniFile();

	IMPORT_C TUid GetUidFromFileL(const TDesC& aSection, const TDesC& aTag);
	IMPORT_C TUint GetValueFromFileL(const TDesC& aSection, const TDesC& aTag);

	IMPORT_C static TAny* LoadImplementationL(TUid aIfUid, TInt aKeyOffset);
	IMPORT_C static TAny* LoadImplementationL(TUid aIfUid, TUid aImplUid, TInt aKeyOffset);

	IMPORT_C static void DestroyedImplementation(TUid aKey);

	IMPORT_C static TInt SymbianErrorCode(THCIErrorCode aErrorCode);
	
private:
	void ConstructL(const TDesC& aComponentName);
	CHciUtil();
	
	void GetFromFileL(const TDesC& aSection, const TDesC& aTag, TPtrC& aRetText);
	TUid FormatUidL(TPtrC& aDes);
	TUint FormatValueL(TPtrC& aDes);
	
private:
	TBuf <KMaxHciUtilComponentName> iCompName;
	CIniFileData* iIniFile;
	};

#endif // HCIUTIL_H
