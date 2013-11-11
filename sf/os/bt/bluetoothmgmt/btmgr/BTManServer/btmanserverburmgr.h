// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements backup and restore support for the Bluetooth Manager server.
//

/**
 @file
 @internalAll
*/

#ifndef BTMANSERVERBURMGR_H
#define BTMANSERVERBURMGR_H

#include <connect/abclient.h>

#include "BTRegistryDB.h"

#include "btmanserverburstatemachine.h"

using namespace conn;

_LIT(KBTBackupPanicCat, "BTManServer BUR Panic");
// Used to allow state information to be encoded into a panic code.
const TInt KBTBackupStatePanicMultiplier = 1000000;
const TInt KBTBackupStateMultiplier = 1000;
// Used to seperate the panic and state / event values in a panic code.
const TInt KBTBackupStatePanicMajorDelimiter = 100000;
const TInt KBTBackupStatePanicMinorDelimiter = 100;

/**
TBTBackupPanic - Panics that may be raised by the BUR manager
**/
enum TBTBackupPanic
	{
	EBTBURMgrAlreadyInOperation			= 0,	// A backup/restore event has been received during an ongoing BUR operation
	EBTBURMgrExpectedDataMissing			= 1,	// An operation has found that it is missing essential data
	EBTBURMgrBUROperationStartAlreadyNotified	= 2,	// An MBTNotify instance has already been told that a BUR operation has started (and has not received a corresponding BUROperationStop notification)
	EBTBURMgrBUROperationStartNotNotified		= 3,	// An MBTNotify instance has been notified that a BUR operation has stopped, without receiving notification that it had started.

	EBTBURMgrNoSessionAdded				= 4,	// No session has been registered on BTManServer for the BUR manager
	EBTBURMgrInvalidRemoteDeviceIndex		= 5,	// An index for a remote device is invalid.
	EBTBURMgrInvalidStateTransition			= 6,	// An attempted state transition was invalid.
	EBTBURMgrInvalidState				= 7,	// Either the requested state was invalid or the current state is not valid for some operation.
	EBTBURMgrInvalidStateIndex			= 8,	// A given state index is invalid.
	EBTBURMgrMissingLocalAddress			= 9,	// A nonzero local address was expected but not found
	EBTBURMgrUnexpectedMethodCall			= 10,	// A method was unexpectedly called
	EBTBURMgrArraySizeMisMatch			= 11,	// An array does not have the expected size
	EBTBURMgrUnknownBUREvent			= 12,	// An unrecognised backup or restore event was received.
	EBTBURMgrActiveBackupClientNull			= 13,	// A pointer to a CActiveBackupClient instance is unexpectedly NULL
	EBTBURMgrUnsupportedRegistryVersion		= 14,	// A new version of the registry is being used locally, which is not yet supported by this implementation.
	EBTBURMgrUnexpectedRPropertyError		= 15,	// An RProperty::Get() call returned an error that was not expected.
	EBTBURMgrInvalidBackupRestoreState		= 16,	// The masked value from the KUidBackupRestoreKey P&S key was not in the range of the TBURPartType enum.
	EBTBURMgrDescriptorLengthTooLong		= 17,	// A descriptor length is greater than expected.
	EBTBURMgrDescriptorUnexpectedLength		= 18,	// A descriptor length is different to what was expected.
	};

// The name of the backup file to which the BT registry will be copied to.
_LIT(KBTManServerBackupFileName, "btregistrydb.bak");
// The name of the restore file from which the BT registry will be restored from.
_LIT(KBTManServerRestoreFileName, "btregistrydb.rst");

// Some data range maximum values (where predefined const values are not available elsewhere)
const TUint8 KBTBURMgrMaxGlobalSecurityValue	= 0x1F;		// All security options on in TBTDeviceSecurity
const TUint8 KBTBURMgrMaxPageScanMode		= 0x3;		// Optional page scan mode III
const TUint8 KBTBURMgrMaxPageScanPeriodMode	= 0x2;		// P2

void InvalidStateTransitionPanic(TBTBURState aCurrentState, TBTBUREvent aEvent);


class MBTBURNotify
	{
public:
	/**
	Invoked by CBTManServerBURMgr to provide notification that a backup or restore
	operation has started.
	**/
	virtual void BUROperationStarted()=0;

	/**
	Invoked by CBTManServerBURMgr to provide notification that a backup or restore
	operation has stopped.
	**/
	virtual void BUROperationStopped()=0;

	};

