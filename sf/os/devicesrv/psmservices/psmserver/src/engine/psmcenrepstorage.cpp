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
* Description:  PSM storage for backups and configs
*
*/


#include <f32file.h>
#include "psmcenrepstorage.h"
#include "psmstoragedefines.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmCenRepStorage* CPsmCenRepStorage::NewL( TPsmsrvMode aMode, RFs& aFile )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::NewL()" ) ) );

    CPsmCenRepStorage* self = new( ELeave ) CPsmCenRepStorage( aMode, aFile );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::CPsmCenRepStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmCenRepStorage::CPsmCenRepStorage( TPsmsrvMode aMode, RFs& aFile ) :
    CPsmStorage( aMode, aFile )
    {
    }

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmCenRepStorage::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmCenRepStorage::~CPsmCenRepStorage()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::~CPsmCenRepStorage()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::~CPsmCenRepStorage - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::InitRootElementsL
// -----------------------------------------------------------------------------
//
void CPsmCenRepStorage::InitRootElementsL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::InitRootElementsL()" ) ) );
   
    // Get root element
    iStorageRoot = iStorageFile.DocumentElement();
    
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::InitRootElementsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::InitConfigL
// -----------------------------------------------------------------------------
//
void CPsmCenRepStorage::InitStorageL( const TUint32 aStorageUid )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::InitStorageL()" ) ) );

    // Create storage file parser    
    iDOMImpl.OpenL( );
    User::LeaveIfError( iStorageParser.Open( iDOMImpl ) );
    iStorageUid = aStorageUid;

    // Generate file folder+name+extension
    TBuf<KPsmMaxPathLength> privatePath( KNullDesC );
    iFileSession.PrivatePath( privatePath );

    TBuf<KPsmMaxPathLength> storageFile( KNullDesC );
    storageFile.Append( KPsmPrivateDrive );
    storageFile.Append( privatePath );
    storageFile.Append( KPsmCenRepFolder );
    storageFile.AppendNumUC( iStorageUid, EHex );
    storageFile.Append( KPsmStorageExtension );

    // Initialize PSM config file
    iStorageFile = iStorageParser.ParseFileL( iFileSession, storageFile );
    InitRootElementsL();
    
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::InitStorageL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::ListPassiveConfigsL
// -----------------------------------------------------------------------------
//
void CPsmCenRepStorage::ListPassiveConfigs( RPassiveConfigList& aList )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ListPassiveConfigsL()" ) ) );
    // Read from CenRep directory all the files and generate UIDs from the file names

    CDir* directory = NULL;
    TBuf<KPsmMaxPathLength> privatePath( KNullDesC );
    iFileSession.PrivatePath( privatePath );

    TBuf<KPsmMaxPathLength> storageFolder( KNullDesC );
    storageFolder.Append( KPsmPrivateDrive );
    storageFolder.Append( privatePath );
    storageFolder.Append( KPsmCenRepFolder );

    iFileSession.GetDir( storageFolder, 0, 0, *&directory );

    if ( directory )
        {
        // Get file count and loop all files
        const TInt fileCount( directory->Count() );

        for ( TInt i = 0; i < fileCount; i++ )
            {
            // Get file information
            const TEntry& file( ( *directory )[i] );
            // Get file name and change that to UID value
            TUint32 hexValue( 0 );
            TUint limit( 0xFFFFFFFF );

            // Append string into Lex and get value
            TLex lexValue( file.iName );
            // Read value
            lexValue.BoundedVal( hexValue, EHex, limit );

            aList.Append( hexValue );
            }

        // Finally delete directory
        delete directory;
        }
    else
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ListPassiveConfigsL() - No files found" ) ) );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ListPassiveConfigsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmCenRepStorage::ListPassiveConfigSetL
// -----------------------------------------------------------------------------
//
void CPsmCenRepStorage::ListPassiveConfigSetL( RConfigInfoArray& aPsmConfigArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ListPassiveConfigSetL()" ) ) );

    // Read configuration information from config files and generate config array    
    CPsmStorage::CreateConfigArrayL( aPsmConfigArray, iMode );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmCenRepStorage::ListPassiveConfigSetL() - return" ) ) );
    }

// End of file
