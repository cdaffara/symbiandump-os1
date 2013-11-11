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

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>
#include <f32file.h>
#include <s32file.h>
#include <bluetooth/hci/hciconsts.h>
#include "btmanserverutil.h"
#include "btmanserverburmgr.h"
#include "BTManServer.h"

#include "btmanclient.h"
#include "btdevice.h"
#include <bluetooth/hci/hcitypes.h>
#include <bluetooth/logger.h>
#include "BTSec.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif


/**
Raises an EBTBURMgrInvalidState panic with state information encoded within the panic code.
The first digit denotes the panic code (EBTBURMgrInvalidState).
The second digit is a delimiter and is always 1.
The next two digits represent the expected state
The next digit again is a delimiter and is always 1.
The final two digits represent the actual state. 

For example, the panic code 7101100 indicates an EBTBURMgrInvalidState panic raised because the state
machine is in the EBTBURStateNormal state when it was expected to be in the EBTBURStateBackupRequest state.
**/
void InvalidStatePanic(TBTBURState aExpectedState, TBTBURState aActualState)
	{
	LOG_STATIC_FUNC
	User::Panic(KBTBackupPanicCat, (EBTBURMgrInvalidState*KBTBackupStatePanicMultiplier) + KBTBackupStatePanicMajorDelimiter + (aExpectedState * KBTBackupStateMultiplier) + KBTBackupStatePanicMinorDelimiter + aActualState);
	}

/**
Raises an EBTBURMgrInvalidStateTransition panic with state information encoded within the panic code.
The first digit denotes the panic code (EBTBURMgrInvalidStateTransition).
The second digit is a delimiter and is always 1.
The next two digits represent the expected state
The next digit again is a delimiter and is always 1.
The final two digits represent the actual state. 

For example, the panic code 6101100 indicates an EBTBURMgrInvalidStateTransition panic raised because the state
machine is in the EBTBURStateNormal state when it was expected to be in the EBTBURStateBackupRequest state.
**/
void InvalidStateTransitionPanic(TBTBURState aCurrentState, TBTBUREvent aEvent)
	{
	LOG_STATIC_FUNC
	User::Panic(KBTBackupPanicCat, (EBTBURMgrInvalidStateTransition*KBTBackupStatePanicMultiplier) + KBTBackupStatePanicMajorDelimiter + (aCurrentState * KBTBackupStateMultiplier) + KBTBackupStatePanicMinorDelimiter + aEvent);
	}


/**
CBTManServerBURMgr - Manages backup and restore support for the Bluetooth Manager server.
**/

/**
Constructs a new instance of a CBTManServerBURMgr.
An MBTBURNotify instance may be registered here for notifications. Ownership of the MBTBURNotify instance is not transferred.
@see CBTManServerBURMgr::RequestBURNotification()
**/
CBTManServerBURMgr* CBTManServerBURMgr::NewL(CBTManServer& aBTManServer, MBTBURNotify* aBURNotify, TInt aPriority)
	{
	LOG_STATIC_FUNC
	CBTManServerBURMgr* self = new(ELeave) CBTManServerBURMgr(aBTManServer, aBURNotify, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTManServerBURMgr::CBTManServerBURMgr(CBTManServer& aBTManServer, MBTBURNotify* aBURNotify, TInt aPriority) 
	: CActive(aPriority), 
	iBTManServer(aBTManServer),  
	iBUROperationStartNotified(EFalse)
	{
	LOG_FUNC

	RequestBURNotification(aBURNotify);
	CActiveScheduler::Add(this);
	}

CBTManServerBURMgr::~CBTManServerBURMgr()
	{
	LOG_FUNC

	Cancel();
	iProperty.Close();

	// Close any open session on BT Manager
	NotifyAnyBUROperationStopped();
		
	// Members which either hold data or are NULL
	delete iActiveBackupClient;
	delete iActiveBackupDataClient;
	delete iBackupHandler;
	delete iRestoreHandler;
	delete iLocalAddrFetcher;

	// Member that is instantiated for entire lifetime of CBTManServerBURMgr
	delete iStateMachine;
	}

void CBTManServerBURMgr::ConstructL()
	{
	LOG_FUNC

	// Construct state machine
	iStateMachine = CBTManServerBURMgrStateFactory::NewL(*this);
	// Construction counts as a transition into the default state (EBTBURStateNormal) of the state machine.
	// However, since this does nothing if no backup/restore cleanup is required, we do not need to execute the 
	// state action.
	
	// Setup iProperty to catch BUR flag from Secure Backup Engine
	LEAVEIFERRORL(iProperty.Attach(KUidSystemCategory, KUidBackupRestoreKey));

	// Subscribe to BUR P&S key to catch transitions
	SubscribeToBackupRestoreKey();

	// Also check flag now (it may have already transitioned)
	TInt backupKeyValue = 0;
	LEAVEIFERRORL(iProperty.Get(backupKeyValue));
	TBURPartType backupState = GetBURPartType(backupKeyValue);	

	// If we're not about to perform a restore, check if a restore file exists. If so, try to process restore file.
	// The restore operation may not affect us, or may be postponed. In either case, we try to process the restore file
	// again next time we start.
	if (backupState != EBURRestoreFull && backupState != EBURRestorePartial)
		{
		// Delete any stale backup files
		DeleteBackupFile();
		// See if a restore file needs processing
		CheckForRestoreFileL();
		}

	// Process flag
	ProcessBackupState(backupState);
	}

/**
Translates a given integer obtained from the KUidBackupRestoreKey P&S key to a TBURPartType value.
This will validate the masked value to ensure that it falls within the range of the TBURPartType enum.
In debug builds, a panic will be raised if this value is out of range.
In release builds, EBURUnset is returned if the value is out of range.
**/
TBURPartType CBTManServerBURMgr::GetBURPartType(TInt aValue)
	{
	LOG_FUNC

	switch (aValue & KBURPartTypeMask)
		{
	case EBURUnset:
		return EBURUnset;
	case EBURNormal:
		return EBURNormal;
	case EBURBackupFull:
		return EBURBackupFull;
	case EBURBackupPartial:
		return EBURBackupPartial;
	case EBURRestoreFull:
		return EBURRestoreFull;
	case EBURRestorePartial:
		return EBURRestorePartial;
	default:
		__ASSERT_DEBUG(EFalse, PANIC(KBTBackupPanicCat, EBTBURMgrInvalidBackupRestoreState));
		return EBURUnset;
		}
	}

/**
Attempts to locate a restore file in the private directory of this server.
@return ETrue if the file can be found, EFalse otherwise.
**/
TBool CBTManServerBURMgr::RestoreFilePresent()
	{
	LOG_FUNC

	RFs fsSession;
	TInt err = fsSession.Connect();
	if (err == KErrNone)
		{
		err = fsSession.SetSessionToPrivate(fsSession.GetSystemDrive());
		if (err ==KErrNone)
			{
			TEntry entry;
			err = fsSession.Entry(KBTManServerRestoreFileName, entry);
			}

		fsSession.Close();
		}

	// If restore file was found, err will be KErrNone
	return (err == KErrNone);
	}

/**
Checks for the existence of a restore file. If a restore file is found, then the local device address
is feteched and processing of that restore file is initiated.
**/
void CBTManServerBURMgr::CheckForRestoreFileL()
	{
	LOG_FUNC

	if (RestoreFilePresent())
		{
		// Fetch local address. When this is received, restore file processing will begin.
		iLocalAddrFetcher = CBTLocalAddressFetcher::NewL(*this, iBTManServer.Registry());
		iLocalAddrFetcher->FetchLocalAddress();
		}
	}

/**
Subscribes to the P&S flag provided by the Secure Backup Engine (to receive notification of backup/restore state changes).
**/
void CBTManServerBURMgr::SubscribeToBackupRestoreKey()
	{
	LOG_FUNC

	iProperty.Subscribe(iStatus);
	SetActive();	
	}

/**
Processes a change in the backup/restore state of the device. 
@param aBackupStateValue A value expressing the backup/restore state of the device. 
**/
void CBTManServerBURMgr::ProcessBackupState(TBURPartType aBURStateValue)
	{
	LOG_FUNC

	iBURState = aBURStateValue;

	switch (aBURStateValue)
		{
	case EBURBackupFull:
		// Fall-through
	case EBURBackupPartial:
		iStateMachine->TransitionState(EBTBUREventBackup);
		break;
	case EBURRestoreFull:
		// Fall-through
	case EBURRestorePartial:
		iStateMachine->TransitionState(EBTBUREventRestore);
		break;
	case EBURNormal:
		iStateMachine->TransitionState(EBTBUREventNormal);
		break;
	default:	// EBURUnset
		__ASSERT_DEBUG(aBURStateValue == EBURUnset, PANIC(KBTBackupPanicCat, EBTBURMgrUnknownBUREvent));
		iStateMachine->TransitionState(EBTBUREventUnset);
		}
	}

/**
Registers an MBTBURNotify instance to receive notification of backup/restore operations starting and stopping.
Ownership of this MBTBURNotify instance is not transferred.
Any previous MBTBURNotify instance is deregistered on this method call.
Supplying a NULL pointer will deregister the previously registered instance (if present).
**/
void CBTManServerBURMgr::RequestBURNotification(MBTBURNotify* aBURNotify)
	{
	LOG_FUNC

	iBURNotify = aBURNotify;
	}


/**
Receives a local address from a CBTLocalAddressFetcher instance.
This will trigger processing of a pending restore file.
**/
void CBTManServerBURMgr::SetLocalAddress(TBTDevAddr& aLocalAddr)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aLocalAddr != TBTDevAddr(), PANIC(KBTBackupPanicCat, EBTBURMgrMissingLocalAddress));

	// Use address only if it is non-zero.
	if (aLocalAddr != TBTDevAddr())
		{
		// Take a copy
		iLocalAddr = aLocalAddr;
		// Attempt to start restore file processing.
		// Note that this will fail if we are not in the normal state.
		iStateMachine->TransitionState(EBTBUREventProcessRestoreFile);
		}
	}

