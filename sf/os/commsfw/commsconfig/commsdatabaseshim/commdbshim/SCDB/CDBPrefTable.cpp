// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms Database Connection Preferences Table View definitions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <cdbtemp.h>
#include <cdbpreftable.h>
#include "CDBSTD.H"
#include "commdb_impl.H"
#include "Commsdat_Log.h"

class TCommDbCommonBearerInfo
/**
@internalComponent
*/
    {
public:
    TUint32 iBearerSet;
    TBool iBearerSetExists;
    TUint32 iIapId;
    TBool iIapIdExists;
    };

// TCommDbIapBearer and TCommDbIapConnectionPref definitions


EXPORT_C CCommsDbConnectionPrefTableView::TCommDbIapBearer::TCommDbIapBearer() :
    iBearerSet(0), iIapId(0)
/** Default constructor.
All member data is initialised to zero. */
    {
    }

EXPORT_C CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref::TCommDbIapConnectionPref() :
    iRanking(0), iDirection(ECommDbConnectionDirectionUnknown),
    iDialogPref(ECommDbDialogPrefUnknown), iBearer()
/** Default constructor.
The rank is set to 0, and the direction and dialog preference to unknown. */
    {
    }

EXPORT_C TBool CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref::operator==(const TCommDbIapConnectionPref& aPref) const
/** Overloaded equality operator.
Checks for equality between 'this' and another TCommDbIapConnectionPref object passed in.

@param aPref Object to compare.
@return ETrue if all fields in the objects are equal, otherwise EFalse. */
    {
    return (aPref.iRanking == iRanking &&
            aPref.iDirection == iDirection &&
            aPref.iDialogPref == iDialogPref &&
            aPref.iBearer.iBearerSet == iBearer.iBearerSet &&
            aPref.iBearer.iIapId == iBearer.iIapId);
    }


EXPORT_C CCommsDbConnectionPrefTableView::TCommDbIspBearer::TCommDbIspBearer()
/**
Framework left in place for 6.1BC purposes.
*/
    {
    }


EXPORT_C CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref::TCommDbIspConnectionPref() :
    iBearer()
/**
Framework left in place for 6.1BC purposes.
*/
    {
    }


EXPORT_C TBool CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref::operator==(const TCommDbIspConnectionPref& /*aPref*/) const
/**
 * Check for equality between 'this' and another TCommDbIspConnectionPref object passed in.
 * Framework letf in place for 6.1BC purposes. Returns KErrNotSupported only.
 * @param aPref Preferences.
 * @return KErrNotSupported Function has been deprecated therefore always return with this error.
 */
    {
    return KErrNotSupported;
    }

/**
CCommsDbConnectionPrefTableView definitions
*/

CCommsDbConnectionPrefTableView* CCommsDbConnectionPrefTableView::NewLC(CCommsDatabase& /*aDb*/, const TDbQuery& /*aQuery*/)
/** Create a view. Usually, however, views are created by calling one of the
CCommsDatabase::OpenConnectionPrefTable*LC() functions.

@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
@param aDb reference to the database to use.
@param aQuery Query to preform
@return Reference to the new CCommsDbConnectionPrefTableView. */
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }
CCommsDbConnectionPrefTableView* CCommsDbConnectionPrefTableView::NewLC(CCommsDatabase& aDb, TCommDbConnectionDirection aDirection, TBool aSortRanking)
/** Create a view. Usually, however, views are created by calling one of the
CCommsDatabase::OpenConnectionPrefTable*LC() functions.

@param aDb reference to the database to use.
@param aDirection
@param aSortRanking -> ETrue Sort by ranking, EFalse not sorted
@return Reference to the new CCommsDbConnectionPrefTableView. */
    {
    CCommsDbConnectionPrefTableView* self = new(ELeave) CCommsDbConnectionPrefTableView(aDb);
    CleanupStack::PushL(self);
    self->ConstructL(aDirection, aSortRanking);
	// register view in Database
	aDb.iImpl->RegisterConnPrefViewL(self);
    return self;
    }

