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
// Table/Record Protection
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include "protectdb.h"
#include "CDBSTD.H"
#include "commdb_impl.H"
#include "Commsdat_Log.h"
/**
@internalComponent
*/

/** Creates a `CCommsDatabaseProtect`. If the database filestore does not exist
    the default filestore is duplicated. If no default filestore exists an
    empty database is created. */
EXPORT_C CCommsDatabaseProtect* CCommsDatabaseProtect::NewL()
    {
    CCommsDatabaseProtect* r = new(ELeave) CCommsDatabaseProtect();
    CleanupStack::PushL(r);
    r->ConstructL();
    CleanupStack::Pop(r);
    return r;
    }

/** Creates a `CCommsDatabaseProtect`. If the database filestore exists and
    `aUseDefaultDb` is True the default filestore is duplicated. Otherwise
    an empty database is created. */
EXPORT_C CCommsDatabaseProtect* CCommsDatabaseProtect::NewL(TBool /*aUseDefaultDb*/)
    {
    CCommsDatabaseProtect* r = new(ELeave) CCommsDatabaseProtect();
    CleanupStack::PushL(r);
    r->ConstructL();
    CleanupStack::Pop(r);
    return r;
    }

/** Creates a `CCommsDatabaseProtect` as with `NewL()`. The method of opening:
    (Created, CopiedDefault or Opened) is returned in `aOpeningMethod`. */
EXPORT_C CCommsDatabaseProtect* CCommsDatabaseProtect::NewL(TCommDbOpeningMethod& /*aOpeningMethod*/)
    {
    CCommsDatabaseProtect* r = new(ELeave) CCommsDatabaseProtect();
    CleanupStack::PushL(r);
    r->ConstructL();
    CleanupStack::Pop(r);
    return r;
    }

EXPORT_C CCommsDatabaseProtect::~CCommsDatabaseProtect()
    {
    delete iTableExt;
    }

CCommsDatabaseProtect::CCommsDatabaseProtect()
    {
    }

void CCommsDatabaseProtect::ConstructL()
    {
    iImpl->iDbs = CMDBSession::NewL(KCDVersion1_1);
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CommsDat Session is created!"));
    
    iTableExt = CCommDbTableExtension::NewL();
    }

void CCommsDatabaseProtect::DoTableProtectionL(CMDBRecordSetBase* aRecordSet, TBool aProtect)
	{
	CMDBSession* dbSession = iImpl->iDbs;
    TBool showHidden = dbSession->IsSetAttributeMask(ECDHidden);

    // set all attribute masks as must be able to see every record (already set for ECDPrivate)
    dbSession->SetAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite );

    aProtect ? aRecordSet->SetAttributes(ECDProtectedWrite) : aRecordSet->ClearAttributes(ECDProtectedWrite);
    aRecordSet->ModifyL(*dbSession);

    if (showHidden)
        {
        dbSession->ClearAttributeMask(ECDNoWriteButDelete | ECDProtectedWrite);
        }
    else
        {
        dbSession->ClearAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite);
        }

	}

EXPORT_C TInt CCommsDatabaseProtect::ProtectTable(const TDesC& aTableName)
/**Sets the access right field in the ACCESS_TYPE_TABLE for the table specified in aTableName to RDbRowSet::EReadOnly
@param aTableName Contains the descriptor of a CommDb table name
@return KErrNone or a standard error code
@capability Dependent on table, see the guide page referenced below.
*/
    {
	CMDBRecordSetBase* rs(NULL);
	TRAPD(err, rs = iTableExt->RecordSetFactoryL(aTableName, EFalse, *iImpl->iDbs));

    if (err == KErrNone)
        {
        TRAP(err, DoTableProtectionL(rs, KProtectTable));
        }

	delete rs;
	
    return err;
    }

EXPORT_C TInt CCommsDatabaseProtect::UnProtectTable(const TDesC& aTableName)
/**Sets the access right field in the ACCESS_TYPE_TABLE for the table specified in aTableName to RDbRowSet::EUpdatable
@param aTableName Contains the descriptor of a CommDb table name
@return KErrNone or a standard error code
@capability Dependent on table, see the guide page referenced below.
*/
    {
    CMDBRecordSetBase* rs(NULL);
	TRAPD(err, rs = iTableExt->RecordSetFactoryL(aTableName, EFalse, *iImpl->iDbs));

    if (err == KErrNone)
        {
        TRAP(err, DoTableProtectionL(rs, KUnprotectTable));
        }
    
    delete rs;

    return err;
    }

