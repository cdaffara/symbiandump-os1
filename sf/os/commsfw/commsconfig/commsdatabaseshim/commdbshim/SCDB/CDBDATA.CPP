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
// Table definitions and CCommsDatabase definitions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <e32math.h>
#include <f32file.h>
#include <cdbpreftable.h>
#include <e32property.h>
#include "CDBSTD.H"
#include "Commsdat_Log.h"
#include "commdb_impl.H"
#include <commsdattypesv1_1.h>
#include <commsdat_partner.h>
#include <commsdatutils.h>

using namespace CommsDat;
using namespace CommsDatUtils;

const TInt KGlobalSettingsRecordID = 1;

// TCommDbPublishSubscribeNotification definitions
CCommsDatabaseBase::
			TCommDbPublishSubscribeNotification::TCommDbPublishSubscribeNotification()
	: iUid(KNullUid), iValue(0)
/**
Default Constructor for the publish and susbcribe notificaton which initialises the Uid to zero.
*/
	{
	}


void CCommsDatabaseBase::
			TCommDbPublishSubscribeNotification::Set(TUid aUid,TInt aVal)
/**
Sets the given values of aUid and aVal to the members iUid and aVal.

@param aUid UID to be set
@param aVal Value to be set
*/
	{
	iUid=aUid;
	iValue=aVal;
	}

// TCommDbSystemAgentNotification definitions
// we still support this class from 9.0 onwards, but to work with pubsub its initialisation
// changes
CCommsDatabaseBase::
			TCommDbSystemAgentNotification::TCommDbSystemAgentNotification()
	: iUid(KNullUid), iValue(0)
/**
Default Constructor for the publish and subscribe notification with the old System Agent
name for compatibility which initialises the Uid to zero.
*/
	{
	}

void CCommsDatabaseBase::
			TCommDbSystemAgentNotification::Set(TUid aUid,TInt aVal)
/**
Sets the given values of aUid and aVal to the members iUid and aVal.

@param aUid UID to be set
@param aVal Value to be set
*/
	{
	iUid=aUid;
	iValue=aVal;
	}
// CCommsDatabaseBase definitions
EXPORT_C CCommsDatabaseBase::CCommsDatabaseBase():	iImplNotCreated(EFalse)
/** Default constructor for internal use only. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabaseBase::CCommsDatabaseBase()"));
	TRAPD(err,iImpl = new(ELeave) CCommsDatabaseImpl());
	if(err != KErrNone)
		{
		iImplNotCreated = ETrue;
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("Creation of implementation class for CCommsDatabaseBase faild with Err:%d"),err);
		}
	}

EXPORT_C CCommsDatabaseBase::~CCommsDatabaseBase()
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabaseBase::~CCommsDatabaseBase()"));
	delete iImpl;
	}

EXPORT_C TVersion CCommsDatabaseBase::Version() const
/** Gets the version from CommsDat.

@return The object containing the version information. */
	{
	if (iImplNotCreated)
		{
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("Warning!!! - > Implementation object is not created at this point"));
		// return the most likely value
		return KCDVersion1_1;
		}

	TVersion versionInUse;

	versionInUse = iImpl->iDbs->VersionInUse();

	__FLOG_STMT(TPtrC name = versionInUse.Name();)
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("Version() - > Version Number %S"), &name);

	return versionInUse;
	}

EXPORT_C TInt CCommsDatabaseBase::BeginTransaction()
/** Marks the start of a transaction and gets a shared read-lock on the database.
Other clients of the database can concurrently acquire a shared read-lock
but no client can gain an exclusive write-lock until this transaction (and
any transaction started by other clients) completes as a result of a call
to either CommitTransaction() or RollbackTransaction().

A transaction consists of one or more record updates and/or insertions and/or
deletions. The process of making changes to records within a view is part
of CCommsDbTableView behaviour.

@see CCommsDbTableView::InsertRecord(), CCommsDbTableView::UpdateRecord(),
CCommsDbTableView::DeleteRecord(), CCommsDbTableView::PutRecordChanges()
and CCommsDbTableView::CancelRecordChanges().

@return KErrNone if the database has been successfully locked; the transaction
can start. KErrLocked if another client already has an exclusive write-lock
on the database.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		return KErrNoMemory;
		}

	TRAPD(err, iImpl->iDbs->OpenTransactionL());

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("BeginTransaction Err:%d"),err);
	return err;
	}

EXPORT_C TInt CCommsDatabaseBase::CommitTransaction()
/** Marks the end of a transaction and commits any changes made since the start
of the transaction. The client's shared read-lock is removed if no write operations
were performed, or the exclusive write-lock is removed if write operations
were performed.

@return KErrNone if successful, otherwise another of the system-wide error
codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		return KErrNoMemory;
		}

	TInt i(0);
	TInt err(KErrNone);

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CommitTransaction Entry Point"));

	TRAP(err, iImpl->iDbs->CommitTransactionL());

    if(err == KErrNone)
        {
        // update client side transaction logic in views
        TInt count = iImpl->iTableViews.Count();
        
        for(i=0;i<count;++i)
            {
            iImpl->iTableViews[i]->iTableExt->CommitTransaction();
            }

        count = iImpl->iConnPrefViews.Count();
        for(i=0;i<count;++i)
            {
            iImpl->iConnPrefViews[i]->iTableExt->CommitTransaction();
            }

        }

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CommitTransaction Err:%d"),err);

	return err;
	}

EXPORT_C void CCommsDatabaseBase::RollbackTransaction()
/** Marks the end of a transaction and abandons any changes made since the start
of the transaction. The database is, in effect, rolled back to the state it
was in at the beginning of the transaction. The client's shared read-lock
is removed if no write operations were performed, or the exclusive write-lock
is removed if write operations were performed.
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("RollbackTransaction Entry Point"));

	TInt err(KErrNone);

	if(iImplNotCreated)
		{
		return;
		}

    TRAP(err, iImpl->iDbs->RollbackTransactionL());
    if (err != KErrNone)
        {
        __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("RollbackTransaction failed in commsdat Err:%d"),err);
        }

		// update client side transaction logic in views
		TInt count = iImpl->iTableViews.Count();
		TInt i;
		for(i=0;i<count;++i)
			{
			iImpl->iTableViews[i]->iTableExt->RollBackTransaction();
			}

		count = iImpl->iConnPrefViews.Count();
		for(i=0;i<count;++i)
			{
			iImpl->iConnPrefViews[i]->iTableExt->RollBackTransaction();
			}

    }

EXPORT_C TBool CCommsDatabaseBase::InTransaction()
/** Tests whether a transaction is in progress, i.e. whether a call to BeginTransaction()
has been called.

@return ETrue if a transaction is in progress (BeginTransaction() has been called but not
CommitTransaction() or RollbackTransaction()); EFalse otherwise. */
	{
	if(iImplNotCreated)
		{
		return EFalse;
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("InTransaction"));
	return iImpl->iDbs->IsInTransaction();
	}

