// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTREGISTRYDB_H
#define BTREGISTRYDB_H

class CSecurityDecryptBase;
class CSecurityEncryptBase;

#include <d32dbms.h>
#include <btdevice.h>
#include <btmanclient.h>
#include <f32file.h>
#include <bamatch.h>
#include <btdefcommport.h>
#include <bt_subscribe.h>
#include "btmanclientserver.h"

const static TInt KArrayGranularity=8;
const static TInt KMaxConstraintLen=40;
static const TUid KBTManServerUid = {0x100069cc};

// DB, Table names

// KBTRegsitryDatabaseSecure should not be accessed directly
// Use TBTRegistryDatabaseSecure class instead 
_LIT(KBTRegistryDatabaseSecure, "btregistry");
_LIT(KBTRegistryDatabaseSecurePolicy, "secure[100069cc]");
_LIT(KPersistTable, "PersistData");
_LIT(KDeviceTable, "DeviceData");
_LIT(KCSYTable, "BTCOMMData");
_LIT(KMetaTable, "MetaData");

// values
_LIT8(KDefaultLocalName, "Symbian Device");

// Local device (aka Persistance) table schema
// Post-JS, we only need to persist what users can change
// and this will involve fewer ioctls.
// The Link Mgr will actively set the appropriate values based on
// what the clients have requested of it - so when the stack goes
// and therefore by definition, all the clients have closed their sockets
// then next time something starts the values will be set by the stack
_LIT(KPersistColName_CoD, "CoD");
_LIT(KPersistColName_ScanEnable, "ScanEnable");
_LIT(KPersistColName_LocalName, "LocalName");
_LIT(KPersistColName_PowerSettings, "PowerSettings");
_LIT(KPersistColName_AFHChannelAssessmentMode, "AFHChannelAssessmentMode");
_LIT(KPersistColName_LimitedDiscoverable, "LimitedDiscoverable");
_LIT(KPersistColName_AcceptPairedOnlyMode, "AcceptPairedOnlyMode");

// CSY table schema
_LIT(KBTCOMMColName_Port, "Port");
_LIT(KBTCOMMColName_ServiceUUID, "ServiceUUID");
_LIT(KBTCOMMColName_Security, "Security");
_LIT(KBTCOMMColName_ServiceName, "ServiceName");

// Remote device table schema

_LIT(KDeviceColName_CoD, "CoD");
_LIT(KDeviceColName_CoD_MajorDev, "CoDMajorDev");
_LIT(KDeviceColName_CoD_MinorDev, "CoDMinorDev");
_LIT(KDeviceColName_CoD_Service, "CoDService");
_LIT(KDeviceColName_BluetoothName, "BluetoothName");
_LIT(KDeviceColName_FriendlyName, "FriendlyName");
_LIT(KDeviceColName_LinkKey, "LinkKey");
_LIT(KDeviceColName_LinkKeyType, "LinkKeyType");
_LIT(KDeviceColName_PassKey, "PassKey");
_LIT(KDeviceColName_PageScanMode, "PageScanMode");
_LIT(KDeviceColName_PageScanPeriodMode, "PageScanPeriodMode");	// not used in connection
_LIT(KDeviceColName_PageScanRepMode, "PageScanRepetitionMode");
_LIT(KDeviceColName_LastSeen, "LastSeen");
_LIT(KDeviceColName_LastUsed, "LastConnected");
_LIT(KDeviceColName_GlobalSecSecurity, "GlobalSecurity");
_LIT(KDeviceColName_GlobalSecPasskeyLen, "GlobalPasskeyMinLength");
_LIT(KDeviceColName_UiCookie, "UiCookie");


// Meta table schema - DONT Change this otherwise we may lose version information
_LIT(KMetaColName_VersionMajor, "MajorVersion");
_LIT(KMetaColName_VersionMinor, "MinorVersion");

