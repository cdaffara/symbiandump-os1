// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __EGLTEST_ENDPOINT_STRESS_ENGINE_H__
#define __EGLTEST_ENDPOINT_STRESS_ENGINE_H__

#include "egltest_endpoint_engine_types.h"
#include "egltest_endpoint_engine.h"
#include "localtestbase.h"

#define TESTTABLE(x) sizeof(x) / sizeof(x[0]), x

// This file provides extensions to the egltest engine functionality to 
// support stress tests that aren't practical to do as small tables. The main
// point is to support copying a set of table entries many times.
// Number of sections we expect is needed. This is just an arbitrary value, and a 
// larger value can be used. 
const TInt KMaxSections = 10;

// Max parameters must be a 2^n number so that we can use binary and magig...
const TInt KMaxParams = 4;
// Log2 of KMaxParams
const TInt KMaxParamsLog2 = 2;


// Parameter types. Used to build tables at compile time and passed to factory
// function to request the implementation object for that iterator.
enum TParamType
    {
    EParamTypeNone = 0,
    EParamTypeIterator,
    EParamTypeTable,
    EParamTypeRandomizer
    };

// Parameter declaration. Used to build tables at compile time. 
struct TParamDeclaration
    {
    TParamType iType;
    TInt       iInit;       
    TInt       iEnd;
    TInt       iStep;
    };

// These enums define what should be done when there's
// a failure in a stress test section.
enum TOnErrorPolicy
    {
    // Unknown - this is not a policy, but a placeholder for "uninitialized"
    EOnErrorPolicyUnknown,
    // Break - this policy is where a failure causes a section to be broken,
    // that is, we skip to the next section and remember the number of times this
    // section was run. 
    EOnErrorPolicyBreak,
    // Continue - ignore errors, and continue.
    EOnErrorPolicyContinue,
    // Fail - errors lead to failing the test.
    EOnErrorPolicyFail
    };

// The basic component for producing many table entries from a short table.
struct TStressTestSection
    {
    // Number of iterations to run.
    TInt iIterations;
    // What to do when we have a "fail" during the stress test...
    TOnErrorPolicy iErrorPolicy;
    // Parameter declarations
    TParamDeclaration iParams[KMaxParams];
    // The actual test cases.
    TEngineTestCase iEngineTestCase[KMaxCases];
    };


// Used to create a list of TStessTestSections.
// 
// A typical stress test table would have a epilog, body and prolog, where
// the epilog and prolog are copied once to the table, and the body copied
// many times over. Sometimes, there may be a test that for example creates
// a large number of endpoints, then does something and then cleans up the
// endpoints. This would have 5 sections: 
// prolog to start things off copied once. 
// creation of endpoints (body part 1), copied n times.
// do some work with endpoints (body part 2), copied n times.
// cleanup of endpoints (body part 3), copied n times. 
// epilog to clean up final items, copied once.
struct TStressTestTable
    {
    TTestCaseBase             iTestCaseBase;
    TUint                     iNumSections;
    const TStressTestSection* iSections;
    };


// A collection of constant values for the purpose of marking/extracting the
// parameter index.
enum TExpandEntryFlags 
    {
    EParamBitsInInt  = 32,
    EExpandParamBit = EParamBitsInInt-1,
    EExpandParamFlag = 1 << 31,
    EParamShift      = (30-KMaxParamsLog2),
    EParamMask       = ((KMaxParams-1) << EParamShift),
    EAllFlags = EExpandParamFlag | EParamMask,
    ERemoveFlagsMask = ~EAllFlags,
    };

NONSHARABLE_CLASS(CStressParam) : CBase
    {
public:
    virtual void Initialize(TInt aInit, TInt aEnd, TInt aStep) = 0;
    virtual void Next() = 0;
    virtual TInt Value() = 0;
    virtual ~CStressParam();
    static CStressParam *Factory(const TParamDeclaration &aParamDecl);
    
    };

NONSHARABLE_CLASS(CStressParamIterator) : public CStressParam 
    {
public:
    CStressParamIterator();
    ~CStressParamIterator();
    virtual void Initialize(TInt aInit, TInt aEnd, TInt aStep);
    virtual void Next();
    virtual TInt Value();
private:
    TInt iIterator;
    TInt iInit;
    TInt iEnd;
    TInt iStep;
    };

