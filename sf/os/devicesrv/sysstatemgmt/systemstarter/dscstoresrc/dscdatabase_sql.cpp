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

#include "dscdatabase_sql.h"
#include "dscitem.h"
#include <e32debug.h>
#include <e32math.h>
#include <f32file.h>
#include "e32modes.h"


//DSC Database name
_LIT(KDatabaseName, "%s:[2000836D]DBS_DSC.db");
//Table name
_LIT(KDscTable, "DSC");
_LIT(KItemTable, "DscItem");
//Column name
_LIT(KDscIdCol, "DscId");
_LIT(KDescriptionCol, "Description");
_LIT(KItemIdCol, "ItemId");
_LIT(KFileNameCol, "FileName");
_LIT(KArgListCol, "ArgList");
_LIT(KStartMethodCol, "StartMethod");
_LIT(KTimeoutCol, "Timeout");
_LIT(KNoOfRetriesCol, "NoOfRetries");
_LIT(KMonitorCol, "Monitored");
_LIT(KStartupTypeCol, "StartupType");
_LIT(KViewlessCol, "Viewless");
_LIT(KStartInBackgroundCol, "StartInBackground");


_LIT8(KBeginTrans, "BEGIN");
_LIT8(KRollbackTrans, "ROLLBACK");
_LIT8(KCommitTrans, "COMMIT");

/**
The maximum length for DBMS varchar: 255 characters.
@internalTechnology 
@released
*/
const TInt KDscStoreMaxStringLength =255;

//length of characters in TUint 32
//This is used to calculate the length of sqlCmd in order to allocate adequate buffer.
//characters in TUint32 and TInt32
//This is used to calculate the length of sqlCmd in order to allocate adequate buffer.
const TInt lengthOfTUint=10;	
const TInt lengthOfTInt=11;

//The maximum characters in DSC database name 
const TInt KMaxDatabaseName=40;

CDscDatabase* CDscDatabase::NewL()
	{
	CDscDatabase* db = new(ELeave) CDscDatabase();
	CleanupStack::PushL(db);

	db->ConstructL();

	CleanupStack::Pop(db);
	return db;
	}

//Second phase constructor, Open connection to Dscdatabase.
//If DSC database doesn't exist, create it.	
void CDscDatabase::ConstructL()
	{
	
	TBuf<KMaxDatabaseName> databaseName;
	GetDatabaseName(databaseName);

 	//Open DSC database
 	TInt error=iDatabase.Open(databaseName );

	if (KErrNotFound!=error) //Dsc Database exists
		{
		//If open with error. Leave with error code return from SQLite
		User::LeaveIfError(ConvertErrCode(error));
		//Opened successfully
		return;
		}
	
	//DscDatabase doesn't exist
	//Create security policy: Read: ReadDeviceData, Write: WriteDeviceData, 
	//Schema: ReadDeviceData and WriteDeviceData
	TSecurityPolicy defaultPolicy;
	RSqlSecurityPolicy securityPolicy;
	CleanupClosePushL(securityPolicy);
	User::LeaveIfError(securityPolicy.Create(defaultPolicy));

	const TSecurityPolicy KSchemePolicy(ECapabilityReadDeviceData, ECapabilityWriteDeviceData);
	const TSecurityPolicy KReadPolicy(ECapabilityReadDeviceData);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);

	User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::ESchemaPolicy,KSchemePolicy));
	User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy,KWritePolicy));
	User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EReadPolicy,KReadPolicy));
	
	//Create DSC database according to security policy.
	User::LeaveIfError(iDatabase.Create(databaseName, securityPolicy));

	CleanupStack::PopAndDestroy(&securityPolicy);	
	
	//Create tables for DSC database	
	TRAP(error,CreateTablesL());
	
	if (KErrNone > error)
		{
		//Fail to create tables,  destroy the DB, so next time can recreate it
		DeleteDb();
		}
	User::LeaveIfError(error);
	}	

//Default constructor	
CDscDatabase::CDscDatabase():iIsEnumOpened(EFalse)
	{
	}

CDscDatabase::~CDscDatabase()
	{
	iStmt.Close();
	iDatabase.Close();
	}

