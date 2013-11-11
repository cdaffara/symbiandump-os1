/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  PSM Settings handler
*
*/


#include <ecom/ecom.h>
#include <centralrepository.h>
#include "psmsettingshandler.h"
#include "psmconfigstorage.h"
#include "psmbackupstorage.h"
#include "psmcenrepstorage.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CloseHandler
// closes the storage handler
// -----------------------------------------------------------------------------
//
void CleanupStorage(TAny* aPtr)
    {
    CPsmStorage* self = static_cast<CPsmStorage*> (aPtr);
    //Ignore the Trap as Cleanup can't leave.
    TRAP_IGNORE(self->CloseStorageL());
    }

// -----------------------------------------------------------------------------
// CloseHandler
// closes the backup storage handler
// -----------------------------------------------------------------------------
//
void CleanupBackupStorage(TAny* aPtr)
    {
    CPsmBackupStorage* self = static_cast<CPsmBackupStorage*> (aPtr);
    //Ignore the Trap as Cleanup can't leave.
    TRAP_IGNORE(self->CloseStorageL());
    }
	
// CPsmSettingsHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSettingsHandler* CPsmSettingsHandler::NewL( const TPsmsrvMode aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::NewL()" ) ) );

    CPsmSettingsHandler* self = CPsmSettingsHandler::NewLC( aMode );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmSettingsHandler* CPsmSettingsHandler::NewLC( const TPsmsrvMode aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::NewLC()" ) ) );

    CPsmSettingsHandler* self = new( ELeave ) CPsmSettingsHandler( aMode );
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::CPsmSettingsHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmSettingsHandler::CPsmSettingsHandler( const TPsmsrvMode aMode ) :
    iMode( aMode ),
    iCurrentMode( aMode )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ConstructL()" ) ) );
    User::LeaveIfError( iFileSession.Connect() );
    // Create storage files
    iConfigStorage = CPsmConfigStorage::NewL( iMode, iFileSession );
    iBackupStorage = CPsmBackupStorage::NewL( iMode, iFileSession );
    iCenRepStorage = CPsmCenRepStorage::NewL( iMode, iFileSession );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmSettingsHandler::~CPsmSettingsHandler()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::~CPsmSettingsHandler()" ) ) );

    delete iConfigStorage;
    delete iBackupStorage;
    delete iCenRepStorage;

    iFileSession.Close();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::~CPsmSettingsHandler - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::BackupAndGetSettingsL
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::BackupAndGetSettingsL( 
    RConfigInfoArray& aPsmConfigArray, TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::BackupAndGetSettingsL()" ) ) );
    //Close config handle in case of any error
    CleanupStack::PushL(TCleanupItem(CleanupStorage, iConfigStorage));
    // Change settings from normal config storage
    iConfigStorage->InitStorageL( aRepository );
    ChangeSettingsL( aPsmConfigArray, aRepository, *iConfigStorage );
    iConfigStorage->CloseStorageL();
    CleanupStack::Pop(iConfigStorage);
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::BackupAndGetSettingsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::GetCurrentSettingsL
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::GetSettingsL( 
    RConfigInfoArray& aPsmConfigArray, TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::GetCurrentSettingsL()" ) ) );
    // Get current settings from normal config storage

    // Use own storage in case PSM mode is ongoing by some plugin
    if ( EPsmsrvModeNormal == iMode )
        {
        // Normal mode, get current settings from backup storage
        CPsmBackupStorage* backupStorage = CPsmBackupStorage::NewLC( iMode, iFileSession );
        backupStorage->InitStorageL( aRepository );
        backupStorage->GetConfigurationL( aPsmConfigArray );
        CleanupStack::PopAndDestroy( backupStorage );
        }
    else
        {
        // PSM mode, get current settings from config storage
        CPsmConfigStorage* configStorage = CPsmConfigStorage::NewLC( iMode, iFileSession );
        configStorage->InitStorageL( aRepository );
        configStorage->GetConfigurationL( aPsmConfigArray );
        CleanupStack::PopAndDestroy( configStorage );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::GetCurrentSettingsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::BackupSettingsL
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::BackupSettingsL( 
    const RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::BackupSettingsL()" ) ) );

    if ( EPsmsrvModeNormal == iMode )
        {
        ERROR_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::BackupSettingsL() - Not allowed in normal mode - LEAVE" ) ) );
        // Backup is not allowed in normal mode
        User::Leave( KErrWrite );
        }

    // Use own storage in case PSM mode is ongoing by some plugin
    CPsmBackupStorage* backupStorage = CPsmBackupStorage::NewLC( iMode, iFileSession );
    backupStorage->InitStorageL( aRepository );
    backupStorage->BackupConfigurationL( aPsmConfigArray );
    backupStorage->CloseStorageL();
    CleanupStack::PopAndDestroy( backupStorage );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::BackupSettingsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::Compare
// -----------------------------------------------------------------------------
//
TBool CPsmSettingsHandler::Compare( 
    const TPsmsrvConfigInfo& aSource, 
    const TPsmsrvConfigInfo& aTarget ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::Compare()" ) ) );
    TBool returnValue( EFalse );
    if ( ( aSource.iConfigId == aTarget.iConfigId ) &&
         ( aSource.iConfigType == aTarget.iConfigType ) )
        {
        // Check type and copare values
        switch ( aSource.iConfigType )
            {
            case EConfigTypeInt:
                returnValue = ( aSource.iIntValue == aTarget.iIntValue );
                break;
            case EConfigTypeReal:
                returnValue = ( aSource.iRealValue == aTarget.iRealValue );
                break;
            case EConfigTypeBuf:
                returnValue = ( aSource.iDesValue == aTarget.iDesValue );
                break;
            default:
                returnValue = EFalse;
            }
        }
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::Compare() - return" ) ) );
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::ChangeSettingsL
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::ChangeSettingsL( 
    RConfigInfoArray& aPsmConfigArray, 
    TUint32 aRepository,
    CPsmStorage& aConfigStorage )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeSettingsL()" ) ) );

    TInt err( KErrNone );
    // First, get current settings
    RConfigInfoArray currentSettings;
    CleanupClosePushL( currentSettings );
    
    //Close backup handler in case of any error
    CleanupStack::PushL(TCleanupItem(CleanupBackupStorage, iBackupStorage));
    iBackupStorage->InitStorageL( aRepository );
    iBackupStorage->ListCurrentSettingSetL( currentSettings );

    // Check currently owned settings and compare those to the new ones
    RConfigInfoArray settingsToBackUp;
    CleanupClosePushL( settingsToBackUp );

    for ( TInt i = 0; aPsmConfigArray.Count() != i; i++ )
        {
        TBool found = EFalse;
        for ( TInt a = 0; !found && currentSettings.Count() != a; a++ )
            {
            found = Compare( currentSettings[ a ], aPsmConfigArray[ i ] );
            }

        if ( !found )
            {
            // Add new settings to backup
            settingsToBackUp.AppendL( aPsmConfigArray[ i ] );
            currentSettings.AppendL( aPsmConfigArray[ i ] );
            }
        }

    // Get new values from config
    if ( EPsmsrvModeNormal == iMode )
        {
        // Restore to normal mode, initialize backup storage
        iBackupStorage->GetConfigurationL( aPsmConfigArray );
        }
    else
        {
        // Moving from power save mode to another power save mode
        iBackupStorage->BackupConfigurationL( settingsToBackUp );
        aConfigStorage.GetConfigurationL( aPsmConfigArray );
        }

    if( aPsmConfigArray.Count() == 0 )
        {
        err = KErrNotFound;
        }

    // Check for missing values and get those from backup
    RConfigInfoArray settingsFromBackUp;
    CleanupClosePushL( settingsFromBackUp );

    for ( TInt i = 0; currentSettings.Count() != i; ++i )
        {
        TPsmsrvConfigInfo& currentInfo = currentSettings[ i ];
        TBool found = EFalse;
        TPsmsrvConfigInfo newInfo;
        for ( TInt index = 0; !found && aPsmConfigArray.Count() != index; index++ )
            {
            newInfo = aPsmConfigArray[ index ];
            found = ( newInfo.iConfigId == currentInfo.iConfigId );
            }

        // Check was setting found
        if ( !found )
            {
            // not found, get setting from backup
            settingsFromBackUp.AppendL( currentInfo );
            currentSettings.Remove( i );
            --i;
            }
        else
            {
            // Setting found, update values
            currentInfo = newInfo;
            }
        }

    // Finally, get remaining settings from backup
    if ( settingsFromBackUp.Count() )
        {
        iBackupStorage->GetConfigurationL( settingsFromBackUp );
        }

    iBackupStorage->UpdateCurrentSettingSetL( currentSettings );

    // Close backup storage
    iBackupStorage->CloseStorageL();

    // add settings from backup to settings
    for ( TInt i = 0; i < settingsFromBackUp.Count(); i++ )
        {
        //Ignore the error returned by Append  
        aPsmConfigArray.Append( settingsFromBackUp[ i ]);
        }

    // Cleanup config arrays
    CleanupStack::PopAndDestroy( &settingsFromBackUp );
    CleanupStack::PopAndDestroy( &settingsToBackUp );
    CleanupStack::Pop( iBackupStorage );
    CleanupStack::PopAndDestroy( &currentSettings );

    User::LeaveIfError( err );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeSettingsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::ChangeCenRepSettingsL
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::ChangeCenRepSettingsL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeCenRepSettingsL()" ) ) );

    // First resolve the CenRep config files
    RPassiveConfigList passiveConfigs;
    CleanupClosePushL( passiveConfigs );

    iCenRepStorage->ListPassiveConfigs( passiveConfigs );

    const TInt configCount( passiveConfigs.Count() );

    // Loop found passive configs
    for ( TInt i = 0; i < configCount; i++ )
        {
        RConfigInfoArray psmConfigArray;
        CleanupClosePushL( psmConfigArray );

        const TUint32 repository = passiveConfigs[i];
        //Close Cenrep handler in case of any error
        CleanupStack::PushL(TCleanupItem(CleanupStorage, iCenRepStorage));
        iCenRepStorage->InitStorageL( repository );

        if ( EPsmsrvModeNormal == iMode )
            {
            //Close Cenrep handler in case of any error
    		CleanupStack::PushL(TCleanupItem(CleanupBackupStorage, iBackupStorage));	
            // Moving to normal mode, get passive config set from baskup storage
            iBackupStorage->InitStorageL( repository );
            iBackupStorage->ListPassiveConfigSetL( psmConfigArray );
            iBackupStorage->CloseStorageL();
            CleanupStack::Pop( iBackupStorage );
            }
        else
            {
            // Get configuration set
            iCenRepStorage->ListPassiveConfigSetL( psmConfigArray );
            }

        // Change values
        ChangeCenRepSettingsL( psmConfigArray, repository );

        // close storage
        iCenRepStorage->CloseStorageL();
        CleanupStack::Pop( iCenRepStorage );
        CleanupStack::PopAndDestroy( &psmConfigArray );
        }

    CleanupStack::PopAndDestroy( &passiveConfigs );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeCenRepSettingsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::ChangeCenRepSettingsL
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::ChangeCenRepSettingsL( 
    RConfigInfoArray& aPsmConfigArray, TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeCenRepSettingsL()" ) ) );

    // First read current values from CenRep
    TUid cenRepUid = { aRepository };
    CRepository* cenrep = CRepository::NewLC( cenRepUid );

    TInt setCount( aPsmConfigArray.Count() );

    for ( TInt i = 0; i < setCount; i++ )
        {
        TPsmsrvConfigInfo& info = aPsmConfigArray[i];
        switch ( info.iConfigType )
            {
            case EConfigTypeInt:
                {
                User::LeaveIfError( cenrep->Get( info.iConfigId, info.iIntValue ) );
                break;
                }
            case EConfigTypeReal:
                {
                User::LeaveIfError( cenrep->Get( info.iConfigId, info.iRealValue ) );
                break;
                }
            case EConfigTypeBuf:
                {
                User::LeaveIfError( cenrep->Get( info.iConfigId, info.iDesValue ) );
                break;
                }
            default:
                {
                COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeCenRepSettingsL() - Unknown type LEAVE" ) ) );
                User::Leave( KErrUnknown );
                }
            }
        }

    // Get new values from storage
    TRAP_IGNORE( ChangeSettingsL( aPsmConfigArray, aRepository, *iCenRepStorage ) );

    // Get new count
    setCount = aPsmConfigArray.Count();

    // Write new values to CenRep
    for ( TInt j = 0; j < setCount; j++ )
        {
        TPsmsrvConfigInfo& info = aPsmConfigArray[j];
        switch ( info.iConfigType )
            {
            case EConfigTypeInt:
                {
                User::LeaveIfError( cenrep->Set( info.iConfigId, info.iIntValue ) );
                break;
                }
            case EConfigTypeReal:
                {
                User::LeaveIfError( cenrep->Set( info.iConfigId, info.iRealValue ) );
                break;
                }
            case EConfigTypeBuf:
                {
                User::LeaveIfError( cenrep->Set( info.iConfigId, info.iDesValue ) );
                break;
                }
            default:
                {
                COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeCenRepSettingsL() - Unknown type LEAVE" ) ) );
                User::Leave( KErrUnknown );
                }
            }
        }

    // Cleanup CenRep
    CleanupStack::PopAndDestroy( cenrep );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::ChangeCenRepSettingsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsHandler::SetMode
// -----------------------------------------------------------------------------
//
void CPsmSettingsHandler::SetMode( TPsmsrvMode aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::SetMode()" ) ) );

    // Store previous mode
    iCurrentMode = iMode;

    // Store mode
    iMode = aMode;

    // Update mode to storage handlers also
    iBackupStorage->SetMode( aMode );
    iConfigStorage->SetMode( aMode );
    iCenRepStorage->SetMode( aMode );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmSettingsHandler::SetMode() - return" ) ) );
    }

// End of file
