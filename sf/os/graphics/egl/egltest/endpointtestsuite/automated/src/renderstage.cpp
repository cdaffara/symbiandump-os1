// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


//This render stage sets up a test thread within window server that waits on
//an asynchonous message queue for test commands that are sent from the TEF driver app.
//All render stage calls are simply forwarded to the next stage in the chain. The
//renderstage is a NULL renderstage.


#include "renderstage.h"
#include "remotetestbase.h"
#include <e32math.h>


CRenderStage* CRenderStage::NewL(MWsGraphicDrawerEnvironment* /*aEnv*/, MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage)
    {
    CRenderStage* stage = new(ELeave) CRenderStage();
    CleanupStack::PushL(stage);
    stage->ConstructL(aNextStage);
    CleanupStack::Pop(stage);
    return stage;
    }


CRenderStage::CRenderStage()
    {
    }


void CRenderStage::ConstructL(CWsRenderStage* aNextStage)
    {
    ENDPOINT_ASSERT_DEBUG(aNextStage, User::Invariant());
    BaseConstructL();
    SetNext(aNextStage);

    //Stack and heap sizes.
    static const TInt KStackSize =   0x2000;      //  8KB
    static const TInt KHeapMinSize = 0x1000;      //  4KB
    static const TInt KHeapMaxSize = 0x1000000;   // 16MB

    //Create test environment thread.
    TUint32 random = Math::Random();
    TName threadName;
    _LIT(KThreadNameFormat, "%S-%u");
    _LIT(KEnvName, "EpTestRemoteEnv");
    threadName.Format(KThreadNameFormat, &KEnvName, random);
    User::LeaveIfError(iTestEnvThread.Create(threadName, TestEnvThreadEntryPoint, KStackSize, KHeapMinSize, KHeapMaxSize, this));
    iTestEnvThread.Resume();
    }


CRenderStage::~CRenderStage()
    {
    iTestEnvThread.Kill(KErrNone);
    }


TAny* CRenderStage::ResolveObjectInterface(TUint aTypeId)
    {
    return Next()->ResolveObjectInterface(aTypeId);
    }


void CRenderStage::Begin(const TRegion* aRegion)
    {
    Next()->Begin(aRegion);
    }


void CRenderStage::End(TRequestStatus* aCompositorReady)
    {
    Next()->End(aCompositorReady);
    }


TInt CRenderStage::TestEnvThreadEntryPoint(TAny* /*aSelf*/)
    {
    RDebug::Printf("CRenderStage: Entering Test Environment Thread...");
    
    //Create cleanup stack.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    ASSERT(cleanup);
    
    //Create active scheduler.
    CActiveScheduler* scheduler = new CActiveScheduler();
    ASSERT(scheduler);
    CActiveScheduler::Install(scheduler);
    
    //Create a CRemoteTestEnv.
    CRemoteTestEnv* testEnv = NULL;
    TRAPD(err, testEnv = CRemoteTestEnv::NewL());
    __ASSERT_ALWAYS(err == KErrNone, User::Invariant());
    testEnv->StartReceivingCmds();

    //Clean up.
    delete scheduler;
    delete cleanup;
    RDebug::Printf("CRenderStage: Leaving Test Environment Thread...");
    return KErrNone;
    }

