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

#ifndef __DSCDATABASE_H__
#define __DSCDATABASE_H__

#include <d32dbms.h>
#include <dscstoredefs.h>



extern const TInt KDscStoreMaxStringLength;

class CDscItem;

/** 
A database of DSC items

@internalComponent 
@released
*/
class CDscDatabase: public CBase
	{
public:
	static CDscDatabase* NewL();
	~CDscDatabase();

	void CreateDscL(const TUid& aDscId, const TDesC& aDescription);
	void DeleteDscL(const TUid& aDscId);
	TBool DscExistsL(const TUid& aDscId) const;
	void GetDscDescriptionL(const TUid &aDscId, TDes& aDescription) const;

	void AddItemL(CDscItem& aItem, TDscPosition aPos);
	void ReadItemL(CDscItem& aItem);
	void DeleteItemL(const CDscItem& aItem);

	void UpdateItemL(const CDscItem& aItem);
	TBool ItemExistsL(const CDscItem& aItem) const;

	void EnumOpenLC(const TUid& aDscId);
	CDscItem* EnumReadNextL();
	void EnumClose();
	
	static TInt MaxStringLength();
	
private:
	CDscDatabase();
	void ConstructL();	
	void CreateDatabaseL(const TDesC& aDatabaseName);
	void CreateTablesL();

	void QueryItemL(RDbView& aView,  const CDscItem& aItem) const;
	void ReadDscItemL(RDbRowSet& rowSet, CDscItem& aItem) const;
	TInt GetNextItemIdL(TDscPosition aPos, const TUid& aDscId) const;

	void Rollback();
	void DatabaseBeginLC();
	void DatabaseCommitL();
	static void CleanupDatabaseRollback(TAny *aDatabase);
	static void CleanupEnumRollback(TAny *aDatabase);
	void EnumBeginLC();
	TInt DeleteDb();
	static void GetDatabaseNameL(TDes& aDatabaseName);
	
private:
	RDbs iDbsSession;	
	mutable RDbNamedDatabase iDatabase;
	RDbView iView;
	TBool iIsEnumOpened;
	};

#endif // __DSCDATABASE_H__
