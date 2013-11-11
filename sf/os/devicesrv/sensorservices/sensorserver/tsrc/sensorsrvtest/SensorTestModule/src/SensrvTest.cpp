/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <Stiftestinterface.h>
#include "SensrvTest.h"
#include "SensrvInternalPSKeys.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSensrvTest::CSensrvTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSensrvTest::CSensrvTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )///, iChannelInfoList(10)
    {
    }

// -----------------------------------------------------------------------------
// CSensrvTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensrvTest::ConstructL()
    {
    
    User::LeaveIfError( CreateNewThread() );
    
    
    
    iLog = CStifLogger::NewL( KSensrvTestLogPath, 
                          KSensrvTestLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
                          
                          
    // Set up status P&S value
    DefinePSKeyL( KSensrvLeaveFirstStubConstruction, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvLeaveFirstStubConstruction, 0 );
    DefinePSKeyL( KSensrvLeaveSecondStubConstruction, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvLeaveSecondStubConstruction, 0 );
    DefinePSKeyL( KSensrvFirstStubChannelCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvFirstStubChannelCount, 3 );
    DefinePSKeyL( KSensrvSecondStubChannelCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSecondStubChannelCount, 3 );
    DefinePSKeyL( KSensrvLatestOpenedChannel, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvLatestOpenedChannel, 0 );
    DefinePSKeyL( KSensrvLatestClosedChannel, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvLatestClosedChannel, 0 );
    DefinePSKeyL( KSensrvLatestStartListeningChannel, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvLatestStartListeningChannel, 0 );
    DefinePSKeyL( KSensrvLatestStopListeningChannel, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvLatestStopListeningChannel, 0 );
    DefinePSKeyL( KSensrvSamplingInterval, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSamplingInterval, 5 ); // 5 millisecond
    DefinePSKeyL( KSensrvSignalAftedDataReseived, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSignalAftedDataReseived, 0 );
    DefinePSKeyL( KSensrvStubConstructionJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStubConstructionJam, 0 );
    DefinePSKeyL( KSensrvStartListeningJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStartListeningJam, 0 );
    DefinePSKeyL( KSensrvStopListeningJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStopListeningJam, 0 );    
    DefinePSKeyL( KSensrvOpenChannelAsyncJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvOpenChannelAsyncJam, 0 );
    DefinePSKeyL( KSensrvCloseChannelAsyncJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCloseChannelAsyncJam, 0 );
    DefinePSKeyL( KSensrvChannelOpenedError, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvChannelOpenedError, 0 );
    DefinePSKeyL( KSensrvChannelClosedError, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvChannelClosedError, 0 );
    DefinePSKeyL( KSensrvStubConstructionDelay, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStubConstructionDelay, 0 ); // not delay
    DefinePSKeyL( KSensrvWaitTestCompletion, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvWaitTestCompletion, 1 ); // Wait test completion
    DefinePSKeyL( KSensrvStartListeningLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStartListeningLeave, 0 );
    DefinePSKeyL( KSensrvStopListeningLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStopListeningLeave, 0 );
    DefinePSKeyL( KSensrvDataListeningJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvDataListeningJam, 0 );
    DefinePSKeyL( KSensrvDataListeningBufferFilledError, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvDataListeningBufferFilledError, 0 );
    DefinePSKeyL( KSensrvForceBufferFilledJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvForceBufferFilledJam, 0 );
    DefinePSKeyL( KSensrvForceBufferFilledLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvForceBufferFilledLeave, 0 );
    DefinePSKeyL( KSensrvOpenChannelMethodJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvOpenChannelMethodJam, 0 );
    DefinePSKeyL( KSensrvCloseChannelMethodJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCloseChannelMethodJam, 0 );
    DefinePSKeyL( KSensrvOpenChannelError, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvOpenChannelError, 0 );
    DefinePSKeyL( KSensrvCloseChannelError, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCloseChannelError, 0 );
    DefinePSKeyL( KSensrvChannelOpenedWithError, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvChannelOpenedWithError, 0 );
    DefinePSKeyL( KSensrvPropertyChanged, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvPropertyChanged, 0 );//0 means that property changed notifications are not sent
    DefinePSKeyL( KSensrvOpenChannelMethodPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvOpenChannelMethodPanic, 0 );
    DefinePSKeyL( KSensrvOpenChannelAsyncPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvOpenChannelAsyncPanic, 0 );
    DefinePSKeyL( KSensrvCloseChannelMethodPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCloseChannelMethodPanic, 0 );
    DefinePSKeyL( KSensrvCloseChannelAsyncPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCloseChannelAsyncPanic, 0 );
    DefinePSKeyL( KSensrvStartListeningMethodPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStartListeningMethodPanic, 0 );
    DefinePSKeyL( KSensrvDataListeningPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvDataListeningPanic, 0 );
    DefinePSKeyL( KSensrvStopListeningMethodPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStopListeningMethodPanic, 0 );
    DefinePSKeyL( KSensrvStopListeningAsyncPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvStopListeningAsyncPanic, 0 );
    DefinePSKeyL( KSensrvForceBufferFilledPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvForceBufferFilledPanic, 0 );
    DefinePSKeyL( KSensrvSSYUnloadingCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSSYUnloadingCount, 0 );
    DefinePSKeyL( KSensrvBufferFilledNegativeCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvBufferFilledNegativeCount, 0 );
    DefinePSKeyL( KSensrvGetAllPropertyCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertyCount, 0 );    
    DefinePSKeyL( KSensrvSetPropertyArray, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyArray, 0 );    
    DefinePSKeyL( KSensrvGetPropertyLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetPropertyLeave, 0 );
    DefinePSKeyL( KSensrvGetPropertyJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetPropertyJam, 0 );
    DefinePSKeyL( KSensrvSetPropertyLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyLeave, 0 );
    DefinePSKeyL( KSensrvSetPropertyJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyJam, 0 );
    DefinePSKeyL( KSensrvGetAllPropertiesLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertiesLeave, 0 );
    DefinePSKeyL( KSensrvGetAllPropertiesJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertiesJam, 0 );
    DefinePSKeyL( KSensrvGetPropertyPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetPropertyPanic, 0 );
    DefinePSKeyL( KSensrvSetPropertyPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSetPropertyPanic, 0 );
    DefinePSKeyL( KSensrvGetAllPropertiesPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetAllPropertiesPanic, 0 );
    DefinePSKeyL( KSensrvCheckPropertyDependenciesLeave, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesLeave, 0 );
    DefinePSKeyL( KSensrvCheckPropertyDependenciesJam, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesJam, 0 );
    DefinePSKeyL( KSensrvCheckPropertyDependenciesPanic, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCheckPropertyDependenciesPanic, 0 );
    DefinePSKeyL( KSensrvGetDataOverflowTest, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetDataOverflowTest, 0 );
    DefinePSKeyL( KSensrvGetDataNotFoundTest, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvGetDataNotFoundTest, 0 );    
    DefinePSKeyL( KSensrvCheckPluginLoaded, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvCheckPluginLoaded, 0 );    
    
    TInt err( RProperty::Define( KPSUidSensrvTest, KSensrvTestProcessCommand, RProperty::EByteArray ) );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    RProperty::Set( KPSUidSensrvTest, KSensrvTestProcessCommand, KNullDesC );    

    DefinePSKeyL( KSensrvSingleBufferFilledNegativeCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvSingleBufferFilledNegativeCount, 0 );

    // These keys are listened to by stubs, so do not set them to zero,
    // as it will trigger the listeners of stubs if there are multiple
    // test classes. These keys are only read when they change, so their
    // initial value is irrelevant.
    DefinePSKeyL( KSensrvCreateNewChannelsFirst, RProperty::EInt );
    DefinePSKeyL( KSensrvCreateNewChannelsSecond, RProperty::EInt );
    DefinePSKeyL( KSensrvRemoveNewChannelsFirst, RProperty::EInt );
    DefinePSKeyL( KSensrvRemoveNewChannelsSecond, RProperty::EInt );
    DefinePSKeyL( KSensrvRegisterZeroChannels, RProperty::EInt );
    DefinePSKeyL( KSensrvRegisterInvalidChannels, RProperty::EInt );
    
    DefinePSKeyL( KSensrvFirstStubDoubleTappingChannelCount, RProperty::EInt );
    RProperty::Set( KPSUidSensrvTest, KSensrvFirstStubDoubleTappingChannelCount, 0 );
    }

    
void CSensrvTest::DefinePSKeyL( TUint32 aKey, TInt aAttr )
    {
    TInt err( RProperty::Define( KPSUidSensrvTest, aKey, aAttr ) );
    if ( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    }

// -----------------------------------------------------------------------------
// CSensrvTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvTest* CSensrvTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CSensrvTest* self = new (ELeave) CSensrvTest( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CSensrvTest::~CSensrvTest()
    { 

    // Delete resources allocated from test methods
    Delete();
    
    // Delete logger
    delete iLog; 
    
    iThread.Close();
    
    }
    
    

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CSensrvTest::NewL( aTestModuleIf );

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File