CCommsDbConnectionPrefTableView* CCommsDbConnectionPrefTableView::NewL(CCommsDatabase& aDb, TCommDbConnectionDirection aDirection, TUint32 aRank)
/** Create a view. Usually, however, views are created by calling one of the
CCommsDatabase::OpenConnectionPrefTable*LC() functions.

@param aDb reference to the database to use.
@param aDirection
@param aRank
@return Reference to the new CCommsDbConnectionPrefTableView. */
    {
    CCommsDbConnectionPrefTableView* self = new(ELeave) CCommsDbConnectionPrefTableView(aDb);
	CleanupStack::PushL(self);
    TRAPD(err,self->ConstructL(aDirection, aRank));
	if(err == KErrNotFound)
		{
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDbConnectionPrefTableView::NewL is leaving with KErrOverflow"));
		err = KErrOverflow;
		}
	User::LeaveIfError(err);

	// register view in Database
	aDb.iImpl->RegisterConnPrefViewL(self);
	CleanupStack::Pop(self);
    return self;
    }

CCommsDbConnectionPrefTableView::CCommsDbConnectionPrefTableView(CCommsDatabase& aDb)
    : iDb(aDb)
/**
Constructor

@param aDb reference to the database to use.
*/
    {
    }

void CCommsDbConnectionPrefTableView::ConstructL(const TDbQuery& /*aQuery*/)
/**
Prepares and evaluates the table view using the SQL query aQuery

@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aQuery Query to perform
*/
    {
    User::Leave(KErrNotSupported);
    }

void CCommsDbConnectionPrefTableView::ConstructL(TCommDbConnectionDirection aDirection, TBool aSortRanking)
/**
Creates RecordSet Container and loads date to it.

@param aDirection
@param aSortRanking
*/
    {
    CMDBSession* dbSession = iDb.iImpl->iDbs;
    // create RecordSet object and save a pointer within CTableViewExtension
    iTableExt = CCommDbTableExtension::NewL();

    // object will be deleted by CTableViewExtension object
    CMDBRecordSet<CCDConnectionPrefsRecord>* connPrefRecordSet = new(ELeave) CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
    CleanupStack::PushL(connPrefRecordSet);

    if(aDirection == ECommDbConnectionDirectionUnknown)
    	{
	    // Load all records regardless of direction
        TRAPD(err,connPrefRecordSet->LoadL(*dbSession));
		if((err != KErrNotFound) && (err != KErrNone))
			{
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Connection Preferences Table constructed Direction:%d, Sorted rank: %b, returned Err:%d"),
						aDirection, aSortRanking);
			User::Leave(err);
			}

	    __FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Connection Preferences Table constructed Direction:%d, Sorted rank: %b, returned Err:%d"),
	                aDirection, aSortRanking, err);
    	}
	else
		{
		// Search per direction
	    TBool ret(EFalse);

	    // create 0 record for priming
	    CCDConnectionPrefsRecord* connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	    CleanupStack::PushL(connPrefRecord);

	    // prime
	    connPrefRecord->iDirection = aDirection;

	    // add record to record set
	    connPrefRecordSet->iRecords.AppendL(connPrefRecord);

	    CleanupStack::Pop(connPrefRecord);

	    // Load record
	    TRAPD(err, ret = connPrefRecordSet->FindL(*dbSession));

	    if(err != KErrNone)
	        {
	        __FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Connection Preferences Table constructed Direction:%d, Sorted rank: %b, returned Err:%d"),
	                    aDirection, aSortRanking, err);
	        // error is returned back
	        User::Leave(err);
	        }

		// check if anything is returned back
		if(!ret)
	        {
	        // remove primed record from the record set
			connPrefRecordSet->iRecords.ResetAndDestroy();
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Table: ConnectionPreference with Direction:%d, SortedByRanking:%b is empty"), aDirection, aSortRanking);
	        }
		}

    CleanupStack::Pop(connPrefRecordSet);
     
    iTableExt->SetRecordSet(connPrefRecordSet);

    if (aSortRanking)
        {
        iTableExt->SortByRank();
        }
    }

