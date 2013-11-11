// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <e32std.h>
#include <e32math.h>
#include <e32atomics.h>
#include "egltest_endpoint_engine.h"
#include "egltest_endpoint_stress_engine.h"


CStressTestEngine * CStressTestEngine::NewL(const TStressTestTable aStressTable[], TUint aNumTables)
    {
    CStressTestEngine *self = new (ELeave) CStressTestEngine;
    CleanupStack::PushL(self);
    self->ConstructL(aStressTable, aNumTables);
    CleanupStack::Pop(self);
    return self;
    }


void CStressTestEngine::ConstructL(const TStressTestTable aStressTable[], TUint aNumCases)
    {
    iStressTable = aStressTable;
    iNumCases = aNumCases;
    }


// DoPreamble - called from framework, set up list of Test IDs, so that we can
// mark them done later on - this is different from the regular 
//  CEglTest_Engine_Local code in that the data structure is different.
//
// Then call the common part with the regular CEglTest_Engine_Local 
void CStressTestEngine::DoPreambleL()
    {
    for(TInt curOuterCase = 0; curOuterCase < iNumCases; curOuterCase++)
        {
        const TTestCaseBase& innerCases = iStressTable[curOuterCase].iTestCaseBase;
        TPtrC testIds(innerCases.iRelatedTestIds);
        RegisterTestIdsL(testIds);
        }
    
    CommonPreambleL();
    }

// Derived function from CEglTest_Engine_Local, so that we can keep the local
// test result. 
void CStressTestEngine::DoPostambleL()
    {
    // Don't loose the verdict.
    TVerdict v = TestStepResult();
    CEgltest_Local_Engine::DoPostambleL();
    SetTestStepResult(v);
    }

CStressTestEngine::CStressTestEngine()
    {
    }


CStressTestEngine::~CStressTestEngine()
    {
    }


// Check if the entry needs modification, if so modify it. Modification is 
// indicated by presence of a flag bit and an index for which parameter
// to use. 
void CStressTestEngine::DoModifyL(TInt& aItem)
    {
    if (aItem & EExpandParamFlag)
        {
        TInt paramIndex = (aItem & EParamMask) >> EParamShift;
        if (!iParams[paramIndex])
            {
            ERR_PRINTF2(_L("Using undefined parameter PARAM%d"), paramIndex);
            User::Leave(KErrArgument);
            }
        User::LeaveIfNull(iParams[paramIndex]);
        aItem &= ERemoveFlagsMask;
        aItem += iParams[paramIndex]->Value();
        }
    }


// Modification of parameters of the testcase entry.
void CStressTestEngine::ModifyL(TEngineTestCase &aTestCase)
    {
    DoModifyL(aTestCase.iEndpointIndex);
    DoModifyL(aTestCase.iImageIndex);
    DoModifyL(aTestCase.iArg1);
    DoModifyL(aTestCase.iArg2);
    }


// Move to the next parameter for the parameter set.
void CStressTestEngine::NextParams()
    {
    for(TInt i = 0; i < KMaxParams; i++)
        {
        // Only update parameters that have been created.
        if (iParams[i])
            {
            iParams[i]->Next();
            }
        }
    }


// Create parameter set.
void CStressTestEngine::CreateParams(const TStressTestSection &aSection)
    {
    for(TInt i = 0; i < KMaxParams; i++)
        {
        ENGINE_ASSERT(!iParams[i]);
        iParams[i] = CStressParam::Factory(aSection.iParams[i]);
        }
    }


// Destroy parameter set.
void CStressTestEngine::DestroyParams()
    {
    for(TInt i = 0; i < KMaxParams; i++)
        {
        // It is harmless to delete a non-existant parameter, so 
        // no check needed.
        delete iParams[i];
        iParams[i] = NULL;
        }
    }


// Set the verdict for this operation on the local exec thread.
void CStressTestEngine::UpdateVerdict(TVerdict aVerdict)
    {
    TRemoteTestParams params;
    params.iEndpointEngine.iEngineTestCase.iCase = ESetVerdictCase;
    params.iEndpointEngine.iEngineTestCase.iEndpointIndex = EPass;
    TExecResult result;
    RunLocalTestCase(params, result);
    RunRemoteTestCase(ESetVerdictCase, params); 
    SetTestStepResult(aVerdict);
    }


