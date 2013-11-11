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

#include <dscitem.h>
#include "dscdatabase.h"
#include "sysstartpanic.h"

/**
The maximum length for a database name: 20 characters.
@internalComponent 
@released
*/
const TInt KMaxDatabaseName=20;

_LIT(KDatabaseName, "%c:DBS_DSC.db");
_LIT(KSecureFormat, "secure[2000836D]");

//Table names
_LIT(KDscTable, "DSC");
_LIT(KItemTable, "DscItem");
//Column names
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
//Index names
_LIT(KIdIndex, "DscIndex");
_LIT(KIndex, "Index");

//Max length of respective number when represented as a string
const TInt KMaxLengthOfEnum = 5;
const TInt KMaxLengthOfTInt = 11;
const TInt KMaxLengthOfBoolean = 1;

//
// SQL Select query definitions
//
_LIT(KSqlSelectDscItemOnId,"SELECT %S, %S, %S, %S, %S, %S, %S, %S, %S, %S, %S FROM %S WHERE %S=%d AND %S=%d");
const TInt KFormatCharCountDscItemOnId=32; //for the %S and %d in the format string
const TInt KSqlSelectDscItemOnIdLength = KSqlSelectDscItemOnId().Length() + KDscIdCol().Length() + KItemIdCol().Length() + 
			KFileNameCol().Length() + KArgListCol().Length() + KStartMethodCol().Length() + KTimeoutCol().Length() + 
			KNoOfRetriesCol().Length() + KMonitorCol().Length() + KStartupTypeCol().Length() + KViewlessCol().Length() + 
			KStartInBackgroundCol().Length() + KItemTable().Length() + KDscIdCol().Length() + KMaxLengthOfTInt + 
			KItemIdCol().Length() + KMaxLengthOfTInt - KFormatCharCountDscItemOnId;

_LIT(KSqlSelectDscItemOnName,"SELECT %S, %S, %S, %S, %S, %S, %S, %S, %S, %S, %S FROM %S WHERE %S=%d AND %S='%S' AND %S ='%S'"); 
const TInt KFormatCharCountDscItemOnName=36; //	the extra characters for the %S, %d in the format string
const TInt KSqlSelectDscItemOnNameLength = KSqlSelectDscItemOnName().Length() + KDscIdCol().Length() + KItemIdCol().Length() + 
			KFileNameCol().Length() + KArgListCol().Length() + KStartMethodCol().Length() + KTimeoutCol().Length() + 
			KNoOfRetriesCol().Length() + KMonitorCol().Length() + KStartupTypeCol().Length() + KViewlessCol().Length() + 
			KStartInBackgroundCol().Length() + KItemTable().Length() + KDscIdCol().Length() + KMaxLengthOfTInt + 
			KFileNameCol().Length() + KArgListCol().Length() - KFormatCharCountDscItemOnName;

_LIT(KSqlQueryAllItemIds,"SELECT %S FROM %S WHERE %S=%d ORDER BY %S");
const TInt KFormatCharCountQueryAllItemIds = 10;
const TInt KSqlQueryAllItemIdsLength = KSqlQueryAllItemIds().Length() + KItemIdCol().Length() + KItemTable().Length() + 
			KDscIdCol().Length() + KMaxLengthOfTInt + KItemIdCol().Length() - KFormatCharCountQueryAllItemIds;

_LIT(KSqlSelectDsc,"SELECT %S FROM %S WHERE %S=%d");
const TInt KFormatCharCountSelectDsc = 8;
const TInt KSqlSelectDscLength = KSqlSelectDsc().Length() + KDscIdCol().Length() + KDscTable().Length() + 
			KDscIdCol().Length() + KMaxLengthOfTInt - KFormatCharCountSelectDsc; 

_LIT(KSqlSelectDscDescription,"SELECT %S FROM %S WHERE %S=%d");
const TInt KFormatCharCountSelectDscDescription = 8;
const TInt KSqlSelectDscDescriptionLength = KSqlSelectDscDescription().Length() + KDescriptionCol().Length() + KDscTable().Length() + 
			KDscIdCol().Length() + KMaxLengthOfTInt - KFormatCharCountSelectDscDescription;

