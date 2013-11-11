// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This file contains step class implementation for DevVideo integration tests.
// 
//


#include "tsi_mmf_devvideo_stepbase.h"

TAny GetDriveName(TDes& aFilename)
    {
    #ifdef __WINS__
        aFilename = _L("c:");
    #elif defined(__MARM__) || defined(__X86GCC__)
        #ifdef TEST_FILES_ON_C_DRIVE
            aFilename = _L("c:");
        #else
            aFilename = _L("z:");
        #endif // TEST_FILES_ON_C_DRIVE
    #endif
    }

/**
 *
 * Set the suite name
 *
 */
void RTestStepDevVideoBase::SetTestSuite(const CTestSuiteDevVideo* aTestSuite)
    {
    iTestSuiteDevVideo = aTestSuite;
    }

/**
 *
 * RTestStepDevVideoBase constructor Implementation
 *
 */
RTestStepDevVideoBase::RTestStepDevVideoBase()
    :iActiveScheduler(NULL)
    {
    }
