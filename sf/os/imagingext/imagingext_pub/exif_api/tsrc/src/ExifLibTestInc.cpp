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


#ifdef __WINSCW__
#define KWinscwChar
#else
#define KWinscwChar &
#endif
// INCLUDE FILES
#include "ExifLibTestInc.h"
#include <fbs.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES 

// CONSTANTS
// #define MMC

// MACROS
#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )

#else

#endif

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


CExifLibTestInc::CExifLibTestInc()
    {
    }



CExifLibTestInc::~CExifLibTestInc()
    {
    }


void CExifLibTestInc::setUpL ()
    {
    }


void CExifLibTestInc::tearDown ()
    {
    }

//
// Own test functions.
//

MTest* CExifLibTestInc::suiteL ()
    {
    // Always use NewL (Do not use NewLC) !!!
    CTestSuite *suite = CTestSuite::NewL(_L8("ExifLibTest"));
    
    suite->addTestL(CTestCaller<CExifTagTest>::NewL(_L8("EXIF.TAG.001"), KWinscwChar(CExifTagTest::ExifTag001L)));
   
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.001"), KWinscwChar(CExifReadTest::ExifRead001L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.002"), KWinscwChar(CExifReadTest::ExifRead002L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.003"), KWinscwChar(CExifReadTest::ExifRead003L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.004"), KWinscwChar(CExifReadTest::ExifRead004L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.005"), KWinscwChar(CExifReadTest::ExifRead005L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.006"), KWinscwChar(CExifReadTest::ExifRead006L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.007"), KWinscwChar(CExifReadTest::ExifRead007L)));

    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.008"), KWinscwChar(CExifReadTest::ExifRead008L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.009"), KWinscwChar(CExifReadTest::ExifRead009L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.010"), KWinscwChar(CExifReadTest::ExifRead010L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.011"), KWinscwChar(CExifReadTest::ExifRead011L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.012"), KWinscwChar(CExifReadTest::ExifRead012L)));
    
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.013"), KWinscwChar(CExifReadTest::ExifRead013L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.014"), KWinscwChar(CExifReadTest::ExifRead014L)));
    
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.016"), KWinscwChar(CExifReadTest::ExifRead016L)));
    
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.001"), KWinscwChar(CExifCreateTest::ExifCreate001L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.002"), KWinscwChar(CExifCreateTest::ExifCreate002L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.003"), KWinscwChar(CExifCreateTest::ExifCreate003L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.004"), KWinscwChar(CExifCreateTest::ExifCreate004L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.005"), KWinscwChar(CExifCreateTest::ExifCreate005L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.006"), KWinscwChar(CExifCreateTest::ExifCreate006L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.007"), KWinscwChar(CExifCreateTest::ExifCreate007L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.008"), KWinscwChar(CExifCreateTest::ExifCreate008L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.009"), KWinscwChar(CExifCreateTest::ExifCreate009L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.010"), KWinscwChar(CExifCreateTest::ExifCreate010L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.011"), KWinscwChar(CExifCreateTest::ExifCreate011L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.012"), KWinscwChar(CExifCreateTest::ExifCreate012L)));
   
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.001"), KWinscwChar(CExifModifyTest::ExifModify001L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.002"), KWinscwChar(CExifModifyTest::ExifModify002L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.003"), KWinscwChar(CExifModifyTest::ExifModify003L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.004"), KWinscwChar(CExifModifyTest::ExifModify004L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.005"), KWinscwChar(CExifModifyTest::ExifModify005L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.006"), KWinscwChar(CExifModifyTest::ExifModify006L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.007"), KWinscwChar(CExifModifyTest::ExifModify007L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.008"), KWinscwChar(CExifModifyTest::ExifModify008L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.009"), KWinscwChar(CExifModifyTest::ExifModify009L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.010"), KWinscwChar(CExifModifyTest::ExifModify010L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.011"), KWinscwChar(CExifModifyTest::ExifModify011L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.012"), KWinscwChar(CExifModifyTest::ExifModify012L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.013"), KWinscwChar(CExifModifyTest::ExifModify013L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.014"), KWinscwChar(CExifModifyTest::ExifModify014L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.015"), KWinscwChar(CExifModifyTest::ExifModify015L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.016"), KWinscwChar(CExifModifyTest::ExifModify016L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.018"), KWinscwChar(CExifModifyTest::ExifModify018L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.019"), KWinscwChar(CExifModifyTest::ExifModify019L)));

    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.001"), KWinscwChar(CExifModifyTest2::ExifModify001L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.002"), KWinscwChar(CExifModifyTest2::ExifModify002L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.003"), KWinscwChar(CExifModifyTest2::ExifModify003L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.004"), KWinscwChar(CExifModifyTest2::ExifModify004L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.005"), KWinscwChar(CExifModifyTest2::ExifModify005L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.006"), KWinscwChar(CExifModifyTest2::ExifModify006L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.007"), KWinscwChar(CExifModifyTest2::ExifModify007L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.008"), KWinscwChar(CExifModifyTest2::ExifModify008L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.009"), KWinscwChar(CExifModifyTest2::ExifModify009L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.010"), KWinscwChar(CExifModifyTest2::ExifModify010L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.011"), KWinscwChar(CExifModifyTest2::ExifModify011L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.012"), KWinscwChar(CExifModifyTest2::ExifModify012L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.013"), KWinscwChar(CExifModifyTest2::ExifModify013L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.014"), KWinscwChar(CExifModifyTest2::ExifModify014L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.015"), KWinscwChar(CExifModifyTest2::ExifModify015L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.016"), KWinscwChar(CExifModifyTest2::ExifModify016L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.018"), KWinscwChar(CExifModifyTest2::ExifModify018L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.019"), KWinscwChar(CExifModifyTest2::ExifModify019L)));

    // Fast parser test cases
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.101"), KWinscwChar(CExifReadTest::ExifRead101L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.102"), KWinscwChar(CExifReadTest::ExifRead102L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.103"), KWinscwChar(CExifReadTest::ExifRead103L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.104"), KWinscwChar(CExifReadTest::ExifRead104L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.105"), KWinscwChar(CExifReadTest::ExifRead105L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.106"), KWinscwChar(CExifReadTest::ExifRead106L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.107"), KWinscwChar(CExifReadTest::ExifRead107L)));

    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.108"), KWinscwChar(CExifReadTest::ExifRead108L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.109"), KWinscwChar(CExifReadTest::ExifRead109L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.110"), KWinscwChar(CExifReadTest::ExifRead110L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.111"), KWinscwChar(CExifReadTest::ExifRead111L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.112"), KWinscwChar(CExifReadTest::ExifRead112L)));
    
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.113"), KWinscwChar(CExifReadTest::ExifRead113L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.114"), KWinscwChar(CExifReadTest::ExifRead114L)));
    
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.116"), KWinscwChar(CExifReadTest::ExifRead116L)));
    
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.101"), KWinscwChar(CExifCreateTest::ExifCreate101L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.102"), KWinscwChar(CExifCreateTest::ExifCreate102L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.103"), KWinscwChar(CExifCreateTest::ExifCreate103L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.104"), KWinscwChar(CExifCreateTest::ExifCreate104L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.105"), KWinscwChar(CExifCreateTest::ExifCreate105L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.106"), KWinscwChar(CExifCreateTest::ExifCreate106L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.107"), KWinscwChar(CExifCreateTest::ExifCreate107L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.108"), KWinscwChar(CExifCreateTest::ExifCreate108L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.109"), KWinscwChar(CExifCreateTest::ExifCreate109L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.110"), KWinscwChar(CExifCreateTest::ExifCreate110L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.111"), KWinscwChar(CExifCreateTest::ExifCreate111L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.112"), KWinscwChar(CExifCreateTest::ExifCreate112L)));
   
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.101"), KWinscwChar(CExifModifyTest::ExifModify101L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.102"), KWinscwChar(CExifModifyTest::ExifModify102L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.103"), KWinscwChar(CExifModifyTest::ExifModify103L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.104"), KWinscwChar(CExifModifyTest::ExifModify104L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.105"), KWinscwChar(CExifModifyTest::ExifModify105L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.106"), KWinscwChar(CExifModifyTest::ExifModify106L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.107"), KWinscwChar(CExifModifyTest::ExifModify107L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.108"), KWinscwChar(CExifModifyTest::ExifModify108L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.109"), KWinscwChar(CExifModifyTest::ExifModify109L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.110"), KWinscwChar(CExifModifyTest::ExifModify110L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.111"), KWinscwChar(CExifModifyTest::ExifModify111L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.112"), KWinscwChar(CExifModifyTest::ExifModify112L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.113"), KWinscwChar(CExifModifyTest::ExifModify113L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.114"), KWinscwChar(CExifModifyTest::ExifModify114L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.115"), KWinscwChar(CExifModifyTest::ExifModify115L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.116"), KWinscwChar(CExifModifyTest::ExifModify116L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.118"), KWinscwChar(CExifModifyTest::ExifModify118L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.119"), KWinscwChar(CExifModifyTest::ExifModify119L)));

    // No tag validity check test cases
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.201"), KWinscwChar(CExifReadTest::ExifRead201L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.202"), KWinscwChar(CExifReadTest::ExifRead202L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.203"), KWinscwChar(CExifReadTest::ExifRead203L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.204"), KWinscwChar(CExifReadTest::ExifRead204L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.205"), KWinscwChar(CExifReadTest::ExifRead205L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.206"), KWinscwChar(CExifReadTest::ExifRead206L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.207"), KWinscwChar(CExifReadTest::ExifRead207L)));

    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.208"), KWinscwChar(CExifReadTest::ExifRead208L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.209"), KWinscwChar(CExifReadTest::ExifRead209L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.210"), KWinscwChar(CExifReadTest::ExifRead210L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.211"), KWinscwChar(CExifReadTest::ExifRead211L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.212"), KWinscwChar(CExifReadTest::ExifRead212L)));
    
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.213"), KWinscwChar(CExifReadTest::ExifRead213L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.214"), KWinscwChar(CExifReadTest::ExifRead214L)));
    
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.216"), KWinscwChar(CExifReadTest::ExifRead216L)));

    // IOP test

    suite->addTestL(CTestCaller<CExifIopTest>::NewL(_L8("EXIF.IOP.001"), KWinscwChar(CExifIopTest::ExifIop001L)));
	suite->addTestL(CTestCaller<CExifIopTest>::NewL(_L8("EXIF.IOP.002"), KWinscwChar(CExifIopTest::ExifIop002L)));
    
    suite->addTestL(CTestCaller<CExifIopTest>::NewL(_L8("EXIF.IOP.003"), KWinscwChar(CExifIopTest::ExifIop003L)));
    
    // CameraAPI test case
    suite->addTestL(CTestCaller<CExifIopTest>::NewL(_L8("EXIF.IOP.005"), KWinscwChar(CExifIopTest::ExifIop005L)));

    // OOM tests