EXPORT_C TBool CCommsDatabaseBase::IsDatabaseWriteLockedL()
/** Tests if the database is write locked.

Tries to open a transaction from another client.

@note Calling InTransaction() is not enough because if the transaction is automatic rather than
explicit, this will still return EFalse.

@return ETrue if the database is write locked, EFalse if not. */
	{
	if(iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	TBool result = iImpl->iDbs->IsInTransaction();

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("IsDatabaseWriteLockedL -> Result:%b"), result);

	return result;
	}

EXPORT_C void CCommsDatabaseBase::ShowHiddenRecords()
/** Ensures that hidden records are included in the search criteria when creating
views using the member functions:

OpenTableLC()

OpenViewMatchingUintLC()

OpenViewMatchingBoolLC()

OpenViewMatchingTextLC()
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("ShowHiddenRecords"));
	iShowHiddenRecords = ETrue;
	// Set mask for hidden Attribute on CommsDat session
    iImpl->iDbs->SetAttributeMask(ECDHidden);
	}

EXPORT_C TInt CCommsDatabaseBase::RequestNotification(TRequestStatus& aStatus)
/** Requests notification when any change is made to the database, whether by this
client or any other.

(If the notifier is not successfully opened the function simply returns an error
code and none of the following applies).

When any change is made to the database the outstanding notification request
completes and TRequestStatus contains a value indicating what type of change
has occurred. The value is one of the enumerators of the RDbNotifier::TEvent
enumeration.

Alternatively, if an outstanding notification request is cancelled by a call
to this CCommsDatabase's CancelRequestNotification() member function, then
the request completes with aStatus set to KErrCancel.

An outstanding notification request is also cancelled if the connection to
the DBMS is severed through a call to DoClose().

This is an asynchonous request.

@param aStatus A reference to the request status object. If the request is
cancelled, this is set to KErrCancel. If the request completes normally, this
is set to one of the enumerators of the RDbNotifier::TEvent enumeration.
@return Any error from opening the notifier: KErrNone if successful, otherwise
another of the system-wide error codes. */
	{
	if(iImplNotCreated)
		{
		return KErrNoMemory;
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabaseBase::RequestNotification(TRequestStatus& aStatus)"));

	return iImpl->RequestNotification(aStatus);
	}

EXPORT_C void CCommsDatabaseBase::CancelRequestNotification()
/** Cancels an outstanding request for notification of changes to the communications
database.

An outstanding request completes with KErrCancel. */
	{
	if(iImplNotCreated)
		{
		return;
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabaseBase::CancelRequestNotification()"));
	
	iImpl->CancelRequestNotification();
	}

EXPORT_C CCommsDbTableView* CCommsDatabaseBase::OpenTableLC(const TDesC& aTableName)
/** Fills the container with all records from table and returns a pointer to view class. The view
excludes hidden records, unless access to them has previously been explicitly
requested by calling the ShowHiddenRecords() member function of this object.

If the process is succesful, the function constructs and returns a pointer
to a CCommsDbTableView object which encapsulates the information on that view.
The pointer is also put onto the cleanup stack.

@param aTableName A reference to a descriptor containing the name of the table
in the communications database to be opened.
@return A pointer to the view object.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabaseBase::OpenTableLC Entry Point"));

	CCommsDbTableView* tableView = CCommsDbTableView::NewLC(*this, aTableName);
    tableView->iTableExt->SetShowHiddenRecords(iShowHiddenRecords);

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("CCommsDatabaseBase::OpenTableLC -> TableName=%S, Hidden=%b"), &aTableName, iShowHiddenRecords);

	return tableView;
	}

EXPORT_C CCommsDbTableView* CCommsDatabaseBase::OpenViewLC(const TDesC& /*aTableName*/,const TDesC& /*aSqlQuery*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aTableName A reference to a descriptor containing the name of a table
in the communications database.
@param aSqlQuery A reference to a descriptor containing the SQL query text.
@return A pointer to the view object.
@capability Dependent on table, see the guide page referenced below. */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C CCommsDbTableView* CCommsDatabaseBase::OpenViewMatchingUintLC(const TDesC& aTableName,
																	const TDesC& aColumnToMatch,
																	TUint32 aValueToMatch)
/** Opens a view onto a specified table based on a matching unsigned integer value,
and returns a pointer to that view. The view includes all those records where
the column, identified by aColumnToMatch, matches the unsigned integer value
aValueToMatch.

The view excludes hidden records, unless access to them has previously been
explicitly requested by calling the ShowHiddenRecords() member function of
this object.

If the open process is succesful, the function constructs and returns a pointer
to a CCommsDbTableView object which encapsulates the information on that view.
The pointer is also put onto the cleanup stack.

@param aTableName A reference to a descriptor containing the name of the table
in the communications database on which a view is to be opened.
@param aColumnToMatch A reference to a descriptor containing the name of the
column to be used in the selection process.
@param aValueToMatch The value to be matched with the content of column aColumnToMatch.

@return A pointer to the view object.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("OpenViewMatchingUintLC Entry Point"));
	CCommsDbTableView* table = DoOpenViewMatchingUintL(aTableName,aColumnToMatch,aValueToMatch,iShowHiddenRecords);
	CleanupStack::PushL(table);
	return table;
	}

EXPORT_C CCommsDbTableView* CCommsDatabaseBase::OpenViewMatchingBoolLC(const TDesC& aTableName,
																	const TDesC& aColumnToMatch,
																	TBool aValueToMatch)
/** Opens a view onto a specified table based on a matching boolean value, and
returns a pointer to that view. The view includes all those records where
the column, identified by aColumnToMatch, matches the boolean value aValueToMatch.

The view excludes hidden records, unless access to them has previously been
explicitly requested by calling the ShowHiddenRecords() member function.

If the open process is successful, the function constructs and returns a pointer
to a CCommsDbTableView object which encapsulates the information on that view.
The pointer is also put onto the cleanup stack.

@param aTableName A reference to a descriptor containing the name of the table of
interest in the communications database.
@param aColumnToMatch A reference to a descriptor containing the name of the
column to be used in the selection process.
@param aValueToMatch The value to be matched with the content of column aColumnToMatch.

@return A pointer to the view object.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("OpenViewMatchingBoolLC Entry Point"));
	// create table
	CCommsDbTableView* tableView = CCommsDbTableView::NewLC(*this, aTableName, aColumnToMatch, aValueToMatch);

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("OpenViewMatchingBoolLC -> TableName:%S, Column Name:%S, MatchingValue:%b"),
					&aTableName, &aColumnToMatch, aValueToMatch);

	return tableView;
	}

EXPORT_C CCommsDbTableView* CCommsDatabaseBase::OpenViewMatchingTextLC(const TDesC& aTableName,
																	const TDesC& aColumnToMatch,
																	const TDesC8& aValueToMatch)
/** Opens a view onto a specified table based on a matching 8 bit text type, and
returns a pointer to that view.

The view includes all those records where the column, identified by aColumnToMatch,
matches the narrow text supplied in the descriptor aValueToMatch.

The view excludes hidden records, unless access to them has previously been
explicitly requested by calling the ShowHiddenRecords() member function of
this object.

If the open process is succesful, the function constructs and returns a pointer
to a CCommsDbTableView object which encapsulates the information on that view.
The pointer is also put onto the cleanup stack.

@param aTableName A reference to a descriptor containing the name of a table
in the communications database.
@param aColumnToMatch A reference to a descriptor containing the name of the
column to be used in the selection process.
@param aValueToMatch The narrow (ASCII) text to be matched with the content
of column aColumnToMatch.
@return A pointer to the view object.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("OpenViewMatchingTextLC Entry Point"));
	CCommsDbTableView* table = DoOpenViewMatchingTextL(aTableName,aColumnToMatch,aValueToMatch,iShowHiddenRecords);
	CleanupStack::PushL(table);
	return table;
	}

EXPORT_C CCommsDbTableView* CCommsDatabaseBase::OpenViewMatchingTextLC(const TDesC& aTableName,
																	const TDesC& aColumnToMatch,
																	const TDesC16& aValueToMatch)
/** Opens a view onto a specified table based on a matching 16 bit text type, and
returns a pointer to that view. The view includes all those records where
the column, identified by aColumnToMatch, matches the wide text supplied in
the descriptor aValueToMatch.

The view excludes hidden records, unless access to them has previously been
explicitly requested by calling the ShowHiddenRecords() member function of
this object.

If the open process is successful, the function constructs and returns a pointer
to a CCommsDbTableView object which encapsulates the information on that view.
The pointer is also put onto the cleanup stack.

@param aTableName A reference to a descriptor containing the name of a table
in the communications database.
@param aColumnToMatch A reference to a descriptor containing the name of the
column to be used in the selection process.
@param aValueToMatch The wide (UNICODE) text to be matched with the content
of column aColumnToMatch.
@return A pointer to the view object.
@capability Dependent on table, see the guide page referenced below. */
	{
	if(iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("OpenViewMatchingTextLC16 Entry Point"));
	CCommsDbTableView* table = DoOpenViewMatchingTextL(aTableName,aColumnToMatch,aValueToMatch,iShowHiddenRecords);
	CleanupStack::PushL(table);
	return table;
	}
void CCommsDatabaseBase::DatabaseUpdateHasOccurred()
/**
Sets the flag to signal that a successful update has occurred
*/
	{
	}

void CCommsDatabaseBase::DoClose()
/**
Close the database and close the notifier and the server
*/
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DoClose Entry Point"));
	if(iImpl)
		{
		iImpl->iDbs->Close();
		}
	}

CCommsDbTableView*	CCommsDatabaseBase::DoOpenViewMatchingUintL(const TDesC& aTableName,
														const TDesC& aColumnToMatch,
														TUint32 aValueToMatch,
														TBool aIncludeHiddenRecords)
/**
Opens a view on the table aTableName containing all records where
aColumnToMatch contains aValueToMatch

@param aTableName A reference to a descriptor containing the name of a table in the communications database.
@param aColumnToMatch A reference to a descriptor containing the name of a column in table aTableName.
@param aValueToMatch  reference to an unsigned integer passed by the caller.
@param includeHiddenRecords A boolean variable whether to include hidden records or not
@return Pointer to CCommsDbTableView
*/
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DoOpenViewMatchingUintL Entry Point"));
	CCommsDbTableView* tableView = CCommsDbTableView::NewL(*this, aTableName, aColumnToMatch, aValueToMatch);

    if(aIncludeHiddenRecords)
        {
        ShowHiddenRecords();
	    tableView->iTableExt->SetShowHiddenRecords(iShowHiddenRecords);
        }

	__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("DoOpenViewMatchingUintL -> TableName:%S, ColumnName:%S, MatchingValue:%d Hidden:%b"), &aTableName, &aColumnToMatch, aValueToMatch,aIncludeHiddenRecords);

	return tableView;
	}