void CCommsDbConnectionPrefTableView::ConstructL(TCommDbConnectionDirection aDirection, TUint32 aRank)
/**
Prepares and evaluates the table view using params

@param aDirection
@param aRank
*/
    {
    TBool ret(EFalse);
    // create RecordSet object and save a pointer within CTableViewExtension
    iTableExt = CCommDbTableExtension::NewL();
    // object will be deleted by CTableViewExtension object
    CMDBRecordSet<CCDConnectionPrefsRecord>* connPrefRecordSet = new(ELeave) CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
    CleanupStack::PushL(connPrefRecordSet);

    // create 0 record for priming
    CCDConnectionPrefsRecord* connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
    CleanupStack::PushL(connPrefRecord);

    // prime
    connPrefRecord->iDirection = aDirection;
    connPrefRecord->iRanking = aRank;

    // add record to record set
    connPrefRecordSet->iRecords.AppendL(connPrefRecord);
    CleanupStack::Pop(connPrefRecord);

    // Load record
    TRAPD(err, ret = connPrefRecordSet->FindL(*iDb.iImpl->iDbs));

    if(err != KErrNone)
        {
        __FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Connection Preferences Table constructed with Ranking: %d and Direction:%d returned Err:%d"),
                    aRank, aDirection, err);
        // error is returned back
        User::Leave(err);
        }
	
	// check if anything is returned back
    if(!ret)
        {
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Table: ConnectionPreference with Direction:%d, Rank:%d is empty"), aDirection, aRank);
		User::Leave(KErrNotFound);
        }

    iTableExt->SetRecordSet(connPrefRecordSet);

    CleanupStack::Pop(connPrefRecordSet);
    }

EXPORT_C CCommsDbConnectionPrefTableView::~CCommsDbConnectionPrefTableView()
/** Destructor.*/
    {
	delete iTableExt;
	iDb.iImpl->UnRegisterConnPrefView(this);
    }


EXPORT_C void CCommsDbConnectionPrefTableView::InsertConnectionPreferenceL(const TCommDbIapConnectionPref& aPref, TBool aReadOnly)
/** Inserts a new IAP connection preference into the connetion preference table.
The supplied `aPref` is checked to ensure that the rank and direction
do not conflict with existing records. The dialog option, bearer set
and IAP are checked for consistancy.
Deprecated in 7.0 but re-instated to accommodate 6.1 BC

@param aPref Settings for the new connection preference.
@param aReadOnly If ETrue the record is set to be read-only. If left unspecified the default is EFalse.
@capability Dependent on table, see the guide page referenced below.
*/
    {
    CMDBSession* dbSession = iDb.iImpl->iDbs;
    CCDConnectionPrefsRecord* connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));

    connPrefRecord->SetRecordId(KCDNewRecordRequest);
    connPrefRecord->iDirection = aPref.iDirection;
    connPrefRecord->iRanking = aPref.iRanking;
    connPrefRecord->iDialogPref = aPref.iDialogPref;
    connPrefRecord->iBearerSet = aPref.iBearer.iBearerSet;
    connPrefRecord->iDefaultIAP = aPref.iBearer.iIapId;

    CMDBRecordSetBase* recordSet = iTableExt->GetRecordSetL();

    if ( recordSet->IsSetAttribute(ECDNoWriteButDelete) || 
         recordSet->IsSetAttribute(ECDProtectedWrite) )
        {
	    __FLOG_STATIC5(KLogComponent, KCommDbShim, _L("InsertConnectionPreferenceL -> TableName=ConnectionPreferences, Ranking=%d, Direction=%d, DialogPref=%d, Bearer=%d, ReadOnly=%b")
                            , aPref.iRanking, aPref.iDirection, aPref.iDialogPref, aPref.iBearer.iBearerSet, aReadOnly);
        User::Leave(KErrAccessDenied);
        }

    if(aReadOnly)
        {
        connPrefRecord->SetAttributes(ECDNoWriteButDelete);
        dbSession->SetAttributeMask(ECDNoWriteButDelete);
        }
	
	iTableExt->InsertConnPrefRecordL(connPrefRecord, *dbSession);

    if (aReadOnly)
        {
        dbSession->ClearAttributeMask(ECDNoWriteButDelete);
        }

    __FLOG_STATIC5(KLogComponent, KCommDbShim, _L("InsertConnectionPreferenceL -> TableName=ConnectionPreferences, Ranking=%d, Direction=%d, DialogPref=%d, Bearer=%d, ReadOnly=%b")
                            , aPref.iRanking, aPref.iDirection, aPref.iDialogPref, aPref.iBearer.iBearerSet, aReadOnly);

    }