class CBTManServer;
class CBTLocalAddressFetcher;
class CBTBackupHandler;
class CBTRestoreHandler;
class CBTActiveBackupDataClient;

NONSHARABLE_CLASS(CBTManServerBURMgr) : public CActive
	{
public:
	static CBTManServerBURMgr* NewL(CBTManServer& aBTManServer, MBTBURNotify* aBURNotify, TInt aPriority=EPriorityStandard);
	~CBTManServerBURMgr();

	void RequestBURNotification(MBTBURNotify* aBURNotify);
	void SetLocalAddress(TBTDevAddr& aLocalAddr);
	void RestoreFileReady();

	// Implementation of normal state action
	void HandleStateNormal();

	// Implementation of backup state-specific actions.
	void HandleStateBackupRequestL();
	void HandleStateBackupRequestError();
	void HandleStateBackupOngoingL();
	void HandleStateBackupOngoingError();
	void HandleStateBackupIgnore();
	
	// Implementation of restore state-specific actions.
	void HandleStateRestoreRequestL();
	void HandleStateRestoreRequestError();
	void HandleStateRestoreOngoingL();
	void HandleStateRestoreOngoingError();
	void HandleStateRestoreIgnore();

	// Implementation of restore file processing state-specific actions.
	void HandleStateProcessRestoreFileL();
	void HandleStateProcessRestoreFileError(TInt aError);
	void HandleStateRestoreFileProcessingComplete();

	inline TInt TransitionState(TBTBUREvent aBUREvent);

private:
	CBTManServerBURMgr(CBTManServer& aBTManServer, MBTBURNotify* aBURNotify, TInt aPriority);
	void ConstructL();

	TBURPartType GetBURPartType(TInt aValue);

	TBool RestoreFilePresent();
	void CheckForRestoreFileL();

	void SubscribeToBackupRestoreKey();
	void ProcessBackupState(TBURPartType aBURStateValue);	
	TBool DoesBURAffectMeL(CActiveBackupClient& aClient);

	void NotifyBUROperationStarted();
	void NotifyBUROperationStopped();
	void NotifyAnyBUROperationStopped();

	void DeleteFile(const TDesC& fileName);
	void DeleteBackupFile();
	void DeleteRestoreFile();

	void RenameBackupFileForRestore();

	// From CActive:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	CBTManServer& iBTManServer;
	MBTBURNotify* iBURNotify;	// Does not own this object.

	TBURPartType iBURState;

	CActiveBackupClient* iActiveBackupClient;
	RProperty iProperty;
	CBTLocalAddressFetcher* iLocalAddrFetcher;

	CBTManServerBURMgrStateFactory* iStateMachine;

	CBTActiveBackupDataClient* iActiveBackupDataClient;
	
	TBool iBUROperationStartNotified;

	CBTBackupHandler* iBackupHandler;
	CBTRestoreHandler* iRestoreHandler;

	TBTDevAddr iLocalAddr;

	};

class CBTRegistryBURData;

NONSHARABLE_CLASS(CBTBackupHandler) : public CBase
	{
public:
	static CBTBackupHandler* NewL(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry);
	~CBTBackupHandler();

	void CreateBackupFileL();

private:
	CBTBackupHandler(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry);
	void ConstructL();

private:
	CBTManServerBURMgr& iBURMgr;
	CBTRegistry& iRegistry;
	CBTRegistryBURData* iRegistryData;
	};

NONSHARABLE_CLASS(CBTRestoreHandler) : public CBase
	{ 
public:
	static CBTRestoreHandler* NewL(CBTManServerBURMgr& aBURMgr, CBTManServer& aManServer);
	~CBTRestoreHandler();

	void RestoreRegistryL(TBTDevAddr& aLocalAddr);

private:
	CBTRestoreHandler(CBTManServerBURMgr& aBURMgr, CBTManServer& aManServer);
	void ConstructL();

	void LoadRestoreDataL();

	void NotifyLocalTableChange();
	void NotifyLocalDeviceNameChange(const TDesC8& aLocalName);
	void NotifyLocalDeviceNameChange(const TDesC16& aLocalName);
	void NotifyRemoteTableChangeL(const TBTDevAddr& aAddress);

private:
	CBTManServerBURMgr& iBURMgr;
	CBTManServer& iManServer;
	CBTRegistryBURData* iRegistryData;

	TBTDevAddr iBTLocalAddr;
	};


