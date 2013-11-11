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
// This does the main work on behalf of the subsessions of liasing with DBMS
// 
//

#include "BTRegistryDB.h"
#include "btmanserverutil.h"
#include <utf.h>
#include <bluetooth/logger.h>
#include <bafl/sysutil.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_REGISTRY);
#endif

#ifdef _DEBUG
PANICCATEGORY("btregdb");
#endif


void Panic(TBTRegistryDBPanic aPanic)
	{
	LOG_STATIC_FUNC
	_LIT(KRegistryDBPanicName, "BT RegistryDB Panic");
	User::Panic(KRegistryDBPanicName, aPanic);
	}
//

CBTRegistry::CBTRegistry()
	{
	LOG_FUNC
	}

CBTRegistry::~CBTRegistry()
	{
	LOG_FUNC
	iDB.Close();
	iDBMSServer.Close();
	}

CBTRegistry* CBTRegistry::NewL()
	{
	LOG_STATIC_FUNC
	CBTRegistry* self = new (ELeave) CBTRegistry;
	CleanupStack::PushL(self);
	self->ConstructL();
#ifdef _DEBUG
	CleanupStack::Check(self);
#endif
	CleanupStack::Pop();
	return self;
	}

void CBTRegistry::ConstructL()
	{
	LOG_FUNC
	// we use a DBMS Server session as multiple clients may
	// be updating their views concurrently.  Without this
	// DBMS would (rightly) panic a rowset already in update
	User::LeaveIfError(iDBMSServer.Connect());
	OpenRegistryL();
	}

void CBTRegistry::OpenRegistryL()
/**
	Open the Registry DB.
	If it doesn't exist, attempt to create the necessary path,file and DB
**/
	{
	LOG_FUNC
	const static TInt KMaxTries=4;
	TInt err;
	TInt loopCount=0;
	

	//	In secure EKA2 DBMS land, registry DB is created in DBMS data cage
	do
		{
		err=iDB.Open(iDBMSServer, TBTRegistryDatabaseSecure()(), KBTRegistryDatabaseSecurePolicy());
			
		if(err == KErrNotFound)
			{
			CreateRegistryL();
			}
		else if (err != KErrNone)
			{
			// If we get here the BT registry file must be corrupt. 
			// Delete the file and go round the while loop once more.
			DeleteCorruptRegistryL();
			}
		else
			{
			TRAP(err,ValidateMetaTableL()); // make sure it's ok version etc.
			if(err==KErrNone)
				{
				TRAP(err,ValidatePersistTableL()); // make sure it's been properly written to
				}
			if (err!=KErrNone)
				{
				// Either the Meta Table validate failed or the Persist Table validate failed.
				// Delete the file and go round the while loop once more.
				DeleteCorruptRegistryL();
				}
			}	
		loopCount++;
		} while (err!=KErrNone && loopCount<KMaxTries);

	if(err != KErrNone)
		{
		User::Leave(KErrAbort);		// give up gracefully
		}

	// Purge any SSP debug link keys left over from a previous SSP debug session
	PurgeDebugLinkKeysL();

	(void)iDB.Compact();	// compact now
	// ready!
	}



RDbTable* CBTRegistry::OpenTableL(const TDesC& aTable)
/**
	Find tables of a given name
	@param	aTable	Name of table to open
	@return table and ownership
**/
	{
	LOG_FUNC
	RDbTable* table = new (ELeave) RDbTable;
	CleanupCloseDeletePushL(table);

	TInt err;

	err = table->Open(iDB, aTable);
	User::LeaveIfError(err);

	CleanupStack::Pop(); // table

	return table;
	}

RDbTable* CBTRegistry::OpenPersistTableL()
/**
	Find the persist table (local device settings)
	and return it and ownership

	@return ownership of table
**/
	{
	LOG_FUNC
	return OpenTableL(KPersistTable);
	}

RDbTable* CBTRegistry::OpenDeviceTableL()
/**
	Find the remote device table
	@return table and ownership
**/
	{
	LOG_FUNC
	return OpenTableL(KDeviceTable);
	}

RDbTable* CBTRegistry::OpenCSYTableL()
/**
	Find the CSY table
	@return table and ownership
**/
	{
	LOG_FUNC
	return OpenTableL(KCSYTable);
	}