CCommsDbTableView*	CCommsDatabaseBase::DoOpenViewMatchingTextL(const TDesC& aTableName,
															const TDesC& aColumnToMatch,
															const TDesC8& aValueToMatch,
															TBool aIncludeHiddenRecords)
/**
Opens a view on the table aTableName containing all records where
aColumnToMatch contains aValueToMatch

@param aTableName A reference to a descriptor containing the name of a table in the communications database.
@param aColumnToMatch A reference to a descriptor containing the name of a column in table aTableName.
@param aValueToMatch  reference to an unsigned integer passed by the caller.
@param includeHiddenRecords A boolean variable whether to include hidden records or not
@return Pointer to CCommsDbTableView
*/
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DoOpenViewMatchingTextL Entry Point"));
	CCommsDbTableView* tableView = CCommsDbTableView::NewL(*this, aTableName, aColumnToMatch, aValueToMatch);

    if(aIncludeHiddenRecords)
        {
        ShowHiddenRecords();
	    tableView->iTableExt->SetShowHiddenRecords(iShowHiddenRecords);
        }

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("DoOpenViewMatchingTextL -> TableName:%S, ColumnName:%S, Hidden:%b"), &aTableName, &aColumnToMatch,aIncludeHiddenRecords);

	return tableView;
	}

CCommsDbTableView*	CCommsDatabaseBase::DoOpenViewMatchingTextL(const TDesC& aTableName,
															const TDesC& aColumnToMatch,
															const TDesC16& aValueToMatch,
															TBool aIncludeHiddenRecords)
