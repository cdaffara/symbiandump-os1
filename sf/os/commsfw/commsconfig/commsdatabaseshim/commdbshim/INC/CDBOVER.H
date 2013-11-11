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

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#ifndef CDBOVER_H
#define CDBOVER_H

#include <e32base.h>
#include <commdb.h>
#include <cdbpreftable.h>

class CCommDbOverride;
class CCommDbLongDesOverride;
class TCommDbIapConnectionPrefOverride;

class CCommDbOverrideSettings : public CBase
/** Repository of override settings for columns in tables.
@deprecated
	Overrides are deprecated from v9.1

	Holds all information about CommDb overrides.
	Allows the client to store, retrieve and test the existence of overrides.
*/
	{
public:
	/** @deprecated v7.0

	Override setting flags returned by PartialFull().
	The only use of TParamList is during the comparison of two override sets. */
	enum TParamList
		{
		/** The override settings are partial; the CCommDbOverrideSettings object maintains
		override values for only some of the columns. */
		EParamListPartial,
		/** The override settings are full; the CCommDbOverrideSettings object maintains
		override values for all columns */
		EParamListFull
		};

	/** The value types that may be stored in as overrides
		@deprecated
			Overrides are deprecated from v9.1
		@internalComponent "Could just use TDbColType" */
	enum TValueType
		{
	/** a TUint32. */
		EIntValue,
	/** a TBool. */
		EBoolValue,
	/** a TDes8. */
		EDes8Value,
	/** a TDes16. */
		EDes16Value,
	/** a TDes. */
		ELongDesValue,
	/** Unkown type. */
		EUnknownType
		};

public:
	IMPORT_C static CCommDbOverrideSettings* NewL(TParamList aParamList);
	IMPORT_C virtual ~CCommDbOverrideSettings();

	// Setting override values in the override set
	IMPORT_C TInt SetIntOverride(const TDesC& aTableName, const TDesC& aColumnName, TUint32 aValue);
	IMPORT_C TInt SetBoolOverride(const TDesC& aTableName, const TDesC& aColumnName, TBool aValue);
	IMPORT_C TInt SetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, const TDesC8& aValue);
	IMPORT_C TInt SetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, const TDesC16& aValue);
	IMPORT_C TInt SetLongDesOverride(const TDesC& aTableName, const TDesC& aColumnName, const TDesC& aValue);

	//  Getting override values from the override set. Returns `KErrNotFound` if the
	//	column has not been overriden.
	IMPORT_C TInt GetIntOverride(const TDesC& aTableName, const TDesC& aColumnName, TUint32& aValue);
	IMPORT_C TInt GetBoolOverride(const TDesC& aTableName, const TDesC& aColumnName, TBool& aValue);
	IMPORT_C TInt GetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, TDes8& aValue);
	IMPORT_C TInt GetDesOverride(const TDesC& aTableName, const TDesC& aColumnName, TDes16& aValue);
	IMPORT_C TInt GetLongDesOverride(const TDesC& aTableName, const TDesC& aColumnName, TDes& aValue);
	IMPORT_C TInt GetLongDesOverrideLength(const TDesC& aTableName, const TDesC& aColumnName, TInt& aLength);

	IMPORT_C TInt SetConnectionPreferenceOverride(const CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref& aPref);

	IMPORT_C TInt GetConnectionPreferenceOverride(CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref& aPref);

	IMPORT_C TBool Compare(CCommDbOverrideSettings* aOverrides) const;

	IMPORT_C TBool IsOverridden(const TDesC& aTableName, const TDesC& aColumnName, TValueType aType);

	inline TParamList PartialFull() const;

	IMPORT_C static CCommDbOverrideSettings* NewL(TParamList aParamList, TCommDbDatabaseType aDbType);

	IMPORT_C TInt GetConnectionPreferenceOverride(CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref& aPref);		// will be deprecated in 7.0

	IMPORT_C TInt SetConnectionPreferenceOverride(const CCommsDbConnectionPrefTableView::TCommDbIspConnectionPref& aPref);	// will be deprecated in 7.0

protected:
	CCommDbOverrideSettings(TParamList aParamList);
	virtual void ConstructL();
	TInt FindOverride(const TDesC& aTableName, const TDesC& aColumnName, TValueType aType) const;
	TBool IllegalOverride(const TDesC& aTableName,  const TDesC& aColumnName) const;
	void CheckL(const TDesC& aTableName, const TDesC& aColumnName, TValueType aType) const;
	static TBool MatchIapOverride(const TCommDbIapConnectionPrefOverride& aFirst,
								  const TCommDbIapConnectionPrefOverride& aSecond);
protected:
	TParamList iPartialFull;
	RPointerArray<CCommDbOverride> iOverrides;
	RArray<TCommDbIapConnectionPrefOverride> iIapOverrides;
	CCommsDatabase* iDb;

	};

#include <cdbover.inl>

#endif