void CBTRegistry::ValidateMetaTableL()
/**
	Go to Meta table and check version numbers etc
**/
	{
	LOG_FUNC
	RDbTable* table = OpenTableL(KMetaTable);
	CleanupCloseDeletePushL(table);

	// get the processSID
	TBool success = table->FirstL();
	if (!success)
		{
		LOG(_L("BT Registry DB Meta Table CORRUPT!"));
		User::Leave(KErrEof);
		}

	table->GetL();
	

	//put version checking in when we think about backup server
	TUint major = table->ColUint32(ColumnNoL(KMetaColName_VersionMajor, *table));
	TUint minor = table->ColUint32(ColumnNoL(KMetaColName_VersionMinor, *table));
	LOG(_L("CBTRegistry opened registry DB Meta Table"));
	LOG1(_L("\tMajorVersion: %d"), major); 
	LOG1(_L("\tMinorVersion: %d"), minor); 

	if(KRegistryDBVersionMajor != major || 
			KRegistryDBVersionMinor != minor)
		{
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PopAndDestroy(table);
	}

void CBTRegistry::ValidatePersistTableL()
/**
	Go to Persist table and check local device values
**/
	{
	LOG_FUNC
	RDbTable* table = OpenTableL(KPersistTable);
	CleanupCloseDeletePushL(table);

	// get the processSID
	TBool success = table->FirstL();
	if (!success)
		{
		LOG(_L("BT Registry DB Local Device Table (Persist Table) CORRUPT!"));
		User::Leave(KErrEof);
		}
		
	table->GetL();
	
#ifdef _DEBUG
	// check the Local device configuration (aka Stack Persistance data)
	TPtrC8 addrBuf(table->ColDes8(ColumnNoL(KColName_DeviceAddress, *table)));
	TUint32 CoD = table->ColUint32(ColumnNoL(KPersistColName_CoD, *table));
	TUint8 scanEnable = table->ColUint8(ColumnNoL(KPersistColName_ScanEnable, *table));
	TPtrC8 name = table->ColDes8(ColumnNoL(KPersistColName_LocalName, *table));
	TUint8 powerSettings = table->ColUint8(ColumnNoL(KPersistColName_PowerSettings, *table));
	TUint8 limDisc = table->ColUint8(ColumnNoL(KPersistColName_LimitedDiscoverable, *table));
	TBool afhChannelAssessmentMode = table->ColUint8(ColumnNoL(KPersistColName_AFHChannelAssessmentMode, *table));
	TBool acceptPairedOnlyMode = table->ColUint8(ColumnNoL(KPersistColName_AcceptPairedOnlyMode, *table));
	LOG(_L("CBTRegistry opened registry DB Persist Table"));
	LOG6(_L("\tDevice Address (zero is acceptable): %02x%02x %02x%02x%02x%02x"),
			addrBuf[0],
			addrBuf[1],
			addrBuf[2],
			addrBuf[3],
			addrBuf[4],
			addrBuf[5]
			);
	LOG1(_L("\tCoD: 0x%02x"),CoD);
	LOG1(_L("\tScan Enable: 0x%02x"),scanEnable);
	LOG1(_L8("\tLocal Name: \"%S\""),&name); //name is an 8 bit string
	LOG1(_L("\tPower Settings: 0x%02x"),powerSettings);
	LOG1(_L("\tLimited Discoverable: 0x%02x"),limDisc);
	LOG1(_L("\tAFHChannelAssessmentMode: %d"),afhChannelAssessmentMode);
	LOG1(_L("\tAcceptPairedOnlyMode: %d"),acceptPairedOnlyMode);
#endif

	CleanupStack::PopAndDestroy(table);
	}

void CBTRegistry::CreateRegistryL()
/**
	Creates the Database and Tables we need
**/
	{
	LOG_FUNC
	//	In secure DBMS land, the BT Manager DB is created in DBMS's data cage.
	TInt err = iDB.Create(iDBMSServer, TBTRegistryDatabaseSecure()(), KBTRegistryDatabaseSecurePolicy()); // This leaves iDB in an open state.


	User::LeaveIfError(err);
	
	SetupDefaultRegistryL();
	
	// close the database
	iDB.Close();	
	LOG(_L("BT Registry DB Created successfully"));
	}

void CBTRegistry::PurgeDebugLinkKeysL()
/**
 	Remove (if present) any SSP debug link keys that may have been created by a previous SSP debug session.
**/
	{
	LOG_FUNC
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.MatchLinkKeyTypeL(ELinkKeyDebug);

	//We dont use the bookmark here, so employ a dummy bookmark.
	TDbBookmark dummy;
	RDbView* view = OpenViewL(query, dummy);
	CleanupCloseDeletePushL(view);

	if (!view->IsEmptyL())
		{
		// Debug link keys found in registry, unpair these.
		UnpairViewL(*view);
		}
	
	CleanupStack::PopAndDestroy(2, &query);	// view and query
	}


void CBTRegistry::CreatePersistTableL()
/**
	Create the table with all that is necessary for storing local device details
**/
	{
	LOG_FUNC
	CDbColSet* theColumns = CDbColSet::NewLC();

	// add in the columns for the Local device configuration (aka Stack Persistance data)
	theColumns->AddL(TDbCol(KColName_DeviceAddress,						EDbColText8));	// Binary not allowed in SQL
	theColumns->AddL(TDbCol(KPersistColName_CoD,						EDbColUint32));
	theColumns->AddL(TDbCol(KPersistColName_ScanEnable,					EDbColUint8));
	// specify the maximum local name length to be KMaxBluetoothNameLen
	theColumns->AddL(TDbCol(KPersistColName_LocalName,					EDbColText8, KMaxBluetoothNameLen));
	theColumns->AddL(TDbCol(KPersistColName_PowerSettings,				EDbColUint8));
	theColumns->AddL(TDbCol(KPersistColName_LimitedDiscoverable,		EDbColBit));
	theColumns->AddL(TDbCol(KPersistColName_AFHChannelAssessmentMode,	EDbColBit));
	theColumns->AddL(TDbCol(KPersistColName_AcceptPairedOnlyMode,		EDbColBit));
	User::LeaveIfError(iDB.CreateTable(KPersistTable, *theColumns));

	CleanupStack::PopAndDestroy(theColumns);
	}

void CBTRegistry::CreateDeviceTableL()
/**
	Create the table with all that is necessary to store details about remote devices
	
	To provide better CoD searching than the underlying SQL supports
	this table uses >1 column for device class of remote device
	
	This also allows for future versions of the CoD field in the BT Spec
**/
	{
	LOG_FUNC
	CDbColSet* theColumns = CDbColSet::NewLC();

	theColumns->AddL(TDbCol(KColName_ProcessSID,			EDbColUint32));	//SID of process adding record
	theColumns->AddL(TDbCol(KColName_DeviceAddress,			EDbColText8));	// Binary not allowed in SQL
	theColumns->AddL(TDbCol(KDeviceColName_CoD_MajorDev,	EDbColUint32));
	theColumns->AddL(TDbCol(KDeviceColName_CoD_MinorDev,	EDbColUint32));
	theColumns->AddL(TDbCol(KDeviceColName_CoD_Service,		EDbColUint32));
	theColumns->AddL(TDbCol(KDeviceColName_BluetoothName,	EDbColText8, KMaxBluetoothNameLen));	// keep UTF8
	theColumns->AddL(TDbCol(KDeviceColName_FriendlyName,	EDbColText16, KMaxBluetoothNameLen));	// possible UNICODE?
	theColumns->AddL(TDbCol(KDeviceColName_LinkKey,			EDbColText8));	// Binary not allowed in SQL
	theColumns->AddL(TDbCol(KDeviceColName_PassKey,			EDbColText8));	// Binary not allowed in SQL	
	theColumns->AddL(TDbCol(KDeviceColName_PageScanMode,	EDbColUint8));
	theColumns->AddL(TDbCol(KDeviceColName_PageScanRepMode,	EDbColUint8));
	theColumns->AddL(TDbCol(KDeviceColName_PageScanPeriodMode,	EDbColUint8));
	theColumns->AddL(TDbCol(KDeviceColName_LastSeen,		EDbColInt64));
	theColumns->AddL(TDbCol(KDeviceColName_LastUsed,		EDbColInt64));
	theColumns->AddL(TDbCol(KDeviceColName_GlobalSecSecurity,	EDbColUint8));
	theColumns->AddL(TDbCol(KDeviceColName_GlobalSecPasskeyLen,	EDbColUint32));
	theColumns->AddL(TDbCol(KDeviceColName_LinkKeyType,		EDbColUint8));
	theColumns->AddL(TDbCol(KDeviceColName_UiCookie,		EDbColText8)); // Can only simulate "bit-masked" searching in SQL if stored as text

	User::LeaveIfError(iDB.CreateTable(KDeviceTable, *theColumns));

	CleanupStack::PopAndDestroy(theColumns);
	}

void CBTRegistry::CreateMetaTableL()
/**
	Stores useful information about the database
	Version - useful for attempting to see if we can restore an old db
	Originating process
**/
	{
	LOG_FUNC
	CDbColSet* theColumns = CDbColSet::NewLC();

	theColumns->AddL(TDbCol(KMetaColName_VersionMajor,	EDbColUint32));
	theColumns->AddL(TDbCol(KMetaColName_VersionMinor,	EDbColUint32));
	theColumns->AddL(TDbCol(KColName_ProcessSID,		EDbColUint32));	//SID of process adding record

	User::LeaveIfError(iDB.CreateTable(KMetaTable, *theColumns));

	CleanupStack::PopAndDestroy(theColumns);

	SetMetaDataL();
	}

void CBTRegistry::CreateCSYTableL()
/**
	Create the table with all that is necessary to store details about virtual serial ports
**/
	{
	LOG_FUNC
	CDbColSet* theColumns = CDbColSet::NewLC();

	theColumns->AddL(TDbCol(KColName_ProcessSID,		EDbColUint32));	//SID of process adding record
	theColumns->AddL(TDbCol(KBTCOMMColName_Port,		EDbColUint32));	
	theColumns->AddL(TDbCol(KColName_DeviceAddress,		EDbColText8));	//Device address
	theColumns->AddL(TDbCol(KBTCOMMColName_ServiceUUID,	EDbColText8));	//SDP UUID - 16bytes
	theColumns->AddL(TDbCol(KBTCOMMColName_Security,	EDbColUint8));	//Security
	theColumns->AddL(TDbCol(KBTCOMMColName_ServiceName,	EDbColText));	//ServiceName

	User::LeaveIfError(iDB.CreateTable(KCSYTable, *theColumns));

	CleanupStack::PopAndDestroy(theColumns);
	}


TDbColNo CBTRegistry::ColumnNoL(const TDesC& aColName, const RDbRowSet& aRowSet)
/**
Looks up column names and returns the column number

	@param	aColName the name of the column
	@param	aRowSet	the rowset in which to find the column
	@return the column number with aColName
**/
	{
	LOG_FUNC
	// get the schema from the table - we own this
#ifdef _DEBUG
	CDbColSet* schema = NULL;
	TRAPD(err, schema = aRowSet.ColSetL());
	if(err == KErrNoMemory)
		{
		User::Leave(err);
		}
	//This __ASSERT_DEBUG is here to catch any 'Leaves'
	//in RDbRowSet::ColSetL other than that\those using
	//KErrNoMemory.
	__ASSERT_DEBUG(err == KErrNone, Panic(EBTManColSetError));
#else
	CDbColSet* schema = aRowSet.ColSetL(); // no need to push onto cleanup
#endif
	// get the column number, based on name
	TDbColNo column = schema->ColNo(aColName);
	delete schema;
	return column;
	}



TBool CBTRegistry::DevicePresentL(const TBTDevAddr& aAddress)
/**
	Checks to see if a device is present in the Registry
	@param	aAddress	The key to finding a device
	@return True if Device is present, false if not
**/
	{
	LOG_FUNC
	// form an SQL query for the device with address aAddress
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindDeviceL(aAddress);

	TDbBookmark dummy;
	RDbView* view = NULL;

	// open the view with that SQL query
	view = OpenViewL(query, dummy);	// uses a private view...
	CleanupCloseDeletePushL(view);

	TInt count = view->CountL();
	
	__ASSERT_ALWAYS(count <= 1, Panic(ETooManyRemoteDeviceEntries));

	CleanupStack::PopAndDestroy(2, &query); // view and query
	return (count!=0);
	}

void CBTRegistry::CreateDeviceL(const CBTDevice& aDetails, TBool aStoreUiCookie, const TSecureId& aClientSID)
/**
	Add a new record into the DB

	@param	aDetails	The new device details to add
	@param	aClientUid	The ID of the process adding the new record
	@leave	KErrCorrupt if device to add is not valid
	@leave	KErrAlreadyExists if device to add is already present
**/
	{
	LOG_FUNC
	if (!aDetails.IsValidBDAddr())
		{
		User::Leave(KErrCorrupt);
		}

	if (DevicePresentL(aDetails.BDAddr()))
		{
		User::Leave(KErrAlreadyExists);
		}

	RDbTable* table = OpenDeviceTableL();
	CleanupCloseDeletePushL(table);

	table->InsertL();
	table->SetColL(ColumnNoL(KColName_DeviceAddress, *table), aDetails.BDAddr().Des());

	table->SetColL(ColumnNoL(KColName_ProcessSID, *table), aClientSID);

	// got some initial details; stick them in now...
	SetDeviceL(*table, aDetails, !aStoreUiCookie);	// does the remaining columns
	CleanupStack::PopAndDestroy(table);
	}

void CBTRegistry::UnpairL(RDbRowSet& aRowSet)
/**
	Unpairs a set of devices

	@pre	A rowset constraining the view to a single device
	@param	aRowSet	The set of records to unpair
	@see    DBMS for leave codes
**/
	{
	LOG_FUNC
	aRowSet.UpdateL();
	aRowSet.SetColNullL(ColumnNoL(KDeviceColName_LinkKey, aRowSet));
	aRowSet.SetColNullL(ColumnNoL(KDeviceColName_PassKey, aRowSet));
	aRowSet.SetColNullL(ColumnNoL(KDeviceColName_LinkKeyType, aRowSet));
	
	CleanPutL(aRowSet);
	}

void CBTRegistry::UnpairViewL(RDbRowSet& aRowSet)
/**
	Unpairs all sets of devices in the supplied rowset

	@pre	A rowset constraining the view 
	@param	aRowSet	The set of records to unpair
	@see    DBMS for leave codes
**/
	{
	LOG_FUNC
	aRowSet.FirstL();
	TInt c = aRowSet.CountL();
	for (TInt i=0; i<c; i++)
		{
		UnpairL(aRowSet);
		aRowSet.NextL();
		}
	}

void CBTRegistry::UpdateDeviceL(RDbRowSet& aRowSet, const CBTDevice& aDetails)
/**
	Given a rowset (from a previous find) and
	mark the rowset for update, and make the changes

	@param	aRowSet	The set of rows to update - typically one row...
**/
	{
	LOG_FUNC
	aRowSet.UpdateL();
	SetDeviceL(aRowSet, aDetails);
	}

void CBTRegistry::UpdateNameL(const TBTDevAddr& aAddress,
							  const TDesC8& aName,
							  TBTManServerRequest aRequest)
/**
	Update the name of a device - either friendly or Bluetooth name

	@param	aAddress    The device to update
	@param  aName	    The new name
	@param  aRequest	Determines the type of name to change

	Could change this so subsession opens the rowset and just gives that to this
**/
	{
	LOG_FUNC
	// Find the row
	RBTDbQuery deviceQuery;
	CleanupClosePushL(deviceQuery);
	deviceQuery.FindDeviceL(aAddress);

	TDbBookmark dummy;

	RDbView* view = OpenViewL(deviceQuery, dummy);
	CleanupCloseDeletePushL(view);

	view->UpdateL();

	switch (aRequest)
		{
	case EBTRegistryModifyFriendlyName:
		{
		TBuf<KMaxFriendlyNameLen> temp;
		User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(temp,aName));
		view->SetColL(ColumnNoL(KDeviceColName_FriendlyName, *view), temp);
		break;
		}

	case EBTRegistryModifyBluetoothName:
		{
		view->SetColL(ColumnNoL(KDeviceColName_BluetoothName, *view), aName);
		break;
		}
	default:
		Panic(EBadNameToUpdate);
		}
	CleanPutL(*view);
	CleanupStack::PopAndDestroy(2, &deviceQuery);	// view and deviceQuery
	}