// Look at the error value and update the verdict accordingly.
// Returns true if we should stop the iterating over the rest of the section.
// @param aSection - the section currently being executed.
// @param aCopy - the current iteration of the loop. 
// @param aCopies - total number of iterations to run. 
// @param aCurSection - numeric indication of the section. 
// @param aPreviousCopiesRun - updated according to policy to indicate how
//      many iterations to run the next section. 
TBool CStressTestEngine::HandleError(
        const TStressTestSection &aSection, 
        TInt aCopy, 
        TInt aCopies, 
        TInt aCurSection,
        TInt &aPreviousCopiesRun)
    {
    switch(aSection.iErrorPolicy)
        {
        case EOnErrorPolicyContinue:
            // Just let it pass...
            UpdateVerdict(EPass);
            break;
        case EOnErrorPolicyFail:
            // We should not have errors here.
            UpdateVerdict(EFail);
            break;
        case EOnErrorPolicyBreak:
            // Do not continue after errors.
            INFO_PRINTF4(_L("Ran only %d iterations of %d of section %d [not a failure]"), 
                    aCopy, aCopies, aCurSection);
            aPreviousCopiesRun = aCopy-1;
            UpdateVerdict(EPass);
            return ETrue;
        default:
            ENGINE_ASSERT(0);
            break;
        }
    return EFalse;
    }

// The main loop for the test. 
// Brief pseudo-code: 
// for each testcase
//   for each surface_type
//     for each section
//       for each copy of the section
//         perform engine operation
// Copy is really just an iteration. 
// The above pseudo-code is very simplified, but shows the overall flow. 
// Complications ommitted are:
// 1. We should adjust the number of iterations to run if there are errors.
// 2. We need to check for errors, and depending on circumstances, alter
//    the verdict.
TVerdict CStressTestEngine::doTestStepL()
    {
    INFO_PRINTF2(_L("Executing test with %d cases..."), iNumCases);

    for(TInt tests = 0; tests < iNumCases; tests++)
        {
        TTestCase tc;
        RDebug::Printf("%s:%d: tests = %d", __FILE__, __LINE__, tests);
        tc.iBase = iStressTable[tests].iTestCaseBase;
        TPtrC testIds(tc.iBase.iRelatedTestIds);
        SetCurrentTestIds(testIds);
        for(TInt surfIter= 0; surfIter < tc.iBase.iSurfaceTypeCount; surfIter++)
            {
            TInt previousCopiesRun = 1;
            TSurfaceType surfType = tc.iBase.iSurfaceTypeList[surfIter];
            
            INFO_PRINTF4(_L("Running testcase GRAPHICS-EGL-%s: %s (surf=%d)"),
                    tc.iBase.iRelatedTestIds, tc.iBase.iName, surfType);

            TRemoteTestParams configParams = {};
            configParams.iEndpointEngineConfig.iLogErrors = EFalse; 
            StartRemoteTestStep(configParams);
            
            for(TInt curSection = 0; curSection < iStressTable[tests].iNumSections; curSection++)
                {
                INFO_PRINTF3(_L("Section %d, result=%d"), curSection, TestStepResult());
                const TStressTestSection &section = iStressTable[tests].iSections[curSection];
                CreateParams(section);
                TInt copies = section.iIterations;
                if (section.iErrorPolicy == EOnErrorPolicyBreak)
                    {
                    if (section.iIterations != 1 && previousCopiesRun == 1)
                        {
                        previousCopiesRun = section.iIterations;
                        }
                    // We should not run more than iCopies times, but also not 
                    // allow previousCopiesRun to be zero to completely avoid 
                    // running a section.
                    copies = Min(section.iIterations, Max(previousCopiesRun, 1));
                    }

                for(TInt copy = 0; copy < copies; copy++)
                    {
                    // Make a copy and put it into the "tc" variable that we set up earlier....
                    TInt i = 0;
                    for(; section.iEngineTestCase[i].iCase != EFinishedCase; i++)
                        {
                        TEngineTestCase ec = section.iEngineTestCase[i];
                        ModifyL(ec);
                        tc.iEngineTestCase[i] = ec;
                        }
                    tc.iEngineTestCase[i].iCase = EFinishedCase;
                    TRAPD(err, RunSingleCaseL(tc, surfType));
                    if (err != KErrNone)
                        {
                        INFO_PRINTF2(_L("RunSingleCaseL left with an error: %d"), err);
                        }
                    if (err != KErrNone || TestStepResult() != EPass)
                        {
                        if (HandleError(section, copy, copies, 
                                curSection, previousCopiesRun))
                            {
                            break;
                            }
                        }
                    NextParams();
                    }
                DestroyParams();
                }              
            // params are not used in this call, so we just pass some suitable type in.
            // Save the result, as the remote side may have failed, but it's
            // not a real failure in a stress-test.
            TVerdict v = TestStepResult();
            EndRemoteTestStep(configParams);
            SetTestStepResult(v);
            }
        }
    return TestStepResult();
    }