//Destroy DSC database, 
TInt CDscDatabase::DeleteDb()
	{
	iDatabase.Close();
	
	TBuf<KMaxDatabaseName> databaseName;
	GetDatabaseName(databaseName);
	return iDatabase.Delete(databaseName);
	}

void CDscDatabase::CreateTablesL()
	{
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	//Allocate memory for create item table sql cmd which has the most characters. 
	_LIT(KCreateItemTable,"CREATE TABLE %S (%S INTEGER NOT NULL, %S INTEGER NOT NULL, %S VARCHAR (%d)NOT NULL,\
		 %S VARCHAR(%d), %S TINYINT, %S INTEGER, %S INTEGER, %S BIT, %S TINYINT, %S BIT, %S BIT, \
		UNIQUE(%S, %S))");
	//the extra characters for the %S, %d in the format string
	const TInt extraLength=32;
	//chars in KDscStoreMaxStringLength
	const TInt lengthOfKDscStoreMaxStringLength=3;
	
	// The maximum number of characters for create table DscItem sql command		
	const TInt length=KCreateItemTable().Length()+KItemTable().Length()+KDscIdCol().Length()+KItemIdCol().Length()+ 
					KFileNameCol().Length()+KArgListCol().Length()+
					KStartMethodCol().Length()+KTimeoutCol().Length()+KNoOfRetriesCol().Length()+KMonitorCol().Length()+
					KStartupTypeCol().Length()+KViewlessCol().Length()+KStartInBackgroundCol().Length()+
					KDscIdCol().Length()+KItemIdCol().Length()-extraLength+2* lengthOfKDscStoreMaxStringLength;

	sqlCmd.CreateL(length);					

	//format a sql cmd for create table dsc
	_LIT(KCreateDscTable,"CREATE TABLE %S (%S INTEGER NOT NULL, %S VARCHAR(%d), PRIMARY KEY(%S))");
	sqlCmd.Format(KCreateDscTable,&KDscTable,&KDscIdCol,&KDescriptionCol, KDscStoreMaxStringLength, &KDscIdCol);


	PrintSqlCmd(sqlCmd);

	//Start a new transaction		
	DatabaseBeginLC();

	//Create Table DSC 
	TInt error=iDatabase.Exec(sqlCmd);
	User::LeaveIfError(ConvertErrCode(error));
	
	//Create Table DscItem	

	sqlCmd.Format(KCreateItemTable,&KItemTable,&KDscIdCol, &KItemIdCol,&KFileNameCol,KDscStoreMaxStringLength,&KArgListCol,
				KDscStoreMaxStringLength, &KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol, &KDscIdCol, &KItemIdCol);

	PrintSqlCmd(sqlCmd);

	error=iDatabase.Exec(sqlCmd);
	User::LeaveIfError(ConvertErrCode(error));
	DatabaseCommitLP();
	CleanupStack::PopAndDestroy(&sqlCmd);


	
	}

#ifdef _DEBUG
//Help, print out SQL message
void CDscDatabase::PrintSqlCmd(const TDesC& aMess) const
	{
	RDebug::RawPrint(aMess);
	}
#else
void CDscDatabase::PrintSqlCmd(const TDesC& /*aMess*/) const
	{

	}
#endif

//Add a DSC with aDscId to DSC DB. if the aDscId exists, leave with KErrAlreadyExists
void CDscDatabase::CreateDscL(const TUid& aDscId, const TDesC& aDescription)
	{
	//If the DSC is opened for enumeration, leave with KErrLocked
	if (iIsEnumOpened)
		{
		User::Leave( KErrLocked);
		}
		
	//Start a new transaction		
	DatabaseBeginLC();

	//Insert aDscId in Table DSC. If aDscId exists, leave with KErrAlreadyExists	
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	
	_LIT(KSQLQuery,"INSERT INTO %S (%S,%S) VALUES (%d,'%S')");

	//Allocate memory for insert sql cmd
	const TInt length=KSQLQuery().Length()+ KDscTable().Length()+KDscIdCol().Length()+
					KDescriptionCol().Length()+aDescription.Length();
	sqlCmd.CreateL(length);
	

	sqlCmd.Format(KSQLQuery,&KDscTable,&KDscIdCol,&KDescriptionCol, aDscId, &aDescription);

	PrintSqlCmd(sqlCmd);		
	
	TInt error=iDatabase.Exec(sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);
	User::LeaveIfError(ConvertErrCode(error));

	DatabaseCommitLP();	
	}