_LIT(KSqlSelectAllItemsInDsc,"SELECT %S, %S, %S, %S, %S, %S, %S, %S, %S, %S, %S FROM %S WHERE %S=%d ORDER BY %S"); 
const TInt KFormatCharCountSelectAllItemsInDsc = 30;
const TInt KSqlSelectAllItemsInDscLength = KSqlSelectAllItemsInDsc().Length() + 
					KDscIdCol().Length() + KItemIdCol().Length() + KFileNameCol().Length() + KArgListCol().Length() + 
					KStartMethodCol().Length() + KTimeoutCol().Length() + KNoOfRetriesCol().Length() + KMonitorCol().Length() + 
					KStartupTypeCol().Length() + KViewlessCol().Length() + KStartInBackgroundCol().Length() +
					KItemTable().Length() +	KDscIdCol().Length() + KMaxLengthOfTInt + KItemIdCol().Length() - KFormatCharCountSelectAllItemsInDsc;

//
// SQL DML definitions
//
_LIT(KSqlUpdateUsingName,"UPDATE %S SET %S=%d,%S=%d, %S=%d,%S=%b,%S=%d,%S=%b, %S=%b WHERE %S=%d AND %S='%S' AND %S='%S'");
const TInt KFormatCharCountUpdateUsingName = 42;
const TInt KSqlUpdateUsingNameLength = KSqlUpdateUsingName().Length() + KItemTable().Length() + 
			KStartMethodCol().Length() + KMaxLengthOfEnum + KTimeoutCol().Length() + KMaxLengthOfTInt + 
			KNoOfRetriesCol().Length() + KMaxLengthOfTInt + KMonitorCol().Length() + KMaxLengthOfBoolean + 
			KStartupTypeCol().Length() + KMaxLengthOfEnum + KViewlessCol().Length() + KMaxLengthOfBoolean + 
			KStartInBackgroundCol().Length() + KMaxLengthOfBoolean + KDscIdCol().Length() +  KMaxLengthOfTInt + 
			KFileNameCol().Length() + KArgListCol().Length() - KFormatCharCountUpdateUsingName;

_LIT(KSqlUpdateUsingId,"UPDATE %S SET %S=%d,%S=%d, %S=%d,%S=%b,%S=%d,%S=%b, %S=%b WHERE %S=%d AND %S=%d");
const TInt KFormatCharCountUpdateUsingId = 38;
const TInt KSqlUpdateUsingIdLength = KSqlUpdateUsingName().Length() + KItemTable().Length() + 
			KStartMethodCol().Length() + KMaxLengthOfEnum + KTimeoutCol().Length() + KMaxLengthOfTInt + 
			KNoOfRetriesCol().Length() + KMaxLengthOfTInt + KMonitorCol().Length() + KMaxLengthOfBoolean + 
			KStartupTypeCol().Length() + KMaxLengthOfEnum + KViewlessCol().Length() + KMaxLengthOfBoolean + 
			KStartInBackgroundCol().Length() + KMaxLengthOfBoolean + KDscIdCol().Length() +  KMaxLengthOfTInt + 
			KItemIdCol().Length() + KMaxLengthOfTInt - KFormatCharCountUpdateUsingId;

_LIT(KSqlDeleteItemUsingId,"DELETE FROM %S WHERE %S=%d AND %S=%d");
const TInt KFormatCharCountDeleteItemUsingId = 8;
const TInt KSqlDeleteItemUsingIdLength = KSqlDeleteItemUsingId().Length() + KItemTable().Length() + KDscIdCol().Length() + 
			KMaxLengthOfTInt + KItemIdCol().Length() + KMaxLengthOfTInt - KFormatCharCountDeleteItemUsingId;

_LIT(KSqlDeleteItemUsingName,"DELETE FROM %S WHERE %S=%d AND %S='%S' AND %S= '%S'");
const TInt KFormatCharCountDeleteItemUsingName = 14;
const TInt KSqlDeleteItemUsingNameLength = KSqlDeleteItemUsingName().Length() + KItemTable().Length() + KDscIdCol().Length() + 
			KMaxLengthOfTInt + KFileNameCol().Length() + KArgListCol().Length() - KFormatCharCountDeleteItemUsingName;

_LIT(KSqlInsertDscItem,"INSERT INTO %S (%S,%S,%S,%S,%S,%S,%S,%S,%S,%S,%S) VALUES (%d,%d,'%S','%S',%d,%d,%d,%b,%d,%b,%b)");
const TInt KFormatCharCountInsertDscItem = 46;
const TInt KSqlInsertDscItemLength = KSqlInsertDscItem().Length() + KItemTable().Length() + 
			KDscIdCol().Length() + KItemIdCol().Length() + KFileNameCol().Length() + KArgListCol().Length() + 
			KStartMethodCol().Length() + KTimeoutCol().Length() + KNoOfRetriesCol().Length() + KMonitorCol().Length() +
			KStartupTypeCol().Length() + KViewlessCol().Length() + KStartInBackgroundCol().Length() +
			(4*KMaxLengthOfTInt) + (2*KMaxLengthOfEnum) + (3*KMaxLengthOfBoolean) - KFormatCharCountInsertDscItem;