void CBTRegistry::UpdateLocalDeviceL(const TBTLocalDevice& aLocalDevice)
/**
	@param	aLocalDevice The new settings
**/
	{
	LOG_FUNC
	// obliterate what's there
	RDbTable* table = OpenPersistTableL();
	CleanupCloseDeletePushL(table);	

	TInt count = table->CountL();
	__ASSERT_DEBUG(count <=1, Panic(ETooManyLocalDeviceEntries));
	if (!count)
		{
		// we need to insert the one and only row as this is a fresh table
		table->InsertL();
		}
	else
		{
		// go to the only row
		TBool success = table->FirstL();
		if (!success)
			{
			LOG(_L("BT Registry DB Local Device Table (Persist Table) CORRUPT!"));
			User::Leave(KErrEof);
			}

		table->UpdateL();
		}

	if (aLocalDevice.IsValidAddress())
		table->SetColL(ColumnNoL(KColName_DeviceAddress, *table),
					aLocalDevice.Address().Des());

	if (aLocalDevice.IsValidDeviceClass())
		table->SetColL(ColumnNoL(KPersistColName_CoD, *table),
					static_cast<TUint>(aLocalDevice.DeviceClass()));

	if (aLocalDevice.IsValidDeviceName())
		table->SetColL(ColumnNoL(KPersistColName_LocalName, *table),
					aLocalDevice.DeviceName());

	if (aLocalDevice.IsValidScanEnable())
		table->SetColL(ColumnNoL(KPersistColName_ScanEnable, *table),
					static_cast<TUint>(aLocalDevice.ScanEnable()));

	if (aLocalDevice.IsValidPowerSetting())
		table->SetColL(ColumnNoL(KPersistColName_PowerSettings, *table),
					static_cast<TUint>(aLocalDevice.PowerSetting()));

	if (aLocalDevice.IsValidAFHChannelAssessmentMode())
		table->SetColL(ColumnNoL(KPersistColName_AFHChannelAssessmentMode, *table),
					static_cast<TBool>(aLocalDevice.AFHChannelAssessmentMode()));

	if (aLocalDevice.IsValidLimitedDiscoverable())
		table->SetColL(ColumnNoL(KPersistColName_LimitedDiscoverable, *table),
					static_cast<TBool>(aLocalDevice.LimitedDiscoverable()));

	if (aLocalDevice.IsValidAcceptPairedOnlyMode())
		table->SetColL(ColumnNoL(KPersistColName_AcceptPairedOnlyMode, *table),
					static_cast<TBool>(aLocalDevice.AcceptPairedOnlyMode()));

	CleanPutL(*table);
	CleanupStack::PopAndDestroy(table);
	}