//Delete the DSC with aDscId, all items related to aDscId are deleted too.  
void CDscDatabase::DeleteDscL(const TUid& aDscId)
	{
	//If the DSC is opened for enumeration, leave with KErrLocked
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}
	
	//Start a new transaction		
	DatabaseBeginLC();
	if (!DscExistsL(aDscId))
		{
		// aDscId doesn't exist, leave with KErrNotFound		
		User::Leave(KErrNotFound);
		}
	
	//Delete all items related with aDscId first
	_LIT(KSQLQuery,"DELETE FROM %S WHERE %S=%d ");
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);

	//	the extra characters for the %S, %d in the format string
	const TInt extraLength=6;
	//Allocate memory for delete sql cmd
	const TInt length=KSQLQuery().Length()+ KItemTable().Length()+KDscIdCol().Length()+
				lengthOfTUint-extraLength	;
	sqlCmd.CreateL(length);
	sqlCmd.Format(KSQLQuery,&KItemTable,&KDscIdCol,aDscId);
	TInt error=iDatabase.Exec(sqlCmd);
	User::LeaveIfError(ConvertErrCode(error));
	
	//Delete aDscId from Table DSC
	sqlCmd.Format(KSQLQuery,&KDscTable,&KDscIdCol,aDscId);	
	error=iDatabase.Exec(sqlCmd);
	User::LeaveIfError(ConvertErrCode(error));
	CleanupStack::PopAndDestroy(&sqlCmd);
	DatabaseCommitLP();	
	}

//Check the existance of the aDscId	
TBool CDscDatabase::DscExistsL(const TUid& aDscId) const
	{

	RSqlStatement stmt;
	CleanupClosePushL(stmt);

	_LIT(KSQLQuery,"SELECT %S FROM %S WHERE %S=%d ");

	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);

	//	the extra characters for the %S, %d in the format string
	const TInt extraLength=8;
	//Allocate memory for delete sql cmd
	const TInt length=KSQLQuery().Length()+ KDscTable().Length()+2*KDscIdCol().Length()+
				lengthOfTUint-extraLength	;
	sqlCmd.CreateL(length);
	sqlCmd.Format(KSQLQuery, &KDscIdCol, &KDscTable,&KDscIdCol,aDscId);

	TInt error=stmt.Prepare(iDatabase, sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);
	
	User::LeaveIfError(ConvertErrCode(error));
	
	error=stmt.Next();
	User::LeaveIfError(ConvertErrCode(error));

	CleanupStack::PopAndDestroy(&stmt);	
	return KSqlAtRow==error ? ETrue:EFalse;			
	}

//Check the existance of aItem
TBool CDscDatabase::ItemExistsL( const CDscItem& aItem) const
	{

	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	
	TBool result=QueryItemL(stmt, aItem);



	CleanupStack::PopAndDestroy(&stmt);  
	
	return result;
	}