_LIT(KSqlInsertDsc,"INSERT INTO %S (%S,%S) VALUES (%d,'%S')");
const TInt KFormatCharCountInsertDsc = 10;
const TInt KSqlInsertDscLength = KSqlInsertDsc().Length() + KDscTable().Length() + KDscIdCol().Length() + KDescriptionCol().Length() +
			KMaxLengthOfTInt - KFormatCharCountInsertDsc;

_LIT(KSqlDeleteUsingId,"DELETE FROM %S WHERE %S=%d ");
const TInt KFormatCharCountDeleteUsingId = 6;
const TInt KMaxTableNameLength = Max(KItemTable().Length(), KDscTable().Length());
const TInt KSqlDeleteUsingIdLength = KSqlDeleteUsingId().Length() + KMaxTableNameLength + KDscIdCol().Length() + KMaxLengthOfTInt - 
			KFormatCharCountDeleteUsingId;

//
//SQL DDL definitions
//
_LIT(KSqlCreateItemTable,"CREATE TABLE %S (%S INTEGER NOT NULL, %S INTEGER NOT NULL, %S VARCHAR (%d) NOT NULL, %S VARCHAR(%d), %S TINYINT, %S INTEGER, %S INTEGER, %S BIT, %S TINYINT, %S BIT, %S BIT)");
const TInt KFormatCharCountCreateItemTable = 28;
const TInt KSqlCreateItemTableLength = KSqlCreateItemTable().Length() + KItemTable().Length() + KDscIdCol().Length() + 
				KItemIdCol().Length() + KFileNameCol().Length() + KMaxLengthOfTInt + KArgListCol().Length() + KMaxLengthOfTInt +
				KStartMethodCol().Length() + KTimeoutCol().Length() + KNoOfRetriesCol().Length() + KMonitorCol().Length() +
				KStartupTypeCol().Length() + KViewlessCol().Length() + KStartInBackgroundCol().Length()	- KFormatCharCountCreateItemTable;

_LIT(KSqlCreateDscTable,"CREATE TABLE %S (%S INTEGER NOT NULL, %S VARCHAR(%d))");	
const TInt KFormatCharCountCreateDscTable = 8;
const TInt KSqlCreateDscTableLength = KSqlCreateDscTable().Length() + KDscTable().Length() + KDscIdCol().Length() + 
			KDescriptionCol().Length() + KMaxLengthOfTInt - KFormatCharCountCreateDscTable;

_LIT(KSqlCreateDscIndex,"CREATE UNIQUE INDEX %S ON %S (%S)");
const TInt KFormatCharCountCreateDscIndex = 6;
const TInt KSqlCreateDscIndexLength = KSqlCreateDscIndex().Length() + KIdIndex().Length() + KDscTable().Length() + KDscIdCol().Length() - 
			KFormatCharCountCreateDscIndex;

_LIT(KSqlCreateItemIndex,"CREATE UNIQUE INDEX %S ON %S (%S, %S)");
const TInt KFormatCharCountCreateItemIndex = 8;
const TInt KSqlCreateItemIndexLength = KSqlCreateItemIndex().Length() + KIndex().Length() + KItemTable().Length() + KDscIdCol().Length() +
			KItemIdCol().Length() - KFormatCharCountCreateItemIndex;

const TInt KMaxDdlLength = Max(Max(KSqlCreateDscTableLength,KSqlCreateItemTableLength), Max(KSqlCreateDscIndexLength,KSqlCreateItemIndexLength));
// The maximum length for varchar
const TInt KDscStoreMaxStringLength = KDbMaxStrLen;


/** Help, print out SQL query */
static void DebugPrint(const TDesC& aQuery)
	{
	(void)aQuery;
#ifdef _DEBUG
	RDebug::RawPrint(aQuery);
#endif
	}

/**
Verify aItem. Will leave for following reasons:
- if filename is empty
- if filename or argList is >KDbMaxStrLen
*/
static void LeaveIfFileParamsNotValidL(const CDscItem& aItem)
	{
	//filename and args in a CDscItem is guaranteed to be trimmed of unnecessary whitespace
	if(	(aItem.FileName().Length() == 0) || 
		(aItem.FileName().Length() > KDbMaxStrLen) ||
		(aItem.Args().Length() > KDbMaxStrLen) )
		{
		User::Leave(KErrArgument);
		}
	}