/**
Receives notification that a restore file has been provided by the Secure Backup Engine.
Upon receiving this notification, the restore file is renamed to have the appropriate extension 
The actual restore to the registry is postponed until the next time BTManServer starts, 
so any ongoing BT connections are not affected.
**/
void CBTManServerBURMgr::RestoreFileReady()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreOngoing, InvalidStatePanic(EBTBURStateRestoreOngoing, iStateMachine->GetCurrentState()));

	// Rename restore file
	RenameBackupFileForRestore();
	}

void CBTManServerBURMgr::HandleStateNormal()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateNormal, InvalidStatePanic(EBTBURStateNormal, iStateMachine->GetCurrentState()));

	if (iActiveBackupClient)
		{
		// Backup or restore finished
		if (iBackupHandler)
			{
			// Backup operation finished
			// Delete backup file and destroy backup handler
			DeleteBackupFile();
			delete iBackupHandler;
			iBackupHandler = NULL;
			}
		else
			{
			// Restore operation finished
			delete iActiveBackupDataClient;
			iActiveBackupDataClient = NULL;
			}

		// Destroy active backup client
		delete iActiveBackupClient;
		iActiveBackupClient = NULL;

		// Notify that backup/restore operation has finished
		NotifyBUROperationStopped();
		}
	}

/**
Determines if a BUR operation affects the Bluetooth Manager server.
@return ETrue if a full backup or restore is ongoing, otherwise the return value
given by CActiveBackupClient::DoesPartialBURAffectMeL()
**/
TBool CBTManServerBURMgr::DoesBURAffectMeL(CActiveBackupClient& aClient)
	{
	LOG_FUNC

	if (iBURState == EBURBackupFull || iBURState == EBURRestoreFull)
		{
		return ETrue;
		}
	else
		{
		return aClient.DoesPartialBURAffectMeL();
		}
	}

void CBTManServerBURMgr::HandleStateBackupRequestL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateBackupRequest, InvalidStatePanic(EBTBURStateBackupRequest, iStateMachine->GetCurrentState()));

	// Determine if this backup operation affects us and transition to next state based on this outcome.
	iActiveBackupClient = CActiveBackupClient::NewL();

	if (DoesBURAffectMeL(*iActiveBackupClient))
		{
		// We're affected, proceed with backup
		iStateMachine->TransitionState(EBTBUREventBackupProceed);
		}
	else
		{
		// We're not effected, don't do any backup handling
		iStateMachine->TransitionState(EBTBUREventBackupReject);
		}
	}

void CBTManServerBURMgr::HandleStateBackupRequestError()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateBackupRequest, InvalidStatePanic(EBTBURStateBackupRequest, iStateMachine->GetCurrentState()));

	// Transition to normal state
	iStateMachine->TransitionState(EBTBUREventAbortStateAction);
	}

void CBTManServerBURMgr::HandleStateBackupOngoingL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateBackupOngoing, InvalidStatePanic(EBTBURStateBackupOngoing, iStateMachine->GetCurrentState()));
	__ASSERT_DEBUG(iActiveBackupClient, PANIC(KBTBackupPanicCat, EBTBURMgrActiveBackupClientNull));

	// Provide notification that a BUR operation has started.
	NotifyBUROperationStarted();

	// Construct backup handler and prepare for backup
	iBackupHandler = CBTBackupHandler::NewL(*this, iBTManServer.Registry());
	iBackupHandler->CreateBackupFileL();

	// Signal that we're ready for (passive) backup of prepared file
	iActiveBackupClient->ConfirmReadyForBURL(KErrNone);

	// Next state transition will be invoked when BUR P&S key changes
	}

void CBTManServerBURMgr::HandleStateBackupOngoingError()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateBackupOngoing, InvalidStatePanic(EBTBURStateBackupOngoing, iStateMachine->GetCurrentState()));

	// Transition to normal state
	iStateMachine->TransitionState(EBTBUREventAbortStateAction);
	}

void CBTManServerBURMgr::HandleStateBackupIgnore()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateBackupIgnore, InvalidStatePanic(EBTBURStateBackupIgnore, iStateMachine->GetCurrentState()));

	// Destroy active backup client
	delete iActiveBackupClient;
	iActiveBackupClient = NULL;

	// Next state transition will be invoked when BUR P&S key changes
	}

void CBTManServerBURMgr::HandleStateRestoreRequestL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreRequest, InvalidStatePanic(EBTBURStateRestoreRequest, iStateMachine->GetCurrentState()));

	// We do want a callback here. Even though the actual restore operation has been handled passively, we
	// still need notificaton that the passive restore has completed (so we can then deal with the file).
	iActiveBackupDataClient = CBTActiveBackupDataClient::NewL(*this);
	iActiveBackupClient = CActiveBackupClient::NewL(iActiveBackupDataClient);

	// Determine if this restore operation affects us and transition to next state based on this outcome
	if (DoesBURAffectMeL(*iActiveBackupClient))
		{
		// We're affected, proceed with restore
		iStateMachine->TransitionState(EBTBUREventRestoreProceed);
		}
	else
		{
		// We're not effected, don't do any restore handling
		iStateMachine->TransitionState(EBTBUREventRestoreReject);
		}
	}

void CBTManServerBURMgr::HandleStateRestoreRequestError()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreRequest, InvalidStatePanic(EBTBURStateRestoreRequest, iStateMachine->GetCurrentState()));
	
	// Transition to normal state
	iStateMachine->TransitionState(EBTBUREventAbortStateAction);
	}

void CBTManServerBURMgr::HandleStateRestoreOngoingL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreOngoing, InvalidStatePanic(EBTBURStateRestoreOngoing, iStateMachine->GetCurrentState()));

	// Cancel any local address requests
	if (iLocalAddrFetcher)
		{
		iLocalAddrFetcher->Cancel();
		}

	// Provide notification that a BUR operation has started.
	NotifyBUROperationStarted();

	// Signal that we're ready for active restore of prepared file
	// (note that no data is actively restored, but we still need to invoke this method)
	iActiveBackupClient->ConfirmReadyForBURL(KErrNone);

	// Next state transition will be invoked when BUR P&S key changes
	}

void CBTManServerBURMgr::HandleStateRestoreOngoingError()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreOngoing, InvalidStatePanic(EBTBURStateRestoreOngoing, iStateMachine->GetCurrentState()));

	// Transition to normal state
	iStateMachine->TransitionState(EBTBUREventAbortStateAction);
	}

void CBTManServerBURMgr::HandleStateRestoreIgnore()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreIgnore, InvalidStatePanic(EBTBURStateRestoreIgnore, iStateMachine->GetCurrentState()));

	// Destroy active backup client
	delete iActiveBackupClient;
	delete iActiveBackupDataClient;
	iActiveBackupClient = NULL;
	iActiveBackupDataClient = NULL;

	// Next state transition will be invoked when BUR P&S key changes
	}

