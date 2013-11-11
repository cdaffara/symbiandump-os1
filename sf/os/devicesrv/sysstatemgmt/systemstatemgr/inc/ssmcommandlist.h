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

#ifndef __SSMCOMMANDLIST_H__
#define __SSMCOMMANDLIST_H__

#include <e32base.h>
#include <ssm/ssmcmd.hrh>

class CSsmCommandUtilProvider;
class CSsmCommandListImpl;
class RWriteStream;
class RReadStream;
class MSsmCommand;

/**
@publishedPartner
@released
*/
NONSHARABLE_CLASS (CSsmCommandList) : public CBase
	{
public:
	IMPORT_C static CSsmCommandList* NewL();
	IMPORT_C static	CSsmCommandList* NewLC();
	IMPORT_C ~CSsmCommandList();	

	operator class MSsmCommandList&();

	IMPORT_C void AppendL(MSsmCommand* aCmd);
	IMPORT_C void SetDelayBetweenCommands(TInt aDelayBetweenCommands);
	IMPORT_C const MSsmCommand* operator[](TInt aIndex) const;
	
/**
@internalTechnology
*/	
	IMPORT_C static CSsmCommandList* NewLC(const CSsmCommandList* aSsmCommandList, CSsmCommandUtilProvider* aUtilProvider);
	IMPORT_C TInt DelayBetweenCommands() const;

	IMPORT_C void InternalizeL(RReadStream& aReadStream, CSsmCommandUtilProvider& aUtilProvider);
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;

	IMPORT_C void Execute(TInt aIndex, TRequestStatus& aStatus);
	IMPORT_C void ExecuteCancel( TInt aIndex );
	IMPORT_C TInt Count() const;	
	IMPORT_C void DeferredDelete();

	IMPORT_C TCmdErrorSeverity Severity(TInt aCommandIndex) const;
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C void GetDataToValidateCommandlist(TInt& aPublishSysStateCmdCount, TInt& aPublishSwpCmdCount) const;
	CSsmCommandListImpl* Impl() const;
private:
	CSsmCommandList();
	void ConstructL();
	void ConstructL(const CSsmCommandList* aSsmCommandList, CSsmCommandUtilProvider* aUtilProvider);

private:
	CSsmCommandListImpl* iImpl;
	};
	
#endif // __SSMCOMMANDLIST_H__
