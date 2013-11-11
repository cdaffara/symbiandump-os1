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
// General window tests
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "twindowsizecache.h"

CTWindowSizeCacheTest::CTWindowSizeCacheTest(CTestStep* aStep) : CTWsGraphicsBase(aStep)
    {}

CTWindowSizeCacheTest::~CTWindowSizeCacheTest()
    {
    }

void CTWindowSizeCacheTest::ConstructL()
    {}


void CTWindowSizeCacheTest::TestCreateRWindowL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(KErrNone == err);

    if (ETestSizeCacheEnabled == aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }
    
    RWindowGroup wg(rws);
    err = wg.Construct(0);
    CleanupClosePushL(wg);
    TEST(KErrNone == err);

    // w1 State 0
    RWindow w1(rws);
    err = w1.Construct(wg,1);
    CleanupClosePushL(w1);
    TEST(KErrNone == err);

    // w2 State 0
    RWindow w2(rws);
    err = w2.Construct(wg,2);
    CleanupClosePushL(w2);
    TEST(KErrNone == err);

    CleanupStack::Pop(2);
    
    // w1 State Final
    w1.Close();
    // w2 State Final
    w2.Close();
    
    RWindow w3(rws);
    // w3 State 0
    err = w3.Construct(wg,1);
    CleanupClosePushL(w3);
    TEST(KErrNone == err);
    
    CleanupStack::Pop(2);

    // w3 State Final
    w3.Close();
    
    wg.Close();

    RWindowGroup wg2(rws);
    err = wg2.Construct(0);
    CleanupClosePushL(wg2);
    TEST(KErrNone == err);
    
    RWindowGroup wg3(rws);
    err = wg3.Construct(1);
    CleanupClosePushL(wg3);    
    TEST(KErrNone == err);

    // w4 State 0
    RWindow w4(rws);
    err = w4.Construct(wg2,2);
    CleanupClosePushL(w4);    
    TEST(KErrNone == err);
    
    // w5 State 0
    RWindow w5(rws);
    err = w5.Construct(wg3,3);
    CleanupClosePushL(w5);    
    TEST(KErrNone == err);
    
    CleanupStack::Pop(5);

    // w4 State Final
    w4.Close();
    
    // w5 State Final
    // Check nothing bad happens when calling Close more than once on RWindow
    w5.Close();
    w5.Close();

    // Check nothing bad happens when calling Close more than once on RWindowGroup
    wg2.Close();
    wg2.Close();
    
    wg3.Close();
    
    // Check nothing bad happens when calling Close more than once on RWsSession
    rws.Close();
    rws.Close();
    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestCreateRBlankWindowL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(KErrNone == err);

    if (ETestSizeCacheEnabled == aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }
    
    RWindowGroup wg(rws);
    err = wg.Construct(0);
    CleanupClosePushL(wg);
    TEST(KErrNone == err);

    // w1 State 0
    RBlankWindow w1(rws);
    err = w1.Construct(wg,1);
    CleanupClosePushL(w1);
    TEST(KErrNone == err);

    // w2 State 0
    RBlankWindow w2(rws);
    err = w2.Construct(wg,2);
    CleanupClosePushL(w2);
    TEST(KErrNone == err);

    CleanupStack::Pop(2);

    // w1 State Final
    w1.Close();
    // w2 State Final
    w2.Close();
    
    RBlankWindow w3(rws);
    // w3 State 0
    err = w3.Construct(wg,1);
    CleanupClosePushL(w3);
    TEST(KErrNone == err);
    
    CleanupStack::Pop(2);
    
    // w3 State Final
    w3.Close();
    
    wg.Close();

    RWindowGroup wg2(rws);
    err = wg2.Construct(0);
    CleanupClosePushL(wg2);
    TEST(KErrNone == err);
    
    RWindowGroup wg3(rws);
    err = wg3.Construct(1);
    CleanupClosePushL(wg3);    
    TEST(KErrNone == err);

    // w4 State 0
    RBlankWindow w4(rws);
    err = w4.Construct(wg2,2);
    CleanupClosePushL(w4);    
    TEST(KErrNone == err);
    
    // w5 State 0
    RBlankWindow w5(rws);
    err = w5.Construct(wg3,3);
    CleanupClosePushL(w5);    
    TEST(KErrNone == err);
    
    CleanupStack::Pop(5);

    // w4 State Final
    w4.Close();
    
    // w5 State Final
    // Check nothing bad happens when calling Close more than once on RBlankWindow
    w5.Close();
    w5.Close();

    wg2.Close();
    wg3.Close();
    rws.Close();
    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestCreateRBackedUpWindowL(TSizeCacheStatus aEnableWindowSizeCache, TDisplayMode aDisplayMode)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(KErrNone == err);

    if (ETestSizeCacheEnabled == aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }
    
    RWindowGroup wg(rws);
    err = wg.Construct(0);
    CleanupClosePushL(wg);
    TEST(KErrNone == err);

    // w1 State 0
    RBackedUpWindow w1(rws);
    err = w1.Construct(wg,aDisplayMode,1);
    CleanupClosePushL(w1);
    TEST(KErrNone == err);

    // w2 State 0
    RBackedUpWindow w2(rws);
    err = w2.Construct(wg,aDisplayMode,2);
    CleanupClosePushL(w2);
    TEST(KErrNone == err);

    CleanupStack::Pop(2);

    // w1 State Final
    w1.Close();
    
    // w2 State Final
    w2.Close();
    
    RBackedUpWindow w3(rws);
    // w3 State 0
    err = w3.Construct(wg,aDisplayMode,1);
    CleanupClosePushL(w3);
    TEST(KErrNone == err);
    
    CleanupStack::Pop(2);

    // w3 State Final
    w3.Close();
    
    wg.Close();

    RWindowGroup wg2(rws);
    CleanupClosePushL(wg2);
    err = wg2.Construct(0);
    TEST(KErrNone == err);
    
    RWindowGroup wg3(rws);
    err = wg3.Construct(1);
    CleanupClosePushL(wg3);    
    TEST(KErrNone == err);

    // w4 State 0
    RBackedUpWindow w4(rws);
    err = w4.Construct(wg2,aDisplayMode,2);
    CleanupClosePushL(w4);    
    TEST(KErrNone == err);
    
    // w5 State 0
    RBackedUpWindow w5(rws);
    err = w5.Construct(wg3,aDisplayMode,3);
    CleanupClosePushL(w5);    
    TEST(KErrNone == err);
    
    CleanupStack::Pop(5);

    // w4 State Final
    w4.Close();
    
    // w5 State Final
    // Check nothing bad happens when calling Close more than once on RBackedUpWindow
    w5.Close();
    w5.Close();

    wg2.Close();
    wg3.Close();
    rws.Close();
    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowNoCacheEntryL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowNoCacheEntrySetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TPoint point1 (10101,897);
    TSize size1 (10001,123);
    err = w1.SetExtentErr(point1, size1);
    TEST(KErrNone == err);
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowNoCacheEntrySetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    err = w1.SetSizeErr(size1);
    TEST(KErrNone == err);
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowNoCacheEntrySetExtentL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TPoint point1 (10101,897);
    TSize size1 (10001,123);
    w1.SetExtent(point1, size1);
    rws.Flush();
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowNoCacheEntrySetSizeL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowNoCacheEntryEnableWindowSizeCacheL()
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    rws.EnableWindowSizeCacheL();

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    rws.EnableWindowSizeCacheL();
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheCleanL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheCleanSizeL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size1);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheCleanEnableWindowSizeCacheL()
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    rws.EnableWindowSizeCacheL();

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 1
    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    rws.EnableWindowSizeCacheL();

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheCleanEnableWindowSizeCacheLSizeL()
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    rws.EnableWindowSizeCacheL();

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 1
    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    rws.EnableWindowSizeCacheL();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size1);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 
    rws.Flush();

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetExtentL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TPoint point2 (20202,223);
    TSize size2 (20002,223);
    w1.SetExtent(point2, size2); 
    rws.Flush();

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    err = w1.SetSizeErr(size2); 
    TEST(KErrNone == err);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TPoint point2 (20202,223);
    TSize size2 (20002,223);
    err = w1.SetExtentErr(point2, size2); 
    TEST(KErrNone == err);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeTwiceL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    TSize size3 (30003,323);
    // w1 State 2
    w1.SetSize(size3); 
    rws.Flush();

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TPoint point3 (30303,232);
    TSize size3 (30003,323);
    w1.SetExtent(point3, size3); 
    rws.Flush();

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeSetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TSize size3 (30003,323);
    err = w1.SetSizeErr(size3);
    TEST(KErrNone == err);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TPoint point3 (30303,232);
    TSize size3 (30003,323);
    err = w1.SetExtentErr(point3, size3); 
    TEST(KErrNone == err);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtySetSizeEnableWindowSizeCacheL()
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    rws.EnableWindowSizeCacheL();

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    rws.EnableWindowSizeCacheL();

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