EXPORT_C void CCommsDbConnectionPrefTableView::InsertConnectionPreferenceL(const TCommDbIspConnectionPref& /*aPref*/, TBool /*aReadOnly*/)
/**
 * Override version of InsertConnectionPreferenceL for BC purposes.
 * @param aPref Preferences.
 * @param aReadOnly Read only?
 * @deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.
 *
 */
    {
    User::Leave(KErrNotSupported);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::UpdateBearerL(const TCommDbIapBearer& aUpdate, TBool aReadOnly)
/** Update the bearer set and IAP for the currently selected IAP connection
preference record. The dialog option, bearer set and IAP are checked for
consistancy.
Deprecated in 7.0 but re-instated to accomodate 6.1 BC.
Update the bearer set and iap for the currently selected connection	preference.
The dialog option, bearer set and IAP are checked for consistancy. The function sets the bearer set
(CONNECT_PREF_BEARER_SET) and IAP (CONNECT_PREF_IAP) fields. The direction field (CONNECTION_PREF_DIRECTION)
cannot be altered after it has been written to the record.
@param aUpdate Updated settings for the connection preference. Note that all fields need to be set,
including any unchanged fields.
@param aReadOnly If true, the record is set to be read-only. If unspecified, EFalse by default.
@leave KErrAccessDenied The record cannot be updated as it is read-only;
KErrArgument The specified IAP (aPref.iBearer.iIapId) cannot be used, as it does not fall in the bearer set
defined by aPref.iBearer.iBearerSet and have the same direction as aPref.iDirection;
KErrNotFound There is no current record in the view.
@capability Dependent on table, see the guide page referenced below.
*/
    {
    CMDBSession* dbSession = iDb.iImpl->iDbs;
    CMDBRecordBase* currentRecord = iTableExt->GetCurrentRecordL();
	CCDConnectionPrefsRecord* conPref = static_cast<CCDConnectionPrefsRecord*>(currentRecord);
    conPref->iBearerSet = aUpdate.iBearerSet;
    conPref->iDefaultIAP = aUpdate.iIapId;

    
    if ( currentRecord->IsSetAttribute(ECDNoWriteButDelete) || 
         currentRecord->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDbConnectionPrefTableView::UpdateBearerL is leaving with KErrAccessDenied"));
        User::Leave(KErrAccessDenied);
        }

    if(aReadOnly)
        {
        currentRecord->SetAttributes(ECDNoWriteButDelete); 
        dbSession->SetAttributeMask(ECDNoWriteButDelete);
        }

    // modify record
    currentRecord->ModifyL(*dbSession);

    if (aReadOnly)
        {
        dbSession->ClearAttributeMask(ECDNoWriteButDelete);
        }
    
    __FLOG_STATIC3(KLogComponent, KCommDbShim, _L("UpdateBearerL -> TableName=ConnectionPreferences, Bearer=%d, IAP=%d, ReadOnly=%b")
                                , aUpdate.iBearerSet, aUpdate.iIapId, aReadOnly);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::UpdateBearerL(const TCommDbIspBearer& /*aUpdate*/, TBool /*aReadOnly*/)
/**
 * Override version of UpdateBearerL for BC purposes.
 * @deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

 * @param aUpdate Update.
 * @param aReadOnly Read only?
 * @return KErrNotSupported.
 */

    {
    User::Leave(KErrNotSupported);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::ReadConnectionPreferenceL(TCommDbIapConnectionPref& aPref)
/** Gets the IAP connection preferences record currently selected in the view.

@param aPref On return, the connection preferences
@leave KErrNotFound There is no current record in the view
@leave KErrUnknown A field in the connection preference record is null
@capability Dependent on table, see the guide page referenced below. */
    {
    CMDBRecordBase* currentRecord = iTableExt->GetCurrentRecordL();
	CCDConnectionPrefsRecord* connPref = (static_cast<CCDConnectionPrefsRecord*>(currentRecord));

	if (connPref->iDirection.IsNull()
		|| connPref->iRanking.IsNull()
		|| connPref->iDialogPref.IsNull()
		|| connPref->iBearerSet.IsNull() )
		{
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("ReadConnectionPreferenceL is Leaving with KErrUnknown (One of the field has NULL value)"));
		User::Leave(KErrUnknown);
		}

	TInt dialogPref = connPref->iDialogPref;
	
    aPref.iDirection = connPref->iDirection;
    aPref.iRanking = connPref->iRanking;
    aPref.iDialogPref = static_cast<TCommDbDialogPref>(dialogPref);
    aPref.iBearer.iBearerSet = connPref->iBearerSet;
    aPref.iBearer.iIapId = connPref->iDefaultIAP;
	

    __FLOG_STATIC4(KLogComponent, KCommDbShim, _L("ReadConnectionPreferenceL -> TableName=ConnectionPreferences, Ranking=%d, Direction=%d, DialogPref=%d, Bearer=%d")
                            , aPref.iRanking, aPref.iDirection, aPref.iDialogPref, aPref.iBearer.iBearerSet);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::ReadConnectionPreferenceL(TCommDbIspConnectionPref& /*aPref*/)
/**@deprecated in v7.0

Framework left to support BC with v6.1
@leave KErrNotSupported
*/
    {
    User::Leave(KErrNotSupported);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::UpdateDialogPrefL(const TCommDbDialogPref& aUpdate)
/** Updates the dialog preference field (CONNECT_PREF_DIALOG_PREF) in the currently selected
connection preferences IAP record in the view.

@param aUpdate New value for the dialog preference field
@leave KErrAccessDenied The record cannot be updated as it is read-only
@leave KErrNotFound There is no current record in the view
@capability Dependent on table, see the guide page referenced below. */
    {
    CMDBRecordBase* currentRecord = iTableExt->GetCurrentRecordL();

    (static_cast<CCDConnectionPrefsRecord*>(currentRecord))->iDialogPref = aUpdate;
    
    if ( currentRecord->IsSetAttribute(ECDNoWriteButDelete) || 
         currentRecord->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("UpdateDialogPrefL -> TableName=ConnectionPreferences, DialogPref=%d is leaving with KErrAccessDenied"), aUpdate);
        User::Leave(KErrAccessDenied);
        }

    // store change
    currentRecord->ModifyL(*iDb.iImpl->iDbs);

    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("UpdateDialogPrefL -> TableName=ConnectionPreferences, DialogPref=%d"), aUpdate);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::SwapConnectionPreferencesL(TCommDbConnectionDirection aDirection, TUint32 aFirstRank, TUint32 aSecondRank)
// Restores the cursor in case of a Leave in `DoSwapConnectionPreferencesL()` though it probably shouldn't.
/** Swaps the bearer (CONNECT_PREF_BEARER_SET) and the IAP (CONNECT_PREF_IAP) fields
between two connection preferences records.

The records to use are specified by their rank (CONNECT_PREF_RANKING) and
direction (CONNECTION_PREF_DIRECTION).

Note that the dialogue option is not altered.

@param aDirection Direction of the connection preferences records
@param aFirstRank Rank of the first record to swap
@param aSecondRank Rank of the second record to swap
@leave KErrAccessDenied The record cannot be updated as it is read-only
@leave KErrArgument There is more than one record with the same ranking as
one of the rankings
@leave KErrNotFound A record with one of the rankings does not exist
@leave KErrOverflow One of the rankings is greater than the maximum allowed
@capability Dependent on table, see the guide page referenced below. */
    {
    CMDBSession* dbSession = iDb.iImpl->iDbs;
	if((aFirstRank > 2) || (aSecondRank > 2))
		{
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("SwapConnectionPreferencesL -> TableName=ConnectionPreferences, FirstRank=%d, SecondRank=%d is leaveing with Err=KErrOverflow (Rankings are greater then allowed 2)"), aFirstRank, aSecondRank);
		User::Leave(KErrOverflow);
		}

	TInt isInTransaction = dbSession->IsInTransaction();

	// start internal transaction if it is not already in transaction
	if (!isInTransaction)
		{
		dbSession->OpenTransactionL();

		TRAPD(err, DoSwapConnectionPreferencesL(aDirection, aFirstRank, aSecondRank));
		if(err == KErrNone)
			{
			dbSession->CommitTransactionL();
			}
		else
			{
			__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("SwapConnectionPreferencesL -> TableName=ConnectionPreferences, FirstRank=%d, SecondRank=%d RollBackTransaction Err=%d"), aFirstRank, aSecondRank, err);
			dbSession->RollbackTransactionL();
			User::Leave(err);
			}
		}
	else
		{
		DoSwapConnectionPreferencesL(aDirection, aFirstRank, aSecondRank);
		}

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("SwapConnectionPreferencesL -> TableName=ConnectionPreferences, FirstRank=%d, SecondRank=%d"), aFirstRank, aSecondRank);
    }