/**
Opens a view on the table aTableName containing all records where
aColumnToMatch contains aValueToMatch

@param aTableName A reference to a descriptor containing the name of a table in the communications database.
@param aColumnToMatch A reference to a descriptor containing the name of a column in table aTableName.
@param aValueToMatch  reference to an unsigned integer passed by the caller.
@param includeHiddenRecords A boolean variable whether to include hidden records or not
@return Pointer to CCommsDbTableView
*/
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DoOpenViewMatchingTextL16 Entry Point"));
	CCommsDbTableView* tableView = CCommsDbTableView::NewL(*this, aTableName, aColumnToMatch, aValueToMatch);

    if(aIncludeHiddenRecords)
        {
        ShowHiddenRecords();
	    tableView->iTableExt->SetShowHiddenRecords(iShowHiddenRecords);
        }

	__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("DoOpenViewMatchingTextL -> TableName:%S, ColumnName:%S, MatchingValue:%S Hidden:%b"), &aTableName, &aColumnToMatch, &aValueToMatch, aIncludeHiddenRecords);

	return tableView;
	}

CCommsDbTableView* CCommsDatabaseBase::DoOpenTableViewL(const TDesC& /*aTableName*/,const TDesC& /*aSqlQuery*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aTableName A reference to a descriptor containing the name of a table in the communications database.
@param aSqlQuery A reference to a descriptor containing the SQL query text
@return Pointer to CCommsDbTableView
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CCommsDbTableView* CCommsDatabaseBase::DoOpenIAPTableViewL(const TDesC& /*aTableName*/,const TDesC& /*aSqlQuery*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aTableName A reference to a descriptor containing the name of a table in the communications database.
@param aSqlQuery A reference to a descriptor containing the SQL query text
@return Pointer to CCommsDbTableView
*/
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CCommsDatabaseBase::BeginInternalTransactionL()
/**
Lock the database for internal use - begin a transaction if we can!
*/
    {
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("BeginInternalTransaction() Entry Point"));
    if (!InTransaction() && !IsDatabaseWriteLockedL())
        {
        TInt ret = BeginTransaction();
        if (ret == KErrNone)
            {
            iInInternalTransaction = ETrue;
            __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("BeginInternalTransaction()"));
            }
        else
            {
            iInInternalTransaction = EFalse;
            User::Leave(ret);
            }
        }
    }

TInt CCommsDatabaseBase::CommitInternalTransaction()
/**
Unlocks the database from internal use - end a transaction.
If the commit fails then do the rollback.

@return KErrNone if successful, otherwise another of the system-wide error codes.
*/
    {
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CommitInternalTransaction() Entry Point"));
    TInt ret = KErrNone;
    if (iInInternalTransaction)
        {
        ret = CommitTransaction();
        iInInternalTransaction = EFalse;
        if (ret != KErrNone)
            {
            RollbackTransaction();
            }
        else
            {
            __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CommitInternalTransaction()"));
            }
        }
    return ret;
    }

void CCommsDatabaseBase::RollbackInternalTransaction()
/**
Rollback the database after internal use - end a transaction
*/
    {
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("RollbackInternalTransaction() Entry Point"));
    if (iInInternalTransaction)
        {
        RollbackTransaction();
        iInInternalTransaction = EFalse;

        __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("RollbackTransaction()"));
        }
    }

void CCommsDatabaseBase::NotifyChangeL(const TDesC& /*aSetting*/, TUint32 /*aVal*/)
/**
Does the notification when the transaction is committed

Nothing to do. Everything is done in CommsDat from v9.1

@param aSetting Setting to get
@param aVal Setting value.
*/
	{
	}


TInt CCommsDatabaseBase::NotifyAllChanges()
/**
Does the notification for all the changes that are stored in the member variable iNotifications.

Nothing to do. Everything is done in CommsDat from v9.1

@return KErrNone if successful;
		KErrPermissionDenied if the caller process failed the Write Policy;
		KErrNotFound if a property was not found

*/
	{
	return KErrNone;
	}

void CCommsDatabaseBase::ConvertToUid(const TDesC& /*aSetting*/,
								  TUint32 /*aVal*/,
								  TCommDbPublishSubscribeNotification& /*aNotification*/)
/**
Checks for the type of the setting passed and sets the value in the Commdb publish subscribe notification appropriately.

Nothing to do. Everything is done in CommsDat from v9.1

@param aSetting Setting to get
@param aVal Setting value.
@param aNotification A reference to TCommDbSystemAgentNotification
*/
	{
	}

TUint32 CCommsDatabaseBase::GetNewNumber(TUid /*aUid*/)
/**
Generates a new number for the given UID

Nothing to do. Everything is done in CommsDat from v9.1

@param aUid UID for which the new number is to be generated
@return The new number generated
*/
	{
	return 0;
	}
/**
   Check that a given client has the capabilites required to perform the given operation on the given field

   @param aType     Specifies the requested operation (read or write)
   @param aField    Specifies the field to perform the operation on
   @param aMessage  Contains the capabilites of the client
   @return KErrNone on success, one of the system-wide error codes otherwise
*/
TInt CCommsDatabaseBase::CheckDBCapability( RDbs::TPolicyType /*aType*/, const TDesC& /*aField*/, const RMessagePtr2* /*aMessage*/ )
	{
	/** TODO: Call Capability inside CommsDatUtil  */
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CheckDBCapability()"));
	return KErrNotFound;
	}
/**
   Check the capabilites required to read a field from the database

   @param aField the name of the field to be read
   @param aMessage Message indicating the capabilites to be checked
   @return KErrNone on success, one of the system-wide error codes otherwise
*/
EXPORT_C TInt CCommsDatabaseBase::CheckReadCapability( const TDesC& aField, const RMessagePtr2* aMessage )
	{
	TInt ret = EFalse;
	TRAPD(err, DoCheckReadCapabilityL(aField, aMessage));
	if(err == KErrNone)
		{
		err = ret;
		}

	return err;
	}

TInt CCommsDatabaseBase::DoCheckReadCapabilityL( const TDesC& aField, const RMessagePtr2* aMessage )
	{
	CCommsDatUtils* commsUtils = CCommsDatUtils::NewL();
	TInt ret = commsUtils->CheckReadCapability(aField, aMessage);
	delete commsUtils;

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CheckReadCapability(), ret:%d"), ret);

	return ret;
	}
/**
   Check the capabilites required to write a field to the database

   @param aField the name of the field to be written
   @param aMessage Message indicating the capabilites to be checked
   @return KErrNone on success, one of the system-wide error codes otherwise
*/
EXPORT_C TInt CCommsDatabaseBase::CheckWriteCapability( const TDesC& aField, const RMessagePtr2* aMessage )
	{
	TInt ret = EFalse;
	TRAPD(err, ret = DoCheckWriteCapabilityL(aField, aMessage));
	if(err == KErrNone)
		{
		err = ret;
		}

	return err;
	}