void CBTRegistry::SetMetaDataL()
/**
	Set the values for the MetaData Table
**/
	{
	LOG_FUNC
	// put defaults in
	RDbTable* table = OpenTableL(KMetaTable);
	CleanupCloseDeletePushL(table);
	
#ifdef _DEBUG
	TInt c=0;
	TRAP_IGNORE(c=table->CountL());
	__ASSERT_DEBUG(c==0, Panic(EMetaTableBroken));
#endif

	table->InsertL();

	table->SetColL(ColumnNoL(KColName_ProcessSID, *table), RProcess().SecureId());

	table->SetColL(ColumnNoL(KMetaColName_VersionMajor, *table), KRegistryDBVersionMajor);
	table->SetColL(ColumnNoL(KMetaColName_VersionMinor, *table), KRegistryDBVersionMinor);

	CleanPutL(*table);
	CleanupStack::PopAndDestroy(table);
	}

void CBTRegistry::SetDeviceL(RDbRowSet& aRowSet, const CBTDevice& aDetails, TBool aIgnoreUiCookie)
/**
	Set the values for the device record if the Details are valid

	@param	aRowSet		The rowset to adjust...typically one row
	@param 	aDetails	The new details
**/
	{
	LOG_FUNC
	if (aDetails.AsNamelessDevice().IsValidPageScanMode())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_PageScanMode, aRowSet), static_cast<TUint>(aDetails.AsNamelessDevice().PageScanMode()));
		}
	if (aDetails.AsNamelessDevice().IsValidPageScanRepMode())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_PageScanRepMode, aRowSet), static_cast<TUint>(aDetails.AsNamelessDevice().PageScanRepMode()));
		}
	if (aDetails.AsNamelessDevice().IsValidPageScanPeriodMode())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_PageScanPeriodMode, aRowSet), static_cast<TUint>(aDetails.AsNamelessDevice().PageScanPeriodMode()));
		}
	if (aDetails.IsValidDeviceClass())
		{
		const TBTDeviceClass& cod = aDetails.DeviceClass();
		
		aRowSet.SetColL(ColumnNoL(KDeviceColName_CoD_MajorDev, aRowSet), static_cast<TUint>(cod.MajorDeviceClass()));
		aRowSet.SetColL(ColumnNoL(KDeviceColName_CoD_MinorDev, aRowSet), static_cast<TUint>(cod.MinorDeviceClass()));
		aRowSet.SetColL(ColumnNoL(KDeviceColName_CoD_Service, aRowSet), static_cast<TUint>(cod.MajorServiceClass()));
		}
	if (aDetails.IsValidDeviceName())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_BluetoothName, aRowSet), aDetails.DeviceName());
		}
	if (aDetails.IsValidFriendlyName())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_FriendlyName, aRowSet), aDetails.FriendlyName());
		}
	if (aDetails.IsValidUsed())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_LastUsed, aRowSet), aDetails.Used().Int64());
		}
	if (aDetails.IsValidSeen())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_LastSeen, aRowSet), aDetails.Seen().Int64());
		}
	if (aDetails.IsValidGlobalSecurity())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_GlobalSecSecurity, aRowSet), static_cast<TUint>(aDetails.GlobalSecurity().SecurityValue()));
		aRowSet.SetColL(ColumnNoL(KDeviceColName_GlobalSecPasskeyLen, aRowSet), static_cast<TUint>(aDetails.GlobalSecurity().PasskeyMinLength()));
		}
	if (aDetails.IsValidLinkKey())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_LinkKey, aRowSet), aDetails.LinkKey());
		aRowSet.SetColL(ColumnNoL(KDeviceColName_LinkKeyType, aRowSet), aDetails.LinkKeyType());
		}
	else
		{
		LOG(_L("Sec\tNot setting link key"))
		}
	if (aDetails.IsValidPassKey())
		{
		aRowSet.SetColL(ColumnNoL(KDeviceColName_PassKey, aRowSet), aDetails.PassKey());
		}
	if (aDetails.IsValidUiCookie() && !aIgnoreUiCookie)
		{
		static const TUint8 KCookieBinaryRepresentationWidth = 32; // 32bits encoded as binary string.
		TBuf8<KCookieBinaryRepresentationWidth> textCookieValue;
		textCookieValue.NumFixedWidth(aDetails.UiCookie(), EBinary, KCookieBinaryRepresentationWidth);
		aRowSet.SetColL(ColumnNoL(KDeviceColName_UiCookie, aRowSet), textCookieValue);
		}
	
	CleanPutL(aRowSet);
	}

void CBTRegistry::CleanPutL(RDbRowSet& aRowSet)
/**
	Try to put the changes into a RowSet
	If this fails cancel the update and reset
	Then re-leave
**/
	{
	LOG_FUNC
	TRAPD(dbResult, aRowSet.PutL());

	if (dbResult!=KErrNone)
		{
		aRowSet.Cancel();
		aRowSet.Reset();
		User::Leave(KErrCorrupt); // this will complete the client message
		}
	}

RDbView* CBTRegistry::OpenDeviceL(const TBTDevAddr& aAddr, TDbBookmark& aBookmark)
/**
	Open a view onto a given record in the Registry
	@return a bookmark to the entry should the caller want it
**/
	{
	LOG_FUNC
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindDeviceL(aAddr);

	// open the device
	RDbView* view = NULL;
	view = OpenViewL(query, aBookmark);

	CleanupCloseDeletePushL(view);

#ifdef _DEBUG
	TInt c=0;
	TRAP_IGNORE(c=view->CountL());
	__ASSERT_DEBUG(c<=1, Panic(ETooManyRemoteDeviceEntries));
#endif

	CleanupStack::Pop(view); // view (pass ownership)
	CleanupStack::PopAndDestroy(&query); // query
	return view;
	}