// Columns common to all tables
_LIT(KColName_DeviceAddress, "DeviceAddress");
_LIT(KColName_ProcessSID, "ProcessSID");


//

const TUint32	KRegistryDBVersionMajor		= 1;
const TUint32	KRegistryDBVersionMinor		= 2;
// Version history:
// 1.0 - Baseline
// 1.1 - Adjusted the Time/Date fields for 64 bit integers (remote device table).
// 1.2 - Added additional fields Link-key type and UI cookie (remote device table).

// Note: Any changes to the registry version will also require changes to the CBTRegistryBURData class.

// Useful SQL literals
_LIT(KSQLIsNotNull, " IS NOT NULL");
_LIT(KSQLCommaSpace, ", ");
_LIT(KSQLQuote, "'");
_LIT(KSQLHash, "#");
_LIT(KSQLEqual, "=");
_LIT(KSQLGreaterEqual, ">=");
_LIT(KSQLAnd, " AND ");
_LIT(KSQLOr, " OR ");
_LIT(KSQLLike, " LIKE ");
_LIT(KSQLOpenParentheses, "(");
_LIT(KSQLCloseParentheses, ")");

_LIT(KSQLSelectAllConstrained, "select * from %S WHERE %S");
_LIT(KSQLSelectAll, "select * from %S");

_LIT(KSQLWhere, "WHERE");


enum TBTRegistryDBPanic
	{
	ECorruptDeviceEntry,
	ENotOneLocalDeviceEntry,
	ETooManyLocalDeviceEntries,
	ETooManyRemoteDeviceEntries,
	EDeviceNotFound,
	EBadNameToUpdate,
	EMetaTableBroken,
	EColumnNotFound,
	EBTManColSetError,
	};


// forward declarations
class RBTDbQuery;