//-------------------------------------------------------------------------------------------
void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 
    rws.Flush();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size2);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TPoint point2 (20202,223);
    TSize size2 (20002,223);
    w1.SetExtent(point2, size2); 
    rws.Flush();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size2);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    err = w1.SetSizeErr(size2); 
    TEST(KErrNone == err);

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size2);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TPoint point2 (20202,223);
    TSize size2 (20002,223);
    err = w1.SetExtentErr(point2, size2); 
    TEST(KErrNone == err);

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size2);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeTwiceL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    TSize size3 (30003,323);
    // w1 State 2
    w1.SetSize(size3); 
    rws.Flush();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size3);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TPoint point3 (30303,232);
    TSize size3 (30003,323);
    w1.SetExtent(point3, size3); 
    rws.Flush();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size3);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetSizeErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TSize size3 (30003,323);
    err = w1.SetSizeErr(size3);
    TEST(KErrNone == err);

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size3);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TPoint point3 (30303,232);
    TSize size3 (30003,323);
    err = w1.SetExtentErr(point3, size3); 
    TEST(KErrNone == err);

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size3);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

void CTWindowSizeCacheTest::TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeEnableWindowSizeCacheL()
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    rws.EnableWindowSizeCacheL();

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    rws.EnableWindowSizeCacheL();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size2);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

//------------------------------------------------------------------------------------------

void CTWindowSizeCacheTest::TestRBlankWindowSetSizeSetExtentL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    err = wg.Construct(0);
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    // w1 State 0
    RBlankWindow w1(rws);
    err = w1.Construct(wg,1);
    CleanupClosePushL(w1);
    TEST(err==KErrNone);

    // w1 State 0
    TSize size1 (10001,123);
    w1.SetSize(size1);
    rws.Flush();
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    TPoint point3 (30303,232);
    TSize size3 (30003,323);
    w1.SetExtent(point3, size3);
    rws.Flush();

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size3);

    // w1 State 2
    TSize size4 (40004,423);
    w1.SetSize(size4);
    
    // w1 State 1
    TSize retSize3 = w1.Size();
    TEST(retSize3 == size4);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