RDbView* CBTRegistry::OpenViewL(const TDesC& aSQLQuery, TDbBookmark& aBookmark)
/**
	Returns a view resulting from the execution of the aSQLQuery
	Update aBookmark to be the beginning of the view
**/
	{
	LOG_FUNC
	RDbView* view = new (ELeave) RDbView;
	CleanupCloseDeletePushL(view);	

	// open the view based on the SQL query
	User::LeaveIfError(view->Prepare(iDB, aSQLQuery));
	// could separate this into separate Evaluates, but expected to be smallish DB
	User::LeaveIfError(view->EvaluateAll());

	// move to first row, and bookmark it
	view->FirstL();
	aBookmark = view->Bookmark();	// set bookmark in case user would like it

	CleanupStack::Pop(view);
	return view;	// returns ownership
	}

RDbView* CBTRegistry::OpenViewL(const RBTDbQuery& aQuery, TDbBookmark& aBookmark)
/**
	Overload allowing just a RBTDbQuery to be executed - hides the SQL query
**/
	{
	LOG_FUNC
	return OpenViewL(aQuery.QueryBuf(), aBookmark);
	}

void CBTRegistry::IncrementRowL(RDbRowSet& aRowSet, TDbBookmark& aBookmark)
/**
	Move to the next row in a RowSet and return a bookmark should the caller be interested
**/
	{
	LOG_FUNC
	aRowSet.NextL();	// move onto next row for next get
	aBookmark = aRowSet.Bookmark();
	}

void CBTRegistry::GetRowL(RDbRowSet& aRowSet, const TDbBookmark& aBookmark)
/**
	Mark the rowset for reading
**/
	{
	LOG_FUNC
	if (!aRowSet.AtEnd())
		{
		aRowSet.GotoL(aBookmark);
		aRowSet.GetL();		// get the row (which is a remote device on this table)
		}
	else
		{
		User::Leave(KErrEof);
		}
	}


void CBTRegistry::GetNamelessDetailsL(TBTNamelessDevice& aDevice, 
									  RDbRowSet& aRowSet, 
									  TBool includeKeys)
/**
	Gets the next device (row) out of a rowset and puts them in a CBTDevice
	The CBTDevice and its ownership is returned to the caller
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(!aRowSet.IsColNull(ColumnNoL(KColName_DeviceAddress, aRowSet)), Panic(ECorruptDeviceEntry));

	TPtrC8 addrBuf(aRowSet.ColDes8(ColumnNoL(KColName_DeviceAddress, aRowSet)));
	aDevice.SetAddress(static_cast<TBTDevAddr>(addrBuf));

	TDbColNo column;

	column = ColumnNoL(KDeviceColName_PageScanMode, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		aDevice.SetPageScanMode(aRowSet.ColUint8(column));
		}

	column = ColumnNoL(KDeviceColName_PageScanPeriodMode, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		aDevice.SetPageScanPeriodMode(aRowSet.ColUint8(column));
		}

	column = ColumnNoL(KDeviceColName_PageScanRepMode, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		aDevice.SetPageScanRepMode(aRowSet.ColUint8(column));
		}

/*	column = ColumnNoL(KDeviceColName_CoD, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		aDevice.SetDeviceClass(aRowSet.ColUint32(column));
		}
*/
	// all CoD columns must be set - just test one
	column = ColumnNoL(KDeviceColName_CoD_MajorDev, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		TUint majorDeviceClass = aRowSet.ColUint(column);

		column = ColumnNoL(KDeviceColName_CoD_MinorDev, aRowSet);
		TUint minorDeviceClass = aRowSet.ColUint(column);

		column = ColumnNoL(KDeviceColName_CoD_Service, aRowSet);
		TUint serviceClass = aRowSet.ColUint(column);
		
		aDevice.SetDeviceClass(TBTDeviceClass(static_cast<TUint16>(serviceClass),
											  static_cast<TUint8>(majorDeviceClass),
											  static_cast<TUint8>(minorDeviceClass)));
		
		}
	
	column = ColumnNoL(KDeviceColName_GlobalSecSecurity, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		TBTDeviceSecurity devSec;
		devSec.SetSecurityValue(aRowSet.ColUint8(column));
		column = ColumnNoL(KDeviceColName_GlobalSecPasskeyLen, aRowSet);
		devSec.SetPasskeyMinLength(aRowSet.ColUint32(column));
				
		aDevice.SetGlobalSecurity(devSec);
		}

	column = ColumnNoL(KDeviceColName_LinkKey, aRowSet);
	TDbColNo typeColumn = ColumnNoL(KDeviceColName_LinkKeyType, aRowSet);

	if (!aRowSet.IsColNull(column))
		{
		if(includeKeys)
			{
			TBTLinkKey linkKey;
			linkKey.Copy(aRowSet.ColDes8(column));
			aDevice.SetLinkKey(linkKey, static_cast<TBTLinkKeyType>(aRowSet.ColUint8(typeColumn)));
			}
		else
			{
			LOG(_L("Device paired but lack of capability to retrieve link key"));
			aDevice.SetPaired(static_cast<TBTLinkKeyType>(aRowSet.ColUint8(typeColumn)));
			}
		}
	else
		{
		LOG(_L("Sec\tDevice has NULL LinkKey column => Unpaired"))
		}

	column = ColumnNoL(KDeviceColName_PassKey, aRowSet);

	if (!aRowSet.IsColNull(column))
		{
		if(includeKeys)
			{
			TBTPinCode passKey;
			passKey.Copy(aRowSet.ColDes8(column));
			aDevice.SetPassKey(passKey);
			}
		else
			{
			LOG(_L("Device has PIN code but lack of capability to retrieve PIN code"));
			}
		}
	else
		{
		LOG(_L("Sec\tDevice has NULL PinCode column"))
		}
		
	column = ColumnNoL(KDeviceColName_LastSeen, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		aDevice.SetSeen(TTime(aRowSet.ColInt64(column)));
		}

	column = ColumnNoL(KDeviceColName_LastUsed, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		aDevice.SetUsed(TTime(aRowSet.ColInt64(column)));
		}
	
	column = ColumnNoL(KDeviceColName_UiCookie, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		TPtrC8 cookieBuf(aRowSet.ColDes8(column));
		TLex8 lexer(cookieBuf);
		TUint32 cookieValue = 0;
		TInt err = lexer.Val(cookieValue, EBinary);
		if(err != KErrNone)
			{
			// discard the specific lexer errors, the fundemental issue
			// is that the device entry is corrupted in someway.
			User::Leave(KErrCorrupt);
			}
		aDevice.SetUiCookie(cookieValue);
		}
	}

const TBTNamelessDevice* CBTRegistry::GetNextNamelessDeviceLC(RDbRowSet& aRowSet, 
															  TDbBookmark& aBookmark, 
															  TBool includeKeys)
/**
	Get the next row from the rowset; building and return TBTNamelessDevice via heap
**/
	{
	LOG_FUNC
	GetRowL(aRowSet, aBookmark);
	// there is a row to get, instantiate an object to receive details
	TBTNamelessDevice* device = new(ELeave) TBTNamelessDevice; // for passing ownership
	CleanupStack::PushL(device);
	GetNamelessDetailsL(*device, aRowSet, includeKeys);
	IncrementRowL(aRowSet, aBookmark);
	return device;
	}

CBTDevice* CBTRegistry::GetNextDeviceL(RDbRowSet& aRowSet, 
											 TDbBookmark& aBookmark, 
											 TBool includeLinkKey)