TInt CCommsDatabaseBase::DoCheckWriteCapabilityL( const TDesC& aField, const RMessagePtr2* aMessage )
	{
	CCommsDatUtils* commsUtils = CCommsDatUtils::NewL();
	TInt ret = commsUtils->CheckWriteCapability(aField, aMessage);
	delete commsUtils;

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CheckWriteCapability(), ret:%d"), ret);

	return ret;
	}

EXPORT_C TInt CCommsDatabaseBase::InitializeFilestore()
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
@internalTechnology
*/
	{
	return KErrNotSupported;
	}


// CCommsDatabase definitions
EXPORT_C CCommsDatabase* CCommsDatabase::NewL()
/** Allocates and constructs a new communications database object on the heap.
If the database filestore does not exist the default filestore is duplicated.
If no default filestore exists an empty database is created.

If there is insufficient memory available to create the object, the function
leaves. If allocation is successful, it returns a pointer to the new object.

@return A pointer to a communications database object. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL() Entry Point"));
	CCommsDatabase* r = new(ELeave) CCommsDatabase();
	CleanupStack::PushL(r);

	if(r->iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	TCommDbOpeningMethod openingMethod;
	r->DoOpenL(openingMethod, ETrue);
	CleanupStack::Pop(r);
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL() Exit Point"));
	return r;
	}

EXPORT_C CCommsDatabase* CCommsDatabase::NewL(TBool aUseDefaultDb)
/** Creates a CCommsDatabase. If the database filestore exists and aUseDefaultDb
is ETrue the default filestore is duplicated. Otherwise an empty database
is created. If the commdb doesn't exist and this and aUseDefaultDb is EFalse
the this function will leave.

@param aUseDefaultDb Use the default database.
@leave KErrNotSupported when aUseDefaultDb is EFalse and the database filestore
does not exists.
@return A pointer to a communications database object. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL()[TBool] Entry Point"));
	CCommsDatabase* r = new(ELeave) CCommsDatabase();
	CleanupStack::PushL(r);

	if(r->iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	TCommDbOpeningMethod openingMethod;
	r->DoOpenL(openingMethod, aUseDefaultDb);
	CleanupStack::Pop(r);
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL() Exit Point"));
	return r;
	}

EXPORT_C CCommsDatabase* CCommsDatabase::NewL(TCommDbOpeningMethod& aOpeningMethod)
/** Creates a CCommsDatabase as with NewL(). The method of opening (Created, CopiedDefault
or Opened) is returned in aOpeningMethod.

@param aOpeningMethod On return, the opening method.
0@return A pointer to a communications database object. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL()[TCommDbOpeningMethod] Entry Point"));
	CCommsDatabase* r = new(ELeave) CCommsDatabase();

	CleanupStack::PushL(r);

	if(r->iImplNotCreated)
		{
		User::Leave(KErrNoMemory);
		}

	r->DoOpenL(aOpeningMethod, ETrue);
	CleanupStack::Pop(r);
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL() Exit Point"));
	return r;
	}

EXPORT_C CCommsDatabase* CCommsDatabase::NewL(TCommDbDatabaseType /*aDbType*/)
/**
 * Re-instated override variant of NewL function in order to maintain BC with 6.1
 * @param aDbType Whether Database is IAP or ISP version. All Db's are IAP type now.
 * @return CCommDbOverrideSettings* the calling function takes ownership of the returned object
 */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL()[TCommDbDatabaseType] Entry Point"));
	return NewL();
	}

EXPORT_C CCommsDatabase* CCommsDatabase::NewL(TCommDbDatabaseType /*aDbType*/, TCommDbOpeningMethod& aOpeningMethod)
/**
 * Re-instated override variant of NewL function in order to maintain BC with 6.1
 * Creates a CCommsDatabase and gets the method of opening (Created, CopiedDefault or Opened).
 * @param aDbType Whether Database is IAP or ISP version. All Db's are IAP type now.
 * @param aOpeningMethod On return, the method of opening.
 * @return CCommDbOverrideSettings* the calling function takes ownership of the returned object.
 */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::NewL() Entry Point"));
	return NewL(aOpeningMethod);
	}

CCommsDatabase::CCommsDatabase()
	: CCommsDatabaseBase()
/**
Default Constructor
*/
	{
	}


EXPORT_C CCommsDatabase::~CCommsDatabase()
/** Frees all resources owned by this object, prior to its destruction.

In particular, the destructor closes the communications database and disconnects
from the DBMS. */
	{
	}


EXPORT_C void CCommsDatabase::GetGlobalSettingL(const TDesC& aSetting, TUint32& aVal)
/** Gets a global settings table integer field.

If a client attempts to get the default Dial Out IAP with a Version 6.1 database,
the function gets the default IAP in the first connection preference for the
outgoing direction if it is a dial out ISP-type IAP and returns its ID, otherwise
an error is returned.

If a client attempts to get the ASK_USER_BEFORE_DIAL field with a Version
6.1 database, the function returns a true value if CONNECT_PREF_DIALOG_PREF
in the first connection preference has been set to produce a prompt, otherwise
false.

@param aSetting Setting to get.
@param aValue On return, the setting value.
@leave KErrNotSupported if aSetting is invalid; KErrNotFound if the global setting has not been set.
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("GetGlobalSettingL(const TDesC& aSetting, TUint32& aVal)"));

	CCDGlobalSettingsRecord* gsRec = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
	CleanupStack::PushL(gsRec);

	TInt tempType(0);
	CMDBField<TUint32>* ptrField = NULL;
	TRAPD(err, ptrField = static_cast<CMDBField<TUint32>*>(gsRec->GetFieldByNameL(aSetting, tempType)));

	// To maintain CommDb behaviour
	if (err == KErrNotFound)
		{
		User::Leave(KErrNotSupported);
		}

	ptrField->SetRecordId(KGlobalSettingsRecordID);
	ptrField->LoadL(*iImpl->iDbs);

	if(ptrField->IsNull())
		{
		User::Leave(KErrNotFound);
		}

	aVal = static_cast<TUint32&>(*ptrField);

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("CCommsDatabase::GetGlobalSettingL Setting=%S, Value=%d"),&aSetting,aVal);

	CleanupStack::PopAndDestroy(gsRec);
	}


EXPORT_C void CCommsDatabase::GetGlobalSettingL(const TDesC& aSetting, TDes& aVal)
/** Gets a global settings table string field.

@param aSetting Setting to get.
@param aValue On return, the setting value
@leave KErrNotSupported if aSetting is invalid; KErrNotFound if the global setting has not been set
@capability Dependent on table, see the guide page referenced below. */
	{

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("GetGlobalSettingL(const TDesC& aSetting, TDes& aVal)"));

	if (aSetting.CompareF(TPtrC(KCDTypeNameBearerAvailabilityCheckTSY)) == 0)
		{
		// load the field
		//
		CMDBField<TDesC>* descField = new(ELeave) CMDBField<TDesC>(KCDTIdBearerAvailabilityCheckTSY);
		CleanupStack::PushL(descField);
		descField->SetMaxLengthL(KMaxTextLength);
		descField->SetRecordId(KGlobalSettingsRecordID);
		descField->LoadL(*iImpl->iDbs);
		
		if(descField->IsNull())
			{
			__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::GetGlobalSettingL Setting leaves with KErrNotFound(Field has NULL value)"));
			User::Leave(KErrNotFound);
			}
		else
			{
			aVal = *descField;
			CleanupStack::PopAndDestroy(descField);
			}
		}
	else
		{
		// there is only one descriptor global setting so leave otherwise
		//
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDatabase::GetGlobalSettingL Setting=%S leaves with KErrNotSupported(Setting doesn't exist!)"), &aSetting);
		User::Leave(KErrNotSupported);
		}

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("CCommsDatabase::GetGlobalSettingL Setting=%S, Value=%S"), &aSetting, &aVal);
	}

