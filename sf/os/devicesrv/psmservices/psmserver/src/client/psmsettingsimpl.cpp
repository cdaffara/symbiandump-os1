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
* Description:  Power save mode client class
*
*/



#include <psmsrvdomaincrkeys.h>
#include <centralrepository.h>
#include "psmsettingsimpl.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::~CPsmSettingsImpl
// Constructor.
// -----------------------------------------------------------------------------
//
CPsmSettingsImpl::~CPsmSettingsImpl()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::~CPsmSettingsImpl()" ) ) );
    if ( iOwnSession )
        {
        iPsmServer.Close();
        }

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::~CPsmSettingsImpl() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::CPsmSettingsImpl
// Constructor.
// -----------------------------------------------------------------------------
//
CPsmSettingsImpl::CPsmSettingsImpl()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::CPsmSettingsImpl()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::CPsmSettingsImpl() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CPsmSettingsImpl::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::ConstructL()" ) ) );
    User::LeaveIfError( iPsmServer.Connect() );
    iOwnSession = ETrue;

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::CPsmSettingsImpl
// Constructor.
// -----------------------------------------------------------------------------
//
CPsmSettingsImpl::CPsmSettingsImpl( RPsmServer& aPsmServer ) :
    iPsmServer( aPsmServer )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::CPsmSettingsImpl()" ) ) );
    iOwnSession = EFalse;
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::CPsmSettingsImpl() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::BackupAndGetSettings
// -----------------------------------------------------------------------------
//
TInt CPsmSettingsImpl::BackupAndGetSettings( 
    RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::BackupAndGetSettings()" ) ) );
    return HandleSettingsRequest( aPsmConfigArray, aRepository, EPsmServerChangeSettings );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::GetSettings
// -----------------------------------------------------------------------------
//
TInt CPsmSettingsImpl::GetSettings( 
    RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::GetSettings()" ) ) );
    return HandleSettingsRequest( aPsmConfigArray, aRepository, EPsmServerGetSettings );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::BackupSettings
// -----------------------------------------------------------------------------
//
TInt CPsmSettingsImpl::BackupSettings( 
    RConfigInfoArray& aPsmConfigArray, const TUint32 aRepository )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::BackupSettings()" ) ) );
    return HandleSettingsRequest( aPsmConfigArray, aRepository, EPsmServerBackupSettings );
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::GetCurrentMode
// -----------------------------------------------------------------------------
//
TInt CPsmSettingsImpl::GetCurrentMode( TInt& aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::GetCurrentMode()" ) ) );
    
    TInt errorCode( KErrNone );
    CRepository* cenrep = NULL;

    TRAP_IGNORE( cenrep = CRepository::NewL( KCRUidPowerSaveMode ) );
    
    if ( cenrep )
        {
        errorCode = cenrep->Get( KPsmCurrentMode, aMode );
        delete cenrep;
        cenrep = NULL;
        }
    else
        {
        errorCode = KErrNoMemory;
        }
    
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::GetCurrentMode - Mode: %i" ), aMode ) );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::GetCurrentMode - return: %i" ), errorCode ) );
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::HandleSettingsRequest
// -----------------------------------------------------------------------------
//
TInt CPsmSettingsImpl::HandleSettingsRequest( 
    RConfigInfoArray& aPsmConfigArray, 
    const TUint32 aRepository,
    const TPsmServerRequest aRequest )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::HandleSettingsRequest()" ) ) );

    TInt configCount( aPsmConfigArray.Count() );
    TInt err( KErrNone );

    RConfigInfoArray configArray;

    TRAP( err, 
    for( TInt i = 0; i < configCount; i++ )
        {
        // RArray::Append might not work correctly, have to use AppendL
        configArray.AppendL( aPsmConfigArray[i] );
        } ); // End of TRAP

    if ( KErrNone == err )
        {
        TInt arraySize( configCount * sizeof( TPsmsrvConfigInfo ) );
    	TPtr8 arrayPtr( ( TUint8* ) &configArray[0], arraySize, arraySize );

        // Create IPC arguments. First is array, second is item count in array and third is repository
    	TIpcArgs args = TIpcArgs( &arrayPtr, configCount, aRepository );

        // Send message to server side
        err = iPsmServer.SendReceiveSync( aRequest, args );

        if ( err == KErrNone && EPsmServerBackupSettings != aRequest )  // Copy configs info to aPsmConfigArray
            {
            // Same configs already exists in both arrays, just update values
            UpdateConfigArray( aPsmConfigArray, configArray );
            }
        }

    configArray.Reset();
    configArray.Close();

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::HandleSettingsRequest - return: %i" ), err ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CPsmSettingsImpl::UpdateConfigArray
// -----------------------------------------------------------------------------
//
void CPsmSettingsImpl::UpdateConfigArray( RConfigInfoArray& aTargetArray, 
                                          RConfigInfoArray& aSourceArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::UpdateConfigArray()" ) ) );

    for ( TInt i = 0; aTargetArray.Count() != i; i++ )
        {
        TBool found = EFalse;
        TPsmsrvConfigInfo& targetInfo = aTargetArray[i];

        for ( TInt a = 0; !found && aSourceArray.Count() != a; a++ )
            {
            TPsmsrvConfigInfo& sourceInfo = aSourceArray[a];

            // check keys
            if ( sourceInfo.iConfigId == targetInfo.iConfigId &&
                 sourceInfo.iConfigType == targetInfo.iConfigType )
                {
                // Add blocked information
                targetInfo.iBlocked = sourceInfo.iBlocked;

                // correct config found, update value
                switch ( targetInfo.iConfigType )
                    {
                    case EConfigTypeInt:
                        {
                        targetInfo.iIntValue = sourceInfo.iIntValue;
                        break;
                        }
                    case EConfigTypeReal:
                        {
                        targetInfo.iRealValue = sourceInfo.iRealValue;
                        break;
                        }
                    case EConfigTypeBuf:
                        {
                        targetInfo.iDesValue.Copy( sourceInfo.iDesValue );
                        break;
                        }
                    default:
                        {
                        COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::UpdateConfigArray() - Unknown type" ) ) );
                        }
                    }
                // No need to loop config array anymore with this set element
                found = ETrue;
                }
            }
        }

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmSettingsImpl::UpdateConfigArray() - return" ) ) );
    }
    
// End of file
