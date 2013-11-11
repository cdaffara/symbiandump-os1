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


#ifndef __RENDERSTAGE_H__
#define __RENDERSTAGE_H__


#include <e32base.h>
#include <graphics/wsrenderstage.h>


//This render stage sets up a test thread within window server that waits on
//an asynchonous message queue for test commands that are sent from the TEF driver app.
//All render stage calls are simply forwarded to the next stage in the chain.
class CRenderStage : public CWsRenderStage
    {
public:
    static CRenderStage* NewL(MWsGraphicDrawerEnvironment* /*aEnv*/, MWsScreen* /*aScreen*/, MWsScreenRedraw* /*aScreenRedraw*/, CWsRenderStage* aNextStage);
    ~CRenderStage();
    TAny* ResolveObjectInterface(TUint aTypeId);
    void Begin(const TRegion* aRegion);
    void End(TRequestStatus* aCompositorReady);

private:
    CRenderStage();
    void ConstructL(CWsRenderStage* aNextStage);
    static TInt TestEnvThreadEntryPoint(TAny* aSelf);

private:
    RThread iTestEnvThread;
    };


#endif
