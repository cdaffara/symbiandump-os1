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
* Description:  Implementation of CTFFactory class
*
*/


// INCLUDE FILES
#include "tftypes.h"
#include "ctffactory.h"
#include "ctfstub.h"
#include "ctfatestenvironment.h"
#include "ctftestcasewrapper.h"
#include "ctfremotestub.h"
#include "ctftestserver.h"
#include "dsytesttooltrace.h"
#include "ctfstubcontrol.h"
#include "ctfactivescheduler.h"

struct TTFThreadParam
    {
    RSemaphore* iSemaphore;
    TInt iStubType;
    TAny* iMessageData1;
    TAny* iMessageData2;
    TAny* iMessageData3;
    TAny* iMessageData4;
    };

// -----------------------------------------------------------------------------
// CTFFactory::NewL
// -----------------------------------------------------------------------------
CTFFactory* CTFFactory::NewL( void )
    {
    CTFFactory* factory = new ( ELeave ) CTFFactory();
    CleanupStack::PushL( factory );
    factory->ConstructL();
    CleanupStack::Pop( factory );
    return factory;
    }


// -----------------------------------------------------------------------------
// CTFFactory::CTFFactory
// -----------------------------------------------------------------------------
CTFFactory::CTFFactory( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFFactory::ServerThreadFunctionL
// -----------------------------------------------------------------------------
void CTFFactory::ServerThreadFunctionL( CTFFactory* aFactory )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ServerThreadFunctionL(0x%x)" ), aFactory ) );
    CTFActiveScheduler* scheduler = new ( ELeave ) CTFActiveScheduler();
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    CTFTestServer* testServer = new ( ELeave ) CTFTestServer( aFactory );
    CleanupStack::PushL( testServer );
    testServer->StartL( KTFRemoteTestServerName );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ServerThreadFunction - Entering scheduler" ) ) );
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( testServer );
    CleanupStack::PopAndDestroy( scheduler );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ServerThreadFunctionL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::ServerThreadFunction
// -----------------------------------------------------------------------------
TInt CTFFactory::ServerThreadFunction( TAny* aData )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ServerThreadFunction(0x%x)" ), aData ) );
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( cleanup != NULL )
        {
        TRAPD( err, ServerThreadFunctionL( ( CTFFactory* )aData ) );
        TRACE_ASSERT( err == KErrNone );
        if( err != KErrNone )
        	{
        	COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ServerThreadFunction - error" ) ) );
        	}
        delete cleanup;
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ServerThreadFunction - return 0" ) ) );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTFFactory::ConstructL
// -----------------------------------------------------------------------------
void CTFFactory::ConstructL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ConstructL()" ) ) );
    TAny* nullPtr = Dll::Tls();
    TRACE_ASSERT( nullPtr == NULL );
    __ASSERT_ALWAYS( nullPtr == NULL, User::Panic( KTFName, ETFPanicTlsExists ) );
    iStubControl = CTFStubControl::NewL();
    Dll::SetTls( this );
    // Starts the local test server
    iTestServer = new ( ELeave ) CTFTestServer( this );
    iTestServer->StartL( KTFLocalTestServerName );
    // Starts the remote test server
    RThread thread;
    User::LeaveIfError( thread.Create( _L( "SERVERTHREAD" ), 
        ServerThreadFunction, KDefaultStackSize, KMinHeapSize, 0x50000, this ) );
    thread.Resume();
    thread.Close();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::ConstructL - return" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFFactory::~CTFFactory
// -----------------------------------------------------------------------------
CTFFactory::~CTFFactory( void )
    {
    // Stub control deletes the stubs. Stub destructors
    // may access this object, so this is not removed
    // from tls before stubs are deleted.
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::~CTFFactory()" ) ) );
    delete iStubControl;
    Dll::SetTls( NULL );
    iPlugins.ResetAndDestroy();
    iPluginInfos.ResetAndDestroy();
    // Close local server
    delete iTestServer;
    // Close remote server
    RTFTestServer server;
    if ( server.Connect( EFalse ) == KErrNone )
        {
        server.PostShutdownMessage();
        server.Close();
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::~CTFFactory - return void" ) ) );
    }
    
    
// -----------------------------------------------------------------------------
// CTFFactory::Stub
// -----------------------------------------------------------------------------
EXPORT_C CTFStub* CTFFactory::Stub( TInt aType )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::Stub(0x%x)" ), aType ) );
    CTFStub* stub = NULL;
    CTFFactory* factory = STATIC_CAST( CTFFactory*, Dll::Tls() );
    if ( factory != NULL )
        {
        CTFStubControl* control = factory->iStubControl;
        stub = control->Stub( aType );
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::Stub - return 0x%x" ), stub ) );
    return stub;
    }


// -----------------------------------------------------------------------------
// CTFFactory::InternalStub
// -----------------------------------------------------------------------------
CTFStub* CTFFactory::InternalStub( TInt aType )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::InternalStub(0x%x)" ), aType ) );
    CTFStub* stub = iStubControl->Stub( aType );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::InternalStub - return 0x%x" ), stub ) );
    return stub;
    }


// -----------------------------------------------------------------------------
// CTFFactory::CallRemoteStubL
// -----------------------------------------------------------------------------
EXPORT_C void CTFFactory::CallRemoteStubL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4, TRequestStatus& aStatus )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::CallRemoteStubL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4, &aStatus ) );
    CTFTestServerWrapper* wrapper = new ( ELeave ) CTFTestServerWrapper();
    CleanupStack::PushL( wrapper );
    wrapper->PostMessageLD( aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4, aStatus );
    CleanupStack::Pop( wrapper );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::CallRemoteStubL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::CallRemoteStubL
