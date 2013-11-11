/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


// INCLUDE FILES
#include <tftestmacros.h>
#include <ctfatestenvironment.h>
#include <psvariables.h>
#include "dosenums.h"
#include "siftrace.h"
#include "ctfdosplugin.h"
#include "ctfdoscontrol.h"
#include "ctfdoscontroltestcaseparam.h"
#include "ctfdoscontroltestcase.h"
#include <psvariables.h>

static void SharedDataUseCaseSuiteL( CTFATestSuite* aSuite );

static void ExtensionUseCaseSuiteL( CTFATestSuite* aSuite );



/***************************** Extension *********************************/  
// DOSSERVER [Open and Close Extension]
// Opens and closes the extension service.
// Tested function:
//   - RDosExtension::Open()
//   - RDosExtension::Close()																					
const TTFDosServerControlTestCaseState KTFExtensionOpenCloseStates[2] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// DOSSERVER [Call Function Sync]
// Tests sync CallFunction with invalid parameter.
// Tested function:
//   - RDosExtension::CallFunction() ( sync )
const TTFDosServerControlTestCaseState KTFExtensionInvalidSyncCallFunctionStates[3] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_CallFunction, 500, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// DOSSERVER [Call Function Async]
// Tests async CallFunction with invalid parameter.
// Tested function:
//   - RDosExtension::CallFunction() ( async )
const TTFDosServerControlTestCaseState KTFExtensionInvalidAsyncCallFunctionStates[3] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_CallFunctionAndComplete, 600, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// DOSSERVER [General Event]
// Tests async CallFunction with invalid parameter.
const TTFDosServerControlTestCaseState KTFExtensionGeneralEventStates[4] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_GeneralEvent, KLDSYDummyEvent, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosEvent_Synchronized, 5, 0, KErrNone, ETFDOSDummyEvent, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// DOSSERVER [General Event Array]
const TTFDosServerControlTestCaseState KTFExtensionGeneralEventArrayStates[4] =
    {
        { ETFDosExtension_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_GeneralEventArray, KLDSYDummyEvent, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosEvent_Synchronized, 5, 0, KErrNone, ETFDOSDummyEvent, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosExtension_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };


/***************************** SharedData *********************************/   
// DOSSERVER [Open and Close SharedData]
// Opens and closes the SharedData service.
// Tested function:
//   - RDosSharedData::Open()
//   - RDosSharedData::Close()
const TTFDosServerControlTestCaseState KTFSharedDataOpenCloseStates[2] = 
    {
        { ETFDosSharedData_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSharedData_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };
          
// DOSSERVER [SharedData - RequestFreeDiskSpace]
// Fill the diskspace and request 256kb of free diskspace from SharedData
// Tested function:
//	 - RDosSharedData::RequestFreeDiskSpace()
const TTFDosServerControlTestCaseState KTFSharedDataRequestFreeDiskSpace[9] = 
    {
        { ETFDosSharedData_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFRFs_Connect, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_FillDisk, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSharedData_CheckDiskSpace, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_RequestFreeDiskSpace, 256, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents},
        { ETFDosSharedData_CheckDiskSpace, 256, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_ReleaseDisk, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFRFs_Close,   0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents }
    };

// DOSSERVER [SharedData - RequestFreeDiskSpaceCancel]
// Fill the diskspace, request 128kb of free diskspace and cancel the request
// Tested function:
//	 - RDosSharedData::RequestFreeDiskSpaceCancel()
const TTFDosServerControlTestCaseState KTFSharedDataRequestFreeDiskSpaceCancel[10] = 
    {
        { ETFDosSharedData_Open, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFRFs_Connect, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_FillDisk, 64, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSharedData_CheckDiskSpace, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_RequestFreeDiskSpace, 256, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents},
        { ETFDosSharedData_RequestFreeDiskSpaceCancel, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents},
        { ETFDosSharedData_Close, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFDosSharedData_CheckDiskSpace, 0, 1, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreParameters },
        { ETFDosSharedData_ReleaseDisk, 0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents },
        { ETFRFs_Close,   0, 0, KErrNone, ETFDosEvent_None, ETFDosFlags_IgnoreUnexpectedEvents | ETFDosFlags_IgnoreParameters }
    };

// C++ default constructor can NOT contain any code, that
// might leave.
//
CTFDosPlugin::CTFDosPlugin( TAny* aInitParams )
: iInitParams( ( CTFStubModuleInterface::TInterfaceInitParams* ) aInitParams )
	{
	}

// Two-phased constructor.
CTFDosPlugin* CTFDosPlugin::NewL( TAny* aInitParams )
	{
	return new ( ELeave ) CTFDosPlugin( aInitParams );
	}

// Destructor.
CTFDosPlugin::~CTFDosPlugin()
	{
    iInitParams = NULL;
	}


CTFStub* CTFDosPlugin::GetStubL( void )
    {
    return new ( ELeave ) CTFDosControl();
    }


void CTFDosPlugin::BuildTestSuiteL( CTFATestSuite* aRootSuite )
    {
    // Memory checks are disabled since DSY runs in separate thread and heap.
    aRootSuite->Environment().SetTestCaseFlags( ETFMayLeakMemory | ETFMayLeakRequests | ETFMayLeakHandles );
    TF_START_TEST_DECLARATION_L( aRootSuite );
        // Use case test cases
        TF_ADD_TEST_SUITE_L( _L( "Use Case Tests" ) );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "SharedData Tests" ), SharedDataUseCaseSuiteL );            
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Extension" ), ExtensionUseCaseSuiteL );
        TF_END_TEST_SUITE();
    TF_END_TEST_DECLARATION();
    }


