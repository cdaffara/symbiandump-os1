/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CCDSYFactory class
*
*/




// INCLUDE FILES 
#include <f32file.h>
#include <data_caging_path_literals.hrh>
#include "CDSYFactory.h"
#include "commondsy_debug.h"

#ifndef RD_STARTUP_CHANGE
#include "CDSYSysUtilsDOSServerPlugin.h"
#include "CDSYHelperDOSServerPlugin.h"
#include "CDSYMTCDOSServerPlugin.h"
#include "CDSYSelftestDOSServerPlugin.h"
#endif //RD_STARTUP_CHANGE

#ifndef RD_STARTUP_CHANGE
#include "CDSYSAEDOSServerPlugin.h"
#endif //RD_STARTUP_CHANGE

#include "CDSYExtensionDOSServerPlugin.h"
#include "CDSYController.h"
#include "CDSYDOSServerRequestManager.h"
#include "CDSYEventManager.h"
#include "CDSYServiceLogicFactory.h"



// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
_LIT( KLDSYIniFile, "Z:\\private\\101f6efa\\CommonDSY.ini" );

// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// LibEntryL
// -----------------------------------------------------------------------------
EXPORT_C CDosFactoryBase* LibEntryL()
    {
    COM_TRACE_( "CDSY - LibEntryL()" );

    COMPONENT_TRACE_THIS_FILE;
    CCDSYFactory* factory = CCDSYFactory::NewL();

    COM_TRACE_1( "CDSY - LibEntryL - return 0x%x", factory );
    return factory;
    }



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CCDSYFactory::CCDSYFactory
// -----------------------------------------------------------------------------
CCDSYFactory::CCDSYFactory()
: iCDSYController( NULL )
, iCDSYDOSServerRequestManager( NULL )
, iCDSYEventManager( NULL )
, iDSYServiceLogicFactory( NULL )
    {
    COM_TRACE_( "CDSY - CCDSYFactory::CCDSYFactory()" );

    COM_TRACE_( "CDSY - CCDSYFactory::CCDSYFactory - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYFactory::~CCDSYFactory
// -----------------------------------------------------------------------------
CCDSYFactory::~CCDSYFactory()
    {
    COM_TRACE_( "CDSY - CCDSYFactory::~CCDSYFactory()" );

    if ( iDSYServiceLogicFactory != NULL )
        {
        iDSYServiceLogicFactory->DeInitialize();
        }
    delete iDSYServiceLogicFactory;
    iLDSYLibrary.Close();
    delete iCDSYEventManager;
    delete iCDSYDOSServerRequestManager;
    delete iCDSYController;

    COM_TRACE_( "CDSY - CCDSYFactory::~CCDSYFactory - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYFactory::ConstructL
// -----------------------------------------------------------------------------
void CCDSYFactory::ConstructL()
    {
    COM_TRACE_( "CDSY - CCDSYFactory::ConstructL()" );

    iCDSYController = CCDSYController::NewL();
    iCDSYDOSServerRequestManager = CCDSYDOSServerRequestManager::NewL( *iCDSYController );
    iCDSYEventManager = CCDSYEventManager::NewL( *iCDSYController, *this );
    LoadLDSYModuleL();
    COM_TRACE_1( "CDSY - CCDSYFactory::ConstructL - ret == %d", iCDSYDOSServerRequestManager->iLDSYFind );
    
    if ( iCDSYDOSServerRequestManager->iLDSYFind )
        {
        iDSYServiceLogicFactory->InitializeL( *iCDSYController );
        }

    COM_TRACE_( "CDSY - CCDSYFactory::ConstructL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYFactory::NewL
// -----------------------------------------------------------------------------
CCDSYFactory* CCDSYFactory::NewL()
    {
    COM_TRACE_( "CDSY - CCDSYFactory::NewL()" );

    CCDSYFactory* factory = new ( ELeave ) CCDSYFactory();
    CleanupStack::PushL( factory );
    factory->ConstructL();
    CleanupStack::Pop( factory );

    COM_TRACE_1( "CDSY - CCDSYFactory::NewL - return 0x%x", factory );
    return factory;
    }



// -----------------------------------------------------------------------------
// CCDSYFactory::LoadLDSYModuleL
// -----------------------------------------------------------------------------
void CCDSYFactory::LoadLDSYModuleL()
    {
    COM_TRACE_( "CDSY - CCDSYFactory::LoadLDSYModuleL()" );

    RFs fs;
    RFile file;

    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    TInt err = file.Open( fs, KLDSYIniFile, EFileRead );
    TRACE_ASSERT( err == KErrNone );
    if ( err != KErrNone )
        {
        COM_TRACE_( "CDSY - CCDSYFactory::LoadLDSYModuleL - CommonDSY.ini file missing" );
        CleanupStack::PopAndDestroy();
        iCDSYDOSServerRequestManager->iLDSYFind = EFalse;
        }
    else
        {
        CleanupClosePushL( file );

        TBuf8<KMaxFileName> rawFileName;
    
        User::LeaveIfError( file.Read( rawFileName ) );

        CleanupStack::PopAndDestroy( 2 );

        HBufC* unicodeFileName = HBufC::NewLC( rawFileName.Length() );
        TPtr fileNamePtr( unicodeFileName->Des() );
        fileNamePtr.Copy( rawFileName );

        TLibraryFunction entryPoint;

	    _LIT(KDSYBinaryLocDrive, "z:");

	    TBuf<256> dsyBinaryLoc(KDC_SHARED_LIB_DIR);

    	dsyBinaryLoc.Insert(0, KDSYBinaryLocDrive);
	    User::LeaveIfError( iLDSYLibrary.Load( fileNamePtr, dsyBinaryLoc ) );

        entryPoint = iLDSYLibrary.Lookup( 1 );
        TRACE_ASSERT( entryPoint != NULL );
        iDSYServiceLogicFactory = ( MCDSYServiceLogicFactory * )entryPoint();
        TRACE_ASSERT( iDSYServiceLogicFactory != NULL );
        CleanupStack::PopAndDestroy( unicodeFileName );

        COM_TRACE_( "CDSY - CCDSYFactory::LoadLDSYModuleL - return void" );
        }
    }

#ifndef RD_STARTUP_CHANGE
// -----------------------------------------------------------------------------
// CCDSYFactory::NewSysUtilsServiceL
// -----------------------------------------------------------------------------
CDosSysUtilsBase* CCDSYFactory::NewSysUtilsServiceL()
    {
    API_TRACE_( "CDSY - CCDSYFactory::NewSysUtilsServiceL()" );

    CCDSYSysUtilsDOSServerPlugin *sysUtilsDOSServerPlugin = CCDSYSysUtilsDOSServerPlugin::NewL( *iCDSYDOSServerRequestManager );

    API_TRACE_1( "CDSY - CCDSYFactory::NewSysUtilsServiceL - return 0x%x", sysUtilsDOSServerPlugin );
    return sysUtilsDOSServerPlugin;
    }

// -----------------------------------------------------------------------------
// CCDSYFactory::NewHelperServiceL
// -----------------------------------------------------------------------------
CDosHelperBase* CCDSYFactory::NewHelperServiceL()
    {
    API_TRACE_( "CDSY - CCDSYFactory::NewHelperServiceL()" );

    CCDSYHelperDOSServerPlugin* helperDOSServerPlugin = CCDSYHelperDOSServerPlugin::NewL( *iCDSYDOSServerRequestManager );

    API_TRACE_1( "CDSY - CCDSYFactory::NewHelperServiceL - return 0x%x", helperDOSServerPlugin );
    return helperDOSServerPlugin;
    }

// -----------------------------------------------------------------------------
// CCDSYFactory::NewMtcServiceL
// -----------------------------------------------------------------------------
CDosMtcBase* CCDSYFactory::NewMtcServiceL()
    {
    API_TRACE_( "CDSY - CCDSYFactory::NewMtcServiceL()" );

    CCDSYMtcDOSServerPlugin* mtcDOSServerPlugin = CCDSYMtcDOSServerPlugin::NewL( *iCDSYDOSServerRequestManager );
    
    API_TRACE_1( "CDSY - CCDSYFactory::NewMtcServiceL - return 0x%x", mtcDOSServerPlugin );
    return mtcDOSServerPlugin;
    }

// -----------------------------------------------------------------------------
// CCDSYFactory::NewSelfTestServiceL
// -----------------------------------------------------------------------------
CDosSelfTestBase* CCDSYFactory::NewSelfTestServiceL()
    {
    API_TRACE_( "CDSY - CCDSYFactory::NewSelfTestServiceL()" );
    
    CCDSYSelfTestDOSServerPlugin* selfTestDOSServerPlugin = CCDSYSelfTestDOSServerPlugin::NewL( *iCDSYDOSServerRequestManager );

    API_TRACE_1( "CDSY - CCDSYFactory::NewSelfTestServiceL - return 0x%x", selfTestDOSServerPlugin );
    return selfTestDOSServerPlugin;
    }
#endif //RD_STARTUP_CHANGE  

#ifndef RD_STARTUP_CHANGE
// -----------------------------------------------------------------------------
// CCDSYFactory::NewSaeServiceL
// -----------------------------------------------------------------------------
CDosSaeBase* CCDSYFactory::NewSaeServiceL()
    {
    API_TRACE_( "CDSY - CCDSYFactory::NewSaeServiceL()" );

    CCDSYSaeDOSServerPlugin* saeDOSServerPlugin = CCDSYSaeDOSServerPlugin::NewL( *iCDSYDOSServerRequestManager );
    
    API_TRACE_1( "CDSY - CCDSYFactory::NewSaeServiceL - return 0x%x", saeDOSServerPlugin );
    return saeDOSServerPlugin;
    }
#endif //RD_STARTUP_CHANGE

// -----------------------------------------------------------------------------
// CCDSYFactory::NewExtensionServiceL
// -----------------------------------------------------------------------------
CDosExtensionBase* CCDSYFactory::NewExtensionServiceL()
    {
    API_TRACE_( "CDSY - CCDSYFactory::NewExtensionServiceL()" );

    CCDSYExtensionDOSServerPlugin* extensionDOSServerPlugin = CCDSYExtensionDOSServerPlugin::NewL( *iCDSYDOSServerRequestManager );

    API_TRACE_1( "CDSY - CCDSYFactory::NewExtensionServiceL - return 0x%x", extensionDOSServerPlugin );
    return extensionDOSServerPlugin;
    }









    
