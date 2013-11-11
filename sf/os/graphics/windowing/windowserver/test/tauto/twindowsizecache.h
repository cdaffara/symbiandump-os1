// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TWINDOWSIZECACHE_H__
#define __TWINDOWSIZECACHE_H__

#include <e32std.h>
#include <w32std.h>
#include "AUTO.H"

#include "../nga/CLIENT/w32comm.h"

#include "../tlib/testbase.h"
#include "TGraphicsHarness.h"

//-----------------------------------------------------------
class CTWindowSizeCacheTest : public CTWsGraphicsBase
    {
public:
    enum TSizeCacheStatus
        {
        ETestSizeCacheNotEnabled,
        ETestSizeCacheEnabled,
        };
    enum TRWindowAction
        {
        ESetExtent,
        ESetExtentErr,
        ESetSize,
        ESetSizeErr,
        ECheckSize,
        EFlushSession,
        EEnableWindowSizeCache,
        EMaxAction,
        };
public:
    CTWindowSizeCacheTest(CTestStep* aStep);
    ~CTWindowSizeCacheTest();
    TestState DoTestL();
    void ConstructL();
//    
    void TestCreateRWindowL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestCreateRBlankWindowL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestCreateRBackedUpWindowL(TSizeCacheStatus aEnableWindowSizeCache, TDisplayMode aDisplayMode);
//    
    void TestRWindowNoCacheEntryL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowNoCacheEntrySetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowNoCacheEntrySetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowNoCacheEntrySetExtentL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowNoCacheEntrySetSizeL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowNoCacheEntryEnableWindowSizeCacheL();
//
    void TestRWindowCacheEntryExistsCacheCleanL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheCleanSizeL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheCleanEnableWindowSizeCacheL();
    void TestRWindowCacheEntryExistsCacheCleanEnableWindowSizeCacheLSizeL();
//
    void TestRWindowCacheEntryExistsCacheDirtySetSizeL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetExtentL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetSizeTwiceL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetSizeSetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtySetSizeEnableWindowSizeCacheL();
//    
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeTwiceL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeEnableWindowSizeCacheL();
//
    void TestRBlankWindowSetSizeSetExtentL(TSizeCacheStatus aEnableWindowSizeCache);
    void TestRBackedUpWindowSetSizeErrSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache);
//    
    void SetRandomSeed(TInt64 aSeed);
    inline TInt NextRandom(TUint aMax = 0);
//    
    TUint64 TestRWindowSizePerformanceL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations);
//    
    static void CleanupWindowPool(TAny* aPtr);
    void RWindowPerformAction(TRWindowAction aAction, TSizeCacheStatus aEnableWindowSizeCache, RWsSession& aWs, RWindow& aWindow, TSize& aExpectedSize);
    void TestRandomisedStressL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations);
    void TestRandomisedStressMultiSessionL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations);
    void TestRandomisedStressCacheEntryVolumeL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations, TUint aWindowPoolSize);
    void TestStressEnableWindowSizeCacheL(TUint aIterations);
    
protected:
//from  CTGraphicsStep
    virtual void RunTestCaseL(TInt aCurTestCase);
    
private:
    TInt64 iSeed;
    };

//-----------------------------------------------------------
inline TInt CTWindowSizeCacheTest::NextRandom(TUint aMax)
    {
    TInt random = Math::Rand(iSeed);
    if (aMax != 0)
        {
        random = random % aMax;
        }
    return random;
    }

//-----------------------------------------------------------
class CTWindowSizeCacheTestStep : public CTGraphicsStep
    {
public:
    CTWindowSizeCacheTestStep();
protected:
    //from CTGraphicsStep
    virtual CTGraphicsBase* CreateTestL();
    };

_LIT(KTWindowSizeCacheTestStep,"TWindowSizeCacheTest");

//-----------------------------------------------------------
#endif // TWINDOWSIZECACHE