//Query the aItem from Table DscItem	
TBool CDscDatabase::QueryItemL(RSqlStatement& aStmt, const CDscItem& aItem) const
	{

	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);


	TUid dscId=aItem.DscId();
	TPtrC name=aItem.FileName();
	
	//If fileName is NULL, query aItem by DSC ID and ItemId
	if (name ==KNullDesC)
		{
		_LIT(KSQLQuery,"SELECT %S, %S, %S, %S, %S, %S, %S, %S, %S, %S, %S FROM %S WHERE %S=%d AND %S=%d ");
		//	the extra characters for the %S, %d in the format string
		const TInt extraLength=32;
		//Allocate memory for search sql cmd
		const TInt length=KSQLQuery().Length()+2*KDscIdCol().Length()+KItemIdCol().Length()+ KFileNameCol().Length()
							+KArgListCol().Length()+KStartMethodCol().Length()+KTimeoutCol().Length()+KNoOfRetriesCol().Length()
							+KMonitorCol().Length()+KStartupTypeCol().Length()+KViewlessCol().Length()+KStartInBackgroundCol().Length()
							+KItemTable().Length()+lengthOfTInt+lengthOfTUint+KItemIdCol().Length()-extraLength;
		sqlCmd.CreateL(length);
		sqlCmd.Format(KSQLQuery,&KDscIdCol, &KItemIdCol,&KFileNameCol,&KArgListCol,
				&KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol, &KItemTable,&KDscIdCol, dscId, &KItemIdCol,aItem.ItemId());
		}
	//Otherwise query aItem by DSC ID, fileName and arguments	
	else 
		{
		if(name.Length() >KDscStoreMaxStringLength || aItem.Args().Length() >KDscStoreMaxStringLength)
		{
		User::Leave(KErrArgument);
		}

		TBuf<KDscStoreMaxStringLength> argList=aItem.Args();
		argList.TrimAll();
		
		_LIT(KSQLQuery,"SELECT  %S, %S, %S, %S, %S, %S, %S, %S, %S, %S, %S  FROM %S WHERE %S=%d AND %S= '%S' AND %S = '%S' "); 
		//	the extra characters for the %S, %d in the format string
		const TInt extraLength=36;
		//Allocate memory for search sql cmd
		const TInt length=KSQLQuery().Length()+2*KDscIdCol().Length()+KItemIdCol().Length()+ KFileNameCol().Length()
							+KArgListCol().Length()+KStartMethodCol().Length()+KTimeoutCol().Length()+KNoOfRetriesCol().Length()
							+KMonitorCol().Length()+KStartupTypeCol().Length()+KViewlessCol().Length()+KStartInBackgroundCol().Length()
							+KItemTable().Length()+lengthOfTUint+KFileNameCol().Length()+aItem.FileName().Length()
						+KArgListCol().Length()+argList.Length()-extraLength;
		sqlCmd.CreateL(length);
		sqlCmd.Format(KSQLQuery,&KDscIdCol, &KItemIdCol,&KFileNameCol,&KArgListCol,	&KStartMethodCol, &KTimeoutCol,
					 &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,&KViewlessCol, &KStartInBackgroundCol, 
					 &KItemTable,&KDscIdCol, dscId, &KFileNameCol,&name, &KArgListCol, &argList);
		}
 	

	PrintSqlCmd(sqlCmd);

	
	TInt error=aStmt.Prepare(iDatabase, sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);  		
	if(KErrNone <= error)
		{
		error=aStmt.Next();  //no error for non existing item
		}

	return KSqlAtRow==error? ETrue: EFalse;
	}

//Open a result set of items with aDscID for enumeration, need to 
//call EnumClose() to close the set after enumeration.	
void CDscDatabase::EnumOpenLC(const TUid& aDscId)
	{
	//If the DSC is opened for enumeration, leave with KErrLocked
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}
	EnumBeginLC();
	iIsEnumOpened=ETrue;

	//Leave with KErrNotFound if aDscId doesn't exist
	if (!DscExistsL(aDscId))
		{
		User::Leave(KErrNotFound);
		}

	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	_LIT(KSQLQuery,"SELECT %S, %S, %S, %S, %S, %S, %S, %S, %S, %S, %S FROM %S WHERE %S=%d ORDER BY %S"); 
	//	the extra characters for the %S, %d in the format string
	const TInt extraLength=30;
	//Allocate memory for delete sql cmd
	const TInt length=KSQLQuery().Length()+ 2*KDscIdCol().Length()+2*KItemIdCol().Length()+ KFileNameCol().Length()
							+KArgListCol().Length()+KStartMethodCol().Length()+KTimeoutCol().Length()+KNoOfRetriesCol().Length()
							+KMonitorCol().Length()+KStartupTypeCol().Length()+KViewlessCol().Length()+KStartInBackgroundCol().Length()
							+KItemTable().Length()+lengthOfTUint-extraLength;
	sqlCmd.CreateL(length);
	sqlCmd.Format(KSQLQuery, &KDscIdCol, &KItemIdCol,&KFileNameCol,&KArgListCol,
				&KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol,&KItemTable,&KDscIdCol,aDscId, &KItemIdCol);


	//Open a result stmt contains all items in aDscId
	TInt error=iStmt.Prepare(iDatabase, sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);
	User::LeaveIfError(ConvertErrCode(error));
	}

