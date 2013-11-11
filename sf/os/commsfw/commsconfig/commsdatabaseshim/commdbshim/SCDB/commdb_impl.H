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
// Comms Database public interface
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef COMMDB_IMPL_H
#define	COMMDB_IMPL_H

#include <d32dbms.h>
#include <metadatabase.h>
#include <commsdattypesv1_1.h>

#include <dial.h>
#include <cdbcols.h>
#include <cdblen.h>
#include <d32dbms.h>

#include "NotifyClient.H"

#include <e32property.h>

class CCommsDatabaseBase;
class CCommsDbTableView;

using namespace CommsDat;

// Params used for customising SetRecordSetL call
const TBool KPreparePriming 		= ETrue;
const TBool KDoNotPreparePriming 	= EFalse;
const TBool KLoadTable	 			= ETrue;
const TBool KDoNotLoadTable		 	= EFalse;

const TBool KTemplateCall			= ETrue;

const TBool KProtectTable 			= ETrue;
const TBool KUnprotectTable 		= EFalse;
const TBool KProtectRecord 			= ETrue;
const TBool KUnprotectRecord 		= EFalse;

const TBool KHidden					= ETrue;
const TBool KNotHidden				= EFalse;

const TBool KReadOnly				= ETrue;
const TBool KNotReadOnly			= EFalse;

// CCommDbTableExtension class
//
// Contains data items & access functions moved here rather than CCommsDbConnectionPrefTableView
// to preserve the size of the class and hence BC.
//
NONSHARABLE_CLASS(CCommDbTableExtension) : public CBase
/**
CCommDbTableExtension class
Contains data items & access functions moved here rather than CCommsDbConnectionPrefTableView
to preserve the size of the class and hence BC.

@internalComponent
*/
	{
public:
	static CCommDbTableExtension* NewL();
	~CCommDbTableExtension();

	void GetRecordInfoL(const TDesC& aColumn, TDbColType& aColType, TUint32& aAttrib);
	TDbColType GetColType(TInt aType);
	TInt GetMaxTypeLength(TInt aType);

	TInt GetTableAccess(RDbRowSet::TAccess &aAccess);

	CMDBRecordSetBase* GetRecordSetL();
	TInt GetRecordSet(CMDBRecordSetBase*& aRecSet);
	
	CMDBRecordBase* GetCurrentRecordL();
	TInt GetCurrentRecord(CMDBRecordBase*& aRecord);

	TInt GetRecordSetIndex(TInt &aIndex);
	TInt GetRecordSetIndexL();
	void RefreshDatabaseL(CMDBSession& aDbs);
	
	TInt GotoFirstRecord(CMDBSession& aDbs);
	TInt GotoNextRecord(CMDBSession& aDbs);
	TInt GotoPreviousRecord(CMDBSession& aDbs);

	CMDBRecordSetBase* RecordSetFactoryL(const TDesC& aTableName, TBool preparePriming, CMDBSession& aDbs);
	TBool IsGenericRecord(TMDBElementId aElementId) const;

	void SetRecordSetL(const TDesC& aTableName, CMDBSession& aDbs, TBool preparePriming, TBool aLoadTable, TBool aTemplate = EFalse);
	void SetRecordSet(CMDBRecordSetBase* aRecordSetContainer);

	void InsertConnPrefRecordL(CCDConnectionPrefsRecord* aConnPref, CMDBSession& aDbs);
	void InsertRecordL(TBool aCopy, TUint32& aId, CMDBSession& aDbs, TBool aIsTemplate);
	void DeleteCurrentRecordL(CMDBSession& aDbs, TBool aContainerOnly = EFalse);
	TInt CancelRecordChanges(CMDBSession& aDbs);

	void ResetArrays(CMDBSession& aSession);
	void ResetAndDestroyL();
	
	void CommitTransaction();
	void RollBackTransaction();
	
	// sorting functions
	void SortById();
	void SortByRank();

	void SetShowHiddenRecords(TBool aShowHidden)
		{
		iShowHidden = aShowHidden;
		}
private:
	CCommDbTableExtension();

	static TInt CompareRecordsById(const CMDBRecordBase& aLeft, const CMDBRecordBase& aRight);
	static TInt CompareRecordsByRank(const CMDBRecordBase& aLeft, const CMDBRecordBase& aRight);
	
	TInt GetCurrentRecordIndex() const;
	TBool AdvanceIndexToPrevValidRecord(TInt& aIdx) const;
	TBool AdvanceIndexToNextValidRecord(TInt& aIdx) const;
	
	TBool FindFirstNextValidRecord(TBool aResetToStart);
	TBool FindFirstPrevValidRecord();
	TInt FindValueInRecSet(TInt aValue) const;

	void UpdateLastCommitSeq();
	TInt MaybeRefreshCurrentRecord(CMDBSession& aDbs);
private:
	CMDBRecordSetBase* 	iRecordSetContainer;
	CMDBRecordBase*	iCurrentRecord;
	CMDBRecordBase* iEndOfTableRecordBackup;
	CMDBRecordBase* iBeginingOfTableRecordBackup;
	TBool iShowHidden;
	RProperty iCommitSeqProperty;
	TInt iLastCommitSeq;
public:
	RArray<TUint32> iInsertedRecords;
	RArray<TUint32> iDeletedRecords;
	};

class CCommsDbTableView;
class CCommsDbConnectionPrefTableView;
class CCommDbTableExtension;

NONSHARABLE_CLASS(CCommsDatabaseImpl) : public CBase
	{
	public:
		friend class CCommsDatabaseBase;
	private:
		CCommsDatabaseImpl();
		~CCommsDatabaseImpl();
	public:
		void RegisterViewL(CCommsDbTableView* aTable);
		void UnRegisterView(CCommsDbTableView* aTable);
		void RegisterConnPrefViewL(CCommsDbConnectionPrefTableView* aTable);
		void UnRegisterConnPrefView(CCommsDbConnectionPrefTableView* aTable);
        void CheckGlobalSettingsTableExistsL();
		TInt RequestNotification(TRequestStatus& aStatus);
		void CancelRequestNotification();
	private:
		RPointerArray<CCommsDbTableView> iTableViews;
		RPointerArray<CCommsDbConnectionPrefTableView> iConnPrefViews;
        TBool iHaveCheckedGSTableExists;
        //Notification
        RCommsdatNotifier iShimNotifier;
	public:
		CMDBSession* iDbs;
	};

#endif
// COMMDB_IMPL_H