CDscDatabase* CDscDatabase::NewL()
	{
	CDscDatabase* self = new(ELeave) CDscDatabase();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CDscDatabase::ConstructL()
	{
	//Open database connection 
	User::LeaveIfError(iDbsSession.Connect());
	
	TBuf<KMaxDatabaseName> databaseName;
	GetDatabaseNameL(databaseName);
	const TInt error = iDatabase.Open(iDbsSession, databaseName, KSecureFormat);
	if(KErrNotFound == error)
		{
		CreateDatabaseL(databaseName);
		}
	else
		{
		User::LeaveIfError(error);
		}
	}	

//Default constructor	
CDscDatabase::CDscDatabase() : iIsEnumOpened(EFalse)
	{
	}
	
CDscDatabase::~CDscDatabase()
	{
	if(iIsEnumOpened)
		{
		Rollback();
		}
	iView.Close();
	iDatabase.Close();
	iDbsSession.Close();
	}

//Destroy DSC database
TInt CDscDatabase::DeleteDb()
	{
	return iDatabase.Destroy();
	}

void CDscDatabase::CreateDatabaseL(const TDesC& aDatabaseName)
	{
	//create it and also open it
	User::LeaveIfError(iDatabase.Create(iDbsSession, aDatabaseName, KSecureFormat));
	
	//Create tables for DSC database
	TRAPD(error, CreateTablesL());
	if (KErrNone != error)
		{
		//Create table fail,  destroy the DB, so next time can recreate it
		DeleteDb();
		}
	
	User::LeaveIfError(error);
	}

void CDscDatabase::CreateTablesL()
	{
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KMaxDdlLength);	
		
	//Start a new transaction		
	DatabaseBeginLC();

	//Create Table DSC
	sqlCmd.Format(KSqlCreateDscTable, &KDscTable, &KDscIdCol, &KDescriptionCol, KDbUndefinedLength);
	DebugPrint(sqlCmd);
	User::LeaveIfError(iDatabase.Execute(sqlCmd));

	//Create unique index for Table DSC
	sqlCmd.Format(KSqlCreateDscIndex, &KIdIndex, &KDscTable, &KDscIdCol);
	DebugPrint(sqlCmd);
 	User::LeaveIfError(iDatabase.Execute(sqlCmd));
 	
	//Create Table DscItem	
	sqlCmd.Format(KSqlCreateItemTable, &KItemTable, &KDscIdCol, &KItemIdCol, &KFileNameCol, KDbUndefinedLength, &KArgListCol,
				KDbUndefinedLength, &KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol);
	DebugPrint(sqlCmd);
	User::LeaveIfError(iDatabase.Execute(sqlCmd));
	
	//Create unique index for Table DscItem
	sqlCmd.Format(KSqlCreateItemIndex, &KIndex, &KItemTable, &KDscIdCol, &KItemIdCol);
	DebugPrint(sqlCmd);
	User::LeaveIfError(iDatabase.Execute(sqlCmd));
	
	DatabaseCommitL(); //CommitL + CleanupStack::Pop()
	
 	CleanupStack::PopAndDestroy(&sqlCmd);
	}

//Add a DSC with aDscId to DSC DB. if the aDscId exists, leave with KErrAlreadyExists
void CDscDatabase::CreateDscL(const TUid& aDscId, const TDesC& aDescription)
	{
	//The maximum length of aDescription is KDbMaxStrLen
	if(aDescription.Length() > KDbMaxStrLen)
		{
		User::Leave(KErrArgument);
		}
	
	//If the DSC is opened for enumeration, leave with KErrLocked
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}
		
	//Start a new transaction		
	DatabaseBeginLC();

	//Insert aDscId in Table DSC. If aDscId exists, will leave with KErrAlreadyExists	
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KSqlInsertDscLength + aDescription.Length());
	
	sqlCmd.Format(KSqlInsertDsc, &KDscTable, &KDscIdCol, &KDescriptionCol, aDscId, &aDescription);
	DebugPrint(sqlCmd);
	User::LeaveIfError(iDatabase.Execute(sqlCmd));
	
	CleanupStack::PopAndDestroy(&sqlCmd);
	
	DatabaseCommitL(); //CommitL + CleanupStack::Pop()
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
		
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KSqlDeleteUsingIdLength);
	
	//Delete all items related with aDscId first
	sqlCmd.Format(KSqlDeleteUsingId, &KItemTable, &KDscIdCol, aDscId);
	DebugPrint(sqlCmd);
	User::LeaveIfError(iDatabase.Execute(sqlCmd));

	//Then delete the row in Table DSC
	sqlCmd.Format(KSqlDeleteUsingId, &KDscTable, &KDscIdCol, aDscId);	
	DebugPrint(sqlCmd);
	User::LeaveIfError(iDatabase.Execute(sqlCmd));
	
	CleanupStack::PopAndDestroy(&sqlCmd);
	
	DatabaseCommitL(); //CommitL + CleanupStack::Pop()
	}

