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


#include "psmconfigstorage.h"
#include "psmstoragedefines.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmConfigStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmConfigStorage* CPsmConfigStorage::NewL( TPsmsrvMode aMode, RFs& aFile )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::NewL()" ) ) );

    CPsmConfigStorage* self = CPsmConfigStorage::NewLC( aMode, aFile );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmConfigStorage::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmConfigStorage* CPsmConfigStorage::NewLC( TPsmsrvMode aMode, RFs& aFile )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::NewLC()" ) ) );

    CPsmConfigStorage* self = new( ELeave ) CPsmConfigStorage( aMode, aFile );
    
    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmConfigStorage::CPsmConfigStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmConfigStorage::CPsmConfigStorage( TPsmsrvMode aMode, RFs& aFile ) :
    CPsmStorage( aMode, aFile )
    {
    }

// -----------------------------------------------------------------------------
// CPsmConfigStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmConfigStorage::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmConfigStorage::~CPsmConfigStorage()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::~CPsmConfigStorage()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::~CPsmConfigStorage - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmConfigStorage::InitRootElementsL
// -----------------------------------------------------------------------------
//
void CPsmConfigStorage::InitRootElementsL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::InitRootElementsL()" ) ) );
   
    // Get root element
    if ( iStorageFile.NotNull() )
        {
        iStorageRoot = iStorageFile.DocumentElement();               
        }
    
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::InitRootElementsL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmConfigStorage::InitConfigL
// -----------------------------------------------------------------------------
//
void CPsmConfigStorage::InitStorageL( const TUint32 aStorageUid )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::InitStorageL()" ) ) );

    iStorageUid = aStorageUid;

    // Create storage file parser    
    iDOMImpl.OpenL( );
    User::LeaveIfError( iStorageParser.Open( iDOMImpl ) );

    // Generate file folder+name+extension
    TBuf<KPsmMaxPathLength> privatePath( KNullDesC );
    iFileSession.PrivatePath( privatePath );

    TBuf<KPsmMaxPathLength> storageFile( KNullDesC );
    storageFile.Append( KPsmPrivateDrive );
    storageFile.Append( privatePath );
    storageFile.AppendNumUC( iStorageUid, EHex );
    storageFile.Append( KPsmStorageExtension );

    // Initialize PSM config file. Leave if config file is not found    
    TRAPD( parseErr, iStorageFile = iStorageParser.ParseFileL( iFileSession, storageFile ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::InitStorageL() - Parse file started: %i" ), parseErr ) );
    User::LeaveIfError( parseErr );
    
    // Initialize root elements
    InitRootElementsL();    

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmConfigStorage::InitStorageL() - return" ) ) );
    }

// End of file