void CBTManServerBURMgr::HandleStateProcessRestoreFileL()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateProcessRestoreFile, InvalidStatePanic(EBTBURStateProcessRestoreFile, iStateMachine->GetCurrentState()));

	// Start restore file processing
	iRestoreHandler = CBTRestoreHandler::NewL(*this, iBTManServer);
	iRestoreHandler->RestoreRegistryL(iLocalAddr);

	iStateMachine->TransitionState(EBTBUREventProcessRestoreFileComplete);
	}

void CBTManServerBURMgr::HandleStateProcessRestoreFileError(TInt aError)
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateProcessRestoreFile, InvalidStatePanic(EBTBURStateProcessRestoreFile, iStateMachine->GetCurrentState()));

	// Delete restore handler
	delete iRestoreHandler;
	iRestoreHandler = NULL;
	
	// If aError is anything other than KErrNoMemory, then delete restore file.
	// OOM errors may be temporary, and a subsequent restore attempt may succeed.
	if (aError != KErrNoMemory)
		{
		DeleteRestoreFile();
		}

	// Transition to normal state
	iStateMachine->TransitionState(EBTBUREventAbortStateAction);
	}

void CBTManServerBURMgr::HandleStateRestoreFileProcessingComplete()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iStateMachine->GetCurrentState() == EBTBURStateRestoreFileProcessingComplete, InvalidStatePanic(EBTBURStateRestoreFileProcessingComplete, iStateMachine->GetCurrentState()));
	
	// Delete restore file (we're now done with it)
	DeleteRestoreFile();

	// Delete restore handler
	delete iRestoreHandler;
	iRestoreHandler = NULL;

	iStateMachine->TransitionState(EBTBUREventRestoreFileTransitionNormal);
	}

/**
Sends notification to a registered MBTBURNotify instance that a backup or restore operation has started.
@see CBTManServerBURMgr::RequestBURNotification()
**/
void CBTManServerBURMgr::NotifyBUROperationStarted()
	{
	LOG_FUNC
	__ASSERT_DEBUG(!iBUROperationStartNotified, PANIC(KBTBackupPanicCat, EBTBURMgrBUROperationStartAlreadyNotified));
	
	if (!iBUROperationStartNotified)
		{
		iBUROperationStartNotified = ETrue;

		if (iBURNotify)
			{
			iBURNotify->BUROperationStarted();
			}
		}
	}

/**
Sends notification to the MBTBURNotify instance that a backup or restore operation has started. 
This should correspond to a call to NotifyBUROperationStarted().
@see CBTManServerBURMgr::RequestBURNotification()
**/
void CBTManServerBURMgr::NotifyBUROperationStopped()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iBUROperationStartNotified, PANIC(KBTBackupPanicCat, EBTBURMgrBUROperationStartNotNotified));
	
	if (iBUROperationStartNotified)
		{
		iBUROperationStartNotified = EFalse;

		if (iBURNotify)
			{
			iBURNotify->BUROperationStopped();
			}
		}
	}

/**
Sends notification to the MBTBURNotify instance that a backup or restore operation has started. 
This will send notification only if NotifyBUROperationStarted() has been called without a corresponding 
call to NotifyBUROperationStopped().
This is intended for use in error situations only.
**/
void CBTManServerBURMgr::NotifyAnyBUROperationStopped()
	{
	LOG_FUNC

	// We deliberately want to fail silently here if no stop notification is expected.
	if (iBUROperationStartNotified)
		{
		iBUROperationStartNotified = EFalse;

		if (iBURNotify)
			{
			iBURNotify->BUROperationStopped();
			}
		}
	}

/**
Utility function to delete a named file from the private directory of this process on the system drive.
This is a best-efforts attempt which fails silently on error.
@param aFileName The name of the file to delete.
**/
void CBTManServerBURMgr::DeleteFile(const TDesC& aFileName)
	{
	LOG_FUNC

	RFs fsSession;
	// Attempt to delete file
	// This is best-efforts, failing silently on any error (as there is no remedy action that we can take).
	if (fsSession.Connect() == KErrNone)
		{
		if (fsSession.SetSessionToPrivate(fsSession.GetSystemDrive()) == KErrNone)
			{
			// Try to ensure read-only and system attributes are not set
			TUint attrs;
			TInt err = fsSession.Att(aFileName, attrs);
			if (err == KErrNone)
				{
				// Unset the files read-only and system attributes if these are set.
				TUint unsetAttrs = (attrs & KEntryAttReadOnly) | (attrs & KEntryAttSystem);
				if (unsetAttrs != KEntryAttNormal)
					{
					err = fsSession.SetAtt(aFileName, KEntryAttNormal, unsetAttrs);
					}

				// Attempt to delete the file, if we've managed to successfully retrieve and reset the file attributes. 
				// Note that this is best-efforts.
				if (err == KErrNone)
					{
					static_cast<void>(fsSession.Delete(aFileName));
					}
				}
			}

		fsSession.Close();
		}
	}	

/**
Deletes the backup file (with name KBTManServerBackupFileName) from the private directory of this process on the system drive.
This is a best-efforts attempt which fails silently on error.
**/
void CBTManServerBURMgr::DeleteBackupFile()
	{
	LOG_FUNC

	DeleteFile(KBTManServerBackupFileName);
	}

/**
Deletes the restore file (with name KBTManServerRestoreFileName) from the private directory of this process on the system drive.
This is a best-efforts attempt which fails silently on error.
**/
void CBTManServerBURMgr::DeleteRestoreFile()
	{
	LOG_FUNC

	DeleteFile(KBTManServerRestoreFileName);
	}

/**
Renames a restored backup file from KBTManServerBackupFileName to KBTManServerRestoreFileName.
This facilitates processing of the restore file when the Bluetooth Manager server next starts.
This is a best-efforts attempt, as no remedial action can be taken if this operation fails.
**/
void CBTManServerBURMgr::RenameBackupFileForRestore()
	{
	LOG_FUNC

	// Delete any previous restore file
	DeleteRestoreFile();
	
	RFs fsSession;
	if (fsSession.Connect() == KErrNone)
		{
		static_cast<void>(fsSession.Rename(KBTManServerBackupFileName, KBTManServerRestoreFileName));
		fsSession.Close();
		}
	}

void CBTManServerBURMgr::RunL()
	{
	LOG_FUNC
	
	if (iStatus == KErrNone)
		{
		// Subscribe to catch the next transition
		SubscribeToBackupRestoreKey();

		// Handle this event
		TInt backupKeyValue = 0;
		LEAVEIFERRORL(iProperty.Get(backupKeyValue));
		TBURPartType backupState = GetBURPartType(backupKeyValue);

		ProcessBackupState(backupState);
		}
	}

TInt CBTManServerBURMgr::RunError(TInt /*aError*/)
	{
	LOG_FUNC

	// Problem occured in obtaining backup key value.
	// Ignore the error, as there is nothing we can do.
	return KErrNone;
	}

void CBTManServerBURMgr::DoCancel()
	{
	LOG_FUNC
	
	// Cancel our subscription
	iProperty.Cancel();
	}

/**
 CBTBackupHandler - Handles the task of backing up the BT registry into a backup file ready for passive backup.
 **/
CBTBackupHandler* CBTBackupHandler::NewL(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry)
	{
	LOG_STATIC_FUNC

	CBTBackupHandler* result = new(ELeave) CBTBackupHandler(aBURMgr, aRegistry);
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result);
	return result;
	}

CBTBackupHandler::CBTBackupHandler(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry) 
	: iBURMgr(aBURMgr), 
	iRegistry(aRegistry)
	{
	LOG_FUNC
	}

void CBTBackupHandler::ConstructL()
	{
	LOG_FUNC

	iRegistryData=CBTRegistryBURData::NewL();
	}

CBTBackupHandler::~CBTBackupHandler()
	{
	LOG_FUNC

	delete iRegistryData;
	}

