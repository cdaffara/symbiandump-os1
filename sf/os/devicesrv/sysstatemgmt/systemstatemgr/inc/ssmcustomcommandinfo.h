// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMCUSTOMCOMMANDINFO_H__
#define __SSMCUSTOMCOMMANDINFO_H__

#include <e32base.h>
#include <ssm/ssmcmd.hrh>

class RReadStream;
class RWriteStream;

/** 
The maximum size of user-specific (TDesC8) data passed to a Custom Command.
@see CCmdCustomCommand
@see CSsmCustomCommandInfo
@publishedPartner
@released
*/
const TInt KMaxDLLParams = 4096;

/** 
Information required to construct a custom command.
@see TCmdCustomCommandLibUnloading
@see CCmdCustomCommand
@publishedPartner
@released
*/
class CSsmCustomCommandInfo : public CBase
	{
public:
	IMPORT_C CSsmCustomCommandInfo();
	IMPORT_C ~CSsmCustomCommandInfo();

	IMPORT_C void SetL(const TDesC& aFileName, TInt32 aOrdinal, TCmdCustomCommandLibUnloading aUnloading, TInt16 aRetries, const TDesC8& aParams);
	IMPORT_C void Clear();
	
	IMPORT_C const TFileName& FileName() const;
	IMPORT_C TInt32 Ordinal() const;
	IMPORT_C TCmdCustomCommandLibUnloading Unloading() const;
	IMPORT_C TInt16 Retries() const;
	IMPORT_C const TPtrC8 Params() const;

	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;

	IMPORT_C CSsmCustomCommandInfo& operator=(const CSsmCustomCommandInfo& aInfo);

private:
	CSsmCustomCommandInfo(const CSsmCustomCommandInfo& aInfo);

private:
	TFileName iFileName;
	TInt32 iOrdinal;
	TCmdCustomCommandLibUnloading iUnloading;
	TInt16 iRetries;
	HBufC8* iParams;
	};

#endif // __SSMCUSTOMCOMMANDINFO_H__
