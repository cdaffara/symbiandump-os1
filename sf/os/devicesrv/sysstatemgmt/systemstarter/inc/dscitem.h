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

#ifndef __DSCITEM_H__
#define __DSCITEM_H__

#include <e32base.h>
#include <startupproperties.h>

/** CDscItem class containing application/process information required by the AMAStarter to launch an AMA. 
It is inherit from CStartupProperties

When an instance of this class is created it contains default values for all properties. 

@publishedAll
@released
*/
NONSHARABLE_CLASS (CDscItem) : public CBase
	{
public:
	IMPORT_C static CDscItem* NewL();
	IMPORT_C static CDscItem* NewLC();
	IMPORT_C static CDscItem* NewL(const TDesC& aFileName, const TDesC& aArgs);
	IMPORT_C static CDscItem* NewLC(const TDesC& aFileName, const TDesC& aArgs);
	IMPORT_C ~CDscItem();

	IMPORT_C TUid DscId() const;
	IMPORT_C TInt ItemId() const;
	IMPORT_C TPtrC FileName() const;
	IMPORT_C TPtrC Args() const;
	IMPORT_C TStartupType StartupType() const;
	IMPORT_C TStartMethod StartMethod() const;
	IMPORT_C TInt NoOfRetries() const;
	IMPORT_C TInt Timeout() const;
	IMPORT_C TBool Monitored() const;
	IMPORT_C TBool Viewless() const;
	IMPORT_C TBool StartInBackground() const;
	IMPORT_C const CStartupProperties& StartupProperties() const;

	IMPORT_C void SetDscId(const TUid& aDscId);
	IMPORT_C void SetItemId(const TInt aItemId); // @internalComponent
	IMPORT_C void SetFileParamsL(const TDesC& aFileName, const TDesC& aArgs);
	IMPORT_C void SetStartMethodL(TStartMethod aStartMethod);
	IMPORT_C void SetNoOfRetriesL(TInt aNumRetries);
	IMPORT_C void SetTimeoutL(TInt aTimeout);
	IMPORT_C void SetMonitored(TBool aMonitored);
	IMPORT_C void SetStartupType(TStartupType aType);
	IMPORT_C void SetViewless(TBool aViewless);
    IMPORT_C void SetStartInBackground(TBool aStartInBackground);

    IMPORT_C void Reset();
private:
	CDscItem();
	void ConstructL();

private:
	TUid iDscId;
	TInt iItemId;
	CStartupProperties* iStartupProperties;
	};

#endif   	
	