//------------------------------------------------------------------------------------------

void CTWindowSizeCacheTest::TestRBackedUpWindowSetSizeErrSetExtentErrL(TSizeCacheStatus aEnableWindowSizeCache)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    err = wg.Construct(0);
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    // w1 State 0
    RBlankWindow w1(rws);
    err = w1.Construct(wg,1);
    CleanupClosePushL(w1);
    TEST(err==KErrNone);

    // w1 State 0
    TSize size1 (10001,123);
    err = w1.SetSizeErr(size1);
    TEST(err==KErrNone);
    
    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);
    
    // w1 State 2
    TSize size2 (20002,223);
    err = w1.SetSizeErr(size2); 
    TEST(err==KErrNone);

    // w1 State 2
    TPoint point3 (30303,232);
    TSize size3 (30003,323);
    err = w1.SetExtentErr(point3, size3);
    TEST(err==KErrNone);

    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size3);

    // w1 State 2
    TSize size4 (40004,423);
    err = w1.SetSizeErr(size4);
    TEST(err==KErrNone);
    
    // w1 State 1
    TSize retSize3 = w1.Size();
    TEST(retSize3 == size4);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

//------------------------------------------------------------------------------------------

TUint64 CTWindowSizeCacheTest::TestRWindowSizePerformanceL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations)
    {
    TUint64 timeDeltaMicroSeconds = 0;
    
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    TEST(KErrNone == err);
    CleanupClosePushL(rws);

    if (ETestSizeCacheEnabled == aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    err = wg.Construct(0);
    TEST(KErrNone == err);
    CleanupClosePushL(wg);

    // w1 State 0
    RWindow w1(rws);
    err = w1.Construct(wg,1);
    TEST(KErrNone == err);
    CleanupClosePushL(w1);
    
    TSize size1 (10001,123);
    w1.SetSize(size1);

    TTime start;
    TTime end;
    start.UniversalTime();
    for (TUint iter = 0; iter < aIterations; ++iter)
        {
        // w1 State 1
        TSize retSize1 = w1.Size();
        TEST(retSize1 == size1);
        }
    end.UniversalTime();

    TTimeIntervalMicroSeconds delta = end.MicroSecondsFrom(start);
    timeDeltaMicroSeconds = delta.Int64();
    INFO_PRINTF4(_L("TestRWindowSizePerformanceL - WindowSizeCacheEnabled %d, Iterations %d, Microseconds %Ld"), aEnableWindowSizeCache, aIterations, timeDeltaMicroSeconds);
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();

    CleanupStack::Pop();
    wg.Close();

    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    return timeDeltaMicroSeconds;
    }


//------------------------------------------------------------------------------------------
/*
Sets the random seed used by CTWindowSizeCacheTest::NextRandom.
 */
void CTWindowSizeCacheTest::SetRandomSeed(TInt64 aSeed)
    {
    INFO_PRINTF2(_L("Random Seed - %d"), aSeed);
    iSeed = aSeed;
    }

/*
Performs an action on RWindow or RWsSession as specified by TRWindowAction aAction.
When the size of the window is changed, aExpectedSize is changed to the size we expect the window to be the next time RWindow::Size is called.
When RWindow::Size is called, we check the size returned matches aExpectedSize.
If aAction requests RWsSession::EnableWindowSizeCache, we call this if TSizeCacheStatus allows this.

This function was designed for use with random stress tests, with a pseudo-random sequence driving TRWindowAction.  
 */
void CTWindowSizeCacheTest::RWindowPerformAction(TRWindowAction aAction, TSizeCacheStatus aEnableWindowSizeCache, RWsSession& aWs, RWindow& aWindow, TSize& aExpectedSize)
    {
    switch (aAction)
        {
        case ESetExtent :
            {
            TPoint point(NextRandom(), NextRandom());
            aExpectedSize.iHeight = NextRandom();
            aExpectedSize.iWidth = NextRandom();
            aWindow.SetExtent(point, aExpectedSize);
            aExpectedSize.iHeight = (aExpectedSize.iHeight < 0) ? 0 : aExpectedSize.iHeight;
            aExpectedSize.iWidth = (aExpectedSize.iWidth < 0) ? 0 : aExpectedSize.iWidth;
            }
            break;
        case ESetExtentErr :
            {
            TPoint point(NextRandom(), NextRandom());
            aExpectedSize.iHeight = NextRandom();
            aExpectedSize.iWidth = NextRandom();
            TInt err = aWindow.SetExtentErr(point, aExpectedSize);
            TEST(KErrNone == err);
            aExpectedSize.iHeight = (aExpectedSize.iHeight < 0) ? 0 : aExpectedSize.iHeight;
            aExpectedSize.iWidth = (aExpectedSize.iWidth < 0) ? 0 : aExpectedSize.iWidth;
            }
            break;
        case ESetSize :
            {
            aExpectedSize.iHeight = NextRandom();
            aExpectedSize.iWidth = NextRandom();
            aWindow.SetSize(aExpectedSize);
            aExpectedSize.iHeight = (aExpectedSize.iHeight < 0) ? 0 : aExpectedSize.iHeight;
            aExpectedSize.iWidth = (aExpectedSize.iWidth < 0) ? 0 : aExpectedSize.iWidth;
            }
            break;
        case ESetSizeErr :
            {
            aExpectedSize.iHeight = NextRandom();
            aExpectedSize.iWidth = NextRandom();
            TInt err = aWindow.SetSizeErr(aExpectedSize);
            TEST(KErrNone == err);
            aExpectedSize.iHeight = (aExpectedSize.iHeight < 0) ? 0 : aExpectedSize.iHeight;
            aExpectedSize.iWidth = (aExpectedSize.iWidth < 0) ? 0 : aExpectedSize.iWidth;
            }
            break;
        case ECheckSize :
            {
            TSize retSize1 = aWindow.Size();
            TEST(retSize1 == aExpectedSize);
            }
            break;
        case EFlushSession :
            {
            aWs.Flush();
            }
            break;
        case EEnableWindowSizeCache :
            {
            if (ETestSizeCacheEnabled == aEnableWindowSizeCache)
                {
                aWs.EnableWindowSizeCacheL();
                }
            }
            break;
        default :
            break;
        }
    }

void CTWindowSizeCacheTest::TestRandomisedStressL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    TEST(KErrNone == err);
    CleanupClosePushL(rws);

    if (aEnableWindowSizeCache)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    err = wg.Construct(0);
    TEST(KErrNone == err);
    CleanupClosePushL(wg);

    // w1 State 0
    RWindow w1(rws);
    err = w1.Construct(wg,1);
    TEST(KErrNone == err);
    CleanupClosePushL(w1);
    
    TSize size1 (10001,123);
    w1.SetSize(size1);

    for (TUint iter = 0; iter < aIterations; ++iter)
        {
        TRWindowAction action = static_cast<TRWindowAction>(NextRandom(EMaxAction));
        RWindowPerformAction(action, aEnableWindowSizeCache, rws, w1, size1);
        }
    
    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();

    CleanupStack::Pop();
    rws.Close();
    __UHEAP_MARKEND;
    }