void CCommsDbConnectionPrefTableView::DoSwapConnectionPreferencesL(TCommDbConnectionDirection aDirection, TUint32 aFirstRank, TUint32 aSecondRank)
// Restores the cursor in case of a Leave in `DoSwapConnectionPreferencesL()` though it probably shouldn't.
/** Swaps the bearer (CONNECT_PREF_BEARER_SET) and the IAP (CONNECT_PREF_IAP) fields
between two connection preferences records.

The records to use are specified by their rank (CONNECT_PREF_RANKING) and
direction (CONNECTION_PREF_DIRECTION).

Note that the dialogue option is not altered.

@param aDirection Direction of the connection preferences records
@param aFirstRank Rank of the first record to swap
@param aSecondRank Rank of the second record to swap
@leave KErrAccessDenied The record cannot be updated as it is read-only
@leave KErrArgument There is more than one record with the same ranking as
one of the rankings
@leave KErrNotFound A record with one of the rankings does not exist
@leave KErrOverflow One of the rankings is greater than the maximum allowed
@capability Dependent on table, see the guide page referenced below. */
    {
    CMDBSession* dbSession = iDb.iImpl->iDbs;
    CMDBRecordSetBase* rs = iTableExt->GetRecordSetL();

	// Is it writable?
	if ( rs->IsSetAttribute(ECDNoWriteButDelete) || 
     	 rs->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DoSwapConnectionPreferencesL is leaving with KErrAccessDenied (Table query)"));
        User::Leave(KErrAccessDenied);
        }

    TInt indexFirstRank(0);
    TInt indexSecondRank(0);

    TBool foundFirstRank(EFalse);
    TBool foundSecondRank(EFalse);

    for (TInt i=0;i<rs->iRecords.Count();++i)
        {
        CCDConnectionPrefsRecord* prefRecord = static_cast<CCDConnectionPrefsRecord*>(rs->iRecords[i]);
        if(prefRecord->iDirection == aDirection)
            {
            if(prefRecord->iRanking == aFirstRank)
                {
                indexFirstRank = i;
                
                if (foundFirstRank)
                	{
                	// Found this rank twice!
                	User::Leave(KErrArgument);
                	}
                
                foundFirstRank = ETrue;
                }

            if(prefRecord->iRanking == aSecondRank)
                {
                indexSecondRank = i;

                if (foundSecondRank)
                	{
                	// Found this rank twice!
                	User::Leave(KErrArgument);
                	}
                
                foundSecondRank = ETrue;
                }
            }
        }


    // if both ranikings are valid, exchange values
    if(foundFirstRank && foundSecondRank)
        {
        if(indexFirstRank != indexSecondRank)
            {
            CCDConnectionPrefsRecord* firstRecord = static_cast<CCDConnectionPrefsRecord*>(rs->iRecords[indexFirstRank]);
            CCDConnectionPrefsRecord* secondRecord = static_cast<CCDConnectionPrefsRecord*>(rs->iRecords[indexSecondRank]);

            if ( firstRecord->IsSetAttribute(ECDNoWriteButDelete) || 
                 firstRecord->IsSetAttribute(ECDProtectedWrite)   ||
                 secondRecord->IsSetAttribute(ECDNoWriteButDelete) || 
                 secondRecord->IsSetAttribute(ECDProtectedWrite) )
                {
				__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DoSwapConnectionPreferencesL is leaving with KErrAccessDenied (Record query)"));
                User::Leave(KErrAccessDenied);
                }

          
            // temp. delete record from database
			CCDConnectionPrefsRecord* firstRec = static_cast<CCDConnectionPrefsRecord*>(rs->iRecords[indexFirstRank]);
            firstRec->DeleteL(*dbSession);
           
             // gather data records
            TInt firstBearerSet = firstRecord->iBearerSet;
            TInt firstIapId = firstRecord->iDefaultIAP;

            TInt secondBearerSet = secondRecord->iBearerSet;
            TInt secondIapId = secondRecord->iDefaultIAP;
         
            // modify the second one
            secondRecord->iBearerSet = firstBearerSet;
            secondRecord->iDefaultIAP = firstIapId;
            secondRecord->ModifyL(*dbSession);
			
			// change and restore the changed first record
		
            firstRecord->iBearerSet = secondBearerSet;
            firstRecord->iDefaultIAP = secondIapId;
		    firstRecord->StoreL(*dbSession);
            }
        }
    else
        {
        // one or both rankings are not valid
        __FLOG_STATIC3(KLogComponent, KCommDbShim, _L("SwapConnectionPreferencesL -> TableName=ConnectionPreferences, Direction=%d, FirstRank=%d, SecondRank=%d, Err: Rankings are not valid!!!")
                            ,aDirection, aFirstRank, aSecondRank);
        User::Leave(KErrNotFound);
        }

    __FLOG_STATIC3(KLogComponent, KCommDbShim, _L("SwapConnectionPreferencesL -> TableName=ConnectionPreferences, Direction=%d, FirstRank=%d, SecondRank=%d")
                            ,aDirection, aFirstRank, aSecondRank);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::ChangeConnectionPreferenceRankL(TUint32 aNewRank)
/** Sets the rank field (CONNECT_PREF_RANKING) in the currently selected connection preference
record in the view.

Note that if the new rank is not zero, and there is already a record with
this rank and the same direction, then the existing record's rank is set to
zero. This means that record is not used when the system finds the preferred
connections.

@param aNewRank The new rank for the record.
@leave KErrAccessDenied The record cannot be updated as it is read-only
@leave KErrNotFound There is no current record in the view
@leave KErrOverflow aNewRank is greater than the maximum allowed
@capability Dependent on table, see the guide page referenced below. */
    {

	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) || 
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("ChangeConnectionPreferenceRankL is leaving with KErrAccessDenied"));
        User::Leave(KErrAccessDenied);
        }

    if (aNewRank != 0)
        {

		if(aNewRank>2)
			{
			__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("ChangeConnectionPreferenceRankL is leaving with KErrOverflow (Rank is greater then 2)"));
			User::Leave(KErrOverflow);
			}

        // get current direction
        const TCommDbConnectionDirection direction = (static_cast<CCDConnectionPrefsRecord*>(record))->iDirection;
		
		CCommsDbConnectionPrefTableView* view = NULL;
        TRAPD(err, view = iDb.OpenConnectionPrefTableViewOnRankLC(direction, aNewRank);CleanupStack::Pop());

		if(err == KErrNone)
			{
			CleanupStack::PushL(view);

			view->SetRankL(0);

			CleanupStack::PopAndDestroy(view);
			}        

        SetRankL(aNewRank);
        // Refresh the data since there was a change of data in another view
        iTableExt->RefreshDatabaseL(*iDb.iImpl->iDbs);
        }
    else
        {
        SetRankL(0);
        }
    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("ChangeConnectionPreferenceRankL -> TableName=ConnectionPreferences, NewRank=%d") , aNewRank);
    }