/**
	Get the next row from the rowset; building and return CBTDevice via heap
**/
	{
	LOG_FUNC
	GetRowL(aRowSet, aBookmark);
	// there is a row to get, so instantiate an object to receive details
	CBTDevice* device = CBTDevice::NewLC();
	GetNamelessDetailsL(device->AsNamelessDevice(), aRowSet, includeLinkKey);

	TDbColNo column = ColumnNoL(KDeviceColName_BluetoothName, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		device->SetDeviceNameL(aRowSet.ColDes8(column));
		}

	column = ColumnNoL(KDeviceColName_FriendlyName, aRowSet);
	if (!aRowSet.IsColNull(column))
		{
		TBuf<KMaxFriendlyNameLen> unicodeBuf;
		unicodeBuf.Copy(aRowSet.ColDes(column));
		device->SetFriendlyNameL(unicodeBuf);
		}
	IncrementRowL(aRowSet, aBookmark);
	CleanupStack::Pop(device);
	return device;
	}

const TUid CBTRegistry::CreatingProcessUidL(RDbRowSet& aRowSet)
/**
	@return	Value read from DB of the process that created a rowset
**/
	{
	LOG_FUNC
#ifdef _DEBUG
	TInt c=0;
	TRAP_IGNORE(c=aRowSet.CountL());
	__ASSERT_DEBUG(c==1, Panic(ETooManyRemoteDeviceEntries));
#endif

	aRowSet.GetL();	// this makes the semantics of this function a bit odd...it leaves the rowset open for read
	TInt val = aRowSet.ColUint32(ColumnNoL(KColName_ProcessSID, aRowSet));
	return TUid::Uid(val);
	}


void CBTRegistry::DeleteViewL(RDbRowSet& aRowSet, TBool aDeleteAll, const TSecureId& aSecureId)
/**
	Delete all the rows in the set whose originating Id is the same as 
	aSecureId
	
	@param aRowSet The rowset whose records are to be deleted
	@param aDeleteAll If set to ETrue, all rows are deleted. If EFalse, only those
	rows whose SID value is same as aSecureId will be deleted.
	@param aSecureId The SID of the calling process.
**/
	{
	LOG_FUNC
	aRowSet.FirstL();
	TInt c = aRowSet.CountL();
	
	if(	c == 0 )
		{
		//	No rows, nothing to do
		return;
		}
		
	TDbColNo secureIdColNo=ColumnNoL(KColName_ProcessSID, aRowSet);
	//	There should always be a ProcessSID column present in the view
	__ASSERT_ALWAYS(secureIdColNo != KDbNullColNo, Panic(EColumnNotFound));
	
	for (TInt i=0; i<c; i++)
		{
		//	Retrieve the current row ready to be accessed
		aRowSet.GetL();
		if( aDeleteAll || (aRowSet.ColUint32(secureIdColNo) == aSecureId) )
			{
			aRowSet.DeleteL();
			}
		//	Regardless of the DeleteL or no DeleteL, the cursor remains
		//	in the same location, so we always need to call NextL.
		aRowSet.NextL();
		}
	// better try to compact now to avoid ballooning
	(void)iDB.Compact();
	}


TBTLocalDevice* CBTRegistry::GetLocalDeviceLC()
/**
	Get the single row pertaining to the local device
	Build and return a TBTLocalDevice
**/
	{
	LOG_FUNC
	TBTLocalDevice* device = new(ELeave) TBTLocalDevice; // for passing ownership
	CleanupStack::PushL(device);

	RDbTable* table = OpenPersistTableL();
	CleanupCloseDeletePushL(table);

#ifdef _DEBUG
	TInt c=0;
	TRAP_IGNORE(c=table->CountL());
	__ASSERT_DEBUG(c==1, Panic(ENotOneLocalDeviceEntry));
#endif


	TBool success = table->FirstL();	// go to only row in table
	if (!success)
		{
		LOG(_L("BT Registry DB Local Device Table (Persist Table) CORRUPT!"));
		User::Leave(KErrEof);
		}

	table->GetL();		// mark for retrieval

	TPtrC8 addrBuf(table->ColDes8(ColumnNoL(KColName_DeviceAddress, *table)));
	device->SetAddress(static_cast<TBTDevAddr>(addrBuf));

	device->SetDeviceClass(table->ColUint32(ColumnNoL(KPersistColName_CoD, *table)));
	device->SetDeviceName(table->ColDes8(ColumnNoL(KPersistColName_LocalName, *table)));
	device->SetScanEnable(static_cast<THCIScanEnable>(table->ColUint8(ColumnNoL(KPersistColName_ScanEnable, *table))));
	device->SetPowerSetting(table->ColUint8(ColumnNoL(KPersistColName_PowerSettings, *table)));
	device->SetAFHChannelAssessmentMode(table->ColUint8(ColumnNoL(KPersistColName_AFHChannelAssessmentMode, *table)));
	device->SetLimitedDiscoverable(table->ColUint8(ColumnNoL(KPersistColName_LimitedDiscoverable, *table)));
	device->SetAcceptPairedOnlyMode(table->ColUint8(ColumnNoL(KPersistColName_AcceptPairedOnlyMode, *table)));
	CleanupStack::PopAndDestroy(table); //table
	return device;
	}

TBTLocalDevice* CBTRegistry::GetLocalDeviceL()
	{
	LOG_FUNC
	TBTLocalDevice* device = GetLocalDeviceLC();
	CleanupStack::Pop(device);
	return device;
	}



const TBTCommPortSettings* CBTRegistry::GetCommPortSettingsLC(const TBTCommPortSettings& aSettings)
/**
	Get the virtual serial port settings for the port referred to in aSettings
**/
	{
	LOG_FUNC
	TBTCommPortSettings* settings = new(ELeave) TBTCommPortSettings; // for passing ownership
	CleanupStack::PushL(settings);

	RDbView* view = OpenCommPortLC(aSettings);

	// mark for retrieval
	view->GetL();
	// get the unit number
	settings->SetPort(aSettings.Port());
	// get the device address
	TPtrC8 addrBuf(view->ColDes8(ColumnNoL(KColName_DeviceAddress, *view)));
	settings->SetBTAddr(static_cast<TBTDevAddr>(addrBuf));

	// get the uuid
	TUUID uuid;
	uuid.SetL(view->ColDes8(ColumnNoL(KBTCOMMColName_ServiceUUID, *view)));
	settings->SetUUID(uuid);

	// get the service uuid and name
	settings->SetSecurityLevel(view->ColUint8(ColumnNoL(KBTCOMMColName_Security, *view)));
	settings->SetName(view->ColDes(ColumnNoL(KBTCOMMColName_ServiceName, *view)));

	CleanupStack::PopAndDestroy(view); //closes view
	// don't pop settings - C function
	return settings;	
	}

void CBTRegistry::PutCommPortSettingsInTableL(RDbRowSet& aRowSet,
											  const TBTCommPortSettings& aSettings) 
/**
	Update the port settings
**/
	{
	LOG_FUNC
	aRowSet.SetColL(ColumnNoL(KBTCOMMColName_Port, aRowSet), aSettings.Port());
	aRowSet.SetColL(ColumnNoL(KColName_DeviceAddress, aRowSet), aSettings.BDAddr().Des());
	aRowSet.SetColL(ColumnNoL(KBTCOMMColName_ServiceUUID, aRowSet), aSettings.UUID().Des());
	aRowSet.SetColL(ColumnNoL(KBTCOMMColName_Security, aRowSet), static_cast<TUint>(aSettings.SecurityLevel()));
	aRowSet.SetColL(ColumnNoL(KBTCOMMColName_ServiceName, aRowSet), aSettings.Name());

	TRAPD(err,aRowSet.PutL());
	if (err!=KErrNone)
		{
		aRowSet.Cancel();
		aRowSet.Reset();
		User::Leave(err);
		}
	}