EXPORT_C void CCommsDatabase::SetGlobalSettingL(const TDesC& aSetting, TUint32 aVal)

/** Sets a global settings table integer field.

If a client attempts to set the default Dial Out IAP with a Version 6.1 database,
the function sets the default IAP in the first connection preference for the
outgoing direction to the specified IAP, and the bearer set to CSD.

If a client attempts to set the ASK_USER_BEFORE_DIAL field with a Version
6.1 database, the function sets CONNECT_PREF_DIALOG_PREF in the first connection
preference to produce a prompt if true is specified, or do not prompt if false
is specified.

@param aSetting Setting to get.
@param aValue Setting value.
@leave KErrNotSupported aSetting is invalid.
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("SetGlobalSettingL(const TDesC& aSetting, TUint32 aVal)"));

	iImpl->CheckGlobalSettingsTableExistsL();

	CCDGlobalSettingsRecord* gsRec = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
	CleanupStack::PushL(gsRec);

	TInt tempType(0);
	CMDBField<TUint32>* ptrField = NULL;
	TRAPD(err, ptrField = static_cast<CMDBField<TUint32>*>(gsRec->GetFieldByNameL(aSetting, tempType)));

	// To maintain CommDb behaviour.
	//
	if (err == KErrNotFound)
		{
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDatabase::SetGlobalSettingL Setting=%S is leaving with KErrNotSupported(Setting doesn't exist!)"), &aSetting);
		User::Leave(KErrNotSupported);
		}

	*ptrField = aVal;

	ptrField->SetRecordId(KGlobalSettingsRecordID);
	ptrField->ModifyL(*iImpl->iDbs);

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("CCommsDatabase::SetGlobalSettingL Setting=%S, ElementId = %08x, Value=%d"), &aSetting, *(ptrField->Data()), aVal);

	CleanupStack::PopAndDestroy(gsRec);
   }


EXPORT_C void CCommsDatabase::SetGlobalSettingL(const TDesC& aSetting, const TDesC& aVal)
/** Sets a global settings table string field.

@param aSetting Setting to get.
@param aValue On return, the setting value.
@leave KErrNotSupported aSetting is invalid.
@capability Dependent on table, see the guide page referenced below. */

	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("SetGlobalSettingL(const TDesC& aSetting, const TDesC& aVal)"));

	iImpl->CheckGlobalSettingsTableExistsL();

	CCDGlobalSettingsRecord* gsRec = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
	CleanupStack::PushL(gsRec);

	TInt tempType(0);
	CMDBField<TDesC>* ptrField = NULL;
	TRAPD(err, ptrField = static_cast<CMDBField<TDesC>*>(gsRec->GetFieldByNameL(aSetting, tempType)));

	// To maintain CommDb behaviour.
	//
	if (err == KErrNotFound)
		{
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDatabase::SetGlobalSettingL Setting=%S is leaving with KErrNotSupported(Setting doesn't exist!)"), &aSetting);
		User::Leave(KErrNotSupported);
		}

	ptrField->SetMaxLengthL(aVal.Length());
	*ptrField = aVal;

	ptrField->SetRecordId(KGlobalSettingsRecordID);
	ptrField->ModifyL(*iImpl->iDbs);

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("CCommsDatabase::SetGlobalSettingL Setting=%S, Value=%S"), &aSetting, &aVal);

	CleanupStack::PopAndDestroy(gsRec);
	}


EXPORT_C void CCommsDatabase::ClearGlobalSettingL(const TDesC& aSetting)
/** Clears a global settings table field.

@param aSetting Setting to clear.
@leave KErrNotSupported aSetting is invalid
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDatabase::ClearGlobalSettingL Entry Point"));

	iImpl->CheckGlobalSettingsTableExistsL();

	CCDGlobalSettingsRecord* gsRec = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
	CleanupStack::PushL(gsRec);

	TInt tempType(0);
	CMDBElement* ptrField(NULL);
	TRAPD(err, ptrField = gsRec->GetFieldByNameL(aSetting,tempType));

	// To maintain CommDb behaviour.
	if (err == KErrNotFound)
		{
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDatabase::ClearGlobalSettingL Setting=%S is leaving with KErrNotSupported(Setting doesn't exist!)"), &aSetting);
		User::Leave(KErrNotSupported);
		}

	ptrField->SetRecordId(KGlobalSettingsRecordID);

	ptrField->DeleteL(*iImpl->iDbs);

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDatabase::ClearGlobalSettingL Setting=%S"), &aSetting);

	CleanupStack::PopAndDestroy(gsRec);
	}


EXPORT_C void CCommsDatabase::GetDefaultTsyL(TDes& aValue)
/** Retrieves the default TSY used for bearer availibility, SMS and data services.
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("GetDefaultTsyL Entry Point"));
	GetGlobalSettingL(TPtrC(KCDTypeNameBearerAvailabilityCheckTSY), aValue);
	}

EXPORT_C void CCommsDatabase::GetCurrentDialOutSettingL(const TDesC& /*aSetting*/, TUint32& /*aValue*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@leave KErrNotSupported No longer supported.
@param aSetting Setting to get.
@param aValue On return, setting value.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CCommsDatabase::GetCurrentDialInSettingL(const TDesC& /*aSetting*/, TUint32& /*aValue*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

Past use: Retrieve settings for the dial in IAP.

@leave KErrNotSupported No longer supported.
@param aSetting Setting to get.
@param aValue On return, setting value. 
*/
    {
	User::Leave(KErrNotSupported);
	}