//#if ( ( defined __WINS__ ) || ( defined __WINSCW__) )

    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.MEM.015"), KWinscwChar(CExifReadTest::ExifRead015L)));
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.MEM.115"), KWinscwChar(CExifReadTest::ExifRead115L)));
    suite->addTestL(CTestCaller<CExifTagTest>::NewL(_L8("EXIF.TAG.MEM.002"), KWinscwChar(CExifTagTest::ExifTag002L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.MEM.013"), KWinscwChar(CExifCreateTest::ExifCreate013L)));
    suite->addTestL(CTestCaller<CExifCreateTest>::NewL(_L8("EXIF.CREATE.MEM.113"), KWinscwChar(CExifCreateTest::ExifCreate113L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.MEM.017"), KWinscwChar(CExifModifyTest::ExifModify017L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.MEM.117"), KWinscwChar(CExifModifyTest::ExifModify117L)));
    suite->addTestL(CTestCaller<CExifModifyTest2>::NewL(_L8("EXIF.MODIFY2.MEM.017"), KWinscwChar(CExifModifyTest2::ExifModify017L)));
    suite->addTestL(CTestCaller<CExifIopTest>::NewL(_L8("EXIF.IOP.MEM.004"), KWinscwChar(CExifIopTest::ExifIop004L)));

//#endif
    // Fast parser test cases
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.FAST.017"), KWinscwChar(CExifReadTest::ExifRead017L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.FAST.020"), KWinscwChar(CExifModifyTest::ExifModify020L)));

    // Unknown tags test cases
    // Modify image that has unknown (vendor specific) tags and write it back. This needs manual verification
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.UNKNOWN.021"), KWinscwChar(CExifModifyTest::ExifModify021L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.UNKNOWN.022"), KWinscwChar(CExifModifyTest::ExifModify022L)));
    suite->addTestL(CTestCaller<CExifModifyTest>::NewL(_L8("EXIF.MODIFY.UNKNOWN.023"), KWinscwChar(CExifModifyTest::ExifModify023L)));

    // No tag validity checking test cases
    suite->addTestL(CTestCaller<CExifReadTest>::NewL(_L8("EXIF.READ.NOTAGCHECKING.018"), KWinscwChar(CExifReadTest::ExifRead018L)));

    return suite;
    }
