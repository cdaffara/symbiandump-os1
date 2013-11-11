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
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "tsmallwindowopengl.h"


const TInt KOpenGLResolution = 2;


CTWindow* CTSmallWindowOpenGL::NewL(RWsSession &aWs, 
        const RWindowTreeNode &aParent, 
        const TPoint& aStartingPoint,
        const TSize& aWindowSize)
    {
    CTSmallWindowOpenGL* self = new (ELeave)CTSmallWindowOpenGL(aStartingPoint, aWindowSize);
    CleanupStack::PushL(self);
    self->ConstructL(aWs, aParent);
    CleanupStack::Pop(self);
    return self;
    }

CTSmallWindowOpenGL::CTSmallWindowOpenGL(const TPoint& aStartingPoint, const TSize& aWindowSize) :
        CTFlowWindowOpenGL(aStartingPoint, aWindowSize)
    {
    // empty
    }

void CTSmallWindowOpenGL::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
    {
    CTFlowWindowOpenGL::ConstructL(aWs, aParent);
    SetResolutionL(KOpenGLResolution);    
    }

CTSmallWindowOpenGL::~CTSmallWindowOpenGL()
    {
    // empty
    }