EXPORT_C CCommsDbConnectionPrefTableView* CCommsDatabase::OpenConnectionPrefTableLC()
/** Opens a view on the whole Connection Preferences table and returns a pointer to it.

Shows hidden records only if requested.

When the use of the view object is complete it should be popped from the
cleanup stack, and deleted.

@return The opened view
@capability Dependent on table, see the guide page referenced below. */
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("GetCurrentDialInSettingL Entry Point"));
	// create instace of connection preference table
	CCommsDbConnectionPrefTableView* tableView = CCommsDbConnectionPrefTableView::NewLC(*this, ECommDbConnectionDirectionUnknown, EFalse);

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("OpenConnectionPrefTableLC:  ConnectionPreferencesView is created"));
	return tableView;
	}

EXPORT_C CCommsDbConnectionPrefTableView* CCommsDatabase::OpenConnectionPrefTableLC(TCommDbConnectionDirection aDirection)
/** Opens a view on the records in the Connection Preferences table with a specified
direction and return a pointer to it.

Shows hidden records only if requested..

When the use of the view object is complete, it should be popped from the
cleanup stack, and deleted.

@param aDirection Direction of the records to include in the view
@return The opened view
@capability Dependent on table, see the guide page referenced below. */
	{
	// create instace of connection preference table
	CCommsDbConnectionPrefTableView* tableView = CCommsDbConnectionPrefTableView::NewLC(*this, aDirection, EFalse);

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("OpenConnectionPrefTableLC: ConnectionPreferenceTable View is created with Direction:%d"),aDirection);
	return tableView;
	}

EXPORT_C CCommsDbConnectionPrefTableView* CCommsDatabase::OpenConnectionPrefTableInRankOrderLC(TCommDbConnectionDirection aDirection)
/** Opens a view on the records in the Connection Preferences table with a specified
direction, and with records sorted into ranking order.

Records are sorted in rank order from ranking 1 first. Records with rank 0
are not included.

Shows hidden records only if requested.

When the use of the view object is complete, it should be popped from the
cleanup stack, and deleted.

@param aDirection Direction of the records to include in the view
@return The opened view
@capability Dependent on table, see the guide page referenced below. */
	{
	// create instace of connection preference table
	CCommsDbConnectionPrefTableView* tableView = CCommsDbConnectionPrefTableView::NewLC(*this, aDirection, ETrue);

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("OpenConnectionPrefTableLC: ConnectionPreferenceTable View is created with direction:%d and sorted ranking"), aDirection);
	return tableView;
	}

EXPORT_C CCommsDbConnectionPrefTableView* CCommsDatabase::OpenConnectionPrefTableViewOnRankLC(TCommDbConnectionDirection aDirection,
	TUint32 aRank)
/** Opens a view on the records in the Connection Preferences table with a specified
direction and ranking.

When the use of the view object is complete, it should be popped from the
cleanup stack, and deleted.

@param aDirection Direction of the records to include in the view
@param aRank Ranking of the records to include in the view
@return The opened view
@capability Dependent on table, see the guide page referenced below. */
	{
	//	there are no hidden field in the connection prefs table,
	//	therefore ignore iShowHiddenRecords member

	// create instace of connection preference table
	CCommsDbConnectionPrefTableView* tableView = CCommsDbConnectionPrefTableView::NewL(*this, aDirection, aRank);
	CleanupStack::PushL(tableView);

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("OpenConnectionPrefTableViewOnRankLC: ConnectionPreferenceTable View is created with direction:%d ,rank:%d"), aDirection, aRank);

	return tableView;
	}