//Check the existance of a DSC	
TBool CDscDatabase::DscExistsL(const TUid& aDscId) const
	{
	RDbView view;
	CleanupClosePushL(view);

	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KSqlSelectDscLength);

	sqlCmd.Format(KSqlSelectDsc, &KDscIdCol, &KDscTable, &KDscIdCol, aDscId);
	DebugPrint(sqlCmd);
	User::LeaveIfError(view.Prepare(iDatabase, sqlCmd));
	User::LeaveIfError(view.EvaluateAll());  
	const TBool dscExists = !view.IsEmptyL(); 
	
	CleanupStack::PopAndDestroy(&sqlCmd);
	CleanupStack::PopAndDestroy(&view);
	
	return (dscExists);
	}

void CDscDatabase::GetDscDescriptionL(const TUid &aDscId, TDes& aDescription) const
	{	
	RDbView view;
	CleanupClosePushL(view);

	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KSqlSelectDscDescriptionLength);
	
	sqlCmd.Format(KSqlSelectDscDescription, &KDescriptionCol, &KDscTable, &KDscIdCol, aDscId);
	DebugPrint(sqlCmd);
	User::LeaveIfError(view.Prepare(iDatabase, sqlCmd));
	User::LeaveIfError(view.EvaluateAll());  
	CleanupStack::PopAndDestroy(&sqlCmd);
	
	if(view.IsEmptyL())
		{
		User::Leave(KErrNotFound);
		}
	
 	view.FirstL();
	view.GetL();
	
	//Check the length of aDescription
	TPtrC description(view.ColDes(1));
	if (description.Length() > aDescription.MaxLength())
		{
		User::Leave(KErrOverflow);
		}
	
	aDescription.Zero();	
	aDescription=description;
	
	CleanupStack::PopAndDestroy(&view);
	}

//Check the existance of aItem
TBool CDscDatabase::ItemExistsL( const CDscItem& aItem) const
	{
	RDbView view;
	CleanupClosePushL(view);

	QueryItemL(view, aItem);
	const TBool itemExists = !view.IsEmptyL();

	CleanupStack::PopAndDestroy(&view);  
	return (itemExists);
	}

//Query the aItem from Table DscItem	
void CDscDatabase::QueryItemL(RDbView& aView, const CDscItem& aItem) const
	{
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	
	if (aItem.ItemId())
		{
		sqlCmd.CreateL(KSqlSelectDscItemOnIdLength);
		sqlCmd.Format(	KSqlSelectDscItemOnId, &KDscIdCol, &KItemIdCol, &KFileNameCol, 
						&KArgListCol,&KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, 
						&KMonitorCol, &KStartupTypeCol, &KViewlessCol, &KStartInBackgroundCol, 
						&KItemTable, &KDscIdCol, aItem.DscId(), &KItemIdCol, aItem.ItemId());
		}
	else 
		{
		const TPtrC filename = aItem.FileName();
		const TPtrC argList = aItem.Args();	//whitespace already trimmed
		
		LeaveIfFileParamsNotValidL(aItem);
			
		const TInt length = KSqlSelectDscItemOnNameLength + filename.Length() + argList.Length();
		sqlCmd.CreateL(length);
		sqlCmd.Format(KSqlSelectDscItemOnName, &KDscIdCol, &KItemIdCol, &KFileNameCol,&KArgListCol, &KStartMethodCol, 
					 &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol, &KViewlessCol, &KStartInBackgroundCol, 
					 &KItemTable, &KDscIdCol, aItem.DscId(), &KFileNameCol, &filename, &KArgListCol, &argList);
		}

	DebugPrint(sqlCmd);
	
	User::LeaveIfError(aView.Prepare(iDatabase, sqlCmd));
	User::LeaveIfError(aView.EvaluateAll());  //no error for non existing item
	CleanupStack::PopAndDestroy(&sqlCmd);	
	}


