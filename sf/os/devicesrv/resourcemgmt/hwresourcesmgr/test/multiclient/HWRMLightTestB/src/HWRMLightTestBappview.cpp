// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <coemain.h>

#include "HWRMLightTestBappview.h"

CHWRMLightTestBAppView* CHWRMLightTestBAppView::NewL(const TRect& aRect)
    {
    CHWRMLightTestBAppView* self = CHWRMLightTestBAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

CHWRMLightTestBAppView* CHWRMLightTestBAppView::NewLC(const TRect& aRect)
    {
    CHWRMLightTestBAppView* self = new (ELeave) CHWRMLightTestBAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

CHWRMLightTestBAppView::CHWRMLightTestBAppView()
    {
	// No implementation required
    }

CHWRMLightTestBAppView::~CHWRMLightTestBAppView()
    {
	// No implementation required
    }

void CHWRMLightTestBAppView::ConstructL(const TRect& aRect)
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// Draw this application's view to the screen
void CHWRMLightTestBAppView::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context 
    CWindowGc& gc = SystemGc();
    
    // Gets the control's extent
    TRect rect = Rect();
    
    // Clears the screen
    gc.Clear(rect);
    }


