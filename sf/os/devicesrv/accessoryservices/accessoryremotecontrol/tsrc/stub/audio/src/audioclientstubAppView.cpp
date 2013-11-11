/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Application view implementation
*
*/


// INCLUDE FILES
#include <coemain.h>
#include "audioclientstubAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CaudioclientstubAppView* CaudioclientstubAppView::NewL(const TRect& aRect)
    {
    RDebug::Print( _L("CaudioclientstubAppView::NewL") ) ;
    CaudioclientstubAppView* self = CaudioclientstubAppView::NewLC(aRect);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CaudioclientstubAppView* CaudioclientstubAppView::NewLC(const TRect& aRect)
    {
    RDebug::Print( _L("CaudioclientstubAppView::NewLC") ) ;
    CaudioclientstubAppView* self = new ( ELeave ) CaudioclientstubAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CaudioclientstubAppView::ConstructL(const TRect& aRect)
    {
    RDebug::Print( _L("CaudioclientstubAppView::ConstructL") ) ;
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::CaudioclientstubAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CaudioclientstubAppView::CaudioclientstubAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::~CaudioclientstubAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CaudioclientstubAppView::~CaudioclientstubAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CaudioclientstubAppView::Draw(const TRect& /*aRect*/) const
    {
    RDebug::Print( _L("CaudioclientstubAppView::Draw") ) ;
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect(Rect());

    // Clears the screen
    gc.Clear(drawRect);

    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CaudioclientstubAppView::SizeChanged()
    {
    RDebug::Print( _L("CaudioclientstubAppView::SizeChanged") ) ;
    DrawNow();
    }
// End of File
