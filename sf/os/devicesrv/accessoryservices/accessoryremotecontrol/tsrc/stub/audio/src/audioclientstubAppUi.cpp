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
* Description:  CaudioclientstubAppUi implementation
*
*/


// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <audioclientstub.rsg>
#include <tspclientmapper.h>

#include "audioclientstub.hrh"
#include "audioclientstub.pan"
#include "audioclientstubApplication.h"
#include "audioclientstubAppUi.h"
#include "audioclientstubAppView.h"
#include "audioclientstubremconobserver.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CaudioclientstubAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CaudioclientstubAppUi::ConstructL()
    {
    RDebug::Print( _L("CaudioclientstubAppUi::ConstructL") ) ;
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Create view object
    iAppView = CaudioclientstubAppView::NewL(ClientRect() );

    iTspClientMapper = CTspClientMapper::NewL();
    iTspClientMapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, RProcess().Id() );

    iRemConObserver = CAudioClientStubRemConObserver::NewL();
    }
// -----------------------------------------------------------------------------
// CaudioclientstubAppUi::CaudioclientstubAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CaudioclientstubAppUi::CaudioclientstubAppUi()
    {
    RDebug::Print( _L("CaudioclientstubAppUi::CaudioclientstubAppUi") ) ;
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppUi::~CaudioclientstubAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CaudioclientstubAppUi::~CaudioclientstubAppUi()
    {
    RDebug::Print( _L("CaudioclientstubAppUi::~CaudioclientstubAppUi") ) ;
    if (iAppView)
        {
        delete iAppView;
        iAppView = NULL;
        }
    
    if ( iTspClientMapper )
        {
        iTspClientMapper->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, RProcess().Id() );
        }
    
    delete iTspClientMapper;
    
    delete iRemConObserver;
    }

// -----------------------------------------------------------------------------
// CaudioclientstubAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CaudioclientstubAppUi::HandleCommandL(TInt aCommand)
    {
    RDebug::Print( _L("CaudioclientstubAppUi::HandleCommandL: aCommand=%d"), aCommand ) ;
    switch (aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case ECommand1:
            {

            // Load a string from the resource file and display it
            HBufC* textResource = StringLoader::LoadLC(R_COMMAND1_TEXT);
            CAknInformationNote* informationNote;

            informationNote = new ( ELeave ) CAknInformationNote;

            // Show the information Note with
            // textResource loaded with StringLoader.
            informationNote->ExecuteLD( *textResource);

            // Pop HBuf from CleanUpStack and Destroy it.
            CleanupStack::PopAndDestroy(textResource);
            }
            break;
        case ECommand2:
            {

            }
            break;
        case EHelp:
            {

            }
            break;
        case EAbout:
            {

            CAknMessageQueryDialog* dlg = new (ELeave)CAknMessageQueryDialog();
            dlg->PrepareLC(R_ABOUT_QUERY_DIALOG);
            HBufC* title =
                    iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TITLE);
            dlg->QueryHeading()->SetTextL(*title);
            CleanupStack::PopAndDestroy(); //title
            HBufC* msg = iEikonEnv->AllocReadResourceLC(R_ABOUT_DIALOG_TEXT);
            dlg->SetMessageTextL(*msg);
            CleanupStack::PopAndDestroy(); //msg
            dlg->RunLD();
            }
            break;
        default:
            Panic(EaudioclientstubUi);
            break;
        }
    }
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CaudioclientstubAppUi::HandleStatusPaneSizeChange()
    {
    RDebug::Print( _L("CaudioclientstubAppUi::HandleStatusPaneSizeChange") ) ;
    iAppView->SetRect(ClientRect() );
    }


// End of File
