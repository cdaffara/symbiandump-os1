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


#ifndef __EXIFLIBTEST_H
#define __EXIFLIBTEST_H


//  INCLUDES

#include <e32base.h>

#include <CppUnit/Test.h>
#include <CppUnit/TestCase.h>
#include <CppUnit/TestCaller.h>
#include <CppUnit/TestSuite.h>

#include "ExifLibTestInc.h"
// CLASS DECLARATION


class CExifLibTest : public CTestSuite
    {
    public:
        
        CExifLibTest(); //lint !e1526 is OK.
        ~CExifLibTest();
        
        
        // A function to collect and return a suite of tests
        static MTest* suiteL ();
    protected:
    private:
    };
#endif