//Enumrating the result stmt opened by EnumOpenLC(), if there are no item in the DSC
//or has reached the end of the list, return NULL	
CDscItem* CDscDatabase::EnumReadNextL()
	{
	if (!iIsEnumOpened)
		{
		//EnumOpenLC() has not been called.
		User::Leave(KErrNotReady);
		}
	TInt err=iStmt.Next();
	User::LeaveIfError(ConvertErrCode(err));	

	if (KSqlAtEnd==err)
		{
		//reached to the end of the result set
		return NULL;
		}
		
	//Read item from the result stmt
	CDscItem* item=CDscItem::NewL();
	CleanupStack::PushL(item);
	ReadDscItemL(iStmt, *item);
	CleanupStack::Pop(item);
	return item;
	}	

//Read aItem from the stmt
void CDscDatabase::ReadDscItemL(RSqlStatement& aStmt, CDscItem& aItem)
	{
		TPtrC name=aStmt.ColumnTextL(2);
		TPtrC args=aStmt.ColumnTextL(3);
		aItem.SetDscId(TUid::Uid(aStmt.ColumnInt(0)));
		aItem.SetItemId(aStmt.ColumnInt(1));
		aItem.SetFileParamsL(name, args);
		aItem.SetStartMethodL(static_cast<TStartMethod> (aStmt.ColumnInt(4)));
		aItem.SetFailureParams(aStmt.ColumnInt(6), aStmt.ColumnInt(5));
		aItem.SetMonitored(aStmt.ColumnInt(7));
		aItem.SetStartupType(static_cast<TStartupType> (aStmt.ColumnInt(8)));
		aItem.SetViewless(aStmt.ColumnInt(9));
		aItem.SetStartInBackground( aStmt.ColumnInt(10));
	}
	

//Close the view opened by EnumOpenLC() and rollback the transaction
void CDscDatabase::EnumClose()
	{
	//To avoid double close
	if (iIsEnumOpened)
		{	
		iStmt.Close();
		iIsEnumOpened=EFalse;
		
		//Roll back transaction
		Rollback();
		CleanupStack::Pop();//Pop client CleanupEnumRollback	
		}
	}