void CBTBackupHandler::CreateBackupFileL()
	{
	LOG_FUNC

	// Collect backup registry data
	iRegistryData->ReadFromRegistryL(iRegistry);
	
	// Only continue if the registry is populated (i.e. it may be in its default state, which does not need to be backed up).
	// Determine this by examining the local device address stored in the registry. The default registry has this set to 0x0.
	if (iRegistryData->IsLocalAddressNonZeroL())
		{
		// Construct backup file in private directory.
		RFs fsSession;
		LEAVEIFERRORL(fsSession.Connect());
		CleanupClosePushL(fsSession);
		LEAVEIFERRORL(fsSession.SetSessionToPrivate(fsSession.GetSystemDrive()));
		// Create private path if it does not already exist.
		LEAVEIFERRORL(fsSession.CreatePrivatePath(fsSession.GetSystemDrive()));

		RFileWriteStream fStream;
		// Create backup file, overwriting if necessary (don't care about previous backup files).
		LEAVEIFERRORL(fStream.Replace(fsSession, KBTManServerBackupFileName, EFileWrite));
		fStream.PushL();
			    
		// Write out data
		iRegistryData->WriteToStreamL(fStream);
		fStream.CommitL();

		CleanupStack::PopAndDestroy(2, &fsSession);
		}
	}

/**
 CBTRestoreHandler - Handles the task of restoring the BT registry from a restore file given by passive restore.
 **/
CBTRestoreHandler* CBTRestoreHandler::NewL(CBTManServerBURMgr& aBURMgr, CBTManServer& aManServer)
	{
	LOG_STATIC_FUNC

	CBTRestoreHandler* result = new(ELeave) CBTRestoreHandler(aBURMgr, aManServer);
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result);
	return result;
	}

CBTRestoreHandler::CBTRestoreHandler(CBTManServerBURMgr& aBURMgr, CBTManServer& aManServer) 
	: iBURMgr(aBURMgr), 
	iManServer(aManServer)
	{
	LOG_FUNC
	}
	
void CBTRestoreHandler::ConstructL()
	{
	LOG_FUNC

	iRegistryData = CBTRegistryBURData::NewL();
	}

CBTRestoreHandler::~CBTRestoreHandler()
	{
	LOG_FUNC

	delete iRegistryData;
	}

void CBTRestoreHandler::RestoreRegistryL(TBTDevAddr& aLocalAddr)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aLocalAddr != TBTDevAddr(), PANIC(KBTBackupPanicCat, EBTBURMgrMissingLocalAddress));

	LoadRestoreDataL();
    
	// Compare local address held in restore file with our local address
	if (iRegistryData->IsLocalAddressEqualL(aLocalAddr))
		{
		// Proceed with restore
		CBTRegistry& registry = iManServer.Registry();

		if (iRegistryData->WriteLocalDeviceNameToRegistryL(registry))
			{
			NotifyLocalTableChange();
			NotifyLocalDeviceNameChange(iRegistryData->GetLocalDeviceNameL());
			}

		TInt noRemoteDevices = iRegistryData->CountRemoteDevicesL();
		for (TInt i = 0; i < noRemoteDevices; i++)
			{
			if (iRegistryData->WriteRemoteDeviceToRegistryL(registry, i))
				{
				NotifyRemoteTableChangeL(iRegistryData->GetRemoteDeviceL(i).BDAddr());
				}
			}
		}
	}

void CBTRestoreHandler::LoadRestoreDataL()
	{
	LOG_FUNC

	// Read restore file
	RFs fsSession;
	LEAVEIFERRORL(fsSession.Connect());
	CleanupClosePushL(fsSession);
	LEAVEIFERRORL(fsSession.SetSessionToPrivate(fsSession.GetSystemDrive()));
	// Assuming private directory exists at this point
	
	RFileReadStream fStream;
	LEAVEIFERRORL(fStream.Open(fsSession, KBTManServerRestoreFileName, EFileRead));
	fStream.PushL();

	// Read in data
	iRegistryData->ReadFromStreamL(fStream);

	// Cleanup
	CleanupStack::PopAndDestroy(2, &fsSession);	//fStream and fsSession
	}

/**
Sends a notification that the persistence table has been changed.
The notification is observable through the P&S key KPropertyKeyBluetoothGetRegistryTableChange. 
**/
void CBTRestoreHandler::NotifyLocalTableChange()
	{
	LOG_FUNC
	
	// Notify the P&S key that the remote devices table has changed
	iManServer.Publish(KPropertyKeyBluetoothGetRegistryTableChange, KRegistryChangeLocalTable);
	}

/**
Sends a notification that the local device name has been changed.
This notification is observable through the P&S key KPRopertyKeyBluetoothSetDeviceName.
@param aLocalName The modified local device name as an 8-bit descriptor.
**/
void CBTRestoreHandler::NotifyLocalDeviceNameChange(const TDesC8& aLocalName)
	{
	LOG_FUNC
	
	// The P&S key requires the local device name in unicode format.
	TBuf16<KMaxBluetoothNameLen> localNameUniCode;
	localNameUniCode.Copy(aLocalName);
	
	NotifyLocalDeviceNameChange(localNameUniCode);
	}

/**
Sends a notification that the local device name has been changed.
This notification is observable through the P&S key KPRopertyKeyBluetoothSetDeviceName.
@param aLocalName The modified local device name in unicode format.
**/
void CBTRestoreHandler::NotifyLocalDeviceNameChange(const TDesC16& aLocalName)
	{
	LOG_FUNC
	
	// The KPropertyKeyBluetoothSetDeviceName P&S key may or may not exist at this point.
	TInt err = RProperty::Set( KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetDeviceName, aLocalName);
	if (err != KErrNone && err != KErrNotFound)
		{
		LOG1(_L("CBTRegistryBURData::NotifyLocalDeviceNameChange() - RProperty::Set() failed with %d"), err);
		}
	}

/**
Sends a notification that a device in the remote devices table has been changed.
The notification is observable through the P&S key KPropertyKeyBluetoothGetRegistryTableChange. 
Interested parties can also use RBTRegistry::NotifyViewChange() to detect if the change affects their view.
**/
void CBTRestoreHandler::NotifyRemoteTableChangeL(const TBTDevAddr& aAddress)
	{
	LOG_FUNC
	
	// Construct SQL constraint which selects the device
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindDeviceL(aAddress);

	HBufC* conDes = query.ConstraintBuf().AllocLC();

	// Notify the P&S key that the remote devices table has changed
	iManServer.Publish(KPropertyKeyBluetoothGetRegistryTableChange, KRegistryChangeRemoteTable);

	// Signal notifiers waiting on view change
	iManServer.NotifyViewChange(*conDes);

	// Cleanup constDes and query
	CleanupStack::PopAndDestroy(2, &query);
	}

