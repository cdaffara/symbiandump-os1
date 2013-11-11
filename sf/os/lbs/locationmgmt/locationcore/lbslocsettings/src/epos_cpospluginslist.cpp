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



#include <f32file.h>
#include <ecom/ecom.h>

#include "epos_modulesinternal.h"
#include "epos_modulesdebug.h"
#include "epos_cpospluginproperties.h"
#include <lbs/epos_cposmoduleidlist.h>
#include "epos_cpospluginslist.h"

// ============================= LOCAL FUNCTIONS =============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void ResetAndDestroy( TAny* aArray )
    {
    (static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt FindEcomPlugin( RImplInfoPtrArray& aArray, TUid aUid )
    {
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        if ( aArray[i]->ImplementationUid().iUid == aUid.iUid )
            {
            return i;
            }
        }
    return KErrNotFound;        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool VersionsMatch( const TVersion& aLeft, const TVersion& aRight )
    {
    return ( aLeft.iMajor == aRight.iMajor &&
    		 aLeft.iMinor == aRight.iMinor &&
    		 aLeft.iBuild == aRight.iBuild );
    }

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginsList::CPosPluginsList( RFs& aFsSession )
    : iFs( aFsSession )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginsList::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginsList* CPosPluginsList::NewL( RFs& aFsSession )
    {
    CPosPluginsList* self = new (ELeave) CPosPluginsList( aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPosPluginsList::~CPosPluginsList()
    {
    iPlugins.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosPluginsList::Find( TPositionModuleId aModuleId ) const
    {
    for ( TInt i = 0; i < iPlugins.Count(); i++ )
		{
		if ( iPlugins[i]->ModuleId().iUid == aModuleId.iUid )
    		{
    		return i;
    		}
		}
	return KErrNotFound;		
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const CPosPluginProperties& CPosPluginsList::Plugin( TInt aIndex ) const
    {
    __ASSERT_DEBUG( aIndex >=0 && aIndex < iPlugins.Count(), Panic( KErrArgument ) );
    return *(iPlugins[aIndex]);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginsList::Remove( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >=0 && aIndex < iPlugins.Count(), Panic( KErrArgument ) );
    CPosPluginProperties* oldPlugin = iPlugins[aIndex];
    iPlugins.Remove( aIndex );
    delete oldPlugin;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginsList::UpdateL()
    {
    LOG("CPosPluginsList::UpdateL in");
    // Get ECom list of plugins
    RImplInfoPtrArray pluginArray;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &pluginArray ) );
    
    REComSession::ListImplementationsL( KPosPsyInterfaceUid, pluginArray );
    
	// Remove plugins, not listed by ECom
	for ( TInt i = iPlugins.Count() - 1; i >=0 ; i-- )
		{
		TInt index = FindEcomPlugin( pluginArray, iPlugins[i]->ModuleId() );
		if ( index == KErrNotFound )
			{
            // plugin has been uninstalled
	    	LOG1("CPosPluginsList::UpdateL, ecom plugin 0x%X has been removed",
	    		iPlugins[i]->ModuleId().iUid);
            Remove( i );
			}
		}

    // Go through the ECom list and add missing or upgraded PSYs to the list
    for ( TInt i = 0; i < pluginArray.Count(); i++ )
        {
        CImplementationInformation* plugin = pluginArray[i];
	    LOG2("CPosPluginsList::UpdateL, ecom plugin found 0x%X ('%S')",
	    	plugin->ImplementationUid().iUid, &(plugin->DisplayName()));

        TInt index = Find( plugin->ImplementationUid() );
        if ( index != KErrNotFound )
            {
            // this plugin already existed
            TVersion newVersion( plugin->Version(), 0, 0 );
            if( VersionsMatch( Plugin( index ).Version(), newVersion ) )
                {
                // nothing changed, skip it
                continue;
                }
           else
                {     
                // new version of this plugin is installed
                // reload new one. New info will be added to the end 
                // of the list as the order of plugins in this list
                // does not affect anything
		    	LOG2("CPosPluginsList::UpdateL, this plugin version changed from %d to %d",
		    		Plugin( index ).Version().iMajor, newVersion.iMajor );
                Remove( index );
                }
            }
		else
			{
		    LOG("CPosPluginsList::UpdateL, found new plugin");
			}            

        // load plugin info from CenRep
        TInt err = KErrNone;
        CPosPluginProperties* newPlugin = NULL;
        TRAP( err, newPlugin = CPosPluginProperties::NewL( *plugin, iFs ) );
        if ( !err )
            {
            CleanupStack::PushL( newPlugin );
            iPlugins.AppendL( newPlugin );
            CleanupStack::Pop( newPlugin );
            }
		else
			{
		    LOG1("CPosPluginsList::UpdateL, failed to load properties, err %d", err);
			}            
        }
        
    CleanupStack::PopAndDestroy();  // pluginArray
    LOG("CPosPluginsList::UpdateL out");
    }

// ---------------------------------------------------------------------------
// Required order is following:
//	1) ROM-based plugins
//	2) Post installed plugins
//	3) Others
// ---------------------------------------------------------------------------
//
void CPosPluginsList::SortAsInstalledL( CPosModuleIdList& aPostInstalledList )
	{
	// 1. Move postInstalled to beginning with same order as in
	// input list (installation order)
	TInt postInstalledOffset = 0;
	for ( TInt i = 0; i < aPostInstalledList.Count(); i++ )
		{
		TInt index = Find( aPostInstalledList[i] );
	    MoveL( index, postInstalledOffset++ );
		}

	// 2. Move RomBased before postInstalled
	TInt romBasedOffset = 0;
	for ( TInt i = postInstalledOffset; i < iPlugins.Count(); i++ )
		{
		CPosPluginProperties* plugin = iPlugins[i];
		if ( plugin->IsRomBased() )
			{
			MoveL( i, romBasedOffset++ );
			} 
		}

	// 3. Other plugins ( postinstalled, but not listed as such )		
	// are left in the end of the list as listed by ECom
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CPosPluginsList::Count() const
	{
	return iPlugins.Count();
	}

// ========================= INTERNAL FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPosPluginsList::MoveL( TInt aOldIndex, TInt aNewIndex )
	{
	__ASSERT_DEBUG( aOldIndex >=0 && aOldIndex < iPlugins.Count(), Panic( KErrArgument ) );
	__ASSERT_DEBUG( aNewIndex >=0 && aNewIndex <= iPlugins.Count(), Panic( KErrArgument ) );

	CPosPluginProperties* swap = iPlugins[aOldIndex];
	iPlugins.Remove( aOldIndex );
	iPlugins.InsertL( swap, aNewIndex );
	}