/*
TCleanupOperation for a window pool, implemented as RPointerArray<RWindow>.
 */
void CTWindowSizeCacheTest::CleanupWindowPool(TAny* aPtr)
    {
    RPointerArray<RWindow> windowPool = *(static_cast<RPointerArray<RWindow>*>(aPtr));
    const TUint poolSize = windowPool.Count();
    // Note: ResetAndDestroy deletes referenced objects, so no need to delete items in loop
    for (TUint iter = 0; iter < poolSize; ++iter)
        {
        windowPool[iter]->Close();
        }
    windowPool.ResetAndDestroy();
    }

void CTWindowSizeCacheTest::TestRandomisedStressMultiSessionL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws1;
    TInt err = rws1.Connect();
    TEST(KErrNone == err);
    CleanupClosePushL(rws1);

    // Multiple sessions in one thread
    RWsSession rws2;
    err = rws2.Connect();
    TEST(KErrNone == err);
    CleanupClosePushL(rws2);
    
    RWindowGroup rws1_wg1(rws1);
    err = rws1_wg1.Construct(0);
    CleanupClosePushL(rws1_wg1);
    TEST(KErrNone == err);

    RWindowGroup rws2_wg1(rws2);
    err = rws2_wg1.Construct(0);
    CleanupClosePushL(rws2_wg1);
    TEST(KErrNone == err);

    // Multiple window groups in one session
    RWindowGroup rws2_wg2(rws2);
    err = rws2_wg2.Construct(1);
    CleanupClosePushL(rws2_wg2);
    TEST(KErrNone == err);
    
    // Window pool for all windows owned by session rws1
    RPointerArray<RWindow> rws1_windowPool;
    CleanupStack::PushL(TCleanupItem(CleanupWindowPool,&rws1_windowPool));
    // Each window in window pool has a corresponding expected size at corresponding array index
    RArray<TSize> rws1_expectedSizePool;
    CleanupClosePushL(rws1_expectedSizePool);

    // Window pool for all windows owned by session rws2
    RPointerArray<RWindow> rws2_windowPool;
    CleanupStack::PushL(TCleanupItem(CleanupWindowPool,&rws2_windowPool));
    // Each window in window pool has a corresponding expected size at corresponding array index
    RArray<TSize> rws2_expectedSizePool;
    CleanupClosePushL(rws2_expectedSizePool);
    
    // Populate window pool
    const TUint KWindowCount_rws1_wg1 = 50;
    const TUint KWindowCount_rws2_wg1 = 50;
    const TUint KWindowCount_rws2_wg2 = 50;
    
    // Windows for session 1 belonging to rws1_wg1
    // Many windows - siblings
    for (TUint iter = 0; iter < KWindowCount_rws1_wg1; ++iter)
        {
        RWindow* win = new (ELeave) RWindow(rws1);
        CleanupStack::PushL(win);
        TInt err = win->Construct(rws1_wg1,iter+1);
        TEST(KErrNone == err);
        rws1_windowPool.AppendL(win);
        CleanupStack::Pop(win);
        TSize expectedSize (NextRandom(),NextRandom());
        win->SetSize(expectedSize);
        // Wserv flattens size to zero
        expectedSize.iHeight = (expectedSize.iHeight < 0) ? 0 : expectedSize.iHeight;
        expectedSize.iWidth = (expectedSize.iWidth < 0) ? 0 : expectedSize.iWidth;
        rws1_expectedSizePool.AppendL(expectedSize);
        TSize retSize1 = win->Size();
        TEST(retSize1 == expectedSize);
        }

    // Windows for session 2 belonging to rws2_wg1
    // Many windows - siblings
    for (TUint iter = 0; iter < KWindowCount_rws2_wg1; ++iter)
        {
        RWindow* win = new (ELeave) RWindow(rws2);
        CleanupStack::PushL(win);
        TInt err = win->Construct(rws2_wg1,iter+2);
        TEST(KErrNone == err);
        rws2_windowPool.AppendL(win);
        CleanupStack::Pop();
        TSize expectedSize (NextRandom(),NextRandom());
        win->SetSize(expectedSize);
        // Wserv flattens size to zero
        expectedSize.iHeight = (expectedSize.iHeight < 0) ? 0 : expectedSize.iHeight;
        expectedSize.iWidth = (expectedSize.iWidth < 0) ? 0 : expectedSize.iWidth;
        rws2_expectedSizePool.AppendL(expectedSize);
        TSize retSize1 = win->Size();
        TEST(retSize1 == expectedSize);
        }
    
    // Windows for session 2 belonging to rws2_wg2
    // Many windows - children
    RWindowTreeNode parent = rws2_wg2; 
    for (TUint iter = 0; iter < KWindowCount_rws2_wg2; ++iter)
        {
        RWindow* win = new (ELeave) RWindow(rws2);
        CleanupStack::PushL(win);
        TInt err = win->Construct(parent,KWindowCount_rws2_wg1+iter+2);
        TEST(KErrNone == err);
        parent = *win;
        rws2_windowPool.AppendL(win);
        CleanupStack::Pop();
        TSize expectedSize (NextRandom(),NextRandom());
        win->SetSize(expectedSize);
        // Wserv flattens size to zero
        expectedSize.iHeight = (expectedSize.iHeight < 0) ? 0 : expectedSize.iHeight;
        expectedSize.iWidth = (expectedSize.iWidth < 0) ? 0 : expectedSize.iWidth;
        rws2_expectedSizePool.AppendL(expectedSize);
        TSize retSize1 = win->Size();
        TEST(retSize1 == expectedSize);
        }

    // Stress system by performing actions on windows
    for (TUint iter = 0; iter < aIterations; ++iter)
        {
        TRWindowAction action = static_cast<TRWindowAction>(NextRandom(EMaxAction));
        TUint32 windowPool = NextRandom(2);
        switch (windowPool)
            {
            case 0 :
                {
                TUint32 target = NextRandom(rws1_windowPool.Count());
                RWindowPerformAction(action, aEnableWindowSizeCache, rws1, *(rws1_windowPool[target]) , rws1_expectedSizePool[target]);
                }
                break;
            case 1 :
                {
                TUint32 target = NextRandom(rws2_windowPool.Count());
                RWindowPerformAction(action, aEnableWindowSizeCache, rws2, *(rws2_windowPool[target]) , rws2_expectedSizePool[target]);
                }
                break;
            default :
                User::Invariant();
            }
        }
     

    // Cleanup afterwards
    CleanupStack::Pop(9);

    CleanupWindowPool(&rws1_windowPool);
    rws1_expectedSizePool.Close();
    rws1_wg1.Close();
    rws1.Close();

    CleanupWindowPool(&rws2_windowPool);
    rws2_expectedSizePool.Close();
    rws2_wg1.Close();
    rws2_wg2.Close();
    rws2.Close();
    }