EXPORT_C RDbRowSet::TAccess CCommsDatabaseProtect::GetTableAccessL(const TDesC& aTableName)
/** returns the access rights for aTableName from the ACCESS_TYPE_TABLE
@param aTableName Contains the descriptor of a CommDb table name
@return EUpdatable or EReadOnly
@capability Dependent on table, see the guide page referenced below.
*/
    {
    iTableExt->SetRecordSetL(aTableName, *iImpl->iDbs, EFalse, ETrue);
    RDbRowSet::TAccess access;

    User::LeaveIfError(iTableExt->GetTableAccess(access));

    return access;
    }

void CCommsDatabaseProtect::DoTablesProtectionL(CPtrCArray& aTableList, TBool aProtect)
	{
	CMDBRecordSetBase* rs(NULL);
	
    TUint arraySize = aTableList.Count();

    for(TUint loopCount(0); loopCount<arraySize; ++loopCount)
        {
        __FLOG_STMT(TPtrC tableName = aTableList.MdcaPoint(loopCount);)

	    rs = iTableExt->RecordSetFactoryL(aTableList.MdcaPoint(loopCount), EFalse, *iImpl->iDbs);
		CleanupStack::PushL(rs);
	    
		DoTableProtectionL(rs, aProtect);
		
		CleanupStack::Pop(rs);

        __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("Protect Table: Name = %S"), &tableName);
        }
   	}
	
	
EXPORT_C TInt CCommsDatabaseProtect::ProtectTables(CPtrCArray& aTableList)
/**Sets the access right fields in the ACCESS_TYPE_TABLE for the tables specified
in aTableList to RDbRowSet::EReadOnly
@param aTableList array of CommDb table names
@return KErrNone or a standard error code
@capability Dependent on table, see the guide page referenced below.
*/
    {
    if(InTransaction())
        {
        return KErrLocked;
        }

    TInt err = BeginTransaction();
    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("ProtectTables: BeginTransaction returned %d"), err);
    
    // On success execution recordsets and fileds representing tables from the list
    // will be on the cleanupstack   // VCT eh?
	TRAP(err, DoTablesProtectionL(aTableList, ETrue));
	
    if(err != KErrNone)
        {
        RollbackTransaction();
        __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("ProtectTables unsuccessful, err %d, RollbackTransaction()"), err);
        }
    else
    	{
    	err = CommitTransaction();
        __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("ProtectTables successful, CommitTransaction() returned %d"), err);
    	}
	
	return err;    	
    }

EXPORT_C TInt CCommsDatabaseProtect::UnprotectTables(CPtrCArray& aTableList)
/**Sets the access right fields in the ACCESS_TYPE_TABLE for the tables specified
in aTableList to RDbRowSet::EUpdatable
@param aTableList array of CommDb table names
@return KErrNone or a standard error code
@capability Dependent on table, see the guide page referenced below.
*/
    {
    if(InTransaction())
        {
        return KErrLocked;
        }

    TInt err = BeginTransaction();
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("UnProtectTables: BeginTransaction"));
    
    // On success execution recordsets and fileds representing tables from the list
    // will be on the cleanupstack   // VCT ???
    
    TRAP(err, DoTablesProtectionL(aTableList, KUnprotectTable));
    
    if(err != KErrNone)
        {
        RollbackTransaction();
        __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("UnProtectTables unsuccessful, err %d, RollbackTransaction()"), err);
        }
    else
    	{
    	err = CommitTransaction();
        __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("UnProtectTables successful, CommitTransaction() returned %d"), err);
    	}

    return err;
    }

CCommsDbProtectTableView* CCommsDbProtectTableView::NewL(CCommsDatabaseBase& /*aDb*/,const TDesC& /*aTableName*/,const TDbQuery& /*aQuery*/,TBool /*aUseTemplate*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aDb reference to the database to use.
@param aTableName A reference to a descriptor containing the name of a table in the communications database protect table view.
@param aQuery Query to perform
@param aUseTemplate A boolean value whether to use template while creating the object or not
@return A pointer to a communications database protect table view object on the heap.
*/
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }


CCommsDbProtectTableView::CCommsDbProtectTableView(CCommsDatabaseBase& aDb,const TDesC& aTableName)
    : CCommsDbTableView(aDb,aTableName)
