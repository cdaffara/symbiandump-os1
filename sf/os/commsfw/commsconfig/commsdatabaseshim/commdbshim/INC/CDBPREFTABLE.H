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
// Comms Database Connection Preference Table header
// 
//

/**
 @file
 @publishedAll
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef CDBPREFTABLE_H
#define	CDBPREFTABLE_H

#include <d32dbms.h>
#include <cdbcols.h>
#include <cdblen.h>

class CCommsDatabase;
class TCommDbCommonBearerInfo;
class CCommDbTableExtension;
class CCommsDbTableView;
//Created via CCommsDatabase::OpenConnectionPrefTableLC() or NewLC().
//Uses a CCommsDatabase and has an RDbView for access to the database.
class CCommsDbConnectionPrefTableView : public CBase
/**

Implements actions on records of the Connection Preferences table, to allow clients to set
the preferred order in which connections are attempted.

The class is similiar to CCommsDbTableView, but only allows access to connection
preferences as a set and not to individual fields in the table. This prevents
inappropriate updates of the records. Like CCommsDbTableView, the class has
a group of functions for navigating through the records in the view, and another
group for setting fields in the current record.

Clients do not create this type of object, but get an instance through
CCommsDatabase::OpenConnectionPrefTableLC(),
CCommsDatabase::OpenConnectionPrefTableInRankOrderLC(), or
CCommsDatabase::OpenConnectionPrefTableViewOnRankLC().

In addition to the leave codes documented, all leaving functions can leave
with any error returned by DBMS during database manipulation.

@see CCommDbOverrideSettings::GetConnectionPreferenceOverride()
@see CCommDbOverrideSettings::SetConnectionPreferenceOverride()
@publishedAll
@released */
	{
public:
	friend class CCommsDatabaseBase;

public:
	class TCommDbIapBearer
	/** Encapsulates the bearer set (CONNECT_PREF_BEARER_SET) and IAP (CONNECT_PREF_IAP)
	fields.
	Used in calls to UpdateBearerL() and as a public member of TCommDbIapConnectionPref.
	@publishedAll
	@released */
		{
	public:
		IMPORT_C TCommDbIapBearer();
	public:
		/** Value for the bearer set (CONNECT_PREF_BEARER_SET) field. */
		TUint32 iBearerSet;
		/** Value for the IAP (CONNECT_PREF_IAP) field. */
		TUint32 iIapId;
		};

	class TCommDbIapConnectionPref
	/** Encapsulates the rank (CONNECT_PREF_RANKING), direction (CONNECTION_PREF_DIRECTION),
	and dialog preference (CONNECT_PREF_DIALOG_PREF) fields, plus a TCommDbIapBearer
	object.
	A complete connection preference - containing rank, direction, dialogue option,
	bearers for the dialogue and the prefered IAP.
	@see TCommDbConnectionDirection
	@publishedAll
	@released */
		{
	public:
		IMPORT_C TCommDbIapConnectionPref();
		IMPORT_C TBool operator==(const TCommDbIapConnectionPref& aPref) const;
	public:
		/** Value for the rank (CONNECT_PREF_BEARER_SET) field. */
		TUint32 iRanking;
		/** Value for the direction (CONNECTION_PREF_DIRECTION) field.

		@see TCommDbConnectionDirection */
		TCommDbConnectionDirection iDirection;
		/** Value for the dialog preference (CONNECT_PREF_DIALOG_PREF) field.

		@see TCommDbDialogPref */
		TCommDbDialogPref iDialogPref;
		/** Values for the bearer (CONNECT_PREF_BEARER_SET) and IAP (CONNECT_PREF_IAP) fields.

		@see TCommDbIapBearer */
		TCommDbIapBearer iBearer;
		};

	class TCommDbIspBearer
	/**

	Frame left in place for BC with 6.1
	@publishedAll
	@deprecated 7.0 */
		{
	public:
		IMPORT_C TCommDbIspBearer();
		};

	class TCommDbIspConnectionPref
/**
@publishedAll
@deprecated
*/
		{
	public:
		IMPORT_C TCommDbIspConnectionPref();
		IMPORT_C TBool operator==(const TCommDbIspConnectionPref& aPref) const;
	public:
		TCommDbIspBearer iBearer;
		};

public:
	IMPORT_C virtual ~CCommsDbConnectionPrefTableView();

	static CCommsDbConnectionPrefTableView* NewLC(CCommsDatabase& aDb, const TDbQuery& aQuery);
	static CCommsDbConnectionPrefTableView* NewLC(CCommsDatabase& aDb, TCommDbConnectionDirection aDirection, TBool aSortRanking = EFalse);
	static CCommsDbConnectionPrefTableView* NewL(CCommsDatabase& aDb, TCommDbConnectionDirection aDirection,	TUint32 aRank);

	IMPORT_C void InsertConnectionPreferenceL(const TCommDbIapConnectionPref& aPref, TBool aReadOnly = EFalse);

	IMPORT_C void ReadConnectionPreferenceL(TCommDbIapConnectionPref& aPref);

	IMPORT_C void UpdateBearerL(const TCommDbIapBearer& aUpdate, TBool aReadOnly = EFalse);

	IMPORT_C void UpdateDialogPrefL(const TCommDbDialogPref& aUpdate);

	IMPORT_C void ChangeConnectionPreferenceRankL(TUint32 aNewRank);

	IMPORT_C void DeleteConnectionPreferenceL();

	IMPORT_C void SwapConnectionPreferencesL(TCommDbConnectionDirection aDirection, TUint32 aFirstRank, TUint32 aSecondRank);

	IMPORT_C TInt GotoFirstRecord();

	IMPORT_C TInt GotoNextRecord();

	IMPORT_C TInt GotoPreviousRecord();

	//
	// ISP Connection Preference record operations
	// All these were deprecated in 7.0 but have now
	// been re-instated in limited form to accomodate 6.1 BC
	//
	IMPORT_C void InsertConnectionPreferenceL(const TCommDbIspConnectionPref& aPref, TBool aReadOnly = EFalse);
	IMPORT_C void UpdateBearerL(const TCommDbIspBearer& aUpdate, TBool aReadOnly = EFalse);
	IMPORT_C void ReadConnectionPreferenceL(TCommDbIspConnectionPref& aPref);

protected:
	CCommsDbConnectionPrefTableView(CCommsDatabase& aDb);
	void ConstructL(const TDbQuery& aQuery);
	void ConstructL(TCommDbConnectionDirection aDirection, TBool aSortRanking);
	void ConstructL(TCommDbConnectionDirection aDirection, TUint32 aRank);
	void Close();
	void  OpenL(const TDbQuery& aQuery);
	RDbRowSet::TAccess GetL();
	inline TDbColNo ColNum(const TDesC& aColumn) const;
	void GetUint32L(const TDesC& aColumnName, TUint32& aValue);

private:
	void SetRankL(TUint32 aNewRank);
	void DoSwapConnectionPreferencesL(TCommDbConnectionDirection aDirection, TUint32 aFirstRank, TUint32 aSecondRank);

protected:
	/** DBMS view. Variable not used in shim. Not removed because of BC break */
	RDbView iTableView;
	CCommDbTableExtension* iTableExt;

	/** Comms data base that is being viewed. */
	CCommsDatabase& iDb;
	};

#endif
