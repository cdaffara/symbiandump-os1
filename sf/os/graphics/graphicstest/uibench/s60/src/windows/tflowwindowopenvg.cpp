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


#include "tflowwindowopenvg.h"


// defines after how many updates the scene is actually drawn to the screen
const TInt KSceneInterleaver = 5;


CTWindow* CTFlowWindowOpenVG::NewL(RWsSession &aWs, 
        const RWindowTreeNode &aParent, 
        const TPoint& aStartingPoint,
        const TSize& aWindowSize)
    {
    CTFlowWindowOpenVG* self = new (ELeave)CTFlowWindowOpenVG(aStartingPoint, aWindowSize);
    CleanupStack::PushL(self);
    self->ConstructL(aWs, aParent);
    CleanupStack::Pop(self);
    return self;
    }

CTFlowWindowOpenVG::CTFlowWindowOpenVG(const TPoint& aStartingPoint, const TSize& aWindowSize) : 
        CTWindow(aStartingPoint, aWindowSize)
	{
	// empty
	}

CTFlowWindowOpenVG::~CTFlowWindowOpenVG()
	{
	delete iVGRendering;
	}

void CTFlowWindowOpenVG::ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent)
	{
	CTWindow::ConstructL(aWs, aParent);
	iVGRendering = CEGLRendering::NewL(iWindow);
	}

/**
 * Renders the next scene and draws it to the screen.
 * 
 */
void CTFlowWindowOpenVG::RenderL()
	{
	CTWindow::RenderL();
	// don't update the cover flow display every time
	iInterleaveCounter++;
	if (iInterleaveCounter >= KSceneInterleaver) // todo: should be removed
		{
		iVGRendering->UpdateDisplay();
		iInterleaveCounter = 0;
		}
	}

