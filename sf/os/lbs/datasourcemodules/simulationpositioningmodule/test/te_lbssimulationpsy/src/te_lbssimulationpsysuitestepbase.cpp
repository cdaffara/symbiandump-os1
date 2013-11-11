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
//

/**
 @file Te_LbsSimulationPSYSuiteStepBase.cpp
 @internalTechnology
*/

#include "te_lbssimulationpsysuitestepbase.h"
#include "te_lbssimulationpsysuitedefs.h"

// Device driver constants

TVerdict CTe_LbsSimulationPSYSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsSimulationPSYSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_LbsSimulationPSYSuiteStepBase!"));
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_LbsSimulationPSYSuiteStepBase::~CTe_LbsSimulationPSYSuiteStepBase()
	{
	delete iScheduler;
	}

CTe_LbsSimulationPSYSuiteStepBase::CTe_LbsSimulationPSYSuiteStepBase()
	{
	}

void CTe_LbsSimulationPSYSuiteStepBase::LogErrorAndFail(const TDesC& aBuf)
	{
	ERR_PRINTF1(aBuf);
	SetTestStepResult(EFail);
	}

void CTe_LbsSimulationPSYSuiteStepBase::LogTrap(TInt aIndex)
	{
	_LIT(KMemLoop, "MemoryTest Loop - Possible TRAPped or non-leaving allocation (FAILNEXT=%u).");
	TBuf<128>   logbuf;
	logbuf.Format(KMemLoop, aIndex);
	// Not an error
	INFO_PRINTF1(logbuf);
	}


void CTe_LbsSimulationPSYSuiteStepBase::HeapTestL(const TInt aIndex)
    {
#ifdef __DEBUG__
    TInt err = KErrNone;
    TBool   warn = EFalse;
    const TInt KHeapFailureMaxLoop = KMaxTInt32 - 1;
    for (TInt i = 1; i < KHeapFailureMaxLoop; i++)
        {
        __UHEAP_FAILNEXT(i);
        __UHEAP_MARK;
        
        TRAP(err, DoTestL(aIndex));

        __UHEAP_MARKEND;
        
        // If no error occurred, fake a memory error to
        // make sure that this is the last test. If this
        // last allocation goes wrong, it proves that the
        // FAILNEXT() macro has reached its limit in this
        // test.
        if (err != KErrNoMemory)
            {
            TInt* dummy = new TInt;
            warn = (dummy != NULL);
            delete dummy;
            }

        __UHEAP_RESET;

        if ((err != KErrNoMemory) && !warn)
        	{
            break;
        	}

        if (warn)
            {
            // Commenting this out means we do not catch non leaving allocs
            // unfortunately testing for these generates too many false positives
            // as every use of INFO_PRINT triggers it.
            //LogTrap(aIndex);
            }
        }

    if (err != KErrNone)
        {
        User::Leave(err);
        }
#else
    DoTestL(aIndex);
#endif
    }