MTFStubTestCase* CTFDosPlugin::GetStubTestCaseL( TInt aTestCaseId )
    {
    TF_START_STUB_TEST_CASES( aTestCaseId );

	
	TF_DOS_SERVER_CONTROL_TEST_CASE_L( 1, KTFSharedDataOpenCloseStates ) ;
		
	TF_DOS_SERVER_CONTROL_TEST_CASE_L( 2, KTFSharedDataRequestFreeDiskSpace ) ;
		
	TF_DOS_SERVER_CONTROL_TEST_CASE_L( 3, KTFSharedDataRequestFreeDiskSpaceCancel ) ;	

    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 4, KTFExtensionOpenCloseStates ) ;
    
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 5, KTFExtensionInvalidSyncCallFunctionStates ) ;
    
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 6, KTFExtensionInvalidAsyncCallFunctionStates ) ;
    
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 7, KTFExtensionGeneralEventStates ) ;
    
    TF_DOS_SERVER_CONTROL_TEST_CASE_L( 8, KTFExtensionGeneralEventArrayStates ) ;        
    
  TF_END_STUB_TEST_CASES();
    }
    


static void SharedDataUseCaseSuiteL( CTFATestSuite* aSuite )
	{
	TF_INIT_TEST_SUITE_FUNCTION( aSuite );
	TF_ADD_TEST_CASE_L( 1, _L( "DOSSERVER [Open and Close SharedData]" ) );
	TF_ADD_TEST_CASE_L( 2, _L( "DOSSERVER [SharedData - Request]" ) );
	TF_ADD_TEST_CASE_L( 3, _L( "DOSSERVER [SharedData - Cancel Request]" ) );
	}

static void ExtensionUseCaseSuiteL( CTFATestSuite* aSuite )
    {
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );
    TF_ADD_TEST_CASE_L( 4, _L( "DOSSERVER [Open and Close Extension]" ) );
    TF_ADD_TEST_CASE_L( 5, _L( "DOSSERVER [Call Function Sync]" ) );
    TF_ADD_TEST_CASE_L( 6, _L( "DOSSERVER [Call Function Async]" ) );
    TF_ADD_TEST_CASE_L( 7, _L( "DOSSERVER [General Event]" ) );
    TF_ADD_TEST_CASE_L( 8, _L( "DOSSERVER [General Event Array]" ) );
    }

// End of File