void CTWindowSizeCacheTest::TestRandomisedStressCacheEntryVolumeL(TSizeCacheStatus aEnableWindowSizeCache, TUint aIterations, TUint aWindowPoolSize)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws1;
    TInt err = rws1.Connect();
    TEST(KErrNone == err);
    CleanupClosePushL(rws1);

    if (aEnableWindowSizeCache)
        {
        rws1.EnableWindowSizeCacheL();
        }
  
    RWindowGroup rws1_wg1(rws1);
    err = rws1_wg1.Construct(0);
    TEST(KErrNone == err);
    CleanupClosePushL(rws1_wg1);
    
    RPointerArray<RWindow> windowPool;
    CleanupStack::PushL(TCleanupItem(CleanupWindowPool,&windowPool));
    // Each window in window pool has a corresponding expected size at corresponding array index
    RArray<TSize> expectedSizePool;
    CleanupClosePushL(expectedSizePool);
    
    // Populate window pool
    for (TUint iter = 0; iter < aWindowPoolSize; ++iter)
        {
        RWindow* win = new (ELeave) RWindow(rws1);
        CleanupStack::PushL(win);
        TInt err = win->Construct(rws1_wg1,iter+1);
        TEST(KErrNone == err);
        windowPool.AppendL(win);
        CleanupStack::Pop(win);
        
        TSize expectedSize (NextRandom(),NextRandom());
        win->SetSize(expectedSize);
        // Wserv flattens size to zero
        expectedSize.iHeight = (expectedSize.iHeight < 0) ? 0 : expectedSize.iHeight;
        expectedSize.iWidth = (expectedSize.iWidth < 0) ? 0 : expectedSize.iWidth;
        expectedSizePool.AppendL(expectedSize);
        TSize retSize1 = win->Size();
        TEST(retSize1 == expectedSize);
        }

    // Stress system by performing actions on windows
    for (TUint iter = 0; iter < aIterations; ++iter)
        {
        TRWindowAction action = static_cast<TRWindowAction>(NextRandom(EMaxAction));
        TUint32 target = NextRandom(aWindowPoolSize);
        RWindowPerformAction(action, aEnableWindowSizeCache, rws1, *(windowPool[target]), expectedSizePool[target]);
        }

    // Cleanup afterwards
    CleanupStack::Pop(4);

    CleanupWindowPool(&windowPool);
    expectedSizePool.Close();
    rws1_wg1.Close();
    rws1.Close();
    
    __UHEAP_MARKEND;
    }

//------------------------------------------------------------------------------------------