//Add aItem to DSC at aPos
void CDscDatabase::AddItemL(CDscItem& aItem, TDscPosition aPos)
	{
	//Leave if DB is opened for enumeration
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}
	
	//verify data integrity
	VerifyDataL(aItem);
	
	DatabaseBeginLC();

	TUid dscId=aItem.DscId();
	//Leave if aDscId doesn't exist
	if (!DscExistsL(dscId))
		{
		User::Leave(KErrNotFound);
		}
	//Leave if aItem exists
	if (ItemExistsL( aItem))
		{
		User::Leave(KErrAlreadyExists);
		}
		
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	//Allocate memory for insert item sql cmd which has the most characters. 
	_LIT(KSQLQuery1,"INSERT INTO %S (%S,%S,%S,%S,%S,%S,%S,%S,%S, %S, %S ) VALUES \
					(%d,%d,'%S','%S',%d,%d, %d, %b, %d, %b, %b)");
	//	the extra characters for the %S, %d in the format string
	const TInt extraLength=40;
	const TInt length=KSQLQuery1().Length()+KItemTable().Length()+KDscIdCol().Length()+KItemIdCol().Length()
						+KFileNameCol().Length()+KArgListCol().Length()+KStartMethodCol().Length()
						+KTimeoutCol().Length()+KNoOfRetriesCol().Length()+KMonitorCol().Length()
						+KStartupTypeCol().Length()+KViewlessCol().Length()+KStartInBackgroundCol().Length()
						+4*lengthOfTUint+ aItem.FileName().Length()+aItem.Args().Length()-extraLength;
	sqlCmd.CreateL(length);
	
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	

	//calculate item id according to the position to be inserted
	//add at ELast: pos =max of itemId+1
	if (ELast==aPos )
		{
		_LIT(KSQLQuery,"SELECT  MAX(%S) FROM %S WHERE %S=%d ");
		sqlCmd.Format(KSQLQuery, &KItemIdCol, &KItemTable,&KDscIdCol, dscId );	
		}	
	//add at EFirst: pos=min of itemId-1		
	else 
		{
		_LIT(KSQLQuery,"SELECT  MIN(%S) FROM %S WHERE %S=%d ");
		sqlCmd.Format(KSQLQuery, &KItemIdCol, &KItemTable,&KDscIdCol, dscId );
		}
	

	PrintSqlCmd(sqlCmd);


	TInt err=stmt.Prepare(iDatabase, sqlCmd);
	User::LeaveIfError(ConvertErrCode(err));
	err=stmt.Next();
	User::LeaveIfError(ConvertErrCode(err));
	
	//for empty DSC,  0 will be returned from the stmt, so the initial value
	//of itemId will be -1 or 1 depend on where to insert, 
	//this is slightly different from DBMS implementation, where 
	//initial value for itemId is always 0
	//Since itemId is used as reference for relative orders, this should 
	//be acceptable
	

	//Get the next available itemId for the item
	TInt itemId=GetItemIdL(aPos, stmt);

	CleanupStack::PopAndDestroy(&stmt);

	//set the item id, it will be return to client
	aItem.SetItemId(itemId);
	
	//insert item into DSC
	TPtrC name=aItem.FileName();
	
	//Trim white space in argment list
	TBuf<KDscStoreMaxStringLength> argList=aItem.Args();
	argList.TrimAll();

	sqlCmd.Format(KSQLQuery1,&KItemTable,&KDscIdCol,&KItemIdCol,&KFileNameCol,&KArgListCol,
				&KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol, dscId,itemId,&name, &argList, aItem.StartMethod(),
				aItem.Timeout(), aItem.NoOfRetries(), aItem.Monitored(),
				 aItem.StartupType(), aItem.Viewless(), aItem.StartInBackground());


	PrintSqlCmd(sqlCmd);
	
	err=iDatabase.Exec(sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);
	User::LeaveIfError(ConvertErrCode(err));

	DatabaseCommitLP();
	}

//Delete aItem from DSC 
void CDscDatabase::DeleteItemL( const CDscItem& aItem)
	{
	//Leave if DB is opened for enumeration
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}	

	DatabaseBeginLC();
	
	//Leave if aItem does not exist	
	if (!ItemExistsL( aItem))
		{
		User::Leave(KErrNotFound);
		}
		
		
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);


	TPtrC name=aItem.FileName();
	TUid dscId=aItem.DscId();
	//Delete the item by itemId
	if (name==KNullDesC)
		{
		_LIT(KSQLQuery,"DELETE FROM %S WHERE %S=%d AND %S=%d ");
	//	the extra characters for the %S, %d in the format string
		const TInt extraLength=10;
		const TInt length=KSQLQuery().Length()+KItemTable().Length()+KDscIdCol().Length()+2*lengthOfTUint
						+KItemIdCol().Length()-extraLength;
		sqlCmd.CreateL(length);
		
		sqlCmd.Format(KSQLQuery,&KItemTable,&KDscIdCol, dscId,&KItemIdCol,aItem.ItemId());
		}
	//Delete the item by fileName + arguments	
	else
		{
			
		TBuf<KDscStoreMaxStringLength> argList=aItem.Args();
		argList.TrimAll();
		_LIT(KSQLQuery,"DELETE FROM %S WHERE %S=%d AND %S='%S' AND %S= '%S' ");
	//	the extra characters for the %S, %d in the format string
		const TInt extraLength=14;
		const TInt length=KSQLQuery().Length()+KItemTable().Length()+KDscIdCol().Length()+lengthOfTUint
						+KFileNameCol().Length()+aItem.FileName().Length()+KArgListCol().Length()
						+aItem.Args().Length()-extraLength;
		sqlCmd.CreateL(length);
		
		sqlCmd.Format(KSQLQuery,&KItemTable,&KDscIdCol, dscId,&KFileNameCol,&name, &KArgListCol, &argList);
		}

	PrintSqlCmd(sqlCmd);


	TInt err=iDatabase.Exec(sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);
	
	User::LeaveIfError(ConvertErrCode(err));
	DatabaseCommitLP();

	}
	