//Open a view of items with aDscID for enumeration, need to 
//call EnumClose() to close the view after enumeration.	
void CDscDatabase::EnumOpenLC(const TUid& aDscId)
	{
	//If the DSC is opened for enumeration, leave with KErrLocked
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}
		
	//Start a new transaction to add read-lock on DSC	
	EnumBeginLC();
		
	//Leave with KErrNotFound if aDscId doesn't exist
	if (!DscExistsL(aDscId))
		{
		User::Leave(KErrNotFound);
		}

	iIsEnumOpened = ETrue;

	//Open a view contains all items in aDscId
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KSqlSelectAllItemsInDscLength);
	
	sqlCmd.Format(KSqlSelectAllItemsInDsc, &KDscIdCol, &KItemIdCol, &KFileNameCol, &KArgListCol,
				&KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol, &KItemTable, &KDscIdCol, aDscId, &KItemIdCol);
	DebugPrint(sqlCmd);
	User::LeaveIfError(iView.Prepare(iDatabase, sqlCmd));
	User::LeaveIfError(iView.EvaluateAll());  //no error for non existing item
	CleanupStack::PopAndDestroy(&sqlCmd);
	}


//Enumerating the view opened by EnumOpenLC(), if there are no item in the DSC
//or has reached the end of the list, return NULL	
CDscItem* CDscDatabase::EnumReadNextL()
	{
	if (!iIsEnumOpened)
		{
		//EnumOpenLC() has not been called.
		User::Leave(KErrNotReady);
		}
		
	if (!iView.NextL())
		{
		//reached to the end of the view
		return NULL;
		}
	
	//Read item from the view
	CDscItem* item=CDscItem::NewL();
	CleanupStack::PushL(item);
	ReadDscItemL(iView, *item);

	CleanupStack::Pop(item);
	return item;
	}	

//Read aItem from the view.
//Cannot use ReadItemL because we want the read-lock to remain
void CDscDatabase::ReadDscItemL(RDbRowSet& rowSet, CDscItem& aItem) const
	{	
	rowSet.GetL();

	aItem.SetDscId(TUid::Uid(rowSet.ColInt(1)));
	if( 0 == aItem.ItemId() )
		{
		aItem.SetItemId(rowSet.ColInt(2));
		}
	else
		{
		__ASSERT_ALWAYS(aItem.ItemId() == rowSet.ColInt(2), PanicNow(KPanicDsc, EIdCannotChange));
		}
	aItem.SetFileParamsL(rowSet.ColDes(3), rowSet.ColDes(4));
	aItem.SetStartMethodL(static_cast<TStartMethod> (rowSet.ColInt8(5)));
	aItem.SetNoOfRetriesL(rowSet.ColInt(7));
	aItem.SetTimeoutL(rowSet.ColInt32(6));
	aItem.SetMonitored(rowSet.ColInt8(8));
	aItem.SetStartupType(static_cast<TStartupType> (rowSet.ColInt8(9)));
	aItem.SetViewless(rowSet.ColInt8(10));
	aItem.SetStartInBackground( rowSet.ColInt8(11));
	}

