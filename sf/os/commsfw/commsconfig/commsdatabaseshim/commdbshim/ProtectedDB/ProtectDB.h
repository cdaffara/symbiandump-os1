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
// Protected tables/records functionality
// 
//

/**
 @file ProtectDB.h
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef PROTECTDB_H
#define	PROTECTDB_H

#include <commdb.h>
#include <metadatabase.h>
#include <badesca.h>

using namespace CommsDat;

class CCommsDatabaseProtect : public CCommsDatabase
/**
@internalTechnology
*/
	{
public:
	/** Creates a `CCommsDatabase`. If the database filestore does not exist
		the default filestore is duplicated. If no default filestore exists an
		empty database is created. */
	IMPORT_C static CCommsDatabaseProtect* NewL();
	/** Creates a `CCommsDatabase`. If the database filestore exists and
		`aUseDefaultDb` is True the default filestore is duplicated. Otherwise
		an empty database is created. */
	IMPORT_C static CCommsDatabaseProtect* NewL(TBool aUseDefaultDb);
	/** Creates a `CCommsDatabase` as with `NewL()`. The method of opening:
		(Created, CopiedDefault or Opened) is returned in `aOpeningMethod`. */
	IMPORT_C static CCommsDatabaseProtect* NewL(TCommDbOpeningMethod &aOpeningMethod);

	IMPORT_C ~CCommsDatabaseProtect();

	/**Sets the access right field in the ACCESS_TYPE_TABLE for the table specified
	in aTableName to RDbRowSet::EReadOnly*/
	IMPORT_C TInt ProtectTable(const TDesC& aTableName);
	/**Sets the access right field in the ACCESS_TYPE_TABLE for the table specified
	in aTableName to RDbRowSet::EUpdatable*/
	IMPORT_C TInt UnProtectTable(const TDesC& aTableName);

	/** returns the access rights for aTableName from the ACCESS_TYPE_TABLE*/
	IMPORT_C RDbRowSet::TAccess GetTableAccessL(const TDesC& aTableName);

	/**Sets the access right fields in the ACCESS_TYPE_TABLE for the tables specified
	in aTableList to RDbRowSet::EReadOnly*/
	IMPORT_C TInt ProtectTables(CPtrCArray& aTableList);
	/**Sets the access right fields in the ACCESS_TYPE_TABLE for the tables specified
	in aTableList to RDbRowSet::EUpdatable*/
	IMPORT_C TInt UnprotectTables(CPtrCArray& aTableList);
	

private:
	CCommsDatabaseProtect();
	void ConstructL();

	void DoTableProtectionL(CMDBRecordSetBase* aElement, TBool aProtect);
	void DoTablesProtectionL(CPtrCArray& aTableList, TBool aProtect);
	
private:
	CCommDbTableExtension* iTableExt;
	};

class CCommsDbProtectTableView : public CCommsDbTableView
/**
@internalTechnology
*/
	{
public:
	static CCommsDbProtectTableView* NewL(CCommsDatabaseBase& aDb,const TDesC& aTableName,const TDbQuery& aQuery,TBool aUseTemplate);
	IMPORT_C ~CCommsDbProtectTableView();
	//Protection of settings
	IMPORT_C TInt ProtectTable();
	IMPORT_C TInt UnprotectTable();
	IMPORT_C TInt ProtectRecord();
	IMPORT_C TInt UnprotectRecord();
	IMPORT_C TInt GetTableAccess(RDbRowSet::TAccess& aAccessType);
	IMPORT_C TInt GetRecordAccess(TInt& aAccess);
	
private:
	CCommsDbProtectTableView(CCommsDatabaseBase& aDb, const TDesC& aTableName);
	void ConstructL(const TDbQuery& aQuery,TBool aUseTemplate);
	void DoProtectionL(CMDBSession* aSession, CMDBElement* aElement, TBool aProtect);
	};

#endif