//Update aItem. Unable to update an item only determined by itemId
void CDscDatabase::UpdateItemL( const CDscItem& aItem)
	{
	//Leave if DB is opened for enumeration	
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}

	//verify data integrity,
	VerifyDataL(aItem);
	
	DatabaseBeginLC();
	//Leave if aItem does not exist
	if (!ItemExistsL( aItem))
		{
		User::Leave(KErrNotFound);	
		}
	
	
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	
	TPtrC name=aItem.FileName();	

	TBuf<KDscStoreMaxStringLength> argList=aItem.Args();
	argList.TrimAll();
	TUid dscId=aItem.DscId();		

	//update aItem determined by fileName, argList and dscId. These three fields can't be updated		
	_LIT(KSQLQuery,"UPDATE  %S SET %S=%d,%S=%d, %S=%d,%S=%b,%S=%d,%S=%b, %S=%b WHERE %S=%d AND %S='%S' AND %S='%S'");
	//	the extra characters for the %S, %d in the format string 
	const TInt extraLength=38;
	const TInt length=KSQLQuery().Length()+KItemTable().Length()+KStartMethodCol().Length()+ KTimeoutCol().Length()
					+3* lengthOfTUint + KNoOfRetriesCol().Length() +KMonitorCol().Length()+ KStartupTypeCol().Length()
					+KViewlessCol().Length()+ KStartInBackgroundCol().Length()+KDscIdCol().Length()+ KFileNameCol().Length()
					+KArgListCol().Length()+aItem.FileName().Length()+aItem.Args().Length()-extraLength;
	sqlCmd.CreateL(length);		
	
	sqlCmd.Format(KSQLQuery,&KItemTable, &KStartMethodCol, aItem.StartMethod(), &KTimeoutCol, aItem.Timeout(),
					&KNoOfRetriesCol,aItem.NoOfRetries(), &KMonitorCol, aItem.Monitored(), &KStartupTypeCol,aItem.StartupType(),
					&KViewlessCol, aItem.Viewless(), &KStartInBackgroundCol, aItem.StartInBackground(),
					&KDscIdCol, dscId, &KFileNameCol, &name, &KArgListCol, &argList);
	

	PrintSqlCmd(sqlCmd);

		
	TInt	err=iDatabase.Exec(sqlCmd);
	CleanupStack::PopAndDestroy(&sqlCmd);
	User::LeaveIfError(ConvertErrCode(err));
	DatabaseCommitLP();
	}

//Read aItem from DSC DB either by fileName, argList and dscId, or by itemId and dscId	
void CDscDatabase::ReadItemL(CDscItem& aItem)
	{
	//Leave if DB is opened for enumeration	
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}
		
	DatabaseBeginLC();	
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	

	TBool itemExists=QueryItemL(stmt, aItem);

	//aItem exists
	if(itemExists)
		{
		ReadDscItemL(stmt, aItem);
		
		//Reset all other values to default
		aItem.SetRecoveryParams(EIgnoreOnFailure,EStartupModeUndefined);
		aItem.SetLocalizationParamsL(KNullDesC,0);
		}
	//aItem does not exist
	else
		{
		User::Leave(KErrNotFound);	
		}
	
	CleanupStack::PopAndDestroy(&stmt);
	DatabaseCommitLP();
	}


//Rollback a transaction	
void CDscDatabase::Rollback()
	{
	iDatabase.Exec(KRollbackTrans);
	}

//Verify aItem
void CDscDatabase::VerifyDataL(const CDscItem& aItem)
	{
	TInt noOfRetries=aItem.NoOfRetries();

	//item with empty filename is not allowed to add to DB, or been updated 
	//fileName and argList of aItem is limited to KDscStoreMaxStringLength
	//noOfRetries is restricted to 0 or 1
	//StartMethod is retricted to EFireAndForget or EWaitForStart
	//Recovery policy of DscItem is restricted to IgnoreprocessFailure.	
	if((aItem.FileName()==KNullDesC) ||(aItem.FileName().Length() > KDscStoreMaxStringLength) ||
		(aItem.Args().Length() > KDscStoreMaxStringLength)||
		(noOfRetries!=0 && noOfRetries!=1 ) ||(aItem.Timeout() <0) ||
		(aItem.StartMethod() !=EFireAndForget && aItem.StartMethod() !=EWaitForStart )||
		(aItem.RecoveryMethod() !=EIgnoreOnFailure ))
		{
		User::Leave(KErrArgument);
		}
	}

