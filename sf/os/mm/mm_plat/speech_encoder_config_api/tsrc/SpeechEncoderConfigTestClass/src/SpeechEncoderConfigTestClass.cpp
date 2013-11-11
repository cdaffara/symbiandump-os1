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
* Description: SpeechEncoderConfigTest
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
//#include "SpeechEncoderConfigTestClass.h"
#include "SpeechEncoderConfigTestClass.h"

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
// ?function_name .
//
// Returns: ?value_1:
//          ?value_n: _line1
//                    _line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  //
    ?arg_type arg)  //
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::CSpeechEncoderConfigTestClass
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSpeechEncoderConfigTestClass::CSpeechEncoderConfigTestClass(
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iTestModuleIf(aTestModuleIf)
    {
    }

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSpeechEncoderConfigTestClass::ConstructL()
    {
    iLog = CStifLogger::NewL( KSpeechEncoderConfigTestClassLogPath,
                          KSpeechEncoderConfigTestClassLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

		iTimeoutController = CSimpleTimeout::NewL (this, iLog);

    }

// -----------------------------------------------------------------------------
// CSpeechEncoderConfigTestClass::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSpeechEncoderConfigTestClass* CSpeechEncoderConfigTestClass::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CSpeechEncoderConfigTestClass* self = new (ELeave) CSpeechEncoderConfigTestClass( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CSpeechEncoderConfigTestClass::~CSpeechEncoderConfigTestClass()
    {

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog;
	delete iTimeoutController;
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

    return ( CScriptBase* ) CSpeechEncoderConfigTestClass::NewL( aTestModuleIf );

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
