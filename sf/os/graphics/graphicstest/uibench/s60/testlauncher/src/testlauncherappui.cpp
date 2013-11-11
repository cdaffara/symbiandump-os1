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


#include "testlauncher.pan"
#include "testlauncherappUi.h"
#include "testlauncherappView.h"
#include "testlauncher.hrh"
#include "testlauncherquerydialog.h"
#include "testlistcontainer.h"

#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <testlauncher.rsg>


void CHelloWorldBasicAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Create view object
    iAppView = CHelloWorldBasicAppView::NewL(ClientRect());
    
    iAppContainer = new (ELeave) CFileListContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL(ClientRect());
    AddToStackL(iAppContainer);
    }

CHelloWorldBasicAppUi::CHelloWorldBasicAppUi()
    {
    // empty
    }

CHelloWorldBasicAppUi::~CHelloWorldBasicAppUi()
    {
    delete iAppView;
    if (iAppContainer)
        {
        RemoveFromStack(iAppContainer);
        delete iAppContainer;
        }
    }
// Handles keyevents. In this application keyevents are
// caught in CFileListContainer::OfferKeyEventL()
TKeyResponse CHelloWorldBasicAppUi::HandleKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// Takes care of command handling.
void CHelloWorldBasicAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EHelloWorldBasicCommand1:
            {
            HBufC* defCmd = HBufC::NewLC(16);
            *defCmd = _L("testexecute.exe ");            
			TBuf<1024> usrCmd;
            CHelloWorldQueryDialog* dlg = new (ELeave) CHelloWorldQueryDialog(usrCmd, defCmd);
            dlg->ExecuteLD(R_DIALOG_TEXT_EDIT_QUERY);
            CleanupStack::PopAndDestroy(defCmd);
            
         	TLex lex(usrCmd);
         	TPtrC exe = lex.NextToken();
         	TPtrC arg = lex.Remainder();

            RProcess p;
            TInt err = p.Create(exe, arg);
            if (err != KErrNone) 
            	{
            	CEikonEnv::Static()->InfoMsg(_L("Unable to create process"));
            	break;
            	}            	
            p.Resume();
            p.Close();
            }
            break;

        case EHelloWorldBasicCommand2:
            {
         	TPtrC exe = _L("testexecute.exe");
         	TPtrC arg = _L("c:\\uibench_s60\\te_uibench_s60.script");

            RProcess p;
            TInt err = p.Create(exe, arg);
            if (err != KErrNone) 
            	{
            	CEikonEnv::Static()->InfoMsg(_L("Unable to create process"));
            	break;
            	}
            	
            p.Resume();
            p.Close();
            }
            break;

        case EHelloWorldBasicCommand3:
            {
         	TPtrC exe = _L("testexecute.exe");
         	TPtrC arg = _L("c:\\ui_bench_s60\\");

            RProcess p;
            TInt err = p.Create(exe, arg);
            if (err != KErrNone) 
            	{
            	CEikonEnv::Static()->InfoMsg(_L("Unable to create process"));
            	break;
            	}            	
            p.Resume();
            p.Close();
            }
            break;
        case EHelloWorldBasicCommand4:
        	{
         	TPtrC exe = _L("testexecute.exe");
         	TPtrC arg = _L("c:\\ui_bench_s60\\");

            RProcess p;
            TInt err = p.Create(exe, arg);
            if (err != KErrNone) 
            	{
            	CEikonEnv::Static()->InfoMsg(_L("Unable to create process"));
            	break;
            	}
            	
            p.Resume();
            p.Close();
        	}
        	break;
        case EAknSoftkeySelect:
            {
            iAppContainer->LaunchCurrentL();
            }
            break;
        default:
            Panic(EHelloWorldBasicUi);
            break;
        }
    }

//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
void CHelloWorldBasicAppUi::HandleStatusPaneSizeChange()
{
	CAknAppUi::HandleStatusPaneSizeChange(); //call to upper class
	iAppContainer->SetRect(ClientRect());
    iAppView->SetRect(ClientRect());
}