//Close the view opened by EnumOpenLC() and rollback the transaction
void CDscDatabase::EnumClose()
	{
	//To avoid double close
	if (iIsEnumOpened)
		{
		iView.Close();
		iIsEnumOpened=EFalse;
		
		//Roll back transaction
		Rollback();
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
	
	//verify data integrity,
	LeaveIfFileParamsNotValidL(aItem);
	if(aItem.ItemId() != 0)
		{
		User::Leave(KErrArgument);
		}

	//Start transaction
	DatabaseBeginLC();

	//Leave if aDscId doesn't exist
	if (!DscExistsL(aItem.DscId()))
		{
		User::Leave(KErrNotFound);
		}

	//Leave if aItem exists
	if (ItemExistsL(aItem))
		{
		User::Leave(KErrAlreadyExists);
		}

	const TPtrC filename = aItem.FileName();
	const TPtrC argList = aItem.Args();	//whitespace already trimmed
	const TInt itemId = GetNextItemIdL(aPos, aItem.DscId());
	
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	sqlCmd.CreateL(KSqlInsertDscItemLength + filename.Length() + argList.Length());
	
	//insert the item
	sqlCmd.Format(KSqlInsertDscItem, &KItemTable, &KDscIdCol, &KItemIdCol, &KFileNameCol, &KArgListCol,
				&KStartMethodCol, &KTimeoutCol, &KNoOfRetriesCol, &KMonitorCol, &KStartupTypeCol,
				&KViewlessCol, &KStartInBackgroundCol, aItem.DscId(), itemId, &filename, &argList, aItem.StartMethod(),
				aItem.Timeout(), aItem.NoOfRetries(), aItem.Monitored(),
				aItem.StartupType(), aItem.Viewless(), aItem.StartInBackground());

	DebugPrint(sqlCmd);
	
	User::LeaveIfError(iDatabase.Execute(sqlCmd));
	CleanupStack::PopAndDestroy(&sqlCmd);
	DatabaseCommitL(); //CommitL + CleanupStack::Pop()
	
	//Now aItem is persistent, set the ItemId so it can be read by the client
	aItem.SetItemId(itemId);
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
	
	if (aItem.ItemId())
		{
		sqlCmd.CreateL(KSqlDeleteItemUsingIdLength);
		sqlCmd.Format(KSqlDeleteItemUsingId, &KItemTable, &KDscIdCol, aItem.DscId(), &KItemIdCol, aItem.ItemId());
		}
	else
		{
		const TPtrC filename = aItem.FileName();
		const TPtrC argList = aItem.Args();	//whitespace already trimmed
		
		LeaveIfFileParamsNotValidL(aItem);
	
		const TInt length = KSqlDeleteItemUsingNameLength + filename.Length() + argList.Length();
		sqlCmd.CreateL(length);
		sqlCmd.Format(KSqlDeleteItemUsingName, &KItemTable, &KDscIdCol, aItem.DscId(), &KFileNameCol, &filename, &KArgListCol, &argList);
		}

	DebugPrint(sqlCmd);

	User::LeaveIfError(iDatabase.Execute(sqlCmd));
	CleanupStack::PopAndDestroy(&sqlCmd);
	DatabaseCommitL(); //CommitL + CleanupStack::Pop()
	}
	
//Update aItem. 
void CDscDatabase::UpdateItemL(const CDscItem& aItem)
	{
	//Leave if DB is opened for enumeration
	if (iIsEnumOpened)
		{
		User::Leave(KErrLocked);
		}

	if(aItem.ItemId() == 0)
		{
		LeaveIfFileParamsNotValidL(aItem);
		}

	DatabaseBeginLC();

	//Leave if aItem does not exist
	if (!ItemExistsL( aItem))
		{
		User::Leave(KErrNotFound);	
		}
	
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);

	if (aItem.ItemId())
		{
		sqlCmd.CreateL(KSqlUpdateUsingIdLength);
		sqlCmd.Format(KSqlUpdateUsingId, &KItemTable, &KStartMethodCol, aItem.StartMethod(), &KTimeoutCol, aItem.Timeout(),
				&KNoOfRetriesCol, aItem.NoOfRetries(), &KMonitorCol, aItem.Monitored(), &KStartupTypeCol, aItem.StartupType(),
				&KViewlessCol, aItem.Viewless(), &KStartInBackgroundCol, aItem.StartInBackground(),
				&KDscIdCol, aItem.DscId(), &KItemIdCol, aItem.ItemId());
		}
	else 
		{
		const TPtrC filename = aItem.FileName();
		const TPtrC argList = aItem.Args();	//whitespace already trimmed
		
		const TInt length = KSqlUpdateUsingNameLength + filename.Length() + argList.Length();
		sqlCmd.CreateL(length);		
		sqlCmd.Format(KSqlUpdateUsingName, &KItemTable, &KStartMethodCol, aItem.StartMethod(), &KTimeoutCol, aItem.Timeout(),
						&KNoOfRetriesCol, aItem.NoOfRetries(), &KMonitorCol, aItem.Monitored(), &KStartupTypeCol, aItem.StartupType(),
						&KViewlessCol, aItem.Viewless(), &KStartInBackgroundCol, aItem.StartInBackground(),
						&KDscIdCol, aItem.DscId(), &KFileNameCol, &filename, &KArgListCol, &argList);
		}

	DebugPrint(sqlCmd);
		
	User::LeaveIfError(iDatabase.Execute(sqlCmd));	
	CleanupStack::PopAndDestroy(&sqlCmd);
	DatabaseCommitL(); //CommitL + CleanupStack::Pop()
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

	RDbView view;
	CleanupClosePushL(view);
	
	QueryItemL(view,  aItem);
	if(view.CountL()==0)
		{
		 //aItem does not exist
		User::Leave(KErrNotFound);	
		}
	else
		{
		view.FirstL(); 
		ReadDscItemL(view, aItem);
		}
	
	CleanupStack::PopAndDestroy(2); //view and CleanupDatabaseRollback
	}