void CTWindowSizeCacheTest::TestStressEnableWindowSizeCacheL(TUint aIterations)
    {
    __UHEAP_MARK;
    // w* State Initial
    RWsSession rws;
    TInt err = rws.Connect();
    CleanupClosePushL(rws);
    TEST(err==KErrNone);

    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    for (TUint iter = 0; iter < aIterations; iter++)
        {
        rws.EnableWindowSizeCacheL();
        }

    RWindowGroup wg(rws);
    wg.Construct(0);
    CleanupClosePushL(rws);

    // w1 State 0
    RWindow w1(rws);
    w1.Construct(wg,1);
    CleanupClosePushL(w1);
    
    TSize size1 (10001,123);
    w1.SetSize(size1);

    // w1 State 1
    TSize retSize1 = w1.Size();
    TEST(retSize1 == size1);

    // w1 State 2
    TSize size2 (20002,223);
    w1.SetSize(size2); 

    // w1 State 2
    // Only the first call to EnableWindowSizeCacheL() should have an effect
    // All subsequent calls to EnableWindowSizeCacheL() should have no effect
    for (TUint iter = 0; iter < aIterations; iter++)
        {
        rws.EnableWindowSizeCacheL();
        }
   
    // w1 State 1
    TSize retSize2 = w1.Size();
    TEST(retSize2 == size2);

    // w1 State Final
    CleanupStack::Pop();
    w1.Close();
    
    CleanupStack::Pop();
    wg.Close();
    
    CleanupStack::Pop();
    rws.Close();

    __UHEAP_MARKEND;
    }

/*
Tests 701-706 focus on window creation operating correctly, alternating use of the window size cache.
Tests 707-710 aim to cover state transitions for cache entries in the window size cache for RWindow.
Tests 711-712 performs basic verification of size related functionality for RBlankWindow and RBackedUpWindow.
Test 713 focusses on ensuring a performance gain is achieved with window size caching enabled.
Tests 714-720 aim to stress the window size cache with pseudo-random sequences of RWindow size related functions.

The states in the state machine model used for testing the RWindow size cache in tests 707-710 is as follows:

State: Initial
    Window not yet in existence
   
State 0: No Cache entry
    Window created, window size cache does not contain an entry for this window
    
State 1: Cache entry exists, cache clean
    Window created, window size cache exists, contains a valid size
    
State 2: Cache entry exists, cache dirty
    Window created, window size cache exists, does not contain a valid size
    
State: Final
    Window destroyed
 */