// -----------------------------------------------------------------------------
EXPORT_C void CTFFactory::CallRemoteStubL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::CallRemoteStubL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4 ) );
    RTFTestServer server;
    User::LeaveIfError( server.Connect( EFalse ) );
    TInt result = server.PostMessage( aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4 );
    server.Close();
    User::LeaveIfError( result );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::CallRemoteStubL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::LocalThreadFunctionL
// -----------------------------------------------------------------------------
void CTFFactory::LocalThreadFunctionL( TTFThreadParam* aData )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::LocalThreadFunctionL(0x%x)" ), aData ) );
    // These need to be copied before the semaphore is signalled, since the calling thread
    // deletes the TTFThreadParameter structure after it exits RSemaphore.Wait()
    TInt stubType = aData->iStubType;
    TAny* data1 = aData->iMessageData1;
    TAny* data2 = aData->iMessageData2;
    TAny* data3 = aData->iMessageData3;
    TAny* data4 = aData->iMessageData4;
    aData->iSemaphore->Signal();
    CTFActiveScheduler* scheduler = new ( ELeave ) CTFActiveScheduler();
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );
    CTFTestServerWrapper* wrapper = new ( ELeave ) CTFTestServerWrapper();
    CleanupStack::PushL( wrapper );
    wrapper->PostLocalMessageL( stubType, data1, data2, data3, data4 );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::LocalThreadFunctionL - Entering scheduler" ) ) );
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( wrapper );
    CleanupStack::PopAndDestroy( scheduler );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::LocalThreadFunctionL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::LocalThreadFunction
// -----------------------------------------------------------------------------
TInt CTFFactory::LocalThreadFunction( TAny* aData )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::LocalThreadFunction(0x%x)" ), aData ) );
    TRACE_ASSERT( aData );
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( cleanup != NULL )
        {
        TRAPD( err, LocalThreadFunctionL( ( TTFThreadParam* )aData ) );
        TRACE_ASSERT( err == KErrNone );
        if( err != KErrNone )
        	{
        	COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::LocalThreadFunction - error" ) ) );
        	}
        delete cleanup;
        }
    else
        {
        // Semaphore must be signalled anyway
        ( ( TTFThreadParam* )aData )->iSemaphore->Signal();
        TRACE_ASSERT_ALWAYS;
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::LocalThreadFunction - return 0x%x" ), KErrNone ) );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTFFactory::CallLocalStubL
// -----------------------------------------------------------------------------
EXPORT_C void CTFFactory::CallLocalStubL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::CallLocalStubL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4 ) );
    TTFThreadParam param;
    RSemaphore semaphore;
    User::LeaveIfError( semaphore.CreateLocal( 0 ) );
    CleanupClosePushL( semaphore );
    RThread thread;
    param.iSemaphore = &semaphore;
    param.iStubType = aStubType;
    param.iMessageData1 = aMessageData1;
    param.iMessageData2 = aMessageData2;
    param.iMessageData3 = aMessageData3;
    param.iMessageData4 = aMessageData4;
    User::LeaveIfError( thread.Create( _L( "DUMMYTHREAD" ), LocalThreadFunction, KDefaultStackSize, KMinHeapSize, 0x50000, &param ) );
    thread.Resume();
    semaphore.Wait();
    CleanupStack::PopAndDestroy( &semaphore );
    thread.Close();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::CallLocalStubL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::StubControl
