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



#include <centralrepository.h>

#include "epos_modulesinternal.h"
#include "epos_modulesdebug.h"
#include "epos_cposmodulessettings.h"
#include "epos_cpospostinstalledlist.h"
#include "epos_cposmodulescenrepmanager.h"

// ======================== LOCAL FUNCTIONS ==================================

// ======================== MEMBER FUNCTIONS ==================================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesCenRepManager::CPosModulesCenRepManager()
  : CActive( CActive::EPriorityStandard )
	{
    CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesCenRepManager* CPosModulesCenRepManager::NewL()
    {
    CPosModulesCenRepManager* self = new (ELeave) CPosModulesCenRepManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::ConstructL()
    {
    // Open the Central Repository
    iRepository = CRepository::NewL( KCRUidLocationSettings );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesCenRepManager::~CPosModulesCenRepManager()
	{
    if ( iObserver )
    	{
    	Panic( EPosLocSetPanicObserverNotStopped );
    	}
    delete iRepository;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::InitWorkingAndResetFactoryKeysL()
    {
    // 1. Check working keys
    //  if empty, fill with values from factory keys
    InitWorkingKeysL();
    
    // 2. Reset factory keys
    ResetFactoryKeysL();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings* CPosModulesCenRepManager::LoadSettingsL()
	{
	return LoadSettingsL( 
		KLocSettingsPriorityListWorking,
		KLocSettingsInvisibleListWorking,
		KLocSettingsVersionWorking );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings* CPosModulesCenRepManager::LoadFactorySettingsL()
	{
	return LoadSettingsL( 
		KLocSettingsPriorityListFactory,
		KLocSettingsInvisibleListFactory,
		KLocSettingsVersionFactory );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPostInstalledList* CPosModulesCenRepManager::LoadPostInstalledListL()
	{
	// post-installed list
	HBufC* postInstalled = ReadKeyLC( KLocSettingsInstalledPluginsList );
	CPosPostInstalledList* list = CPosPostInstalledList::NewL( *postInstalled );
	CleanupStack::PopAndDestroy( postInstalled );
	return list;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::SaveSettingsL( const CPosModulesSettings& aSettings )
	{
	HBufC* priorityKeyBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
	HBufC* invisibleKeyBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );

	TPtr priorityPtr( priorityKeyBuf->Des() );
	TPtr invisiblePtr ( invisibleKeyBuf->Des() );
	aSettings.FormatL( priorityPtr, invisiblePtr );

	WriteKeyL( KLocSettingsPriorityListWorking, *priorityKeyBuf );
	WriteKeyL( KLocSettingsInvisibleListWorking, *invisibleKeyBuf );
	WriteKeyL( KLocSettingsVersionWorking, aSettings.Version() );

	CleanupStack::PopAndDestroy( invisibleKeyBuf );
	CleanupStack::PopAndDestroy( priorityKeyBuf );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::SavePostInstalledListL( const CPosPostInstalledList& aIdList )
	{
	HBufC* keyBuf = aIdList.FormatLC();
	WriteKeyL( KLocSettingsInstalledPluginsList, *keyBuf );
	CleanupStack::PopAndDestroy( keyBuf );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::StartTransactionLC()
	{
    LOG("LocSets: StartTransaction");

	TInt err = iRepository->StartTransaction( CRepository::EConcurrentReadWriteTransaction );

    // guaranteed to succeed with this parameter, but there is IPC behind it,
    // so check for error is still needed
	User::LeaveIfError( err );

	iRepository->CleanupRollbackTransactionPushL();
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosModulesCenRepManager::CommitTransactionX()
	{
    LOG("LocSets: CommitTransaction");
	CleanupStack::Pop(); // rollback transaction item

	TUint32 temp;
	return iRepository->CommitTransaction( temp );
	}
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::CancelTransactionX()
    {
    LOG("LocSets: CancelTransaction");
    CleanupStack::Pop(); // rollback transaction item

    iRepository->CancelTransaction();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::StartListeningL( MPosCenrepObserver& aObserver )
    {
    if ( iObserver )
        {
        Panic( EPosLocSetPanicObserverIsAlreadySet );
        }

    iObserver = &aObserver;
	ListenCenrepEvents();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::StopListening()
	{
	Cancel();
	iObserver = NULL;
	}

// ======================== INTERNAL FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::RunL()
    {
    LOG("LocSets: CenRep changed");
    ListenCenrepEvents(); // continue listening
    if ( iObserver )
        {
        iObserver->HandleRepoChangeL();
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::DoCancel()
    {
    iRepository->NotifyCancel( KLocSettingsPriorityListWorking );
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CPosModulesCenRepManager::RunError( TInt aError )
    {
    LOG1("CenRep event handle error %d", aError);
    return KErrNone;
    }
#else
TInt CPosModulesCenRepManager::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
#endif

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::ListenCenrepEvents()
    {
    iRepository->NotifyRequest( KLocSettingsPriorityListWorking, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HBufC* CPosModulesCenRepManager::ReadKeyLC( TUint32 aKeyId )
	{
    HBufC* keyBuf = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr key = keyBuf->Des();

	TInt err = iRepository->Get( aKeyId, key );
	LOG3("LocSets: ReadKeyLC id: %d, val: '%S', err: %d", aKeyId, &key, err);

	if ( err != KErrAbort )
		{
		User::LeaveIfError( err );
		}
    return keyBuf;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::WriteKeyL( TUint32 aKeyId, const TDesC& aValue )
	{
    TInt err = iRepository->Set( aKeyId, aValue );
	LOG3("WriteKeyL, id: %d, val: '%S', err '%d'", aKeyId, &aValue, err);
	if ( err != KErrAbort )
		{
		User::LeaveIfError( err );
		}
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::InitWorkingKeysL()
	{
    TInt err = KErrLocked;
    while ( err == KErrLocked )
    	{
		StartTransactionLC();
		
		// If working priority list is empty, copy factory values
        // of priority and invisible lists to working keys
        // That happens after backup restore from old implementation of this component
        
		HBufC* working = ReadKeyLC( KLocSettingsPriorityListWorking );
		if ( working->Length() == 0 )
			{
            LOG("LocSets: Copying factory settings to working keys");
			HBufC* factory = ReadKeyLC( KLocSettingsPriorityListFactory );
			WriteKeyL( KLocSettingsPriorityListWorking, *factory );
			CleanupStack::PopAndDestroy( factory );

            factory = ReadKeyLC( KLocSettingsInvisibleListFactory );
            WriteKeyL( KLocSettingsInvisibleListWorking, *factory );
            CleanupStack::PopAndDestroy( factory );

            // version key is copied only when factory priority list is integrated

            CleanupStack::PopAndDestroy( working );
            err = CommitTransactionX();
            }
        else
            {
            CleanupStack::PopAndDestroy( working );
            CancelTransactionX();
            break;
            }
    	}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesCenRepManager::ResetFactoryKeysL()
	{
#ifdef _DEBUG	
	TTime start;
	start.UniversalTime();
#endif	
	
    TInt err = KErrLocked;
	while ( err == KErrLocked )
		{
		err = iRepository->Reset( KLocSettingsPriorityListFactory );
		}
    User::LeaveIfError( err );

    err = KErrLocked;
	while ( err == KErrLocked )
		{
		err = iRepository->Reset( KLocSettingsInvisibleListFactory );
		}
    User::LeaveIfError( err );
    
    err = KErrLocked;
    while ( err == KErrLocked )
        {
        err = iRepository->Reset( KLocSettingsVersionFactory );
        }
    User::LeaveIfError( err );
    
#ifdef _DEBUG
    TTime now;
    now.UniversalTime();
    
    TTimeIntervalMicroSeconds interval = now.MicroSecondsFrom( start );
    LOG1("LocSets: Reset factory keys in %ld us", interval.Int64() );
#endif    
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings* CPosModulesCenRepManager::LoadSettingsL(
	TInt32 aPrioritiesKey, 
	TInt32 aInvisibleKey,
	TInt32 aVersionKey )
	{
	// priority list
	HBufC* priorities = ReadKeyLC( aPrioritiesKey );
    
	// invisible list
	HBufC* invisible = ReadKeyLC( aInvisibleKey );
	
	// version string
	HBufC* version = ReadKeyLC( aVersionKey );

	CPosModulesSettings* settings = CPosModulesSettings::NewL( *priorities, *invisible, *version );

	CleanupStack::PopAndDestroy( version );
	CleanupStack::PopAndDestroy( invisible );
	CleanupStack::PopAndDestroy( priorities );
	return settings;
	}

