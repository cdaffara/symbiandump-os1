/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Exif Library Test DLL
*
*/



// INCLUDE FILES


#include "ExifLibTest.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES 
 
// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
IMPORT_C  MTest* CreateTopFct ();

// FORWARD DECLARATIONS


// Starting point of the DLL (Always needed) 
GLDEF_C TInt E32Dll(TDllReason)
    {
    return(KErrNone);
    }

// Exactly one exported function returning the suite of 
// test functions for the test runner in the framework.
// (Always needed)
//
EXPORT_C MTest* CreateTopFct() //lint !e714 
    {
    MTest * testSuite = NULL;
    TRAPD(err, testSuite =  CExifLibTest::suiteL() ) ;
    if (err)
        {
        User::Panic(_L("suiteL has left !"), 99);
        }
    return ( testSuite );
    }


CExifLibTest::~CExifLibTest()
    {
    }


MTest*  CExifLibTest::suiteL ()
    {
    CTestSuite *suite = CTestSuite::NewL(_L8("Test Suite Container"));

    suite->addTestL(CExifLibTestInc::suiteL());

    return suite;
    
    }


