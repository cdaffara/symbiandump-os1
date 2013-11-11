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



#include <lbscommon.h>
#include <centralrepository.h>
#include <ecom/ecom.h>

#include "LocationSettingsPrivateCRKeys.h"

#include "epos_modulesinternal.h"
#include "epos_modulesdebug.h"
#include <lbs/epos_mposmodulesobserver.h>
#include "epos_cpospluginslist.h"
#include "epos_cpospluginproperties.h"
#include "epos_cposmodulessettings.h"
#include "epos_cpospostinstalledlist.h"
#include <lbs/epos_cposmoduleupdate.h>
#include "epos_cposmodulescenrepmanager.h"
#include "EPos_CPosModulesEngine.h"

// =========================== DEBUG TRACE ===================================

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesEngine::CPosModulesEngine() 
	: CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesEngine* CPosModulesEngine::NewL()
    {
    CPosModulesEngine* self = new (ELeave) CPosModulesEngine;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::ConstructL()
    {
    LOG("CPosModulesEngine::ConstructL in");
    User::LeaveIfError( iFs.Connect() );
    iEcom = &( REComSession::OpenL() );
    iPlugins = CPosPluginsList::NewL( iFs );
    iCenRepManager = CPosModulesCenRepManager::NewL();
    
    TInt err = KErrNone;
    TRAP( err, iCenRepManager->InitWorkingAndResetFactoryKeysL() );
    if ( err == KErrNone )
    	{
        SynchronizeL();
        ListenEcomEvents();
    	}
    else if ( err != KErrPermissionDenied )
        {
        User::Leave( err );
        }
    else
        {
        // if InitWorkingAndResetFactoryKeysL failed with KErrPermissionDenied,
        // it means that current client does not have enough capabilities to init 
        // and write settings. This client is left in read-only mode:
        // 1) It will not listen to ECom events, because it cannot update
        // settings anyway. 
        // 2) Before synchronization is done by "more powerful" client, this one
        // may receive not up-to-date settings
        // 3) It will receive CenRep events whenever settings change
        }
    LOG("CPosModulesEngine::ConstructL out");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesEngine::~CPosModulesEngine()
    {
    LOG("CPosModulesEngine::~CPosModulesEngine in");
    Cancel();
	delete iPlugins;
	delete iCenRepManager;
	
	delete iPreviousSettings;
	delete iSettingsCache;
    
    iFs.Close();
    if ( iEcom )
        {
        iEcom->Close();
        }
    REComSession::FinalClose(); // clean ecom session

    LOG("CPosModulesEngine::~CPosModulesEngine out");
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::SetObserverL( MPosModulesObserver& aObserver )
	{
    if ( iObserver )
        {
        Panic( EPosLocSetPanicObserverIsAlreadySet );
        }

	__ASSERT_DEBUG( !iPreviousSettings, Panic( KErrGeneral ) );

    iPreviousSettings = ReadSettingsL();

    iObserver = &aObserver;
	iCenRepManager->StartListeningL( *this );
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::RemoveObserver()
	{
	delete iPreviousSettings;
	iPreviousSettings = NULL;

	iCenRepManager->StopListening();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::GetModuleInfoL(
    TPositionModuleId aModuleId,
    TPositionModuleInfo& aInfo )
    {
	//LOG1("CPosModulesEngine::GetModuleInfoL in, id = 0x%08X", aModuleId.iUid);
    TInt pluginIndex = iPlugins->Find( aModuleId );
    if ( pluginIndex == KErrNotFound ) 
		{
		User::Leave( KErrNotFound );
		}
		
	iPlugins->Plugin( pluginIndex ).GetModuleInfo( aInfo );	

	CPosModulesSettings::TPosModuleAttributes attributes;
	if ( SettingsCacheL().GetAttributes( aModuleId, attributes ) == KErrNone )
		{
		aInfo.SetIsAvailable( attributes.iAvailable );
		
		TPositionQuality quality;
		aInfo.GetPositionQuality( quality );
		quality.SetCostIndicator( attributes.iCost );
		aInfo.SetPositionQuality( quality );
		// visibility flag is not available in TPositionModuleInfo class
		}
	else
		{
		User::Leave( KErrNotFound );
		}

	//LOG("CPosModulesEngine::GetModuleInfoL out");
    }
 
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModuleIdList* CPosModulesEngine::GetPriorityListLC()
	{
	LOG("CPosModulesEngine::GetPriorityListLC in");
	CPosModuleIdList* list = SettingsCacheL().GetPriorityListLC();
	LOG1("CPosModulesEngine::GetPriorityListLC out, %d items in list", list->Count());
	return list;
	}

// ---------------------------------------------------------
// ---------------------------------------------------------
//
TBool CPosModulesEngine::IsModuleInstalled( TPositionModuleId aModuleId )
    {
    return ( iPlugins->Find( aModuleId ) != KErrNotFound );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
//
TBool CPosModulesEngine::IsModuleVisibleL( TPositionModuleId aModuleId )
    {
    if ( !IsModuleInstalled( aModuleId ) )
		{
		User::Leave( KErrNotFound );
		} 

	TBool visible = SettingsCacheL().IsVisible( aModuleId );
	return visible;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::SetModulePriorityL( TPositionModuleId aModuleId, TInt aNewPriority )
	{
	TInt err = KErrLocked;
	while ( err == KErrLocked )
	    {
    	iCenRepManager->StartTransactionLC();
    	CPosModulesSettings* settings = iCenRepManager->LoadSettingsL();
    	CleanupStack::PushL( settings );
    	
    	CPosModuleIdList* list = settings->GetPriorityListLC();
    	
    	if ( aNewPriority < 0 || aNewPriority >= list->Count() ) 
    		{
    		User::Leave( KErrArgument );
    		}
    	
    	TInt index = list->Find( aModuleId );
    	if ( index == KErrNotFound ) 
    		{
    		User::Leave( KErrNotFound );
    		}
    	
    	list->MoveL( index, aNewPriority );
    	
    	settings->SetPrioritiesL( *list );
    	
    	iCenRepManager->SaveSettingsL( *settings );

    	CleanupStack::PopAndDestroy( list );
    	CleanupStack::PopAndDestroy( settings );

    	err = iCenRepManager->CommitTransactionX();
	    }
	User::LeaveIfError( err );    
	ResetSettingsCache();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::SetModulePriorityL( TInt aOldPriority, TInt aNewPriority )
	{
	TInt err = KErrLocked;
	while ( err == KErrLocked )
	    {
    	iCenRepManager->StartTransactionLC();
    	CPosModulesSettings* settings = iCenRepManager->LoadSettingsL();
    	CleanupStack::PushL( settings );
    	
    	CPosModuleIdList* list = settings->GetPriorityListLC();
    	
    	if ( aOldPriority < 0 || aOldPriority >= list->Count() ||
    		 aNewPriority < 0 || aNewPriority >= list->Count() ) 
    		{
    		User::Leave( KErrArgument );
    		}
    	
    	list->MoveL( aOldPriority, aNewPriority );
    	
    	settings->SetPrioritiesL( *list );
    	
    	iCenRepManager->SaveSettingsL( *settings );

    	CleanupStack::PopAndDestroy( list );
    	CleanupStack::PopAndDestroy( settings );

    	err = iCenRepManager->CommitTransactionX();
	    }
	User::LeaveIfError( err );
	ResetSettingsCache();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::SetModulePrioritiesL( const CPosModuleIdList& aModuleIdList )
	{
	TInt err = KErrLocked;
	while ( err == KErrLocked )
	    {
    	iCenRepManager->StartTransactionLC();
    	CPosModulesSettings* settings = iCenRepManager->LoadSettingsL();
    	CleanupStack::PushL( settings );
    	
    	settings->SetPrioritiesL( aModuleIdList );
    	
    	iCenRepManager->SaveSettingsL( *settings );
    	CleanupStack::PopAndDestroy( settings );

    	err = iCenRepManager->CommitTransactionX();
	    }
	User::LeaveIfError( err );
	ResetSettingsCache();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosModulesEngine::PriorityL( TPositionModuleId aModuleId )
    {
    CPosModuleIdList* list = GetPriorityListLC();
    TInt index = list->Find( aModuleId );
    CleanupStack::PopAndDestroy( list );
    if ( index < 0 )
        {
        User::Leave( KErrNotFound );
        }
    return index;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::UpdateModuleL( 
	TPositionModuleId aModuleId, 
	const CPosModuleUpdate& aUpdate )
	{
	LOG1("UpdateModuleL in, id 0x%X", aModuleId.iUid);
	TInt index = iPlugins->Find( aModuleId );	
	if ( index == KErrNotFound )
		{
		User::Leave( KErrNotFound );
		}
		
	TInt err = KErrLocked;
	while ( err == KErrLocked )
	    {
	    iCenRepManager->StartTransactionLC();

        CPosModulesSettings* settings = iCenRepManager->LoadSettingsL();
        CleanupStack::PushL( settings );

	    DoUpdateSettingsL( aModuleId, aUpdate, *settings );

    	iCenRepManager->SaveSettingsL( *settings );
    	CleanupStack::PopAndDestroy( settings );
    	
	    err = iCenRepManager->CommitTransactionX();
	    }
	User::LeaveIfError( err );
	ResetSettingsCache();
	LOG("UpdateModuleL out");
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::SynchronizeL()
    {
    LOG("SynchronizeL in");

	ResetSettingsCache();

    // 1. Reload PSY list from ECom
    iPlugins->UpdateL(); 

	TInt err = KErrLocked;
	while ( err == KErrLocked )
	    {
    	iCenRepManager->StartTransactionLC();
    	
    	CPosPostInstalledList* postInstalledList = iCenRepManager->LoadPostInstalledListL();
    	CleanupStack::PushL( postInstalledList );
    	
    	RemoveUninstalledPluginsL( *postInstalledList );
    	iPlugins->SortAsInstalledL( *postInstalledList );

    	CPosModulesSettings* settings = iCenRepManager->LoadSettingsL();
    	CleanupStack::PushL( settings );

    	CPosModulesSettings* factory = iCenRepManager->LoadFactorySettingsL();
    	CleanupStack::PushL( factory );
    	
    	TBool isSyncWithFactoryNeeded = 
    		NewRomBasedPluginsFound( *settings ) ||
    		settings->Version() != factory->Version();
    	
        if ( isSyncWithFactoryNeeded )
        	{
        	// use factory settings as base for building new list of PSYs.

            // 2. Remove uninstalled PSYs from settings
        	RemoveUninstalledPluginsL( *factory );

        	// 3. Add new plugins in installation order
        	AddNewPluginsL( *factory, *postInstalledList );

        	// 4. Save changes
        	// copy availability and cost from current settings
        	factory->CopyAttributesL( *settings ); 

        	iCenRepManager->SaveSettingsL( *factory ); // factory-set version will be stored
        	}
        else
        	{
        	// use current settings as base for building new list of PSYs
        	
            // 2. Remove uninstalled PSYs from settings
        	RemoveUninstalledPluginsL( *settings );

        	// 3. Add new plugins in installation order
        	AddNewPluginsL( *settings, *postInstalledList );

        	// 4. Save changes
        	iCenRepManager->SaveSettingsL( *settings );
        	}

    	iCenRepManager->SavePostInstalledListL( *postInstalledList );

	    CleanupStack::PopAndDestroy( factory );
    	CleanupStack::PopAndDestroy( settings );
    	CleanupStack::PopAndDestroy( postInstalledList );

    	err = iCenRepManager->CommitTransactionX();
	    }
	User::LeaveIfError( err );
    LOG("SynchronizeL out");
    }

// ========================= INTERNAL FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::RunL()
    {
    LOG("ECOM event");
    ListenEcomEvents(); // continue listening
    SynchronizeL();
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::DoCancel()
    {
    iEcom->CancelNotifyOnChange( iStatus );
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CPosModulesEngine::RunError( TInt aError )
    {
    LOG1("ECOM event handle error %d", aError);
    return KErrNone;
    }
#else
TInt CPosModulesEngine::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }
#endif

// ---------------------------------------------------------------------------
// From MPosCenrepObserver
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::HandleRepoChangeL()
    {
	ResetSettingsCache();

    CPosModulesSettings* oldSettings = iPreviousSettings;
    iPreviousSettings = ReadSettingsL();
    CleanupStack::PushL( oldSettings );

	RArray<TPosModulesEvent> events;
	CleanupClosePushL( events );
	iPreviousSettings->CompareL( *oldSettings, events );

    NotifyObserver( events );

    CleanupStack::PopAndDestroy( &events );
    CleanupStack::PopAndDestroy( oldSettings );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const CPosModulesSettings& CPosModulesEngine::SettingsCacheL()
    {
	if ( !iSettingsCache )
	    {
	    iSettingsCache = ReadSettingsL();
	    }
    return *iSettingsCache;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::ResetSettingsCache()
    {
	delete iSettingsCache;
	iSettingsCache = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosModulesSettings* CPosModulesEngine::ReadSettingsL()
	{
	CPosModulesSettings* settings = NULL;
    TInt err = KErrLocked;
    while ( err == KErrLocked ) // CenRep guarantees that sometime it will be completed
        {
    	iCenRepManager->StartTransactionLC();
    	settings = iCenRepManager->LoadSettingsL();
    	err = iCenRepManager->CommitTransactionX();
    	if ( err )
    	    {
    	    delete settings;
    	    settings = NULL;
    	    }
        }
	User::LeaveIfError( err ); // if other error than KErrLocked
	return settings;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::ListenEcomEvents()
    {
    LOG("CPosModulesEngine::Listen");
    iEcom->NotifyOnChange( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::ResortModuleL( 
	TPositionModuleId aModuleId, 
	CPosModulesSettings& aSettings )
	{
	LOG1("ResortModuleL in, id 0x%X", aModuleId.iUid);
	const CPosPluginProperties& plugin = iPlugins->Plugin( iPlugins->Find( aModuleId ) );

	CPosModuleIdList* list = aSettings.GetPriorityListLC();
	TInt oldPriority = list->Find( aModuleId );
	TInt newPriority = list->Count() - 1; // last
	
	if ( plugin.PreferredPriority() == CPosPluginProperties::EModulePriorityAuto )
		{
		for ( TInt i = 0; i < list->Count(); i++ )
			{
			if ( !( aModuleId == (*list)[i] ) &&
			     IsLeftBetterThanRight( aModuleId, (*list)[i], aSettings ) )
				{
				newPriority = i;
				break;
				}
			}
		}
	else if ( plugin.PreferredPriority() == CPosPluginProperties::EModulePriorityFirst )	
		{
		newPriority = 0;	
		}
	else // EModulePriorityLast
		{
		// just goes last
		}		

	list->MoveL( oldPriority, newPriority );	
	
	aSettings.SetPrioritiesL( *list );
	CleanupStack::PopAndDestroy( list );
	LOG1("ResortModuleL out, new priority %d", newPriority);
	}    
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosModulesEngine::IsLeftBetterThanRight(
	TPositionModuleId aLeftModuleId, 
	TPositionModuleId aRightModuleId,
	CPosModulesSettings& aSettings )
	{
	LOG2("Comparing Modules 0x%X and 0x%X", aLeftModuleId.iUid, aRightModuleId.iUid);
	const CPosPluginProperties& leftPlugin = iPlugins->Plugin( iPlugins->Find( aLeftModuleId ) );
	const CPosPluginProperties& rightPlugin = iPlugins->Plugin( iPlugins->Find( aRightModuleId ) );
	
	TPositionModuleInfo leftInfo;
	leftPlugin.GetModuleInfo( leftInfo );
	TPositionQuality leftQuality;
	leftInfo.GetPositionQuality( leftQuality );
	
	TPositionModuleInfo rightInfo;
	rightPlugin.GetModuleInfo( rightInfo );
	TPositionQuality rightQuality;
	rightInfo.GetPositionQuality( rightQuality );

	CPosModulesSettings::TPosModuleAttributes leftItem;
	aSettings.GetAttributes( aLeftModuleId, leftItem );
	TPositionQuality::TCostIndicator leftCost = leftItem.iCost;
	
	CPosModulesSettings::TPosModuleAttributes rightItem;
	aSettings.GetAttributes( aRightModuleId, rightItem );
	TPositionQuality::TCostIndicator rightCost = rightItem.iCost;

	LOG2("Comparing Modules' cost: %d and %d", leftCost, rightCost);

    // Compare Cost: less is better
    if ( rightCost != TPositionQuality::ECostUnknown )
        {
        // if cost is Unknown left one is worse
        if ( leftCost == TPositionQuality::ECostUnknown ||
        	 leftCost > rightCost )
            {
            return EFalse;
            }
        }

    // Compare HAcc: less is better
    TReal leftHAcc = leftQuality.HorizontalAccuracy();
    TReal rightHAcc = rightQuality.HorizontalAccuracy();

	LOG2("Comparing Modules' HAcc: %f and %f", leftHAcc, rightHAcc);

	// NaN is considered worse
	if ( !Math::IsNaN( rightHAcc ) )
		{
	    if ( Math::IsNaN( leftHAcc ) || leftHAcc > rightHAcc )
	        {
	        return EFalse;
	        }
		}

    // Compare time to next fix: less is better
	// Negative is considered unknown and thus worse
	
    const TInt64 leftTTNF = leftQuality.TimeToNextFix().Int64();
    const TInt64 rightTTNF = rightQuality.TimeToNextFix().Int64();

	LOG2("Comparing Modules' TTNF: %ld and %ld", leftTTNF, rightTTNF);

    if ( rightTTNF >= 0 )
        {
        if ( leftTTNF < 0 || leftTTNF > rightTTNF )
        	{
	        return EFalse;
        	}
        }

	LOG("Comparing Modules result: left is better!");
    return ETrue;
	}	
		
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::FillModuleAttributes( 
	const CPosPluginProperties& aPlugin, 
	CPosModulesSettings::TPosModuleAttributes& aModule )
	{
	aModule.iVisible = aPlugin.IsVisible();
	
	TPositionModuleInfo info;
	aPlugin.GetModuleInfo( info );
	aModule.iAvailable = info.IsAvailable();

	TPositionQuality quality;
	info.GetPositionQuality( quality );
	aModule.iCost = quality.CostIndicator();
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::NotifyObserver( const RArray<TPosModulesEvent>& aEvents )
    {
    TRAP_IGNORE(UpdateLastWorkingGPSPsyKeyL());
    if ( iObserver )
        {
        for ( TInt i = 0; i < aEvents.Count(); i++ )
            {
            LOG2("Notification to client: event %d,uid 0x%X", 
            	aEvents[i].iType, aEvents[i].iModuleId.iUid);
            TRAP_IGNORE( iObserver->HandleSettingsChangeL( aEvents[i] ) );
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::DoUpdateSettingsL(
	TPositionModuleId aModuleId, 
	const CPosModuleUpdate& aUpdate, 
	CPosModulesSettings& aSettings )
    {
	CPosModulesSettings::TPosModuleAttributes attributes;
	if ( aSettings.GetAttributes( aModuleId, attributes ) != KErrNotFound )
		{
		LOG3("UpdateModuleL, current: status %d, cost %d, visible %d", 
			attributes.iAvailable, attributes.iCost, attributes.iVisible);
		if ( aUpdate.UpdateMap() & KPosModUpdateAvailability )
			{
			attributes.iAvailable = aUpdate.AvailabilityUpdate();
			LOG1("UpdateModuleL new status %d", attributes.iAvailable);
			}
		if ( aUpdate.UpdateMap() & KPosModUpdateCost )
			{
			attributes.iCost = aUpdate.CostUpdate();
			LOG1("UpdateModuleL new cost %d", attributes.iCost);
			}
			
		TBool visibilityChanged = EFalse;	
		if ( aUpdate.UpdateMap() & KPosModUpdateVisibility &&
			 attributes.iVisible != aUpdate.VisibilityUpdate() )
			{
			attributes.iVisible = aUpdate.VisibilityUpdate();
			visibilityChanged = ETrue;
			LOG1("UpdateModuleL new visibility %d", attributes.iVisible);
			}

		aSettings.SetAttributesL( aModuleId, attributes );	

		if ( visibilityChanged && attributes.iVisible )
			{
			// module moved from invisible to priority list
			// it needs new priority
			ResortModuleL( aModuleId, aSettings );
			}
		}
	else
		{
		User::Leave( KErrNotFound );
		}	
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::RemoveUninstalledPluginsL( CPosModulesSettings& aSettings )
    {
	CPosModuleIdList* list = aSettings.GetInstalledListLC();
	for ( TInt i = 0; i < list->Count(); i++ )
		{
		TPositionModuleId id = (*list)[i];
		if ( iPlugins->Find( id ) == KErrNotFound )
			{
			aSettings.Remove( id );
			}
		}
	CleanupStack::PopAndDestroy( list );
    }
	
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::RemoveUninstalledPluginsL( CPosPostInstalledList& aPostInstalled )
	{
	for ( TInt i = aPostInstalled.Count() -1 ; i >= 0 ; i-- )
		{
		TPositionModuleId id = aPostInstalled[i];
		TInt index = iPlugins->Find( id );
		if ( index == KErrNotFound ||
			iPlugins->Plugin( index ).IsRomBased() )
			{
			aPostInstalled.Delete( i );
			}
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CPosModulesEngine::NewRomBasedPluginsFound( CPosModulesSettings& aSettings )
	{
	for ( TInt i = 0; i < iPlugins->Count(); i++ )
		{
		const CPosPluginProperties& plugin = iPlugins->Plugin( i );
		TPositionModuleId id = plugin.ModuleId();

		if ( plugin.IsRomBased() && !aSettings.ModuleExists( id ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosModulesEngine::AddNewPluginsL( 
	CPosModulesSettings& aSettings, 
	CPosPostInstalledList& aPostInstalledList )
	{
	for ( TInt i = 0; i < iPlugins->Count(); i++ )
		{
		const CPosPluginProperties& plugin = iPlugins->Plugin( i );
		TPositionModuleId id = plugin.ModuleId();

		if ( !aSettings.ModuleExists( id ) )
			{
			CPosModulesSettings::TPosModuleAttributes item;
			FillModuleAttributes( plugin, item );
			aSettings.AddL( id, item );
			if ( item.iVisible )
			    {
    			ResortModuleL( id, aSettings );
			    }
			}

		if ( !plugin.IsRomBased() &&
			 aPostInstalledList.Find( id ) == KErrNotFound )
			{
			aPostInstalledList.AppendL( id );
			}
		}
	}
	
void CPosModulesEngine::UpdateLastWorkingGPSPsyKeyL()
	{
		const TUid KCRUidDefaultProxyConfiguration = { 0x101f7a7f };
		const TInt KPosMaximumLastWorkingGpsPsyUidStringLength = 24;
		const TUint32 KDefaultProxyLastWorkingGpsPsy = 0x10000001;
		const TInt KPosPsyUidWidth = 8;
		
	    CRepository* repository = CRepository::NewL( KCRUidDefaultProxyConfiguration );
	    CleanupStack::PushL(repository);
	    TBuf< KPosMaximumLastWorkingGpsPsyUidStringLength > idBuf;
	    idBuf.AppendNumFixedWidth( 0, EHex, KPosPsyUidWidth );
	    repository->Set(KDefaultProxyLastWorkingGpsPsy,idBuf ); //error ignored
	    CleanupStack::PopAndDestroy(repository);
	}