/**
CBTRegistryBURData - manages data from the Bluetooth registry that is to be backed up or restored.
**/
CBTRegistryBURData* CBTRegistryBURData::NewL()
	{
	LOG_STATIC_FUNC

	CBTRegistryBURData* self = new(ELeave) CBTRegistryBURData();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTRegistryBURData::CBTRegistryBURData() 
	: iHasRegistryData(EFalse)
	{
	LOG_FUNC
	}

CBTRegistryBURData::~CBTRegistryBURData()
	{
	LOG_FUNC

	if (iRemoteDevices)	// Dont use ClearRegistryData() here as leave may have occured in ReadFromRegistryL() or ReadFromStreamL()
		{
		// Destroy all CBTDevices in iRemoteDevices (necessary before array deletion for CArrayPtrFlat)
		iRemoteDevices->ResetAndDestroy();
		}

	delete iLocalDevice;
	delete iRemoteDevicesSid;
	delete iRemoteDevices;
	}

void CBTRegistryBURData::ConstructL()
	{
	LOG_FUNC

	iRemoteDevicesSid = new (ELeave) CArrayFixFlat<TSecureId>(1);
	iRemoteDevices = new(ELeave) CBTDeviceArray(1);
	}
	
/**
Writes the registry data held in this instance to a given stream.
This does not write those registry fields which contain unset values.
@param aStream The stream to write the registry data to.
**/
void CBTRegistryBURData::WriteToStreamL(RWriteStream& aStream) const
	{
	LOG_FUNC

	// Writes registry data out to a given stream, avoiding any use of class externalisation.
	if (HasRegistryData())
		{
		// This follows a specific file-format (tied to the registry version information), which is documented seperately.

		// We should already have the following information for the local device.
		__ASSERT_DEBUG(iLocalDevice->IsValidAddress(), PANIC(KBTBackupPanicCat, EBTBURMgrExpectedDataMissing));

		// Write out version information
		aStream.WriteUint32L(iRegistryVersionMajor);
		aStream.WriteUint32L(iRegistryVersionMinor);

		// Persistence table:
		// Local device address and name only (both are mandatory).
		aStream.WriteL(iLocalDevice->Address().Des(), KBTDevAddrSize);

		__ASSERT_DEBUG(iLocalDevice->DeviceName().Length() <= KMaxTUint8, PANIC(KBTBackupPanicCat, EBTBURMgrDescriptorLengthTooLong));
		aStream.WriteUint8L(iLocalDevice->DeviceName().Length());
		aStream.WriteL(iLocalDevice->DeviceName(), iLocalDevice->DeviceName().Length());

		// Remote devices table
		TInt rdCount=CountRemoteDevicesL();

		aStream.WriteUint32L(rdCount);
		for (TInt i = 0; i < rdCount; i++)
			{
			const CBTDevice& nextRemDev = GetRemoteDeviceL(i);
			const TBTNamelessDevice& nextRemDevNameless = nextRemDev.AsNamelessDevice();

			__ASSERT_DEBUG(nextRemDev.IsValidBDAddr(), PANIC(KBTBackupPanicCat, EBTBURMgrExpectedDataMissing));

			// Work out what is set and derive bitmask (can use TBTDeviceSet here as is public)
			TUint32 rdSetMask = TBTNamelessDevice::EAddress;
			if (nextRemDev.IsValidDeviceClass())
				{
				rdSetMask |= TBTNamelessDevice::EDeviceClass;
				}
			if (nextRemDev.IsValidLinkKey())
				{
				rdSetMask |= TBTNamelessDevice::ELinkKey;
				}
			if (nextRemDev.IsValidGlobalSecurity())
				{
				rdSetMask |= TBTNamelessDevice::EGlobalSecurity;
				}
			if (nextRemDevNameless.IsValidPageScanRepMode())
				{
				rdSetMask |= TBTNamelessDevice::EPageScanRepMode;
				}
			if (nextRemDevNameless.IsValidPageScanMode())
				{
				rdSetMask |= TBTNamelessDevice::EPageScanMode;
				}
			if (nextRemDevNameless.IsValidPageScanPeriodMode())
				{
				rdSetMask |= TBTNamelessDevice::EPageScanPeriodMode;
				}
			if (nextRemDevNameless.IsValidClockOffset())
				{
				rdSetMask |= TBTNamelessDevice::EClockOffset;
				}
			if (nextRemDev.IsValidUsed())
				{
				rdSetMask |= TBTNamelessDevice::EUsed;
				}
			if (nextRemDev.IsValidSeen())
				{
				rdSetMask |= TBTNamelessDevice::ESeen;
				}
			if (nextRemDev.IsValidPassKey())
				{
				rdSetMask |= TBTNamelessDevice::EPassKey;
				}
			if (nextRemDev.IsValidUiCookie())
				{
				rdSetMask |= TBTNamelessDevice::EUiCookie;
				}
			if (nextRemDev.IsValidDeviceName())
				{
				rdSetMask |= CBTDevice::EDeviceName;
				}
			if (nextRemDev.IsValidFriendlyName())
				{
				rdSetMask |= CBTDevice::EFriendlyName;
				}
			// Now write out bitmask
			aStream.WriteUint32L(rdSetMask);

			// Write valid setting for next remote device
			aStream.WriteUint32L(GetRemoteDeviceEntrySidL(i));
			aStream.WriteL(nextRemDev.BDAddr().Des(), KBTDevAddrSize);
			if (rdSetMask & TBTNamelessDevice::EDeviceClass)
				{
				aStream.WriteUint32L(nextRemDev.DeviceClass().DeviceClass());
				}
			if (rdSetMask & TBTNamelessDevice::ELinkKey)
				{
				const TBTLinkKey& nextRemDevLinkKey = nextRemDev.LinkKey();
				aStream.WriteL(nextRemDevLinkKey, KHCILinkKeySize);
				aStream.WriteUint8L(nextRemDev.LinkKeyType());
				}
			if (rdSetMask & TBTNamelessDevice::EGlobalSecurity)
				{
				TBTDeviceSecurity nextRemDevGlobSec = nextRemDev.GlobalSecurity();
				aStream.WriteUint8L(nextRemDevGlobSec.SecurityValue());
				aStream.WriteUint32L(nextRemDevGlobSec.PasskeyMinLength());
				}
			if (rdSetMask & TBTNamelessDevice::EPageScanRepMode)
				{
				aStream.WriteUint8L(nextRemDevNameless.PageScanRepMode());
				}
			if (rdSetMask & TBTNamelessDevice::EPageScanMode)
				{
				aStream.WriteUint8L(nextRemDevNameless.PageScanMode());
				}
			if (rdSetMask & TBTNamelessDevice::EPageScanPeriodMode)
				{
				aStream.WriteUint8L(nextRemDevNameless.PageScanPeriodMode());
				}
			if (rdSetMask & TBTNamelessDevice::EClockOffset)
				{
				aStream.WriteUint16L(nextRemDevNameless.ClockOffset());
				}
			if (rdSetMask & TBTNamelessDevice::EUsed)
				{
				const TInt64& used = nextRemDev.Used().Int64();
				aStream.WriteInt32L(I64HIGH(used));
				aStream.WriteInt32L(I64LOW(used));
				}
			if (rdSetMask & TBTNamelessDevice::ESeen)
				{
				const TInt64& seen = nextRemDev.Seen().Int64();
				aStream.WriteInt32L(I64HIGH(seen));
				aStream.WriteInt32L(I64LOW(seen));
				}
			if (rdSetMask & TBTNamelessDevice::EPassKey)
				{
				const TBTPinCode& nextRemDevPassKey = nextRemDev.PassKey();
				__ASSERT_DEBUG(nextRemDevPassKey.Length() == KHCIPINCodeSize + 1, PANIC(KBTBackupPanicCat, EBTBURMgrDescriptorUnexpectedLength));
				aStream.WriteL(nextRemDevPassKey, KHCIPINCodeSize + 1);
				}
			if (rdSetMask & TBTNamelessDevice::EUiCookie)
				{
				aStream.WriteUint32L(nextRemDev.UiCookie());
				}
			if (rdSetMask & CBTDevice::EDeviceName)
				{
				__ASSERT_DEBUG(nextRemDev.DeviceName().Length() <= KMaxTUint8, PANIC(KBTBackupPanicCat, EBTBURMgrDescriptorLengthTooLong));
				aStream.WriteUint8L(nextRemDev.DeviceName().Length());
				aStream.WriteL(nextRemDev.DeviceName(), nextRemDev.DeviceName().Length());
				}
			if (rdSetMask & CBTDevice::EFriendlyName)
				{
				__ASSERT_DEBUG(nextRemDev.FriendlyName().Length() <= KMaxTUint8, PANIC(KBTBackupPanicCat, EBTBURMgrDescriptorLengthTooLong));
				aStream.WriteUint8L(nextRemDev.FriendlyName().Length());
				aStream.WriteL(nextRemDev.FriendlyName(), nextRemDev.FriendlyName().Length());
				}
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

/**
Reads registry data from a given stream and stores within this instance.
Any registry data currently held is cleared first.
@param aStream The stream to ready the registry data from.
**/
void CBTRegistryBURData::ReadFromStreamL(RReadStream& aStream)
	{
	LOG_FUNC

	// This follows a specific file-format (tied to the registry version information), which is documented seperately.
	
	// Note that we dont make *ANY* assumptions as to what data should always be available, since the restore file could be corrupt.
	// We will fail the restore if it turns out we do not have enough information, or if we have invalid information.
	
	// WARNING: When a new registry version is produced, this implementaton of backup/restore support will need modification.
	// This assert should be updated to include all supported registry versions.
	__ASSERT_DEBUG(KRegistryDBVersionMajor == 1 && KRegistryDBVersionMinor == 2, PANIC(KBTBackupPanicCat, EBTBURMgrUnsupportedRegistryVersion));
	
	// Extract version information and continue if we can handle it.
	TUint32 regVersionMajor = aStream.ReadUint32L();
	TUint32 regVersionMinor = aStream.ReadUint32L();
	if (regVersionMajor == KRegistryDBVersionMajor && regVersionMinor == KRegistryDBVersionMinor)
		{
		// We support this version and this version only - read in registry data
		ClearRegistryData();

		iLocalDevice = new(ELeave) TBTLocalDevice;
		
		// Note version information for this registry
		iRegistryVersionMajor = regVersionMajor;
		iRegistryVersionMinor = regVersionMinor;

		// Persistence table:
		// Local device address and name only (both fields are mandatory).
		TBTDevAddr address;
		TPtr8 addrPtr(address.Des());
		aStream.ReadL(addrPtr, KBTDevAddrSize);
		if (addrPtr.Length() == KBTDevAddrSize)
			{
			iLocalDevice->SetAddress(address);
			}
		else
			{
			User::Leave(KErrCorrupt);
			}

		TInt deviceNameLength = static_cast<TInt>(aStream.ReadUint8L());
		if (deviceNameLength <= KMaxBluetoothNameLen)
			{
			RBuf8 deviceName;
			deviceName.CreateL(KMaxBluetoothNameLen);
			CleanupClosePushL(deviceName);

			aStream.ReadL(deviceName,deviceNameLength);
			if (deviceName.Length() == deviceNameLength)
				{
				iLocalDevice->SetDeviceName(deviceName);
				}
			else
				{
				User::Leave(KErrCorrupt);
				}
			CleanupStack::PopAndDestroy(&deviceName);
			}
		else
			{
			User::Leave(KErrCorrupt);
			}
		
		// Remote devices table:
		TInt rdCount = aStream.ReadUint32L();
		for (TInt i = 0; i < rdCount; i++)
			{
			// First read bitmask of available data
			TUint32 rdSetMask = aStream.ReadUint32L();

			// Then read available data from file and store
			CBTDevice *rdInstance = CBTDevice::NewLC();
			TBTNamelessDevice& rdNamelessDevInstance = rdInstance->AsNamelessDevice();
			TSecureId rdSid = aStream.ReadUint32L();

			// Remote BT address is mandatory
			if (rdSetMask & TBTNamelessDevice::EAddress)
				{
				TBTDevAddr rdAddr;
				TPtr8 rdAddrPtr(rdAddr.Des());
				aStream.ReadL(rdAddrPtr, KBTDevAddrSize);

				// Ensure address is of required length and is non-zero
				if (rdAddrPtr.Length() == KBTDevAddrSize && rdAddr != TBTDevAddr())
					{
					rdInstance->SetDeviceAddress(rdAddr);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}
			else
				{
				User::Leave(KErrCorrupt);
				}

			if (rdSetMask & TBTNamelessDevice::EDeviceClass)
				{
				TBTDeviceClass rdClass = TBTDeviceClass(aStream.ReadUint32L());
				rdInstance->SetDeviceClass(rdClass);
				}

			if (rdSetMask & TBTNamelessDevice::ELinkKey)
				{
				TBTLinkKey rdLinkKey;
				aStream.ReadL(rdLinkKey, KHCILinkKeySize);
				if (rdLinkKey.Length() == KHCILinkKeySize)
					{
					TUint8 rdLinkKeyType = static_cast<TBTLinkKeyType>(aStream.ReadUint8L());
					// Ensure value is in the valid range (ELinkKeyCombination is zero so (pointless) comparison with this gives warnings).
					if (rdLinkKeyType <= ELinkKeyDebug)
						{
						rdInstance->SetLinkKey(rdLinkKey, static_cast<TBTLinkKeyType>(rdLinkKeyType));
						}
					else
						{
						User::Leave(KErrCorrupt);
						}

					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}

			if (rdSetMask & TBTNamelessDevice::EGlobalSecurity)
				{
				TBTDeviceSecurity rdGlobSec;
				TUint8 globSecValue = aStream.ReadUint8L();
				// Ensure value is in the valid range (lower value is zero so (pointless) comparison with this gives warnings).
				if (globSecValue <= KBTBURMgrMaxGlobalSecurityValue)
					{
					rdGlobSec.SetSecurityValue(globSecValue);

					TUint32 passKeyMinLen = aStream.ReadUint32L();

					if (passKeyMinLen <= KHCIPINCodeSize)
						{
						rdGlobSec.SetPasskeyMinLength(passKeyMinLen);
						rdInstance->SetGlobalSecurity(rdGlobSec);
						}
					else
						{
						User::Leave(KErrCorrupt);
						}
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}

			if (rdSetMask & TBTNamelessDevice::EPageScanRepMode)
				{
				TUint8 rdPageScanRepMode = aStream.ReadUint8L();
				// Ensure value is in the valid range (lower value is zero so (pointless) comparison with this gives warnings).
				if (rdPageScanRepMode <= EPageScanModeR2)
					{
					rdNamelessDevInstance.SetPageScanRepMode(rdPageScanRepMode);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}

			if (rdSetMask & TBTNamelessDevice::EPageScanMode)
				{
				TUint8 rdPageScanMode = aStream.ReadUint8L();
				// Ensure value is in the valid range (lower value is zero so (pointless) comparison with this gives warnings).
				if (rdPageScanMode <= KBTBURMgrMaxPageScanMode)
					{
					rdNamelessDevInstance.SetPageScanMode(rdPageScanMode);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}

			if (rdSetMask & TBTNamelessDevice::EPageScanPeriodMode)
				{
				TUint8 rdPageScanPeriodMode = aStream.ReadUint8L();
				// Ensure value is in the valid range (lower value is zero so (pointless) comparison with this gives warnings).
				if (rdPageScanPeriodMode <= KBTBURMgrMaxPageScanPeriodMode)
					{
					rdNamelessDevInstance.SetPageScanPeriodMode(rdPageScanPeriodMode);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}

			if (rdSetMask & TBTNamelessDevice::EClockOffset)
				{
				TUint16 rdClockOffset = aStream.ReadUint16L();
				rdNamelessDevInstance.SetClockOffset(rdClockOffset);
				}

			if (rdSetMask & TBTNamelessDevice::EUsed)
				{
				TInt32 usedU = aStream.ReadInt32L();
				TInt32 usedL = aStream.ReadInt32L();
				TInt64 rdUsed = MAKE_TINT64(usedU, usedL);
				rdNamelessDevInstance.SetUsed(rdUsed);
				}

			if (rdSetMask & TBTNamelessDevice::ESeen)
				{
				TInt32 seenU = aStream.ReadInt32L();
				TInt32 seenL = aStream.ReadInt32L();
				TInt64 rdSeen = MAKE_TINT64(seenU, seenL);
				rdNamelessDevInstance.SetSeen(rdSeen);	
				}

			if (rdSetMask & TBTNamelessDevice::EPassKey)
				{
				TBTPinCode rdPassKey;
				aStream.ReadL(rdPassKey, KHCIPINCodeSize + 1);
				if (rdPassKey.Length() == KHCIPINCodeSize + 1)
					{
					rdInstance->SetPassKey(rdPassKey);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}
			
			if (rdSetMask & TBTNamelessDevice::EUiCookie)
				{
				TUint32 rdUiCookie = aStream.ReadUint32L();
				rdNamelessDevInstance.SetUiCookie(rdUiCookie);
				}

			if (rdSetMask & CBTDevice::EDeviceName)
				{
				TInt rdNameLen = static_cast<TInt>(aStream.ReadUint8L());
				if (rdNameLen <= KMaxBluetoothNameLen)
					{
					RBuf8 rdName;
					rdName.CreateL(KMaxBluetoothNameLen);
					CleanupClosePushL(rdName);
					
					aStream.ReadL(rdName, rdNameLen);
					if (rdName.Length() == rdNameLen)
						{
						rdInstance->SetDeviceNameL(rdName);
						}
					else
						{
						User::Leave(KErrCorrupt);
						}
					CleanupStack::PopAndDestroy(&rdName);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}

			if (rdSetMask & CBTDevice::EFriendlyName)
				{
				TInt rdFriendlyNameLen = static_cast<TInt>(aStream.ReadUint8L());
				if (rdFriendlyNameLen <= KMaxFriendlyNameLen)
					{
					RBuf rdFriendlyName;
					rdFriendlyName.CreateL(KMaxFriendlyNameLen);
					CleanupClosePushL(rdFriendlyName);

					aStream.ReadL(rdFriendlyName, rdFriendlyNameLen);
					if (rdFriendlyName.Length() == rdFriendlyNameLen)
						{
						rdInstance->SetFriendlyNameL(rdFriendlyName);
						}
					else
						{
						User::Leave(KErrCorrupt);
						}
					CleanupStack::PopAndDestroy(&rdFriendlyName);
					}
				else
					{
					User::Leave(KErrCorrupt);
					}
				}
			
			// Data now stored, add rdInstance to our remote devices list
			iRemoteDevices->AppendL(rdInstance);
			iRemoteDevicesSid->AppendL(rdSid);
			CleanupStack::Pop(rdInstance);
			}

		// Mark that we now have registry data 
		iHasRegistryData = ETrue;
		}
	}

/**
Reads registry data from the registry into this instance.
Any existing registry data held in this instance is cleared first.
@param aRegistry The CBTRegistry instance to use for registry access.
**/
void CBTRegistryBURData::ReadFromRegistryL(CBTRegistry& aRegistry)
	{
	LOG_FUNC

	ClearRegistryData();

	// The meta information from our registry is known.
	iRegistryVersionMajor = KRegistryDBVersionMajor;
	iRegistryVersionMinor = KRegistryDBVersionMinor;

	// Persistence table
	iLocalDevice = aRegistry.GetLocalDeviceL();

	// Remote device table
	// Create a view on the table.
	RBTDbQuery query;
	CleanupClosePushL(query);
	TBTRegistrySearch searchCriteria;
	searchCriteria.FindAll();
	query.SearchL(searchCriteria);
	TDbBookmark bookmark;
	RDbView* view = aRegistry.OpenViewL(query, bookmark);
	CleanupCloseDeletePushL(view);

	// Populate iRemoteDevices from the view.
	while (!view->AtEnd())
		{
		CBTDevice *next = aRegistry.GetNextDeviceL(*view, bookmark, ETrue);
		CleanupStack::PushL(next);
		
		// CBTRegisty::CreatingProcessUidL() requires a rowset with exactly one row.
		// So we have to requery for this device to get a singular row.
		TDbBookmark singleBookmark;
		RDbView* singleView = aRegistry.OpenDeviceL(next->BDAddr(), singleBookmark);
		CleanupCloseDeletePushL(singleView);
		TSecureId nextSid = aRegistry.CreatingProcessUidL(*singleView);
		CleanupStack::PopAndDestroy(singleView);
		
		iRemoteDevices->AppendL(next);
		iRemoteDevicesSid->AppendL(nextSid);
		
		CleanupStack::Pop(next);	// iRemoteDevices now takes ownership
		}

	CleanupStack::PopAndDestroy(2, &query);	// view and query

	// Mark that we now have registry data 
	iHasRegistryData = ETrue;
	}

/**
Updates the persistence table of the registry with local device name held in this instance.
The update is only performed if the local name held in this instance differs from that currently held in the registry.
@param aRegistry The CBTRegistry instance to use for registry access.
@return ETrue if an update was made to the registry.
**/
TBool CBTRegistryBURData::WriteLocalDeviceNameToRegistryL(CBTRegistry& aRegistry) const
	{
	LOG_FUNC

	TBool updateDone = EFalse;
	
	TBTLocalDevice* localDevice = aRegistry.GetLocalDeviceL();
	CleanupStack::PushL(localDevice);

	const TDesC8& localName = GetLocalDeviceNameL();

	if (localDevice->DeviceName() != localName)
		{
		localDevice->SetDeviceName(localName);
		aRegistry.UpdateLocalDeviceL(*localDevice);
		updateDone = ETrue;
		}

	CleanupStack::PopAndDestroy(localDevice);

	return updateDone;
	}

/**
Updates an entry of the remote devices table of the registry with data held in this instance.
If the remote device already exists in the registry, the registry version is updated only if it
does not hold a link key.
Otherwise, the remote device is added to the registry.
@param aRegistry The CBTRegistry instance to use for registry access.
@param aDeviceIndex The remote device held in this CBTRegistryBURData instance to be written to the registry.
@return ETrue if an update was made to the registry.
**/
TBool CBTRegistryBURData::WriteRemoteDeviceToRegistryL(CBTRegistry& aRegistry, TInt aDeviceIndex) const
	{
	LOG_FUNC

	TBool updateDone = EFalse;
	
	// Get device and SID
	const CBTDevice& nextRemDevice = GetRemoteDeviceL(aDeviceIndex);
	TSecureId nextRemDeviceSid = GetRemoteDeviceEntrySidL(aDeviceIndex);

	// Try to add device to registry. If this fails with KErrAlreadyExists, then update existing device.
	TRAPD(err, aRegistry.CreateDeviceL(nextRemDevice, nextRemDevice.IsValidUiCookie(), nextRemDeviceSid));

	if (err == KErrNone)
		{
		// New device added successfully
		updateDone = ETrue;
		}
	else if (err == KErrAlreadyExists)
		{
		// Device already exists. Extract and examine
		TDbBookmark bookmark;
		RDbView* view = aRegistry.OpenDeviceL(nextRemDevice.BDAddr(), bookmark);
		CleanupCloseDeletePushL(view);
		CBTDevice *regDev = aRegistry.GetNextDeviceL(*view, bookmark, ETrue);
		CleanupStack::PushL(regDev);

		if (!regDev->IsValidLinkKey())
			{
			// No link key - safe to restore remote device from file.
			view->FirstL();
			aRegistry.UpdateDeviceL(*view, nextRemDevice);
			updateDone = ETrue;
			}

		CleanupStack::PopAndDestroy(2, view);
		}
	else
		{
		// Unexpected error - pass upward for handling
		User::Leave(err);
		}

	return updateDone;
	}

void CBTRegistryBURData::GetRegistryVersionL(TUint32& aRegistryVersionMajor, TUint32& aRegistryVersionMinor) const
	{
	LOG_FUNC

	if (!HasRegistryData())
		{
		User::Leave(KErrNotFound);
		}
	
	aRegistryVersionMajor = iRegistryVersionMajor;
	aRegistryVersionMinor = iRegistryVersionMinor;
	}

const TDesC8& CBTRegistryBURData::GetLocalDeviceNameL() const
	{
	LOG_FUNC

	if (!HasRegistryData())
		{
		User::Leave(KErrNotFound);
		}

	__ASSERT_DEBUG(iLocalDevice, PANIC(KBTBackupPanicCat, EBTBURMgrExpectedDataMissing));
	return iLocalDevice->DeviceName();
	}

TBool CBTRegistryBURData::IsLocalAddressNonZeroL() const
	{
	LOG_FUNC

	TBTDevAddr zeroAddr;
	return !IsLocalAddressEqualL(zeroAddr);
	}

TBool CBTRegistryBURData::IsLocalAddressEqualL(TBTDevAddr& aAddr) const
	{
	LOG_FUNC

	if (!HasRegistryData())
		{
		User::Leave(KErrNotFound);
		}

	__ASSERT_DEBUG(iLocalDevice, PANIC(KBTBackupPanicCat, EBTBURMgrExpectedDataMissing));
	return (iLocalDevice->Address() == aAddr);
	}

TInt CBTRegistryBURData::CountRemoteDevicesL() const
	{
	LOG_FUNC

	if (!HasRegistryData())
		{
		User::Leave(KErrNotFound);
		}
	
	__ASSERT_DEBUG(iRemoteDevices->Count() == iRemoteDevicesSid->Count(), PANIC(KBTBackupPanicCat, EBTBURMgrArraySizeMisMatch));	
	return iRemoteDevices->Count();
	}

const CBTDevice& CBTRegistryBURData::GetRemoteDeviceL(TInt aDeviceIndex) const
	{
	LOG_FUNC

	if (!HasRegistryData() || !((aDeviceIndex >= 0) && (aDeviceIndex < iRemoteDevices->Count())))
		{
		User::Leave(KErrNotFound);
		}

	__ASSERT_DEBUG(iRemoteDevices->Count() == iRemoteDevicesSid->Count(), PANIC(KBTBackupPanicCat, EBTBURMgrArraySizeMisMatch));	
	return *(iRemoteDevices->At(aDeviceIndex));
	}

TSecureId CBTRegistryBURData::GetRemoteDeviceEntrySidL(TInt aDeviceIndex) const
	{
	LOG_FUNC

	if (!HasRegistryData() || !((aDeviceIndex >= 0) && (aDeviceIndex < iRemoteDevicesSid->Count())))
		{
		User::Leave(KErrNotFound);
		}
	
	__ASSERT_DEBUG(iRemoteDevices->Count() == iRemoteDevicesSid->Count(), PANIC(KBTBackupPanicCat, EBTBURMgrArraySizeMisMatch));	
	return iRemoteDevicesSid->At(aDeviceIndex);
	}

/**
Clears any registry data stored internally within this instance.
If no data is stored, this method does nothing.
**/
void CBTRegistryBURData::ClearRegistryData()
	{
	LOG_FUNC

	// This is a no-op if we have no registry data to clear
	if (HasRegistryData())
		{
		// Clear data held previously
		iRemoteDevicesSid->Reset();
		iRemoteDevices->ResetAndDestroy();
		delete iLocalDevice;
		iLocalDevice = NULL;

		iHasRegistryData = EFalse;
		}
	}

/**
CBTLocalAddressFetcher - Fetches the local device address, or waits for the addresss to become available, and 
passes to CBTManServerBURMgr.
**/
CBTLocalAddressFetcher* CBTLocalAddressFetcher::NewL(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry, TInt aPriority)
	{
	LOG_STATIC_FUNC

	CBTLocalAddressFetcher* result = new(ELeave) CBTLocalAddressFetcher(aBURMgr, aRegistry, aPriority);
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(result);
	return result;
	}

CBTLocalAddressFetcher::CBTLocalAddressFetcher(CBTManServerBURMgr& aBURMgr, CBTRegistry& aRegistry, TInt aPriority) 
	: CActive(aPriority), 
	iBURMgr(aBURMgr), 
	iRegistry(aRegistry)
	{
	LOG_FUNC

	CActiveScheduler::Add(this);
	}

CBTLocalAddressFetcher::~CBTLocalAddressFetcher()
	{
	LOG_FUNC

	// Cancel any outstanding requests
	Cancel();
	iProperty.Close();
	}

void CBTLocalAddressFetcher::ConstructL()
	{
	LOG_FUNC

	// Attach to KPropertyKeyBluetoothGetLocalDeviceAddress
	LEAVEIFERRORL(iProperty.Attach(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothGetLocalDeviceAddress));
	}

void CBTLocalAddressFetcher::FetchLocalAddress()
	{
	LOG_FUNC

	// Subscribe for local address, in case we need to wait
	SubscribeToGetLocalDeviceAddressKey();	

	// Attempt to read the BT address from KPropertyKeyBluetoothGetLocalDeviceAddress key
	// If key is not found or yields a zero address then try the registry.
	// If the registry also holds a zero address then wait for notification from the P&S key.
	TBuf8<KBTDevAddrSize> btAddrDes;
	TBTDevAddr btAddr;
	TInt err = iProperty.Get(btAddrDes);
	
	if (err != KErrNone)
		{
		// Key does not exist or could not be read.
		// Stack may not be loaded, so now try registry.
		// Ignore any errors; if we can't read this now then we will wait for the stack.
		TRAP_IGNORE(btAddr = GetLocalAddressFromRegistryL());
		}
	else
		{
		// Convert btAddrDes to TBTDevAddr if it has the required size.
		if (btAddrDes.Length() == KBTDevAddrSize)
			{
			btAddr = btAddrDes;	
			}

		if (btAddr == TBTDevAddr())
			{
			// Key exists, so stack is loaded, but address is zero, so now try registry
			// Ignore any errors; if we can't read this now then we will wait for the stack.
			TRAP_IGNORE(btAddr = GetLocalAddressFromRegistryL());
			}
		}
	
	// We have tried our best to get the local address.
	// If this is non-zero, provide to CBTManServerBURMgr instance. 
	// Otherwise, wait on P&S key
	if (btAddr != TBTDevAddr())
		{
		// We have a non-zero address.
		// Cancel subscription and inform CBTManServerBURMgr.
		Cancel();
		iBURMgr.SetLocalAddress(btAddr);
		}	
	}

void CBTLocalAddressFetcher::SubscribeToGetLocalDeviceAddressKey()
	{
	LOG_FUNC

	// Subscribe to KPropertyKeyBluetoothGetLocalDeviceAddress key
	iProperty.Subscribe(iStatus);
	SetActive();
	}

TBTDevAddr CBTLocalAddressFetcher::GetLocalAddressFromRegistryL()
	{
	LOG_FUNC
	
	TBTLocalDevice* regLocalDevice=iRegistry.GetLocalDeviceL();
	TBTDevAddr result=regLocalDevice->Address();
	delete regLocalDevice;
	
	return result;	
	}

void CBTLocalAddressFetcher::RunL()
	{
	LOG_FUNC

	if (iStatus == KErrNone)
		{
		// Obtain address and send to CBTManServerBURMgr
		TBuf8<KBTDevAddrSize> btAddrDes;
		TBTDevAddr btAddr;
		TInt err = iProperty.Get(btAddrDes);
				
		__ASSERT_DEBUG(err == KErrNone, PANIC(KBTBackupPanicCat, EBTBURMgrUnexpectedRPropertyError));
		
		if (err == KErrNone) 
			{
			btAddr = btAddrDes;
			iBURMgr.SetLocalAddress(btAddr);
			}
		}
	}

void CBTLocalAddressFetcher::DoCancel()
	{
	LOG_FUNC

	// Cancel our subscription
	iProperty.Cancel();
	}

/**
CBTActiveBackupDataClient - Active callback implementation (for restore notification)
**/
CBTActiveBackupDataClient* CBTActiveBackupDataClient::NewL(CBTManServerBURMgr& aBURMgr)
	{
	LOG_STATIC_FUNC

	CBTActiveBackupDataClient* result = new (ELeave) CBTActiveBackupDataClient(aBURMgr);
	return result;
	}

CBTActiveBackupDataClient::CBTActiveBackupDataClient(CBTManServerBURMgr& aBURMgr) 
	: iBURMgr(aBURMgr)
	{
	LOG_FUNC
	}

CBTActiveBackupDataClient::~CBTActiveBackupDataClient()
	{
	LOG_FUNC
	}

// Backup methods (not used)
void CBTActiveBackupDataClient::AllSnapshotsSuppliedL()
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}
 
void CBTActiveBackupDataClient::ReceiveSnapshotDataL(TDriveNumber /*aDrive*/, TDesC8& /*aBuffer*/, TBool /*aLastSection*/)
	{
	LOG_FUNC

 	//Not supported
	User::Leave(KErrNotSupported);	
	}
 
TUint CBTActiveBackupDataClient::GetExpectedDataSize(TDriveNumber /*aDrive*/)
	{
	LOG_FUNC

	//Not supported - so expected size can be 0
	return 0;
	}
 
void CBTActiveBackupDataClient::GetSnapshotDataL(TDriveNumber /*aDrive*/, TPtr8& /*aBuffer*/, TBool& /*aFinished*/)
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}
 
void CBTActiveBackupDataClient::InitialiseGetBackupDataL(TDriveNumber /*aDrive*/)
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}
 
void CBTActiveBackupDataClient::GetBackupDataSectionL(TPtr8& /*aBuffer*/, TBool& /*aFinished*/)
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}
         
//Restore methods (only the notification method RestoreComplete() is used)
 
void CBTActiveBackupDataClient::InitialiseRestoreBaseDataL(TDriveNumber /*aDrive*/)
	{
	LOG_FUNC

	//Note that we are doing a base restore.
	User::Leave(KErrNotSupported);
	}
 
void CBTActiveBackupDataClient::RestoreBaseDataSectionL(TDesC8& /*aBuffer*/, TBool /*aFinished*/)
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}	
 
void CBTActiveBackupDataClient::InitialiseRestoreIncrementDataL(TDriveNumber /*aDrive*/)
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}
 
void CBTActiveBackupDataClient::RestoreIncrementDataSectionL(TDesC8& /*aBuffer*/, TBool /*aFinished*/)
	{
	LOG_FUNC

	//Not supported
	User::Leave(KErrNotSupported);
	}
 
void CBTActiveBackupDataClient::RestoreComplete(TDriveNumber aDrive)
	{
	LOG_FUNC

	// Allow CBTManServerBURMgr instance to handle arrival of restore file.
	if (aDrive == RFs::GetSystemDrive())
		{
		iBURMgr.RestoreFileReady();
		}
	}
 
         
//General methods
 
void CBTActiveBackupDataClient::TerminateMultiStageOperation()
	{
	LOG_FUNC

	//Dont care - we will see the operation is cancelled when normal or unset mode is invoked.
	}
 
//Test methods
 
TUint CBTActiveBackupDataClient::GetDataChecksum(TDriveNumber /*aDrive*/)
	{
	LOG_FUNC

	//Return an invariant checksum
	return 0;
	}