void CTWindowSizeCacheTest::RunTestCaseL(TInt /*aCurTestCase*/)
    {
    _LIT(KTest1,"Create RWindow, window size cache off");
    _LIT(KTest2,"Create RWindow, window size cache on");
    _LIT(KTest3,"Create RBlankWindow, window size cache off");
    _LIT(KTest4,"Create RBlankWindow, window size cache on");
    _LIT(KTest5,"Create RBackedUpWindow, window size cache off");
    _LIT(KTest6,"Create RBackedUpWindow, window size cache on");
    
    _LIT(KTest7,"RWindow - No cache entry");
    _LIT(KTest8,"RWindow - Cache entry exists, cache clean");
    _LIT(KTest9,"RWindow - Cache entry exists, cache dirty");
    _LIT(KTest10,"RWindow - Cache entry exists, cache dirty->clean");
   
    _LIT(KTest11,"RBlankWindow size cache");
    _LIT(KTest12,"RBackedUpWindow size cache");

    _LIT(KTest13,"RWindow Size Performance");
    _LIT(KTest14,"RWindow Randomised Stress window size cache off");
    _LIT(KTest15,"RWindow Randomised Stress window size cache on");
    _LIT(KTest16,"RWindow Randomised Stress MultiSession cache off");
    _LIT(KTest17,"RWindow Randomised Stress MultiSession cache on");
    _LIT(KTest18,"RWindow Randomised Stress Entry Volume cache off");
    _LIT(KTest19,"RWindow Randomised Stress Entry Volume cache on");
    _LIT(KTest20,"RWsSession stress EnableWindowSizeCacheL");

    const TDisplayMode displayMode = EColor16MAP;
    const TUint KPerformanceIterations = 50000;
    const TUint KStressTestIterations = 3000;
    const TUint KStressWindowPoolSize = 100;
    
    ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);

    switch(++iTest->iState)
        {
/**
@SYMTestCaseID      GRAPHICS-WSERV-0701

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test creation and destruction for instances of RWindow, window size cache off 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, some RWindowGroup and RWindow instances, and closes them.

@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 1:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0701"));
            iTest->LogSubTest(KTest1);
            TestCreateRWindowL(ETestSizeCacheNotEnabled);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0702

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test creation and destruction for instances of RWindow, window size cache on 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, enables window size cache, creates some RWindowGroup and RWindow instances, and closes them.

@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 2:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0702"));
            iTest->LogSubTest(KTest2);
            TestCreateRWindowL(ETestSizeCacheEnabled);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0703

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test creation and destruction for instances of RBlankWindow, window size cache off 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, creates some RWindowGroup and RBlankWindow instances, and closes them.

@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 3:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0703"));
            iTest->LogSubTest(KTest3);
            TestCreateRBlankWindowL(ETestSizeCacheNotEnabled);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0704

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test creation and destruction for instances of RBlankWindow, window size cache on 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, enables window size cache, creates some RWindowGroup and RBlankWindow instances, and closes them.

@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 4:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0704"));
            iTest->LogSubTest(KTest4);
            TestCreateRBlankWindowL(ETestSizeCacheEnabled);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0705

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test creation and destruction for instances of RBackedUpWindow, window size cache off

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, creates some RWindowGroup and RBackedUpWindow instances, and closes them.

@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 5:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0705"));
            iTest->LogSubTest(KTest5);
            TestCreateRBackedUpWindowL(ETestSizeCacheNotEnabled, displayMode);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0706

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test creation and destruction for instances of RBackedUpWindow, window size cache on 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, enables window size cache, creates some RWindowGroup and RBackedUpWindow instances, and closes them.

@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 6:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0706"));
            iTest->LogSubTest(KTest6);
            TestCreateRBackedUpWindowL(ETestSizeCacheEnabled, displayMode);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0707

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RWindow size cache - State: No Cache Entry  

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Perform the following with window size cache on and off:
                        Create RWsSession, creates RWindowGroup and RWindow.
                        Perform one of SetExtent/SetSize/SetExtentErr/SetSizeErr/EnableWindowSizeCacheL.
                        Clean up.
                        
@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 7:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0707"));
            iTest->LogSubTest(KTest7);
            TestRWindowNoCacheEntryL(ETestSizeCacheNotEnabled);
            TestRWindowNoCacheEntryL(ETestSizeCacheEnabled);
            TestRWindowNoCacheEntrySetExtentErrL(ETestSizeCacheNotEnabled);
            TestRWindowNoCacheEntrySetExtentErrL(ETestSizeCacheEnabled);
            TestRWindowNoCacheEntrySetSizeErrL(ETestSizeCacheNotEnabled);
            TestRWindowNoCacheEntrySetSizeErrL(ETestSizeCacheEnabled);
            TestRWindowNoCacheEntrySetExtentL(ETestSizeCacheNotEnabled);
            TestRWindowNoCacheEntrySetExtentL(ETestSizeCacheEnabled);
            TestRWindowNoCacheEntrySetSizeL(ETestSizeCacheNotEnabled);
            TestRWindowNoCacheEntrySetSizeL(ETestSizeCacheEnabled);
            TestRWindowNoCacheEntryEnableWindowSizeCacheL();
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0708

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RWindow size cache State: Cache entry exists, cache clean 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Perform the following with window size cache on and off:
                        Create RWsSession, creates RWindowGroup and RWindow, calls SetSize.
                        Perform one of Size/EnableWindowSizeCacheL.
                        Check that size returned matches expected size.
                        Clean up.
                        
@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 8:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0708"));
            iTest->LogSubTest(KTest8);
            TestRWindowCacheEntryExistsCacheCleanL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheCleanL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheCleanSizeL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheCleanSizeL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheCleanEnableWindowSizeCacheL();
            TestRWindowCacheEntryExistsCacheCleanEnableWindowSizeCacheLSizeL();
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0709

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RWindow size cache State: Cache entry exists, cache dirty

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Perform the following with window size cache on and off:
                        Create RWsSession, creates RWindowGroup and RWindow.
                        Calls SetSize, Size, then SetSize to enter state with dirty cache entry.
                        Perform combinations of SetSize/SetExtent/SetSizeErr/SetExtentErr/EnableWindowSizeCacheL.
                        Check that size returned matches the last size set on the window whenever size is queried.
                        Clean up.
                        
@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 9:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0709"));
            iTest->LogSubTest(KTest9);
            TestRWindowCacheEntryExistsCacheDirtySetSizeL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetExtentL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetExtentL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetExtentErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetExtentErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeTwiceL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeTwiceL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeSetSizeErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeSetSizeErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeSetExtentErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtySetSizeEnableWindowSizeCacheL();
            break;
            
            
/**
@SYMTestCaseID      GRAPHICS-WSERV-0710

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RWindow size cache State: Cache entry exists, cache dirty -> State: Cache entry exists, cache clean 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Perform the following with window size cache on and off:
                        Create RWsSession, creates RWindowGroup and RWindow.
                        Calls SetSize, Size, to enter state with clean cache entry.
                        Perform combinations of SetSize/SetExtent/SetSizeErr/SetExtentErr/EnableWindowSizeCacheL to enter state with dirty cache entry.
                        Calls RWindow::Size to re-enter state with clean cache entry.
                        Check that size returned matches the last size set on the window whenever size is queried.
                        Clean up.
 
@SYMTestExpectedResults Window objects created successfully, no memory leaks.
*/
        case 10:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0710"));
            iTest->LogSubTest(KTest10);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetExtentErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeTwiceL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeTwiceL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetSizeErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetSizeErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentErrL(ETestSizeCacheNotEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeSetExtentErrL(ETestSizeCacheEnabled);
            TestRWindowCacheEntryExistsCacheDirtyToCleanSetSizeEnableWindowSizeCacheL();
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0711

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RBlankWindow Size cache behaviour 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Utilises RBlankWindow::SetSize, RBlankWindow::SetExtent, RBlankWindow::Size.
                    Checks size returned by RBlankWindow::Size.
                    Cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size returned should be last size set on window
*/
        case 11:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0711"));
            iTest->LogSubTest(KTest11);
            TestRBlankWindowSetSizeSetExtentL(ETestSizeCacheNotEnabled);
            TestRBlankWindowSetSizeSetExtentL(ETestSizeCacheEnabled);
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0712

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RBackedUpWindow Size cache behaviour 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Utilises RBackedUpWindow::SetSizeErr, RBackedUpWindow::SetExtentErr, RBackedUpWindow::Size.
                    Checks size returned by RBackedUpWindow::Size.
                    Cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size returned should be last size set on window
*/
        case 12:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0712"));
            iTest->LogSubTest(KTest12);
            TestRBackedUpWindowSetSizeErrSetExtentErrL(ETestSizeCacheNotEnabled);
            TestRBackedUpWindowSetSizeErrSetExtentErrL(ETestSizeCacheEnabled);
            break;
            
/**
@SYMTestCaseID      GRAPHICS-WSERV-0713

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Compare performance of RWindow::Size function with size cache on and off 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, optionally enables window size cache, creates an RWindowGroup and an RWindow 
                    Calls RWindow::Size KPerformanceIterations times.
                    Measures time taken - with and without size cache enabled

@SYMTestExpectedResults Window objects created successfully, no memory leaks, time taken for KPerformanceIterations RWindow::Size() calls with cache enabled should be faster.
*/
        case 13:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0713"));
            iTest->LogSubTest(KTest13);
            TUint64 sizeCacheOffDeltaMicroSeconds = TestRWindowSizePerformanceL(ETestSizeCacheNotEnabled, KPerformanceIterations);
            TUint64 sizeCacheOnDeltaMicroSeconds = TestRWindowSizePerformanceL(ETestSizeCacheEnabled, KPerformanceIterations);
            TEST(sizeCacheOnDeltaMicroSeconds < sizeCacheOffDeltaMicroSeconds);
            INFO_PRINTF1(_L("Accessing size using cache is faster"));
            }
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0714

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Randomised stress test - RWindow size related functions

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, creates some RWindowGroup and RWindow instances, performs random size related actions, cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size for each window matches last size set on window
*/
        case 14:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0714"));
            iTest->LogSubTest(KTest14);
            TInt seed = Math::Random();
            // To replicate a test run, take the random seed from the test log and pass into SetRandomSeed
            SetRandomSeed(seed);
            TestRandomisedStressL(ETestSizeCacheNotEnabled, KStressTestIterations);
            }
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0714

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Randomised stress test - RWindow size related functions, size cache on 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, enables window size cache, creates some RWindowGroup and RWindow instances, performs random size related actions, cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size for each window matches last size set on window
*/
        case 15:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0715"));
            iTest->LogSubTest(KTest15);
            TInt seed = Math::Random();
            // To replicate a test run, take the random seed from the test log and pass into SetRandomSeed
            SetRandomSeed(seed);
            TestRandomisedStressL(ETestSizeCacheEnabled, KStressTestIterations);
            }
            break;

            