EXPORT_C void CCommsDbConnectionPrefTableView::DeleteConnectionPreferenceL()
/** Deletes the currently selected connection preference record in the view.
@capability Dependent on table, see the guide page referenced below. */
    {
    // deletes current row from container and from database
    iTableExt->DeleteCurrentRecordL(*iDb.iImpl->iDbs);

    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("DeleteConnectionPreferenceL -> TableName=ConnectionPreferences"));
    }

EXPORT_C TInt CCommsDbConnectionPrefTableView::GotoFirstRecord()
//Moves the cursor to first connection preference record in this view
/** Sets the first connection preference record in this view as the current record.

@return KErrNotFound if the view has no records. Error codes from DBMS can
also be returned.
@capability Dependent on table, see the guide page referenced below. */
    {
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("GotoFirstRecord -> TableName=ConnectionPreferences"));
    return iTableExt->GotoFirstRecord(*iDb.iImpl->iDbs);
    }

EXPORT_C TInt CCommsDbConnectionPrefTableView::GotoNextRecord()
// Moves the cursor to next connection preference record in this view
/** Sets the next connection preference record in the view as the current record.

@return KErrNotFound if the current record is the last in the view. Error
codes from DBMS can also be returned.
@capability Dependent on table, see the guide page referenced below. */
    {
    __FLOG_STMT(TInt index;)
    __FLOG_STMT(TInt err = iTableExt->GetRecordSetIndex(index);)
    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("GotoNextRecord -> TableName=ConnectionPreferences, RecordNum=%d"), index);

    return iTableExt->GotoNextRecord(*iDb.iImpl->iDbs);
    }

