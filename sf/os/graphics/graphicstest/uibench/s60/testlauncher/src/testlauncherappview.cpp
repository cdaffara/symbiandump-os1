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


#include "testlauncherappView.h"

#include <coemain.h>
#include <aknutils.h>
#include <gdi.h>


CHelloWorldBasicAppView* CHelloWorldBasicAppView::NewL( const TRect& aRect )
    {
    CHelloWorldBasicAppView* self = CHelloWorldBasicAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

CHelloWorldBasicAppView* CHelloWorldBasicAppView::NewLC( const TRect& aRect )
    {
    CHelloWorldBasicAppView* self = new (ELeave) CHelloWorldBasicAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

void CHelloWorldBasicAppView::ConstructL(const TRect& aRect)
    {
	TFontSpec fs(_L("Courier"), 16);
	CWsScreenDevice* scr = CEikonEnv::Static()->ScreenDevice();
	User::LeaveIfError(scr->GetNearestFontInPixels(iFont, fs));

    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::CHelloWorldBasicAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHelloWorldBasicAppView::CHelloWorldBasicAppView():	iDir(1)
    {
    // No implementation required
    }

CHelloWorldBasicAppView::~CHelloWorldBasicAppView()
    {
    CWsScreenDevice* scr = CEikonEnv::Static()->ScreenDevice();
	scr->ReleaseFont(iFont);
    }


// -----------------------------------------------------------------------------
// CHelloWorldBasicAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CHelloWorldBasicAppView::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    _LIT(KMsgP1, "This application enables you to launch ");
    _LIT(KMsgP2, "tests using testexecute.");
    gc.Clear();
    gc.UseFont(iFont);
    gc.DrawText(KMsgP1, TPoint(10,20));
    gc.DrawText(KMsgP2, TPoint(10,35));
    gc.DiscardFont();
    }