RDbView* CBTRegistry::OpenCommPortL(const TBTCommPortSettings& aSettings)
/**
	Return view of the com port specified
**/
	{
	LOG_FUNC
	// Form the SQL query based on the search criteria
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindCommPortL(aSettings.Port());	// just search on port for now

	// execute the SQL with dummy bookmark for now
	TDbBookmark dummy;
	RDbView* view = OpenViewL(query, dummy);
	CleanupCloseDeletePushL(view);

	TInt count = view->CountL();
	
	if (!count)
		{
		User::Leave(KErrNotFound); // nothing in there: which may be OK!
		}

	CleanupStack::Pop(view); // returns ownership - don't destroy
	CleanupStack::PopAndDestroy(&query); // query
	return view;
	}

RDbView* CBTRegistry::OpenCommPortLC(const TBTCommPortSettings& aSettings)
	{
	LOG_FUNC
	RDbView* view = OpenCommPortL(aSettings);
	CleanupCloseDeletePushL(view);
	return view;
	}

void CBTRegistry::DeleteCommPortSettingsL(RDbView& aCommPortSettingsView)
	{
	LOG_FUNC
	aCommPortSettingsView.DeleteL();
	}

void CBTRegistry::AddCommPortSettingsL(const TBTCommPortSettings& aSettings, 
									   const TSecureId& aClientSID)
/**
	Add port settings row
	Create row and enter values in aSettings.
	THIS must only be called if the table has been checked for existing settings
**/
	{
	LOG_FUNC
#ifdef _DEBUG
	// check it's not already there
	TRAPD(err, OpenCommPortL(aSettings));
	ASSERT_DEBUG(err==KErrNotFound);
#endif
	
	RDbTable* table = OpenTableL(KCSYTable);

	//create a record for this port, it wasn't there
	CleanupCloseDeletePushL(table);
	table->InsertL();

	table->SetColL(ColumnNoL(KColName_ProcessSID, *table), aClientSID);	// store originating client ID

	//put the port values in
	PutCommPortSettingsInTableL(*table, aSettings);
	CleanupStack::PopAndDestroy(table); //table
	}

void CBTRegistry::UpdateCommPortSettingsL(RDbView& aView,
										  const TBTCommPortSettings& aSettings) 
/**
	Update the port settings
**/
	{
	LOG_FUNC
	aView.UpdateL();	// mark for update
	PutCommPortSettingsInTableL(aView, aSettings);
	}

void CBTRegistry::DeleteCorruptRegistryL()
	{
	LOG_FUNC
	// The registry is corrupt so we need to create a new one and notify 
	// interested parties that all of the user's pairings may have been lost.

	
	// close the database (if open)
	iDB.Close();
	TInt err = KErrNone;

	// The RDbs method must be used to access the private data cage.
	err = iDBMSServer.DeleteDatabase(TBTRegistryDatabaseSecure()(), KBTManServerUid);  
	if(err)
		{
		LOG1(_L("UNSUCCESSFUL attempt to delete Corrupt BT Registry DB, error %d"), err);
		}
	else
		{
		LOG(_L("Corrupt BT Registry DB Deleted successfully"));
		// Use P&S to let UIs know that the registry may have lost some 
		// of the user's pairings.			
		RProperty::Set(KPropertyUidBluetoothCategory,
					   KPropertyKeyBluetoothCorruptRegistryReset,
					   User::TickCount());	   			   	
		}
	}

void CBTRegistry::SetupDefaultRegistryL()
	{
	LOG_FUNC
	CreateMetaTableL();
	CreatePersistTableL();
	CreateCSYTableL();
	CreateDeviceTableL();

	// now add default values for the tables: set all the states to 'unset'
	TBTLocalDevice defaultDevice;
	
	
	// Set up the defaults - these will get overwritten if there are appropriate values in the ini file
	defaultDevice.SetAddress(TBTDevAddr(0));
	defaultDevice.SetDeviceName(KDefaultLocalName);
	defaultDevice.SetScanEnable(EPageScanOnly);
	defaultDevice.SetLimitedDiscoverable(EFalse);
	
	// set the default device class to be phone|smartphone
	// MajorServiceClass set to zero as there are no default service class bits
	TBTDeviceClass defaultCod (0, EMajorDevicePhone, EMinorDevicePhoneSmartPhone);
	defaultDevice.SetDeviceClass(defaultCod.DeviceClass());
	
	// The registry is being kicked off with a default channel assessment 
	// mode setting of 'enabled'. This is the default if h/w supports 
	// channel assessment. If h/w does not support channel assessment,
	// this value becomes meaningless.
	// Any use of it to update h/w will be rejected.
	defaultDevice.SetAFHChannelAssessmentMode(ETrue);
	defaultDevice.SetAcceptPairedOnlyMode(EFalse);
	
	// Try and get any relevant parameters from the ini file
	TRAP_IGNORE(GetDefaultDeviceFromIniL(defaultDevice)); // Ignore any leaves as they cannot result in corruption of defaultDevice

	UpdateLocalDeviceL(defaultDevice);
//#pragma message("defaults for persist table ini file/licensee?")
	}

//

TInt CBTRegistry::SetDeviceName(const TDesC& aDeviceName, TBTLocalDevice& aDevice)
	{
	// GetDefaultDeviceName() returns 16bit unicode, it need to be converted to 8bit UTF
	TBTDeviceName8 tempBuf;
	TInt error = CnvUtfConverter::ConvertFromUnicodeToUtf8(tempBuf, aDeviceName);
	if(error == KErrNone)
		{
		aDevice.SetDeviceName(tempBuf);
		}
	return error;
	}

TBool CBTRegistry::SetDeviceNameFromIniFile(TBTLocalDevice& aDevice, RBuf8& bufPtr)
	{
	// Default so try the old mechanism.
	_LIT8(KVarDeviceName, "DeviceName");
	TPtrC8 devName(NULL, 0);
	if (FindVar(bufPtr, KVarDeviceName, devName) && devName.Length() <= KMaxBCBluetoothNameLen)
		{
		aDevice.SetDeviceName(devName);
		return ETrue;
		}
	return EFalse;
	}