EXPORT_C TInt CCommsDbConnectionPrefTableView::GotoPreviousRecord()
// Move the cursor to previous connection preference record in this view
/** Sets the previous connection preference record in the view as the current record.

@return KErrNotFound if the current record is the first in the view. Error
codes from DBMS can also be returned.
@capability Dependent on table, see the guide page referenced below. */
    {
    __FLOG_STMT(TInt index;)
    __FLOG_STMT(TInt err = index = iTableExt->GetRecordSetIndex(index);)
    __FLOG_STATIC0(KLogComponent, KCommDbShim, _L("GotoPreviousRecord -> TableName=ConnectionPreferences"));

    return iTableExt->GotoPreviousRecord(*iDb.iImpl->iDbs);
    }

RDbRowSet::TAccess CCommsDbConnectionPrefTableView::GetL()
/**
Gets the operations that can be performed on the current connection preference record
If there isn't an access table at all, then treat the table as updatable.
@return EUpdatable, if All operations can be performed on the rowset.
EReadOnly, Row navigation and reading are permitted.
EInsertOnly, Inserting new rows is the only valid operation on the rowset.
*/
    {
    RDbRowSet::TAccess access;
    iTableExt->GetTableAccess(access);

    return access;
    }

void CCommsDbConnectionPrefTableView::Close()
/**
Closes the rowset and releases any owned resources.
*/
    {
    }

void CCommsDbConnectionPrefTableView::SetRankL(TUint32 aNewRank)
    {
	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	CCDConnectionPrefsRecord* currConnPrefRec = static_cast<CCDConnectionPrefsRecord*>(record);

	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) || 
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommsDbConnectionPrefTableView::SetRankL is leaving with KErrAccessDenied"));
        User::Leave(KErrAccessDenied);
        }

    // change the ranking of a record
    TUint32 currentrank = currConnPrefRec->iRanking;
	
	currConnPrefRec->iRanking = aNewRank;
    // store change

	TRAPD(err,currConnPrefRec->ModifyL(*iDb.iImpl->iDbs));
	if(err == KErrArgument)
		{
		err = KErrOverflow; //commdb specific error code
		}

	if(err != KErrNone)
		{
		currConnPrefRec->iRanking = currentrank;
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("CCommsDbConnectionPrefTableView::SetRankL -> Rank:%d, is leaving with Err:%d"), currentrank, err);
		User::Leave(err);
		}	
	__FLOG_STMT(TInt rank = currConnPrefRec->iRanking;)
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommsDbConnectionPrefTableView::SetRankL -> Rank:%d"), rank);
    }
