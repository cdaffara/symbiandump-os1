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
* Description:  headsetTestModule class member functions
*
*/



// INCLUDE FILES
#include "accMonitoringTestModule.h"
#include <Stiftestinterface.h>
#include <AccMonitor.h>
#include <AccessoryServer.h>
#include <AccMonTestTimer.h>

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
// CAccMonitoringTestModule::CAccMonitoringTestModule
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccMonitoringTestModule::CAccMonitoringTestModule()
    {
	}

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::ConstructL
// Symbian 2nd phase constructor can leave.
//
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CAccMonitoringTestModule::ConstructL()
    {
    iLog = CStifLogger::NewL( KheadsetTestModuleLogPath, 
                          KheadsetTestModuleLogFile);
	iLeaveFlag = EFalse;
	RAccessoryServer acc;
	TInt ret = acc.Connect();
	acc.Disconnect();
	iConnectedAccessory = CAccMonitorInfo::NewL();
	iConnectionEmulator = CConnectionEmulator::NewL();
	iWrongAccessory = EFalse;
    // Sample how to use logging
    _LIT( KLogStart, "headsetTestModule logging starts!" );
    iLog->Log( KLogStart );
    
    // Construct active scheduler
    iActiveScheduler = new ( ELeave ) CActiveScheduler;
    CActiveScheduler::Install( iActiveScheduler );
    iTestTimer = CAccMonTestTimer::NewLC();
	}

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccMonitoringTestModule* CAccMonitoringTestModule::NewL()
    {
    CAccMonitoringTestModule* self = new (ELeave) CAccMonitoringTestModule();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CAccMonitoringTestModule::~CAccMonitoringTestModule()
    {
    delete iLog;
    delete iActiveScheduler;
    delete iConnectionEmulator;
    delete iTestTimer;
    delete iConnectedAccessory;
    iActiveScheduler = NULL;
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::InitL
// InitL is used to Reset the Test Module.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    
    iDoLeave = EFalse;
    
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::GetTestCasesL( 
    const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {

    // Loop through all test cases and create new
    // TTestCaseInfo items and append items to aTestCase array    
    for( TInt i = 0; Case(i).iMethod != NULL; i++ )
        {

        // Allocate new TTestCaseInfo from heap for a testcase definition.
        TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo();

        // PushL TTestCaseInfo to CleanupStack.    
        CleanupStack::PushL( newCase );

        // Set number for the testcase.
        // When the testcase is run, this comes as a parameter to RunTestCaseL.
        newCase->iCaseNumber = i;

        // Set title for the test case. This is shown in UI to user.
        newCase->iTitle.Copy( Case(i).iCaseName );

        // Append TTestCaseInfo to the testcase array. After appended 
        // successfully the TTestCaseInfo object is owned (and freed) 
        // by the TestServer. 
        User::LeaveIfError(aTestCases.Append ( newCase ) );

        // Pop TTestCaseInfo from the CleanupStack.
        CleanupStack::Pop( newCase );

        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified 
// by aTestCase. Test cases that can be run may be requested from 
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CAccMonitoringTestModule::RunTestCaseL( 
    const TInt aCaseNumber,   
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {

    // Return value
    TInt execStatus = KErrNone;

    // Get the pointer to test case function
    TCaseInfo tmp = Case ( aCaseNumber );

    _LIT( KLogStartTC, "Starting testcase [%S]" );
    iLog->Log( KLogStartTC, &tmp.iCaseName);

    // Check that case number was valid
    if ( tmp.iMethod != NULL )
        {
        // Valid case was found, call it via function pointer
        iMethod = tmp.iMethod;        
        execStatus  = ( this->*iMethod )( aResult );
        }
    else
        {
        // Valid case was not found, return error.
        execStatus = KErrNotFound;
        }

    // Return case execution status (not the result of the case execution)
    return execStatus;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
// -----------------------------------------------------------------------------
//
TBool CAccMonitoringTestModule::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt aCaseNumber, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& aFirstMemFailure, 
                                TInt& aLastMemFailure ) 
    {
    _LIT( KLogOOMTestQueryL, "CAccMonitoringTestModule::OOMTestQueryL" );
    iLog->Log( KLogOOMTestQueryL );     

    aFirstMemFailure = Case( aCaseNumber ).iFirstMemoryAllocation;
    aLastMemFailure = Case( aCaseNumber ).iLastMemoryAllocation;

    return Case( aCaseNumber ).iIsOOMTest;

    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::OOMTestResetL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to Reset themselves.  
//
// NOTE: User may add implementation for OOM test environment initialization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CAccMonitoringTestModule::OOMTestResetL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: User may add implementation for OOM test warning handling. Usually no
// implementation is required.
// -----------------------------------------------------------------------------
//
void CAccMonitoringTestModule::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CAccMonitoringTestModule::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: User may add implementation for OOM test environment finalization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CAccMonitoringTestModule::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CAccMonitoringTestModule::NewL();

    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/, 
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

void CAccMonitoringTestModule::ResetArrays()
	{
	// Reset the wired headset
	iWiredHeadsetAccessory.Append( KAccMonHeadset );
	iWiredHeadsetAccessory.Append( KAccMonWired );
    iWiredHeadsetAccessory.Append( KAccMonStereoAudio );
    iWiredHeadsetAccessory.Append( KAccMonSpeakerAudioOutput );
    iWiredHeadsetAccessory.Append( KAccMonMicAudioInput );
    
    // Reset the wireless headset
    iWiredHeadsetAccessory.Append( KAccMonHeadset );
	iWiredHeadsetAccessory.Append( KAccMonBluetooth );
    iWiredHeadsetAccessory.Append( KAccMonMonoAudio );
    
    // Reset the wireless headset
    iWirelessCarKitAccessory.Append( KAccMonCarKit );
	iWirelessCarKitAccessory.Append( KAccMonBluetooth );
    iWirelessCarKitAccessory.Append( KAccMonStereoAudio );
    iWirelessCarKitAccessory.Append( KAccMonSpeakerAudioOutput );
    iWirelessCarKitAccessory.Append( KAccMonMicAudioInput );
    
    // Reset the wired carkit
    iWiredCarKitAccessory.Append( KAccMonCarKit );
	iWiredCarKitAccessory.Append( KAccMonWired );
    iWiredCarKitAccessory.Append( KAccMonStereoAudio );
    iWiredCarKitAccessory.Append( KAccMonSpeakerAudioOutput );
    iWiredCarKitAccessory.Append( KAccMonMicAudioInput );
    
    // Reset the tvout
    iTVOutAccessory.Append( KAccMonAVDevice );
	iTVOutAccessory.Append( KAccMonWired );
    iTVOutAccessory.Append( KAccMonStereoAudio );
    
    // Reset the music stand
    iMusicStandAccessory.Append( KAccMonOffice );
	iMusicStandAccessory.Append( KAccMonWired );
    iMusicStandAccessory.Append( KAccMonStereoAudio );
    iMusicStandAccessory.Append( KAccMonSpeakerAudioOutput );
    iMusicStandAccessory.Append( KAccMonMicAudioInput );
    
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File