// -----------------------------------------------------------------------------
MTFStubControl* CTFFactory::StubControl( void )
    {
    return iStubControl;
    }
    
    
// -----------------------------------------------------------------------------
// CTFFactory::BuildTestSuiteL
// -----------------------------------------------------------------------------
void CTFFactory::BuildTestSuiteL( CTFATestSuite* aRootSuite )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::BuildTestSuiteL(0x%x)" ), aRootSuite ) );
    CTFStubModuleInterface::ListAllImplementationsL( iPluginInfos );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::BuildTestSuiteL - Found %d plug-ins" ), iPluginInfos.Count() ) );
    for ( TInt i = 0; i < iPluginInfos.Count(); i++ )
        {
        CTFStubModuleInterface *stubIF = CTFStubModuleInterface::NewL( iPluginInfos[i]->DataType() );
        CleanupStack::PushL( stubIF );
        User::LeaveIfError( iPlugins.Append( stubIF ) );
        CleanupStack::Pop( stubIF );
        }
    BuildSuiteFromModulesL( aRootSuite );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::BuildTestSuiteL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::BuildSuiteFromModulesL
// -----------------------------------------------------------------------------
void CTFFactory::BuildSuiteFromModulesL( CTFATestSuite* aRootSuite )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::BuildSuiteFromModulesL(0x%x)" ), aRootSuite ) );
    TInt index;
    // Adds stubs from every module
    for ( index = 0; index < iPlugins.Count(); index++ )
        {
        CTFStub* stub = iPlugins[index]->GetStubL();
        if ( stub != NULL )
            {
            CleanupStack::PushL( stub );
            iStubControl->AddStubL( *stub );
            CleanupStack::Pop( stub );
            }
        }
    // Initializes the stubs
    for ( index = 0; index < iStubControl->Count(); index++ )
        {
        CTFStub* stub = iStubControl->StubAt( index );
        stub->InitializeL();
        }
    // Adds test cases from every module
    for ( index = 0; index < iPlugins.Count(); index++ )
        {
        iPlugins[index]->BuildTestSuiteL( aRootSuite );
        }
    // Initializes test cases
    InitializeTestCasesL( aRootSuite );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::BuildSuiteFromModulesL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFFactory::InitializeTestCasesL
// -----------------------------------------------------------------------------
void CTFFactory::InitializeTestCasesL( CTFATestSuite* aSuite )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::InitializeTestCasesL(0x%x)" ), aSuite ) );
    for ( TInt i = 0; i < aSuite->Count(); i++ )
        {
        CTFATest* test = &aSuite->At( i );
        if ( test->Type() == ETFATestTypeSuite )
            {
            //lint -e826 Suspicious pointer-to-pointer conversion
            InitializeTestCasesL( STATIC_CAST( CTFATestSuite*, test ) );
            //lint +e826
            }
        else if ( test->Type() == ETFTestTypeStubWrapper )
            {
            for ( TInt j = 0; j < iPlugins.Count(); j++ )
                {
                MTFStubTestCase* stubCase = iPlugins[j]->GetStubTestCaseL( 
                    STATIC_CAST( CTFATestCase*, test )->Id() );
                if ( stubCase != NULL )
                    {
                    CleanupStack::PushL( stubCase );
                    STATIC_CAST( CTFTestCaseWrapper*, test )->AddStubTestCaseL( *stubCase );
                    CleanupStack::Pop( stubCase );
                    }
                }
            }
        else if ( test->Type() == ETFTestTypeStubRepeater )
            {
            // Repeaters do not contain stub-specific parts
            }
        else 
            {
            // Unrecognized test types are not handled
            }
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFFactory::InitializeTestCasesL - return void" ) ) );
    }




