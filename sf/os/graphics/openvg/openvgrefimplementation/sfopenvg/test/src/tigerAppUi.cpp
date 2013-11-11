/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Implementation of CTigerAppUi class
*/


// INCLUDE FILES
#include "TigerAppUi.h"
#include "TigerContainer.h"
#include <Tiger.rsg>
#include "Tiger.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CTigerAppUi::ConstructL()
// ----------------------------------------------------------
//
void CTigerAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CTigerContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CTigerAppUi::~CTigerAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CTigerAppUi::~CTigerAppUi()
{
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
}

// ------------------------------------------------------------------------------
// CTigerAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CTigerAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CTigerAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CTigerAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/ ,TEventCode /*aType*/)
{
    return EKeyWasNotConsumed;
}

// ----------------------------------------------------
// CTigerAppUi::HandleCommandL(TInt aCommand)
// Command handler
// ----------------------------------------------------
//
void CTigerAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        default:
            break;
        }
    }

// End of File