NONSHARABLE_CLASS(CBTRegistryBURData) : public CBase
	{
public:		
	static CBTRegistryBURData* NewL();
	~CBTRegistryBURData();

	void WriteToStreamL(RWriteStream& aStream) const;
	void ReadFromStreamL(RReadStream& aStream);

	void ReadFromRegistryL(CBTRegistry& aRegistry);
	TBool WriteLocalDeviceNameToRegistryL(CBTRegistry& aRegistry) const;
	TBool WriteRemoteDeviceToRegistryL(CBTRegistry& aRegistry, TInt aDeviceIndex) const;

	inline TBool HasRegistryData() const;

	void GetRegistryVersionL(TUint32& aRegistryVersionMajor, TUint32& aRegistryVersionMinor) const;
	const TDesC8& GetLocalDeviceNameL() const;
	TBool IsLocalAddressNonZeroL() const;
	TBool IsLocalAddressEqualL(TBTDevAddr& aAddr) const;
	TInt CountRemoteDevicesL() const;
	const CBTDevice& GetRemoteDeviceL(TInt aDeviceIndex) const;
	TSecureId GetRemoteDeviceEntrySidL(TInt aDeviceIndex) const;
	
	void ClearRegistryData();

private:
	CBTRegistryBURData();
	void ConstructL();

private:
	TBool iHasRegistryData;

	// Meta table:
	TUint32 iRegistryVersionMajor;
	TUint32 iRegistryVersionMinor;

	// Persistence table
	TBTLocalDevice* iLocalDevice;

	// Remote device table:
	CArrayFixFlat<TSecureId>* iRemoteDevicesSid;
	CBTDeviceArray* iRemoteDevices;

	};

NONSHARABLE_CLASS(CBTLocalAddressFetcher) : public CActive
	{
public:
	// High priority to ensure this AO fires before a client request to BTManServer
	static CBTLocalAddressFetcher* NewL(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry, TInt aPriority=EPriorityHigh);
	~CBTLocalAddressFetcher();

	void FetchLocalAddress();


private:
	CBTLocalAddressFetcher(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry, TInt aPriority);
	void ConstructL();
	
	void SubscribeToGetLocalDeviceAddressKey();
	TBTDevAddr GetLocalAddressFromRegistryL();

	// From CActive
	void RunL();
	void DoCancel();
private:
	CBTManServerBURMgr& iBURMgr;
	CBTRegistry& iRegistry;
	RProperty iProperty;

	};

NONSHARABLE_CLASS(CBTActiveBackupDataClient) : public CBase, public MActiveBackupDataClient
	{
public:
	static CBTActiveBackupDataClient* NewL(CBTManServerBURMgr& aBURMgr);
	~CBTActiveBackupDataClient();

	// Backup methods (not used)
	virtual void AllSnapshotsSuppliedL();
	virtual void ReceiveSnapshotDataL(TDriveNumber aDrive, TDesC8& aBuffer, TBool aLastSection);
	virtual TUint GetExpectedDataSize(TDriveNumber aDrive);
	virtual void GetSnapshotDataL(TDriveNumber aDrive, TPtr8& aBuffer, TBool& aFinished);
	virtual void InitialiseGetBackupDataL(TDriveNumber aDrive);
	virtual void GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished);

	// Restore methods (only the notification method RestoreComplete() is used)
	virtual void InitialiseRestoreBaseDataL(TDriveNumber aDrive);
	virtual void RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished);
	virtual void InitialiseRestoreIncrementDataL(TDriveNumber aDrive);
	virtual void RestoreIncrementDataSectionL(TDesC8& aBuffer, TBool aFinished);
	virtual void RestoreComplete(TDriveNumber aDrive);

	// General methods
	virtual void TerminateMultiStageOperation();

	// Test methods
	virtual TUint GetDataChecksum(TDriveNumber aDrive);
	
private:
	CBTActiveBackupDataClient(CBTManServerBURMgr& aBURMgr);

private:
	CBTManServerBURMgr& iBURMgr;

	};

#include "btmanserverburmgr.inl"

#endif //BTMANSERVERBURMGR_H
