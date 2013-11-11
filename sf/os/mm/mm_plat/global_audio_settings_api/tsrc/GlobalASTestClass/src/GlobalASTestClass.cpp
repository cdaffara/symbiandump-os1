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
* Description: Global Audio Settings Test
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "GlobalASTestClass.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlobalASTestClass::CGlobalASTestClass
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGlobalASTestClass::CGlobalASTestClass(
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf ),
        iTestModuleIf(aTestModuleIf)
    {
    }

// -----------------------------------------------------------------------------
// CGlobalASTestClass::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGlobalASTestClass::ConstructL()
    {
    iLog = CStifLogger::NewL( KGlobalASTestClassLogPath,
                          KGlobalASTestClassLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );

    }

// -----------------------------------------------------------------------------
// CGlobalASTestClass::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGlobalASTestClass* CGlobalASTestClass::NewL( CTestModuleIf& aTestModuleIf )
    {
    CGlobalASTestClass* self = new (ELeave) CGlobalASTestClass( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CGlobalASTestClass::~CGlobalASTestClass()
    {
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    return ( CScriptBase* ) CGlobalASTestClass::NewL( aTestModuleIf );
    }


//  End of File