NONSHARABLE_CLASS(CBTRegistry) : public CBase
/**
	Does all the work with DBMS.  Stateless, so clients pass their views on the DB
	for this class to use on their behalf.
**/
	{
public:
	static CBTRegistry* NewL();
	~CBTRegistry();
	RDbView* OpenViewL(const TDesC& aSQLQuery, TDbBookmark& aBookmark); //opens and transfers ownership
	RDbView* OpenViewL(const RBTDbQuery& aQuery, TDbBookmark& aBookmark); //opens and transfers ownership
	RDbView* OpenDeviceL(const TBTDevAddr& aAddr, TDbBookmark& aBookmark);
	RDbView* OpenNullViewL();
	TDbColNo ColumnNoL(const TDesC& aColName, const RDbRowSet& aRowSet);
// for remote device table
	TBool DevicePresentL(const TBTDevAddr& aAddress);

	void CreateDeviceL(const CBTDevice& aDetails, TBool aStoreUiCookie, const TSecureId& aClientSID);

	const TBTNamelessDevice* GetNextNamelessDeviceLC(RDbRowSet& aRowSet, TDbBookmark& aBookmark, TBool includeKeys);
	CBTDevice* GetNextDeviceL(RDbRowSet& aRowSet, TDbBookmark& aBookmark, TBool includeLinkKey);
	const TUid CreatingProcessUidL(RDbRowSet& aRowSet);
	void UpdateDeviceL(RDbRowSet& aRowSet, const CBTDevice& aDevice);
	void UnpairL(RDbRowSet& aRowSet);
	void UnpairViewL(RDbRowSet& aRowSet);
	
	void DeleteViewL(RDbRowSet& aRowSet, TBool aDeleteAll, const TSecureId& aSecureId);

	void UpdateNameL(const TBTDevAddr& aAddress, const TDesC8& aName, TBTManServerRequest aRequest);
// for local device table
	TBTLocalDevice* GetLocalDeviceLC();
	TBTLocalDevice* GetLocalDeviceL();
	void UpdateLocalDeviceL(const TBTLocalDevice& aDevice);
// CSY table
	const TBTCommPortSettings* GetCommPortSettingsLC(const TBTCommPortSettings& aPort);

	void AddCommPortSettingsL(const TBTCommPortSettings& aSettings, const TSecureId& aClientSID);

	void UpdateCommPortSettingsL(RDbView& aView, const TBTCommPortSettings& aSettings);
	void DeleteCommPortSettingsL(RDbView& aCommPortSettingsView);
	RDbView* OpenCommPortLC(const TBTCommPortSettings& aSettings);
	RDbView* OpenCommPortL(const TBTCommPortSettings& aSettings);

	void GetDefaultDeviceFromIniL(TBTLocalDevice& aDevice);

private:
	RDbTable* OpenTableL(const TDesC& aTable);	// opens and transfers ownership
	RDbTable* OpenPersistTableL();	// opens and transfers ownership
	RDbTable* OpenDeviceTableL();	// opens and transfers ownership
	RDbTable* OpenCSYTableL();	// opens and transfers ownership
	void SetMetaDataL();
	void ValidateMetaTableL();
	void ValidatePersistTableL();
	void CreateMetaTableL();
	void CreateDeviceTableL();
	void CreateCSYTableL();
	void CreatePersistTableL();
	void CreateRegistryL();
	void OpenRegistryL();
	void PurgeDebugLinkKeysL();
	void SetDeviceL(RDbRowSet& aRowSet, const CBTDevice& aDevice, TBool aIgnoreUiCookie = EFalse);
	void IncrementRowL(RDbRowSet& aRowSet, TDbBookmark& aBookmark);
	void GetRowL(RDbRowSet& aRowSet, const TDbBookmark& aBookmark);
	void GetNamelessDetailsL(TBTNamelessDevice& aDevice, RDbRowSet& aRowSet, TBool includeKeys);
	void PutCommPortSettingsInTableL(RDbRowSet& aRowSet, const TBTCommPortSettings& aSettings);
	void CleanPutL(RDbRowSet& aRowSet);
	void ConstructL();
	CBTRegistry();
	void DeleteCorruptRegistryL();
	void SetupDefaultRegistryL();
	TBool FindVar(const TDesC8& aPtr, const TDesC8& aVarName, TPtrC8& aResult) const;
	TBool FindVar(const TDesC8& aPtr, const TDesC8& aVarName, TInt& aResult) const;
	TInt SetDeviceName(const TDesC& aDeviceName, TBTLocalDevice& aDevice);
	TBool SetDeviceNameFromIniFile(TBTLocalDevice& aDevice, RBuf8& bufPtr);
private:
	RDbNamedDatabase	iDB;
	RDbs				iDBMSServer;
	};


NONSHARABLE_CLASS(RBTDbQuery)
/**
	Encapsulates SQL Queries.
	Provides utility functions escaping certain characters, and formatting functions
**/
	{
public:
	RBTDbQuery();
	void FindDeviceL(const TBTDevAddr& aBDAddr);
	void FindCommPortL(TUint32 aUnitNumber);
	void MatchLinkKeyTypeL(TBTLinkKeyType aLinkKeyType);
	void SearchL(const TBTRegistrySearch& aSearch);
	const TDesC& QueryBuf() const;
	TPtrC ConstraintBuf() const;
	void Close();
private:
	const TPtrC QuoteEscapeLC(const TBTDevAddr& aBDAddr) const;
	const TPtrC QuoteEscapeLC(const TDesC& aBDAddr) const;
	void ExtendForAnotherTokenL(RTextBuf& aSQLBuf, TUint aMask);
private:
	RTextBuf			iQueryBuf;
	};


NONSHARABLE_CLASS(TBTRegistryDatabaseSecure)
/**
	Defines location of BT registry database
**/
	{
public:
	TBTRegistryDatabaseSecure();
	const TDesC& operator()() const;
		
private:
	TBuf<KMaxFileName> iBuf;
	};
	
#endif