NONSHARABLE_CLASS(CStressParamTable) : public CStressParam 
    {
public:
    CStressParamTable();
    ~CStressParamTable();
    virtual void Initialize(TInt aTable, TInt aCount, TInt /* aUnused */);
    virtual void Next();
    virtual TInt Value();
private:
    TInt  iIndex;
    TInt* iTable;
    TInt  iCount;
    };

NONSHARABLE_CLASS(CStressParamRandomizer) : public CStressParam
    {
public:
    CStressParamRandomizer();
    ~CStressParamRandomizer();
    virtual void Initialize(TInt aMin, TInt aMax, TInt aMul);
    virtual void Next();
    virtual TInt Value();
private:
    TInt  iMin;
    TInt  iMax;
    TInt  iMul;
    TInt  iCurrent;
    };

#define PARAM_DECL(type, init, end, step) { type, init, end, step }
// Iterator parameter: Goes from init to end in step increments. If the total
// loop is bigger than (end - init), it restarts on init. 
#define PARAM_DECL_ITER(init, end, step) PARAM_DECL(EParamTypeIterator, init, end, step)
// Table iterator. Steps over table. Rstarts at index 0 when going past the end. 
#define PARAM_DECL_TABLE(table) \
    PARAM_DECL(EParamTypeTable, (TInt)table, sizeof(table)/sizeof(table[0]), 0)
// Random generator. generates an integer in the range [min..max[ then 
// multiples by mul. 
// Example: PARAM_DECL_RANDOM(2, 6, 3) will generate numbers that are multiples 
// of three in the range 6..15. 
#define PARAM_DECL_RANDOM(min, max, mul) PARAM_DECL(EParamTypeRandom, min, max, mul)

#define NO_PARAMS  { EParamTypeNone, 0, 0, 0 }

// Make Parameter entry
#define PARAM(n, offset) (EExpandParamFlag | (n << EParamShift) | offset)
#define PARAM0(offset)   PARAM(0, offset)
#define PARAM1(offset)   PARAM(1, offset)
#define PARAM2(offset)   PARAM(2, offset)
#define PARAM3(offset)   PARAM(3, offset)

class CTestExecuteLogger;

// A class to allow tests that run many operations to be written as tables
// without having very long tables. Instead the table structure is (as 
// described for the TStressTestTable), consists of a generic header and
// multiple sections of table that have a "number of times to run" counter. 

// To allow for these changes to the table structure, we derive the 
// CEgttest_Local_Engine class, but provide alternative solutions for a few of
// A class to build stress tests. 
NONSHARABLE_CLASS(CStressTestEngine): public CEgltest_Local_Engine
    {
public:
    static CStressTestEngine *NewL(const TStressTestTable aStressTable[], TUint aNumTables);
    ~CStressTestEngine();
    TVerdict doTestStepL();
    CStressTestEngine();
    virtual void DoPreambleL();
    virtual void DoPostambleL();
    
protected:
    void ConstructL(const TStressTestTable aStressTable[], TUint aNumTables);
    void StartThreadL(TInt aThreadNumber);
    void EndThread(TInt aThreadNumber);
    virtual TThreadFunction GetThreadFunction(TInt aThreadNumber);
    
private:
    void ModifyL(TEngineTestCase &aTestCase);
    void DoModifyL(TInt &aItem);
    
    void CreateParams(const TStressTestSection &aSection);
    void DestroyParams();
    void NextParams();
    TBool HandleError(const TStressTestSection &section, TInt aCopy, 
            TInt aCopies, TInt aCurSection, TInt &aPreviousCopiesRun);
    void UpdateVerdict(TVerdict aVerdict);
    
private:
    const TStressTestTable *iStressTable;
    CStressParam *iParams[KMaxParams];
protected:
    RThread iLoadThread[KMaxLoadThreads];
    TBool   iStopThreadFlag[KMaxLoadThreads];
    };

#endif // __EGLTEST_ENDPOINT_STRESS_ENGINE_H__
