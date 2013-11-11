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


#include "tflowwindowopengl.h"


CTWindow* CTFlowWindowOpenGL::NewL(RWsSession &aWs, 
        const RWindowTreeNode &aParent, 
        const TPoint& aStartingPoint,
        const TSize& aWindowSize)
    {
    CTFlowWindowOpenGL* self = new (ELeave)CTFlowWindowOpenGL(aStartingPoint, aWindowSize);
    CleanupStack::PushL(self);
    self->ConstructL(aWs, aParent);
    CleanupStack::Pop(self);
    return self;
    }

CTFlowWindowOpenGL::CTFlowWindowOpenGL(const TPoint& aStartingPoint, const TSize& aWindowSize) :
        CTWindow(aStartingPoint, aWindowSize)
	{
	// empty
	}

/**
 * Constructs EGL environment, model and output objects which are used to render
 * OpenGL ES content.
 * 
 */
void CTFlowWindowOpenGL::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
    {
    CTWindow::ConstructL(aWs, aParent);
    iEglEnvironment = CEglEnvironment::NewL(iWindow);
    iModel = CModel::NewL(iEglEnvironment->Display(), iEglEnvironment->Surface());
    }

CTFlowWindowOpenGL::~CTFlowWindowOpenGL()
	{
	delete iEglEnvironment;
	delete iModel;
	}

/**
 * Render one OpenGL ES scene and draw it to the screen.
 */
void CTFlowWindowOpenGL::RenderL()
	{
	CTWindow::RenderL();
	iTime++;
	iModel->DrawToBuffer(iTime);
	iEglEnvironment->DrawToWindow();
	}

void CTFlowWindowOpenGL::SetResolutionL(TInt aResolution)
    {
    iModel->SetResolutionL(aResolution);
    }