EXPORT_C void CCommsDatabase::SetAgentL(const TDesC& /*aService*/, const TDesC& /*aAgent*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Agent table has been removed.

@leave KErrNotSupported No longer supported.

@param aService The service that will use the agent. This is one of DIAL OUT ISP TABLE,
DIAL IN ISP TABLE, OUTGOING WCDMA TABLE.
@param aAgent The agent to use.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CCommsDatabase::SetAgentExtL(const TDesC& /*aService*/, const TDesC& /*aAgentExt*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Sets the agent extension to be used when accessing the specified service.

@param aService The service that will use the agent. This is one of DIAL OUT ISP TABLE,
DIAL IN ISP TABLE, OUTGOING WCDMA TABLE.
@param aAgentExt The agent extention to use.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CCommsDatabase::SetAgentClientTimeoutL(const TDesC& /*aService*/, TInt /*aClientTimeout*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Timeout names have been changed and agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Sets the client timeout associated with the specified service.

@param aService The service that the client timeout is associated with. This is one of
DIAL OUT ISP TABLE, DIAL IN ISP TABLE, OUTGOING WCDMA TABLE. 
@param aClientTimeout The client timeout.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CCommsDatabase::SetAgentRouteTimeoutL(const TDesC& /*aService*/, TInt /*aRouteTimeout*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Timeout names have been changed and agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Sets the route timeout associated with the specified service.

@param aService The service that the client route timeout is associated with. This is one of
DIAL OUT ISP TABLE, DIAL IN ISP TABLE, OUTGOING WCDMA TABLE. 
@param aRouteTimeout The route timeout.
*/
	{
	User::Leave(KErrNotSupported);
	}


EXPORT_C void CCommsDatabase::GetAgentL(const TDesC& /*aService*/, TDes& /*aAgent*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Gets the agent associated with the specified service.

@param aService The service that will use the agent. This is one of DIAL OUT ISP TABLE,
DIAL IN ISP TABLE, OUTGOING WCDMA TABLE. 
@param aAgent On return, the agent.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CCommsDatabase::GetAgentExtL(const TDesC& /*aService*/, TDes& /*aAgentExt*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Gets the agent extension associated with the specified service.

@param aService The service that will use the agent. This is one of DIAL OUT ISP TABLE,
DIAL IN ISP TABLE, OUTGOING WCDMA TABLE.
@param aAgentExt On return, the agent extension.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C TInt CCommsDatabase::GetAgentClientTimeoutL(const TDesC& /*aService*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Gets the client timeout associated with the specified service.

@param aService The service that the timeout is associated with. This is one of DIAL
OUT ISP TABLE, DIAL IN ISP TABLE, OUTGOING WCDMA TABLE.
@return The agent timeout setting.
*/
	{
	User::Leave(KErrNotSupported);
	return KErrNotSupported;
	}

EXPORT_C TInt CCommsDatabase::GetAgentRouteTimeoutL(const TDesC& /*aService*/)
/** 
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
Agent table has been removed.
@leave KErrNotSupported No longer supported.

Past use: Gets the route timeout associated with the specified service.

@param aService The service that the route timeout is associated with. This is one of DIAL
OUT ISP TABLE, DIAL IN ISP TABLE, OUTGOING WCDMA TABLE.
@return The route timeout.
*/
	{
	User::Leave(KErrNotSupported);
	return KErrNotSupported;
	}

EXPORT_C void CCommsDatabase::ClearAgentAndExtL(const TDesC& /*aService*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@publishedAll
@deprecated
@param aService The service that will be cleared of all the agent settings.
*/
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C CCommsDbTableView* CCommsDatabase::OpenIAPTableViewMatchingBearerSetLC(TUint32 aBearerSet,
												TCommDbConnectionDirection aDirection)
/** Opens a view on records in the IAP table with a specified range of service
types.

IAP records are included that have the specified bearers and direction, as read
from the Connection preference table.

When the use of the view object is complete, it should be popped from the
cleanup stack, and deleted.

@param aBearerSet Bearers to match. For suitable values, see CONNECT_PREF_BEARER_SET
in the Connection preferences table. Bitwise mask of the TCommDbBearer.
@param aDirection Direction to match.
@leave KErrArgument if either aBearerSet or aDirection is invalid; other DBMS-related error codes.
@return The opened view.
@capability Dependent on table, see the guide page referenced below. */
	{
	// BearerSet checkings
   	if ((aBearerSet & (KCommDbBearerCSD|KCommDbBearerPSD|KCommDbBearerLAN|KCommDbBearerPAN|KCommDbBearerVirtual|KCommDbBearerWLAN)) == 0)
   		{
		User::Leave(KErrArgument);
		}
	// Direction checkings
	if (aDirection < ECommDbConnectionDirectionUnknown || aDirection > ECommDbConnectionDirectionIncoming)
		{
		User::Leave(KErrArgument);
		}

	CCommsDbTableView* tableView = CCommsDbTableView::NewLC(*this, aBearerSet, aDirection);

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("OpenIAPTableViewMatchingBearerSetLC: BearerSet=%d ,Direction=%d"), aBearerSet, aDirection);

	return tableView;
	}

EXPORT_C CCommsDbTableView* CCommsDatabase::OpenIAPTableViewMatchingNetworkLC(TUint32 aNetworkId)
/** Opens a view on the IAP table containing records which match
the specified network.
@capability Dependent on table, see the guide page referenced below. */
	{
	CCommsDbTableView* tableView = CCommsDbTableView::NewLC(*this, aNetworkId);

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("OpenIAPTableViewMatchingNetworkLC: Network=%d ,Direction=%d"), aNetworkId);

	return tableView;
	}


EXPORT_C CCommsDbTableView* CCommsDatabase::OpenViewOnProxyRecordLC(TUint32 aServiceId, const TDesC& aServiceType)
/** Opens a view on records in the Proxies table containing records	that match
the service ID and service type specified.

When the use of the view object is complete, it should be popped from the cleanup
stack, and deleted.

@param aServiceId ID of the service to match
@param aServiceType Service type to match
@leave KErrArgument if either aServiceId or aServiceType is invalid; other DBMS-related error codes.
@return The opened view
@capability Dependent on table, see the guide page referenced below. */
	{
	CCommsDbTableView* tableView = CCommsDbTableView::NewLC(*this, aServiceId, aServiceType);

	return tableView;
	}


void CCommsDatabase::CreateDatabaseL()
/**
@deprecated This method is deprecated

Function not supported
*/
	{
	User::Leave(KErrNotSupported);
	}

void CCommsDatabase::DoOpenL(TCommDbOpeningMethod& aOpeningMethod, TBool /*aUseDefaultDb*/)
/**
Connects to the Commsdat and set version number

@param aOpeningMethod
*/
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CommsDat Session is created!, before"));
	iImpl->iDbs = CMDBSession::NewL(KCDVersion1_1);

	// allow client to see everything that it's capabilities allow
	iImpl->iDbs->SetAttributeMask( ECDPrivate );

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CommsDat Session is created!"));

	aOpeningMethod = ECommDbOpened;
	}

CCommsDbTableView* CCommsDatabase::OpenGlobalSettingsTableLC(const TDesC& aSetting, TBool& aExists)
/**
Opens a view on the global settings table showing the entry (if it exists)
called aSetting. Global settings are hidden therfore require the
`aIncludeHiddenRecords` argument set to True.

@param aSetting
@param aExists On return, will have ETrue if Successful else EFalse
@return pointer to CCommsDbTableView
*/
	{
	aExists = EFalse;
	CCommsDbTableView* view = DoOpenViewMatchingTextL(TPtrC(KCDTypeNameGlobalSettings),TPtrC(KCDTypeNameRecordName),aSetting, ETrue);
	CleanupStack::PushL(view);

	aExists = ETrue;

	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("OpenGlobalSettingsTableLC"));

	return view;
	}

CCommsDatabase::TGlobalSettingType CCommsDatabase::GlobalSettingTypeL(const TDesC& aSetting) const
/**
Returns whether aSetting is a known global setting, a former global
setting supported for compatibility reasons or not known.
These are rougly in the optimal order.....

@param aSetting
@return The type of global setting
*/
	{
	TGlobalSettingType ret = ENotASetting;
	 //	Real global settings
	if (aSetting.CompareF(TPtrC(KCDTypeNameMaxConnectionAttempts)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameMaxRedialAttempts)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameSMSBearer)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameSMSReceiveMode)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameGPRSAttachMode)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameAcceptIncomingGPRS)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameGPRSClassCBearer)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameModemDataFax)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameModemPhoneServicesSMS)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameLocationDataFax)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameLocationPhoneServicesSMS)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameDefaultNetwork)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameMaxMBufHeap)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameDefaultAgent)) == 0)
		{
		ret = EGlobalSetting;
		}
	else if (aSetting.CompareF(TPtrC(KCDTypeNameBearerAvailabilityCheckTSY)) == 0)
		{
		ret = EGlobalSetting;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}

	return ret;
	}

