/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM storage for backups and configs
*
*/


#include "psmbackupstorage.h"
#include "psmstoragedefines.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmBackupStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmBackupStorage* CPsmBackupStorage::NewL( TPsmsrvMode aMode, RFs& aFile )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::NewL()" ) ) );

    CPsmBackupStorage* self = CPsmBackupStorage::NewLC( aMode, aFile );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmBackupStorage* CPsmBackupStorage::NewLC( TPsmsrvMode aMode, RFs& aFile )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::NewLC()" ) ) );

    CPsmBackupStorage* self = new( ELeave ) CPsmBackupStorage( aMode, aFile );

    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::CPsmBackupStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmBackupStorage::CPsmBackupStorage( TPsmsrvMode aMode, RFs& aFile ) :
    CPsmStorage( aMode, aFile )
    {
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmBackupStorage::~CPsmBackupStorage()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::~CPsmBackupStorage()" ) ) );

    // No need to make new backup file at this point
    iChangesMade = EFalse;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::~CPsmBackupStorage - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::InitRootElementsL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::InitRootElementsL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::InitRootElementsL()" ) ) );
   
    // First get document element
    if ( iStorageFile.NotNull() )
        {
        iStorageRoot = iStorageFile.DocumentElement();

        // Check is root found. If not, create new
        if ( iStorageRoot.IsNull() )
            {            
            iStorageRoot = iStorageFile.CreateDocumentElementL( KPsmBackupRootTag() );
            }
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::InitRootElementsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::InitStorageL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::InitStorageL( const TUint32 aStorageUid )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::InitStorageL()" ) ) );

    iStorageUid = aStorageUid;

    // Check backup folder
    CheckBackupLocation();

    // Create storage file parser
    iDOMImpl.OpenL( );
    User::LeaveIfError( iStorageParser.Open( iDOMImpl ) );

    // Generate file folder+name+extension
    TBuf<KPsmMaxPathLength> storageFile( KNullDesC );
    BackupFileLocation( storageFile );

    TRAPD(err, iStorageFile = iStorageParser.ParseFileL( iFileSession, storageFile ));

    // Initialize PSM backup file
    if ( KErrNotFound == err )
        {
        // Backup file does not exist, create new
        iStorageFile.OpenL( iDOMImpl );        
        }
    else
        {
        // Leave if error
        User::LeaveIfError( err );
        }
    InitRootElementsL();
    iChangesMade = EFalse;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::InitStorageL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::CloseStorageL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::CloseStorageL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::CloseStorageL()" ) ) );

    // Use file composer to store changes to XML file, if any changes has been done
    if ( iChangesMade )
        {
        TBuf<KPsmMaxPathLength> storageFile( KNullDesC );
        BackupFileLocation( storageFile );

        iStorageFile.SaveL( iFileSession,
                            storageFile,
                            iStorageRoot,
                            TXmlEngSerializationOptions::KOptionOmitXMLDeclaration );

        iChangesMade = EFalse;
        }

    // Tell base class to finalize closing the storage
    CPsmStorage::CloseStorageL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::CloseStorageL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::FindSetItemL
// -----------------------------------------------------------------------------
//
TXmlEngElement CPsmBackupStorage::FindSetItemL( const TXmlEngElement& aConfigSet, const TInt aKey )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::FindSetItemL()" ) ) );

    // Search correct set item for aKey
    TBool setFound( EFalse );
    RXmlEngNodeList<TXmlEngElement> nodeList;
    CleanupClosePushL(nodeList);
    aConfigSet.GetElementsByTagNameL(nodeList, KPsmSetItem);
    TXmlEngElement setItem = nodeList.Next().AsElement();       

    // Loop until correct set item is found
    while ( setItem.NotNull() && !setFound )    
        {
        // Read key from storage
        TInt setItemKey = GetAttributeIntValueL( setItem, KPsmSetItemKey );

        if ( aKey == setItemKey )
            {
            // Set found, no need to loop any more
            setFound = ETrue;
            }
        else
            {
            // Set not found, continue loop with next config set
            setItem = nodeList.Next().AsElement();            
            }
        }
        
    CleanupStack::PopAndDestroy(&nodeList);
    if ( !setFound )
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::FindSetItemL() - Not found id:%i, LEAVE" ), aKey ) );
        User::Leave( KErrNotFound );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::FindSetItemL() - return" ) ) );
    return setItem;
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::BackupConfigurationL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::BackupConfigurationL( const RConfigInfoArray& aConfigArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::BackupConfigurationL()" ) ) );
    UpdateBackupL( aConfigArray, EPsmsrvModeNormal );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::BackupConfigurationL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::UpdateBackupL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::UpdateBackupL( const RConfigInfoArray& aConfigArray, TInt aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::UpdateBackupL() - Mode: %i" ), aMode ) );

    if ( iStorageRoot.IsNull() )    
        {
        CloseStorageL();
        User::Leave( KErrNotFound );
        }

    // This is used only to backup original settings
    // Search correct settings for aUid
    TInt trapErr( KErrNone );
    
    TXmlEngElement settings;    
    TRAP( trapErr, settings = FindConfigurationSetL( aMode ) );

    // If not found, create new set of backup items
    if ( KErrNotFound == trapErr && settings.IsNull() )
        {
        // Set for the UID not found, create new set to storage
        
        // First param indicates that can have childs, second is the owner document and last is tag
        settings = iStorageFile.CreateElementL( KPsmPluginGroupTag );
        // Add attribute to group, Set mode to 'NormalMode', we don't backup PSM values
        SetAttributeValueL( settings, KPsmPluginGroupAttribute, aMode );
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::UpdateBackupL() - Append set to storage root" ) ) );
        // Add element to correct place
        iStorageRoot.AppendChildL( settings );
        // reset error value
        trapErr = KErrNone;
        }
    else if ( KErrNoMemory == trapErr )
        {
        User::Leave(trapErr);
        }    
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::UpdateBackupL() - Config count: %i" ), aConfigArray.Count() ) );

    // Loop config array and update storage
    for ( TInt i = 0; i < aConfigArray.Count(); i++ )
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::UpdateBackupL() - Loop configs: %i" ), i ) );

        const TPsmsrvConfigInfo& configInfo = aConfigArray[i];

        // Search set element from config and create new if not found
        TXmlEngElement setItem;
        TRAP( trapErr, setItem = FindSetItemL( settings, configInfo.iConfigId ) );
        
        COMPONENT_TRACE( ( _L( "PSM Server - FindSetItemL() TRAP error -  %d" ), trapErr ) );
        
        if ( KErrNoMemory == trapErr && setItem.IsNull() )
            {
            User::Leave(trapErr);
            }
        else if (KErrNoMemory == trapErr)
            {
            setItem.RemoveAttributeL( KPsmSetItemValue );
            User::Leave(trapErr);
            }
        else if ( KErrNotFound == trapErr && setItem.IsNull() )
            {
            // First param indicates that cannot have childs, 
            // second is the owner document and last is tag
            setItem = iStorageFile.CreateElementL( KPsmSetItem );
            // Add key and type attributes as they cannot be updated later
            SetAttributeValueL( setItem, KPsmSetItemKey, configInfo.iConfigId );
            SetAttributeValueL( setItem, KPsmSetItemType, configInfo.iConfigType );
            // Add element to set
            settings.AppendChildL( setItem );
            }
        else
            {
            // remove existing attribute value
            setItem.RemoveAttributeL( KPsmSetItemValue );
            }

        // Finally, change Value attribute
        switch ( configInfo.iConfigType )
            {
            case EConfigTypeInt:
                {
                SetAttributeValueL( setItem, KPsmSetItemValue, configInfo.iIntValue );
                break;
                }
            case EConfigTypeReal:
                {
                SetAttributeValueL( setItem, KPsmSetItemValue, configInfo.iRealValue );
                break;
                }
            case EConfigTypeBuf:
                {
                TBuf8<KPsmMaxAttribLength> buffer( KNullDesC8 );
                buffer.Copy( configInfo.iDesValue );
                SetAttributeValueL( setItem, KPsmSetItemValue, buffer );
                break;
                }
            default:
                {
                COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::UpdateBackupL() - Unknown type LEAVE" ) ) );
                User::Leave( KErrUnknown );
                }
            }
        }

    // Indicate that storage needs to be composed when closing
    iChangesMade = ETrue;
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::UpdateBackupL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::BackupFileLocation
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::BackupFileLocation( TDes& aLocation )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::BackupFileLocation()" ) ) );

    // Generate file folder+name+extension
    TBuf<KPsmMaxPathLength> privatePath( KNullDesC );
    iFileSession.PrivatePath( privatePath );

    TBuf<KPsmMaxPathLength> storageFile( KNullDesC );
    storageFile.Append( KPsmBackupDrive );
    storageFile.Append( privatePath );
    storageFile.Append( KPsmBackupFolder );
    storageFile.AppendNumUC( iStorageUid, EHex );
    storageFile.Append( KPsmStorageExtension );

    aLocation.Copy( storageFile );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::BackupFileLocation - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::CheckBackupLocation
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::CheckBackupLocation()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::CheckBackupLocation()" ) ) );

    TInt err = iFileSession.CreatePrivatePath( EDriveC );

    if ( KErrNone == err || KErrAlreadyExists == err )
        {
        TBuf<KPsmMaxPathLength> storageFile( KNullDesC );
        TBuf<KPsmMaxPathLength> privatePath( KNullDesC );
        iFileSession.PrivatePath( privatePath );

        storageFile.Append( KPsmBackupDrive );
        storageFile.Append( privatePath );
        storageFile.Append( KPsmBackupFolder );

        // Create dir 
        iFileSession.MkDir( storageFile );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::CheckBackupLocation - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::ListPassiveConfigSetL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::ListPassiveConfigSetL( RConfigInfoArray& aPsmConfigArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::ListPassiveConfigSetL()" ) ) );

    // Read configuration information from config files and generate config array
    CPsmStorage::CreateConfigArrayL( aPsmConfigArray, EPsmsrvModeNormal );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::ListPassiveConfigSetL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::ListCurrentSettingSetL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::ListCurrentSettingSetL( RConfigInfoArray& aPsmConfigArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::ListCurrentSettingSetL()" ) ) );

    // Read configuration information from config files and generate config array
    TRAP_IGNORE( CPsmStorage::CreateConfigArrayL( aPsmConfigArray, KPsmCurrentSetMode ) );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::ListCurrentSettingSetL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::UpdateCurrentSettingSetL
// -----------------------------------------------------------------------------
//
void CPsmBackupStorage::UpdateCurrentSettingSetL( const RConfigInfoArray& aConfigArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::BackupConfigurationL()" ) ) );
    UpdateBackupL( aConfigArray, KPsmCurrentSetMode );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmBackupStorage::BackupConfigurationL() - return" ) ) );
    }

// End of file