//Rollback transaction	
void CDscDatabase::Rollback()
	{
	iDatabase.Rollback();
	if(iDatabase.IsDamaged())
		{
		iDatabase.Recover();
		}
	}

//Start a new transaction.Pushes cleanup rollback action to the cleanup stack. 
void CDscDatabase::DatabaseBeginLC()
	{
	User::LeaveIfError(iDatabase.Begin());	
	CleanupStack::PushL(TCleanupItem(CleanupDatabaseRollback, this));
	}
	
//Commit transaction. Pops cleanup rollback action
void CDscDatabase::DatabaseCommitL()
	{
	User::LeaveIfError(iDatabase.Commit());
	// Pop CleanupDatabaseRollback 
	CleanupStack::Pop(); 
	}

//Cleanup rollback action for DatabaseBeginLC()
void CDscDatabase::CleanupDatabaseRollback(TAny *aDatabase)
	{
	ASSERT(aDatabase);
	CDscDatabase* database = static_cast <CDscDatabase*>(aDatabase);
	database->Rollback();
	}

//Start a new transaction for enumeration.Pushes cleanup rollback action to the cleanup stack. 
void CDscDatabase::EnumBeginLC()
	{
	User::LeaveIfError(iDatabase.Begin());	
	CleanupStack::PushL(TCleanupItem(CleanupEnumRollback, this));
	}
		
//Cleanup rollback action for EnumOpenLC()
void CDscDatabase::CleanupEnumRollback(TAny *aDatabase)
	{
	ASSERT(aDatabase);
	if(aDatabase)
		static_cast <CDscDatabase*>(aDatabase)->EnumClose();
	}
	
//Get DSC database name
void CDscDatabase::GetDatabaseNameL(TDes& aDatabaseName)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	// retrieve system drive
	TDriveNumber driveNumber = fs.GetSystemDrive();
 
	// convert to char
    TChar driveChar;
    User::LeaveIfError(RFs::DriveToChar(driveNumber, driveChar));
 
	aDatabaseName.Format(KDatabaseName, (TUint)driveChar);
	CleanupStack::PopAndDestroy(&fs);
	}

	
/**
Helper function. Get the next available itemId for the item.
Reads all existing ItemIds for the specified DscId, then calculates last+1
or first-1 depending on aPos. Will never return 0 as its reserved to mean
not yet persistent.
*/
TInt CDscDatabase::GetNextItemIdL(TDscPosition aPos, const TUid& aDscId) const
	{	
	RDbView view;
	CleanupClosePushL(view);
	RBuf sqlCmd;
	CleanupClosePushL(sqlCmd);
	
	sqlCmd.CreateL(KSqlQueryAllItemIdsLength);
	sqlCmd.Format(KSqlQueryAllItemIds, &KItemIdCol, &KItemTable,&KDscIdCol, aDscId, &KItemIdCol );

	DebugPrint(sqlCmd);
	
	User::LeaveIfError(view.Prepare(iDatabase, sqlCmd));
	User::LeaveIfError(view.EvaluateAll());  		
	CleanupStack::PopAndDestroy(&sqlCmd);
	
	TInt nextId = 1; //add first item with id=1 and reserve 0 to mean "not yet persistent"
	if (aPos==ELast && view.LastL())
		{
		//add at ELast: pos =max of itemId+1
		view.GetL();
		nextId = view.ColInt(1);
		if(KMaxTInt == nextId)
			{
			User::Leave(KErrOverflow);
			}
		//increase, make sure to not use 0 as itemid in the database
		nextId = (-1==nextId) ? (nextId+2) : (nextId+1);
		}		
	else if (aPos==EFirst && view.FirstL())
		{
		//add at EFirst: pos=min of itemId-1	
		view.GetL();
		nextId = view.ColInt(1);
		if(KMinTInt == nextId)
			{
			User::Leave(KErrUnderflow);
			}
		//decrease, but reserve 0 to mean "not yet persistent"
		nextId = (1==nextId) ? (nextId-2) : (nextId-1);
		}
	
	CleanupStack::PopAndDestroy(&view);
	return nextId;
	}

TInt CDscDatabase::MaxStringLength()
	{
	return KDbMaxStrLen; //defined in d32dbms.h
	}