void CBTRegistry::GetDefaultDeviceFromIniL(TBTLocalDevice& aDevice)
	{
	LOG_FUNC
	// Try and open the .ini file and get any relavent parameters
	RFs	fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	
	_LIT(KIniDrive, "Z:");
	_LIT(KIniName, "DefaultBtReg.ini");
	TFileName	iniFileName;
	fileSession.PrivatePath(iniFileName);
	iniFileName.Insert(0, KIniDrive);
	iniFileName.Append(KIniName);
	
	RFile file;
	User::LeaveIfError(file.Open(fileSession, iniFileName, EFileStreamText|EFileRead));
	CleanupClosePushL(file);
	TInt data_size = 0;
	User::LeaveIfError(file.Size(data_size));

	// All text encoding is narrow 8-bit ASCII as ini files generated on a PC will be encoded that way
	// and the bluetooth specification only allows 8-bit ASCII strings so there is no need to convert
	// between ASCII and Unicode and back again.
	RBuf8 bufPtr;
	bufPtr.CreateL(data_size);
	CleanupClosePushL(bufPtr);
	User::LeaveIfError(file.Read(bufPtr));
	
	// Any or all of the following variables may be specified in this ini file:
	//
	// Variable:								Variable Name:			Values Taken:
	//	Default device name						DeviceName				String
	//	Inquiry scan enabled by default			InquiryScan				0 or 1
	//	Page scanning enabled by default		PageScan				0 or 1
	//	Limited discovery enabled by default	LimitedDiscovery		0 or 1
	//	Default device class					DeviceClass				Integer (decimal format)
	//	Allow paired only eanabled by default	AcceptPairedOnlyMode	0 or 1
	//
	// Notes on file syntax:
	// - Variable Names are followed by an ‘=’ character and then the value taken.
	// - There are no whitespaces in variable names.
	// - Variable names may be followed by any number of whitespaces, until the first character of the value taken. Thereafter any whitespaces are considered part of the value until the end of line.
	// - All characters should be narrow (standard 8bit ASCII).
	// - Integers should contain only the standard characters 0 to 9.
	// - Boolean values should be indicated by a 0 or 1.
	// - Strings may contain whitespaces, but any at the beginning will be stripped.
	// - There is no need to terminate the file with a new line.
	// - Anything after "//" on a line is ignored as comments
	

	// Remove any comments from the buffer
	TInt slashPos;
	TInt commentLength;
	TPtr8 comment(NULL, 0);
	_LIT8(KDoubleSlash, "//");
	while ((slashPos = bufPtr.Find(KDoubleSlash)) != KErrNotFound)
		{
		_LIT8(KEol, "\n");
		comment.Set(bufPtr.MidTPtr(slashPos));
		if ((commentLength = comment.Find(KEol)) == KErrNotFound)
			{
			commentLength = comment.Length(); // just to end of file
			}
		bufPtr.Replace(slashPos, commentLength, KNullDesC8);
		}

	// Get the default device name
	// we try to get the device name from deviceattributes.ini as a first resort,
	// deviceattributes.ini is global device configuration for setting global data like the device name

	CDeviceTypeInformation* deviceInfo = NULL;
	TRAPD(ret, deviceInfo = SysUtil::GetDeviceTypeInfoL());
	CleanupStack::PushL(deviceInfo);
	TPtrC16 deviceName;

	if (ret == KErrNone)
		{
		ret = deviceInfo->GetDefaultDeviceName(deviceName);
		if (ret == KErrNone)
			{
			LEAVEIFERRORL(SetDeviceName(deviceName, aDevice));
			}
		else if (ret == CDeviceTypeInformation::KDefaultValue)
			{
			if(!SetDeviceNameFromIniFile(aDevice, bufPtr))
				{
				ret = SetDeviceName(deviceName, aDevice);
				__ASSERT_DEBUG(ret == KErrNone,Panic(ECorruptDeviceEntry));
				if(ret != KErrNone)
					{
					SetDeviceNameFromIniFile(aDevice, bufPtr);
					}
				}
			}
		else
			{
			SetDeviceNameFromIniFile(aDevice, bufPtr);
			}
		}
	else
		{ // if the above did not work, then use the old mechanism
		SetDeviceNameFromIniFile(aDevice, bufPtr);
		}

	// Get default scan enabled status
	TInt scanStatus = aDevice.ScanEnable();
	TInt result = 0;
	_LIT8(KVarInquiryScan, "InquiryScan");
	if (FindVar(bufPtr, KVarInquiryScan, result))
		{
		if (result)
			scanStatus |= EInquiryScanOnly;
		else
			scanStatus &= ~EInquiryScanOnly;  		
		}
	
	_LIT8(KVarPageScan, "PageScan");
	if (FindVar(bufPtr, KVarPageScan, result))
		{
		if (result)
			scanStatus |= EPageScanOnly;
		else
			scanStatus &= ~EPageScanOnly;
		}
	aDevice.SetScanEnable(static_cast<THCIScanEnable>(scanStatus));
	
	
	// Get default limited discovery status
	_LIT8(KVarLimitedDiscovery, "LimitedDiscovery");
	if (FindVar(bufPtr, KVarLimitedDiscovery, result))
		{
		aDevice.SetLimitedDiscoverable(result);	
		}	
	
	
	// Get default device class
	_LIT8(KVarDeviceClass, "DeviceClass");
	if (FindVar(bufPtr, KVarDeviceClass, result))
		{
		aDevice.SetDeviceClass(result);	
		}	
			
	
	// Get default AFH channel assessment only mode
	_LIT8(KAFHChannelAssessmentMode, "AFHChannelAssessmentMode");
	if (FindVar(bufPtr, KAFHChannelAssessmentMode, result))
		{
		aDevice.SetAFHChannelAssessmentMode(result);
		}
			
	
	// Get default accept paired only mode
	_LIT8(KAcceptPairedOnlyMode, "AcceptPairedOnlyMode");
	if (FindVar(bufPtr, KAcceptPairedOnlyMode, result))
		{
		aDevice.SetAcceptPairedOnlyMode(result);
		}


	CleanupStack::PopAndDestroy(4, &fileSession); // deviceinfo, bufPtr, file, fileSession
	}

TBool CBTRegistry::FindVar(const TDesC8& aPtr, const TDesC8 &aVarName, TPtrC8 &aResult) const
	{
	LOG_FUNC
	// Find the variable name
	TInt start=aPtr.Find(aVarName);
	if (start == KErrNotFound)
		{
		return EFalse;
		}
	
	// Now find the end of the value string (end of line or end of file)
	TPtrC8 value(aPtr.Mid(start));
	_LIT8(KEol, "\n");
	TInt length = value.Find(KEol);
	if (length != KErrNotFound)
		{
		if (value[length-1] == '\r')
			{
			length--;
			}
		}
	else
		{
		length = value.Length();
		}
		
	// Now find the start of the value to be parsed
	TLex8 lex(aPtr.Mid(start, length));
	TChar next = lex.Get();
	while (next != '=' && next != NULL)	
		{		
		next = lex.Get(); // Step through until we find a '=' character or EOS
		}
	
	if (next == NULL)
		{
		return EFalse;	// End of string was found before '=' so there's no valid value.
		}
		
	lex.SkipSpace();	// Will now be at the start of the value
	
	// Now copy the variable value into the result
	aResult.Set(lex.Remainder());
	
	return ETrue;
	}


TBool CBTRegistry::FindVar(const TDesC8& aPtr, const TDesC8 &aVarName, TInt &aResult) const
	{
	LOG_FUNC
	TPtrC8 ptr(NULL,0);
	
	if (FindVar(aPtr, aVarName,ptr))
		{
		TLex8 lex(ptr);
		if (lex.Val(aResult) == KErrNone)
			{
			return(ETrue);			
			}
		}
		
	return(EFalse);
	}


TBTRegistryDatabaseSecure::TBTRegistryDatabaseSecure()
	{
	LOG_FUNC
	iBuf.Append(RFs::GetSystemDrive() + 'a'); //Gets the system drive and convert it from enum to a char
	iBuf.Append(':');
	iBuf.Append(KBTRegistryDatabaseSecure);		
	}
		
const TDesC& TBTRegistryDatabaseSecure::operator()() const
	{
	LOG_FUNC
	return iBuf;
	}