/**
@SYMTestCaseID      GRAPHICS-WSERV-0716

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Randomised stress test - RWindow size related functions, multiple sessions 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates two RWsSession instances, creates some RWindowGroup and RWindow instances, performs random size related actions, cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size for each window matches last size set on window
*/
        case 16:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0716"));
            iTest->LogSubTest(KTest16);
            TInt seed = Math::Random();
            // To replicate a test run, take the random seed from the test log and pass into SetRandomSeed
            SetRandomSeed(seed);
            TestRandomisedStressMultiSessionL(ETestSizeCacheNotEnabled, KStressTestIterations);
            }
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0717

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Randomised stress test - RWindow size related functions, multiple sessions, size cache on

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates two RWsSession instances, enables window size cache, creates some RWindowGroup and RWindow instances, performs random size related actions, cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size for each window matches last size set on window
*/
        case 17:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0717"));
            iTest->LogSubTest(KTest17);
            TInt seed = Math::Random();
            // To replicate a test run, take the random seed from the test log and pass into SetRandomSeed
            SetRandomSeed(seed);
            TestRandomisedStressMultiSessionL(ETestSizeCacheEnabled, KStressTestIterations);
            }
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0718

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Randomised stress test - RWindow size related functions, large number of windows 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, creates some RWindowGroup and RWindow instances, performs random size related actions, cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size for each window matches last size set on window
*/
        case 18:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0718"));
            iTest->LogSubTest(KTest18);
            TInt seed = Math::Random();
            // To replicate a test run, take the random seed from the test log and pass into SetRandomSeed
            SetRandomSeed(seed);
            TestRandomisedStressCacheEntryVolumeL(ETestSizeCacheNotEnabled, KStressTestIterations, KStressWindowPoolSize);
            }
            break;
/**
@SYMTestCaseID      GRAPHICS-WSERV-0719

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Randomised stress test - RWindow size related functions, large number of windows 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, enables window size cache, creates some RWindowGroup and RWindow instances, performs random size related actions, cleans up.

@SYMTestExpectedResults Window objects created successfully, no memory leaks, size for each window matches last size set on window
*/
        case 19:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0719"));
            iTest->LogSubTest(KTest19);
            TInt seed = Math::Random();
            // To replicate a test run, take the random seed from the test log and pass into SetRandomSeed
            SetRandomSeed(seed);
            TestRandomisedStressCacheEntryVolumeL(ETestSizeCacheEnabled, KStressTestIterations, KStressWindowPoolSize);
            }
            break;

/**
@SYMTestCaseID      GRAPHICS-WSERV-0720

@SYMDEF             DevCR BBRE-7UWGHL

@SYMTestCaseDesc    Test RWsSession::EnableWindowSizeCacheL() API 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Creates an RWsSession, calls EnableWindowSizeCacheL() multiple times, uses Size related APIs.

@SYMTestExpectedResults No memory leaks.
*/
        case 20:
            {
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0720"));
            iTest->LogSubTest(KTest20);
            TestStressEnableWindowSizeCacheL(KStressTestIterations);
            }
            break;
        default:
            ((CTWindowSizeCacheTestStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
            ((CTWindowSizeCacheTestStep*)iStep)->CloseTMSGraphicsStep();
            TestComplete();
            break;
        }
    ((CTWindowSizeCacheTestStep*)iStep)->RecordTestResultL();
    }

__WS_CONSTRUCT_STEP__(WindowSizeCacheTest)
