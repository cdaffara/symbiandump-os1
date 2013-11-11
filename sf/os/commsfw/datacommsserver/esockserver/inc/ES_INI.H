// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__ES_INI_H__)
#define __ES_INI_H__

#include <e32base.h>

/**
@internalTechnology
*/
#define ESOCK_INI_DATA _L("esock.ini")

_LIT(KEsockIniFileDir,"\\private\\101F7989\\esock\\");
_LIT(KEsockNoBackupDir,"\\private\\101F7989\\esock\\nobackup\\");
_LIT(KEsockWildCard, "*.esk");

// Forward declaration of CommsFW::COwnEntryList
namespace CommsFW 
	{ 
class COwnEntryList;
	}

class CESockIniData : public CBase 
/**
@internalTechnology
*/
	{
public:
	IMPORT_C static CESockIniData* NewL(const TDesC& aName=ESOCK_INI_DATA);
	IMPORT_C static CESockIniData* NewL(const CommsFW::COwnEntryList* aFileList);
	IMPORT_C virtual ~CESockIniData();
	IMPORT_C TBool FindVar(const TDesC& aVarName, TPtrC& aResult) const;
	IMPORT_C TBool FindVar(const TDesC& aVarName, TInt& aResult) const;
	IMPORT_C TBool FindVar(const TDesC& aSection,const TDesC& aVarName,TPtrC& aResult) const;
	IMPORT_C TBool FindVar(const TDesC& aSection,const TDesC& aVarName,TInt& aResult) const;
	IMPORT_C TInt WriteVar(const TDesC& aSection,const TDesC& aVarName,TInt aValue);
	IMPORT_C TInt WriteVar(const TDesC& aSection,const TDesC& aVarName,const TDesC& aValue);
	IMPORT_C void CommitL();
protected:
	IMPORT_C CESockIniData();
	IMPORT_C CESockIniData(const CommsFW::COwnEntryList* aFileList);
	IMPORT_C void ConstructL(const TDesC& aName);
protected:
	HBufC* iName;
	HBufC* iToken;
	TPtr iPtr;
	const CommsFW::COwnEntryList* iFileList;
	};

#endif
