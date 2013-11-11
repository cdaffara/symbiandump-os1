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
//
 
#ifndef __DBUNDO_H__
#define __DBUNDO_H__

#include <commsdattypesv1_1.h>
#include <metadatabase.h>
using namespace CommsDat;
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>

// For Connection Preferences table
#include <cdbpreftable.h>

// COMMDB Connection Preference settings
#include <comms-infras/connectionsettings.h>

// Indexes of the CDMA, GPRS and Dial Out IAP records
// Dependent on the database used
#define KGprsIAPIndex 2
#define KDialOutIAPIndex 1

_LIT( KDialOutIap, "DialOutIsp" );
_LIT( KGprsIap, "NTRas GPRS" );

// Classes to maintain an "undo" linked list to restore database state
// between tests
class CDbAgtBase;

// Base class for storing undo data, derived classes store data specific to the 
// type of column we're dealing with
class CDbUndoBase : public CBase
	{
public:
	virtual void UndoL(CDbAgtBase *aTable)=0;

public:
	TDblQueLink iLink;

protected:
	TPtrC iColumn;
	};

// Derived from CDbUndoBase to store integer values requiring Undo
class CDbUndoInt : public CDbUndoBase
	{
public:

	static CDbUndoInt* NewL(const TDesC &aColumn, const TUint32 &aValue);
	void ConstructL(const TDesC &aColumn, const TUint32 &aValue);

	// Concrete implementation of pure virtual in CDbUndoBase
	virtual void UndoL(CDbAgtBase *aDb);

private:
	// Data specific to the type of column we're are dealing with
	// In this class we undo integer data.
	TUint32 iValue;
	};

// Derived from CDbUndoBase to store boolean values requiring Undo
class CDbUndoBool : public CDbUndoBase
	{
public:

	static CDbUndoBool* NewL(const TDesC &aColumn, const TBool &aValue);
	void ConstructL(const TDesC &aColumn, const TBool &aValue);

	// Concrete implementation of pure virtual in CDbUndoBase
	virtual void UndoL(CDbAgtBase *aDb);

private:
	// Data specific to the type of column we're are dealing with
	// In this class we undo boolean data.
	TBool iValue;
	};

// Derived from CDbUndoBase to store text values requiring Undo
class CDbUndoText : public CDbUndoBase
	{
public:

	static CDbUndoText* NewL(const TDesC &aColumn, const TDesC16& aValue);
	void ConstructL(const TDesC &aColumn, const TDesC16& aValue);

	// Concrete implementation of pure virtual in CDbUndoBase
	virtual void UndoL(CDbAgtBase *aDb);

private:
	// Data specific to the type of column we're are dealing with
	// In this class we undo text data.
	TBuf16<KCommsDbSvrMaxFieldLength> iText;
	};

// Derived from CDbUndoBase to store long text values requiring Undo
class CDbUndoLongText : public CDbUndoBase
	{
public:

	static CDbUndoLongText* NewL(const TDesC &aColumn, const HBufC* aValue);
	void ConstructL(const TDesC &aColumn, const HBufC* aValue);

	// Concrete implementation of pure virtual in CDbUndoBase
	virtual void UndoL(CDbAgtBase *aDb);

private:
	// Data specific to the type of column we're are dealing with
	// In this class we undo long text data.
	HBufC* iLongText;
	};

// Derived from CDbUndoBase to store agent extension name requiring Undo
class CDbUndoAgentExt : public CDbUndoBase
	{
public:

	static CDbUndoAgentExt* NewL(const TDesC &aService, const TDesC& aAgentExt);
	void ConstructL(const TDesC &aService, const TDesC& aAgentExt);

	// Concrete implementation of pure virtual in CDbUndoBase
	virtual void UndoL(CDbAgtBase *aDb);

private:
	// Data specific to the type of column we're are dealing with
	// In this class we undo text data.
	TBuf16<KCommsDbSvrMaxFieldLength> iText;
	};

// Base class for all database access
class CDbAgtBase : public CBase 
	{
	friend class CDbUndoInt;
	friend class CDbUndoBool;
	friend class CDbUndoText;
	friend class CDbUndoLongText;
	friend class CDbUndoAgentExt;
public:
	~CDbAgtBase();
	void SetColumnIntL(const TDesC& aColumn, const TUint32& aValue);
	void SetColumnBoolL(const TDesC& aColumn, const TBool& aValue);
	void SetColumnTextL(const TDesC& aColumn, const TDesC16& aValue);
	void SetColumnLongTextL(const TDesC& aColumn, const TDesC16& aValue);
	void UndoDatabaseChangesL();
	CMDBRecordSetBase* Table();

protected:
	CDbAgtBase();
	void ConstructL();


protected:
	// Used to access the database and retain a view on whichever table is of interest
	CMDBSession *iDb;
	CMDBRecordSetBase* iTable;
	TUint iCurrentRecord;

private:
	void ModifyColumnIntL(const TDesC& aColumn, const TUint32& aValue);
	void ModifyColumnBoolL(const TDesC& aColumn, const TBool& aValue);
	void ModifyColumnTextL(const TDesC& aColumn, const TDesC16& aValue);
	void ModifyColumnLongTextL(const TDesC& aColumn, const TDesC16& aValue);

private:
	// Double linked list header for undo functionality
	TDblQue<CDbUndoBase> iQHeader;
	
	// Double linked list iterator for undo functionality
	TDblQueIter<CDbUndoBase> iQIter;

	};

// GPRS OUTGOING specific class 
class CDbGPRSOutgoingTable : public CDbAgtBase 
	{
public:
	static CDbGPRSOutgoingTable *NewL();
	~CDbGPRSOutgoingTable();

private:
	CDbGPRSOutgoingTable();
	void ConstructL();
	};

// CSD specific class 
class CDbCsdTable : public CDbAgtBase 
	{
public:
	static CDbCsdTable *NewL();
	~CDbCsdTable();
	CMDBRecordSetBase* Table();

private:
	CDbCsdTable();
	void ConstructL();
	};

// MODEM specific class 
class CDbModemTable : public CDbAgtBase 
	{
public:
	static CDbModemTable *NewL();
	~CDbModemTable();

private:
	CDbModemTable();
	void ConstructL();
	};
/*
// Preference table
class CDbPrefTable : public CDbAgtBase 
	{
public:
	static CDbPrefTable *NewL( TCommDbConnectionDirection aDirection, CCommsDbConnectionPrefTableView::TCommDbIapBearer aBearerUpdate, TBool aReadOnly, TCommDbDialogPref aDialogPref );
	~CDbPrefTable();

private:
	CDbPrefTable();
	void ConstructL();
	CCommsDbConnectionPrefTableView* iPrefTable;

	TBuf16<KCommsDbSvrMaxFieldLength> iIapName;
	TCommDbConnectionDirection iDirection;
	CCommsDbConnectionPrefTableView::TCommDbIapBearer iBearerUpdate;
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref iOldPrefs;
	TBool iReadOnly;
	TCommDbDialogPref iDialogPref;
	};

// IAP table
class CDbIapTable : public CDbAgtBase 
	{
public:
	static CDbIapTable *NewL(const TDesC16& aIapName);
	~CDbIapTable();
	void GetBearerSetIapId(TUint32& aIapId);

private:
	CDbIapTable();
	void ConstructL();
	TBuf16<KCommsDbSvrMaxFieldLength> iIapName;
	};
*/
#endif  // DBUNDO
