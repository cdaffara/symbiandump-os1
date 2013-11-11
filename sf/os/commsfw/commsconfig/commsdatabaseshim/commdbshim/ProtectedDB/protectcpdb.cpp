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

#include "protectcpdb.h"
#include "CDBSTD.H"
#include "commdb_impl.H"
#include "Commsdat_Log.h"

CCommsDbProtectConnectPrefTableView* CCommsDbProtectConnectPrefTableView::NewL(CCommsDatabase& /*aDb*/,const TDbQuery& /*aQuery*/)
/**
Allocates and constructs a new communications database connection preference view object.
aUseTemplate defaults to ETrue

@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aDb reference to the database to use.
@param aQuery Query to perform
@return A pointer to a communications database connection preference view object .
*/
	{
	// Never instantiated in the code,
	// Object of this class is used after casting. There is no need for constructing it!
	User::Leave(KErrNotSupported);
	return NULL;
	}

CCommsDbProtectConnectPrefTableView::CCommsDbProtectConnectPrefTableView(CCommsDatabase& aDb)
	: CCommsDbConnectionPrefTableView(aDb)
/**
Default Constructor
*/
	{}


void CCommsDbProtectConnectPrefTableView::ConstructL(const TDbQuery& /*aQuery*/)
/**
Prepares and evaluates the table view using the SQL query aQuery
	
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aQuery Query to perform
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C CCommsDbProtectConnectPrefTableView::~CCommsDbProtectConnectPrefTableView()
/**
Destructor
*/
	{}
	
void CCommsDbProtectConnectPrefTableView::DoProtectionL(CMDBSession* aSession, CMDBElement* aElement, TBool aProtect)
	{
    TBool showHidden = aSession->IsSetAttributeMask(ECDHidden);

    // set all attribute masks as must be able to see every record (already set for ECD Private)
    aSession->SetAttributeMask(ECDHidden | ECDNoWriteButDelete | ECDProtectedWrite );

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

EXPORT_C TInt CCommsDbProtectConnectPrefTableView::ProtectTable()
/**
Protects table
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

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbProtectConnectPrefTableView::ProtectTable, Err:%d"), err);
    return err;
	}

EXPORT_C TInt CCommsDbProtectConnectPrefTableView::UnprotectTable()
/**
Unprotects table

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

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbProtectConnectPrefTableView::UnprotectTable, Err:%d"), err);
    return err;
	}

EXPORT_C TInt CCommsDbProtectConnectPrefTableView::ProtectRecord()
/**
Marks record as read-only so that it cannot be modified
This does not protect it from deletion.

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


EXPORT_C TInt CCommsDbProtectConnectPrefTableView::UnprotectRecord()
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

EXPORT_C TInt CCommsDbProtectConnectPrefTableView::GetTableAccess(RDbRowSet::TAccess& aAccessType)
/**
Checks table's ReadOnly Setting

@param aAccessType Access type
@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
	{
	return iTableExt->GetTableAccess(aAccessType);
	}


EXPORT_C TInt CCommsDbProtectConnectPrefTableView::GetRecordAccess(TInt& aAccess)
/**
Checks record's protection setting
Currently a record can be Writable (0) or Read Only (1).
Read Only records cannot be amended until they are unprotected via this API.

From v9.1 there is behavioural change. Column ordinal value is replaced with ElementID

@param aAccess On return contains the value of the column ordinal(look line above)
@return KErrNone
@capability Dependent on table, see the guide page referenced below.
*/
	{
	CMDBRecordBase* record(NULL);
	TInt err = iTableExt->GetCurrentRecord(record);
	
	if(err == KErrNone)
		{
		if(record->IsSetAttribute(ECDProtectedWrite))
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

//EOF