/**
Constructor

@param aDb reference to the database to use.
@param aTableName A reference to a descriptor containing the name of a table in the communications database protect table view.
*/
    {
    }

void CCommsDbProtectTableView::ConstructL(const TDbQuery& /*aQuery*/,TBool /*aUseTemplate*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

Prepare and evaluate the table view using the SQL query aQuery
*/
    {
    User::Leave(KErrNotSupported);
    }

EXPORT_C CCommsDbProtectTableView::~CCommsDbProtectTableView()
/**
Destructor.
*/
    {
    }

void CCommsDbProtectTableView::DoProtectionL(CMDBSession* aSession, CMDBElement* aElement, TBool aProtect)
	{
    TBool showHidden = aSession->IsSetAttributeMask(ECDHidden);

    // set all attribute masks as must be able to see every record (already set for ECDPrivate)
    aSession->SetAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite);

    aProtect ? aElement->SetAttributes(ECDProtectedWrite) : aElement->ClearAttributes(ECDProtectedWrite);
    aElement->ModifyL(*aSession);

    if (showHidden)
        {
        aSession->ClearAttributeMask(ECDNoWriteButDelete | ECDProtectedWrite);
        }
    else
        {
        aSession->ClearAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite);
        }
	}

EXPORT_C TInt CCommsDbProtectTableView::ProtectTable()
/**
Protects the  table
Protection of settings
@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
    {
	CMDBRecordSetBase* rs(NULL);
	TInt err = iTableExt->GetRecordSet(rs);
	
	if(err == KErrNone)
		{
	    TRAP(err, DoProtectionL(iDb.iImpl->iDbs, rs, KProtectTable));
		}

    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbProtectTableView::ProtectTable, Err:%d"), err);
    return err;
    }

EXPORT_C TInt CCommsDbProtectTableView::UnprotectTable()
/**
Unprotects the  table

@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
    {
	CMDBRecordSetBase* rs(NULL);
	TInt err = iTableExt->GetRecordSet(rs);
	
	if(err == KErrNone)
		{
	    TRAP(err, DoProtectionL(iDb.iImpl->iDbs, rs, KUnprotectTable));
		}

    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbProtectTableView::UnprotectTable, Err:%d"), err);
    return err;
    }

EXPORT_C TInt CCommsDbProtectTableView::ProtectRecord()
/**
Marks record as read-only so that it cannot be modified or deleted.

@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
    {
	CMDBRecordBase* record(NULL);
	TInt err = iTableExt->GetCurrentRecord(record);
	
	if(err == KErrNone)
		{
	    TRAP(err, DoProtectionL(iDb.iImpl->iDbs, record, KProtectRecord));
		}

    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbProtectTableView::ProtectRecord, Err:%d"), err);
    return err;
    }


EXPORT_C TInt CCommsDbProtectTableView::UnprotectRecord()
/**
Marks record as not read-only so that it can be modified or deleted

@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
    {
	CMDBRecordBase* record(NULL);
	TInt err = iTableExt->GetCurrentRecord(record);
	
	if(err == KErrNone)
		{
	    TRAP(err, DoProtectionL(iDb.iImpl->iDbs, record, KUnprotectRecord));
		}

    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbProtectTableView::UnprotectRecord, Err:%d"), err);
    return err;
    }

EXPORT_C TInt CCommsDbProtectTableView::GetTableAccess(RDbRowSet::TAccess& aAccessType)
/**
Checks table's ReadOnly Setting

@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
    {
    return iTableExt->GetTableAccess(aAccessType);
    }


EXPORT_C TInt CCommsDbProtectTableView::GetRecordAccess(TInt& aAccess)
/**
Check record's protection setting
Currently a record can be Writable (0) or Read Only (1).
Read Only records cannot be amended or deleted until they are unprotected via this API.

@param aAccess On return contains the value of the column ordinal
@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
    {
    CMDBRecordBase* record(NULL);
    TInt err = iTableExt->GetCurrentRecord(record);
    if(err == KErrNone)
        {
        if (record->IsSetAttribute(ECDProtectedWrite))
            {
            aAccess = RDbRowSet::EReadOnly;
            }
        else
            {
            aAccess = RDbRowSet::EUpdatable;
            }
        }
    return err;
    }

// EOF