// Derived function to start a thread. 
void CStressTestEngine::StartThreadL(TInt aThreadNumber)
    {
    const TInt KStackSize = 12000;
    const TInt KHeapMinSize = 16000;
    const TInt KHeapMaxSize = 1000000;

    if (aThreadNumber >= KMaxLoadThreads)
        {
        User::Panic(_L("StartThreadL"), __LINE__);
        }
    
    __e32_atomic_store_rel32(&iStopThreadFlag[aThreadNumber], EFalse);
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");

    // Create a loadthread. 
    _LIT(KThreadName, "EpTestLoadThread");
    threadName.Format(KThreadNameFormat, &KThreadName, random);
    TThreadFunction threadFunc = GetThreadFunction(aThreadNumber);
    if (threadFunc == NULL)
        {
        ERR_PRINTF2(_L("Requested thread function %d, got NULL pointer back!"), aThreadNumber);
        User::Leave(KErrArgument);
        }
    TInt err = iLoadThread[aThreadNumber].Create(threadName, threadFunc, 
                    KStackSize, KHeapMinSize, KHeapMaxSize, this, EOwnerThread);
    if(err != KErrNone)
        {
        ERR_PRINTF2(_L("Could not create load thread - err=%d"), err);
        User::Leave(err);
        }
    iLoadThread[aThreadNumber].Resume();
    }


// Terminate a previously started thread.
void CStressTestEngine::EndThread(TInt aThreadNumber)
    {
    TRequestStatus status;
    iLoadThread[aThreadNumber].Logon(status);
    RDebug::Printf("Ending thread %d", aThreadNumber);
    __e32_atomic_store_rel32(&iStopThreadFlag[aThreadNumber], ETrue);
    // Wait for thread to exit.
    User::WaitForRequest(status);
    iLoadThread[aThreadNumber].Close();
    }

// This function is intended to be implemented by a derived class - just print
// an error and return NULL.   
TThreadFunction CStressTestEngine::GetThreadFunction(TInt aThreadNumber)
    {
    ERR_PRINTF2(_L("Should not be called: CStressTestEngine::GetThreadFunction() with %d"), aThreadNumber);
    return NULL;
    }


// Parameter class implementation.
// General description:
// The class uses a virtual interface that has three member functions:
// Initialize(start, end, step) 
//   Setup. The values of start, end and step may be used for other purposes
// Value()
//   Return current value of the parameter. Does not alter the value.
// Next()
//   Update the value to the next one. Where relevant, wrap around to the 
//   starting value if the end has been reached.

// Factory function to create parameter instances.
CStressParam * CStressParam::Factory(const TParamDeclaration &aParamDecl)
    {
    CStressParam *p;
    switch(aParamDecl.iType)
        {
        case EParamTypeNone:
            // Didn't want this parameter - return NULL!
            // We later check if the pointer is NULL and complain if an
            // attempt is made to USE this parameter.
            p = NULL;
            break;
            
        case EParamTypeIterator:
            p = new CStressParamIterator;
            break;
            
        case EParamTypeTable:
            p = new CStressParamTable;
            break;
            
        case EParamTypeRandomizer:
            p = new CStressParamRandomizer;
            break;
            
        default:
            RDebug::Printf("%s:%d: Type not implemented (type = %d)", 
                    __FILE__, __LINE__, aParamDecl.iType);
            p = NULL;
            break;
        }
    if (p)
        {
        p->Initialize(aParamDecl.iInit, aParamDecl.iEnd, aParamDecl.iStep);
        }
    return p;
    }


CStressParam::~CStressParam()
    {
    }


CStressParamIterator::CStressParamIterator()
    {
    }


CStressParamIterator::~CStressParamIterator()
    {
    }


void CStressParamIterator::Initialize(TInt aInit, TInt aEnd, TInt aStep)
    {
    iInit = aInit;
    iEnd = aEnd;
    iStep = aStep;
    iIterator = iInit;
    }

void CStressParamIterator::Next()
    {
    iIterator += iStep;
    if (iStep > 0)
        {
        if (iIterator >= iEnd)
            {
            iIterator = iInit;
            }
        }
    else
        {
        if (iIterator <= iEnd)
            {
            iIterator = iInit;
            }
        }
    }


TInt CStressParamIterator::Value()
    {
    return iIterator;
    }


CStressParamRandomizer::CStressParamRandomizer()
    {
    }


CStressParamRandomizer::~CStressParamRandomizer()
    {
    }


void CStressParamRandomizer::Initialize(TInt aMin, TInt aMax, TInt aMul)
    {
    iMin = aMin;
    iMax = aMax;
    iMul = aMul;
    // Now, pick a new random number. 
    Next();  
    }


void CStressParamRandomizer::Next()
    {
    iCurrent = (((TInt)Math::Random()) % ((iMax - iMin) + iMin)) * iMul;
    }

TInt CStressParamRandomizer::Value()
    {
    return iCurrent;
    }


CStressParamTable::CStressParamTable()
    {
    }


CStressParamTable::~CStressParamTable()
    {
    }


void CStressParamTable::Initialize(TInt aTable, TInt aCount, TInt /* aUnused */)
    {
    iTable = reinterpret_cast<TInt *>(aTable);
    iCount = aCount;
    iIndex = 0;
    }


void CStressParamTable::Next()
    {
    iIndex = (iIndex + 1) % iCount;
    }


TInt CStressParamTable::Value()
    {
    return iTable[iIndex];
    }
