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
* Description: ilbc encoder test
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
//#include "IlbcEncoderIntfcTestClass.h"
#include "IlbcEncoderIntfcTestClass.h"

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
// CIlbcEncoderIntfcTestClass::CIlbcEncoderIntfcTestClass
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CIlbcEncoderIntfcTestClass::CIlbcEncoderIntfcTestClass(
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iTestModuleIf(aTestModuleIf)
    {
    }

// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcTestClass::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CIlbcEncoderIntfcTestClass::ConstructL()
    {
    iLog = CStifLogger::NewL( KIlbcEncoderIntfcTestClassLogPath,
                          KIlbcEncoderIntfcTestClassLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

		iTimeoutController = CSimpleTimeout::NewL (this, iLog);

    }

// -----------------------------------------------------------------------------
// CIlbcEncoderIntfcTestClass::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CIlbcEncoderIntfcTestClass* CIlbcEncoderIntfcTestClass::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CIlbcEncoderIntfcTestClass* self = new (ELeave) CIlbcEncoderIntfcTestClass( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CIlbcEncoderIntfcTestClass::~CIlbcEncoderIntfcTestClass()
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

    return ( CScriptBase* ) CIlbcEncoderIntfcTestClass::NewL( aTestModuleIf );

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