//Start a new transaction.Pushes cleanup rollback action to the cleanup stack. 
void CDscDatabase::DatabaseBeginLC()
	{
	TInt err=iDatabase.Exec(KBeginTrans);	
	User::LeaveIfError(ConvertErrCode(err));
	CleanupStack::PushL(TCleanupItem(CleanupDatabaseRollback,this));
	
	}

//Commit transaction. Pops cleanup rollback action
void CDscDatabase::DatabaseCommitLP()
	{
	TInt err=iDatabase.Exec(KCommitTrans);
	User::LeaveIfError(ConvertErrCode(err)); // force a rollback on error
	// Pop CleanupDatabaseRollback
	CleanupStack::Pop();  
	}

//Cleanup rollback action for DatabaseBeginLC()	
void CDscDatabase::CleanupDatabaseRollback(TAny *aDatabase)
	{
	ASSERT(aDatabase);
	CDscDatabase* database=static_cast <CDscDatabase *>(aDatabase);

	database->Rollback();
	}

//Start a new transaction for enumeration.Pushes cleanup rollback action to the cleanup stack. 
void CDscDatabase::EnumBeginLC()
	{

	TInt err=iDatabase.Exec(KBeginTrans);
	User::LeaveIfError(ConvertErrCode(err));
	
	CleanupStack::PushL(TCleanupItem(CleanupEnumRollback,this));
	
	}	
//Cleanup rollback action for EnumOpenLC()
void CDscDatabase::CleanupEnumRollback(TAny *aDatabase)
	{
	ASSERT(aDatabase);
	CDscDatabase* database=static_cast <CDscDatabase *>(aDatabase);
	database->iStmt.Close();
	database->iIsEnumOpened=EFalse;
		
		//Roll back transaction
	database->Rollback();
	}

//Covert SQLite specific error code to Symbian standard error code
TInt CDscDatabase::ConvertErrCode(TInt aErr) const
	{
	if (-144<=aErr)
		{
		return aErr;
		}
	TInt err;
	switch (aErr)
		{
		case KSqlErrPermission:
			err=KErrPermissionDenied;
			break;
				
		case KSqlErrBusy:
		case KSqlErrLocked:
			err=KErrLocked;
			break;
		
		case KSqlErrCorrupt:
			err=KErrCorrupt;
			break;
		
		case KSqlErrAbort:
			err=KErrAbort;
			break;
		case KSqlErrNotFound:
			err=KErrNotFound;
			break;
		case KSqlErrConstraint:
			err=KErrAlreadyExists;
			break;
	
		case KSqlErrFull:
			err=KErrOverflow;
			break;
					
		default:
			err=KErrGeneral;			
			
		}
	return err;	
	}

//Get DSC database name
void CDscDatabase::GetDatabaseNameL(TDes& aDatabaseName)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	//Retrieve system drive
	TDriveNumber driveNumber=fs.GetSystemDrive();
 
    TChar driveChar;
    User::LeaveIfError(RFs::DriveToChar(driveNumber, driveChar));
 
	aDatabaseName.Format(KDatabaseName,&driveChar);
	CleanupStack::PopAndDestroy(&fs);
	}
//Helper function. Get the next available itemId for the item	
TInt CDscDatabase::GetItemIdL(TDscPosition aPos, RSqlStatement& aStmt)
	{
	TInt itemId=aStmt.ColumnInt(0); 
	
	if(ELast==aPos  )
		{
		if(KMaxTInt32==itemId)
			{
			User::Leave(KErrOverflow);
			}
		++itemId;
		
		}	
	else if(EFirst==aPos )
		{
		if(KMinTInt32==itemId)
			{
			User::Leave(KErrOverflow);
			}
		--itemId;
		}
	return itemId;		
	}
