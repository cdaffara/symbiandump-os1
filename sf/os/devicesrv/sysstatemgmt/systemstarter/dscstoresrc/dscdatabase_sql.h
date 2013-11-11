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
// dscdatabase.h
// 
//

#ifndef __DSCDATABASE_H__
#define __DSCDATABASE_H__

#include <sqldb.h>
#include <startupproperties.h>
#include <startup.hrh>
#include "dscstoredefs.h"


//Classes referenced
class CDscItem;

/** A database of DSC items
	@internalComponent 
	@released
	*/
class CDscDatabase: public CBase
	{
public:

	static CDscDatabase* NewL();
	~CDscDatabase();
	

	void CreateDscL(const TUid& aDscId,  const TDesC& aDescription);
	void DeleteDscL(const TUid& aDscId);
	TBool DscExistsL(const TUid& aDscId) const;

	void AddItemL (CDscItem& aItem, TDscPosition aPos);
	void ReadItemL( CDscItem& aItem);
	void DeleteItemL( const CDscItem& aItem);

	void UpdateItemL( const CDscItem& aItem);
	TBool ItemExistsL(const CDscItem& aItem) const;

	void EnumOpenLC(const TUid& aDscId);

	CDscItem* EnumReadNextL();
	void EnumClose();

private:
	CDscDatabase();
	void ConstructL();
	void OpenDatabaseL();
	void CreateTablesL();

	TBool QueryItemL(RSqlStatement& aStmt,  const CDscItem& aItem) const;
	void ReadDscItemL(RSqlStatement& aStmt, CDscItem& aItem);
	TInt GetItemIdL(TDscPosition aPos, RSqlStatement& aStmt);
	void PrintSqlCmd(const TDesC& aMess) const;
	void VerifyDataL(const CDscItem& aItem);

	void Rollback();
	void DatabaseBeginLC();
	void DatabaseCommitLP();
	static void CleanupDatabaseRollback(TAny *aDatabase);
	static void CleanupEnumRollback(TAny *aDatabase);
	void EnumBeginLC();
	TInt ConvertErrCode(TInt aErr) const;
	//help, destroy DSC database
	TInt DeleteDb();
	void GetDatabaseNameL(TDes& aDatabaseName);
	
private:
	mutable RSqlDatabase iDatabase;
	RSqlStatement iStmt;
	TBool iIsEnumOpened;
	};

#endif // __DSCDATABASE_H__
