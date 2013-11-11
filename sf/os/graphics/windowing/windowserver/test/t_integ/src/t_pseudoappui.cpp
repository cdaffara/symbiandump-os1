// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <apgtask.h>
#include <eikenv.h>
#include <eikmenup.h>
#include <eikstart.h>

#include "t_pseudoappeng.h"
#include "t_pseudoappview.h"
#include "t_pseudoapp.h"
#include "t_pseudoappui.h"

//
// CPseudoAppUi
//

/**
 *	constructL method that creates the AppView object
 *
 */
void CPseudoAppUi::ConstructL()
    {
	RDebug::Print(_L("Create App Framework\n"));

    BaseConstructL();
#ifndef T_PSEUDOAPP1
	iAppView = CPseudoAppView::NewL(ClientRect());
#else
	// t_pseudoapp1 is used in a test that displays two animations on one screen.
	// Window dimensions and positions chosen such that both animations are visible.
	TSize sz = ClientRect().Size();
    TRect rect(sz.iWidth/2, sz.iHeight/2, sz.iWidth, sz.iHeight);
	iAppView = CPseudoAppView::NewL(rect);
#endif //T_PSEUDOAPP1

	//Detect screen rotations
	iCoeEnv->RootWin().EnableScreenChangeEvents();
	}

/**
 *	Destructor
 *
 */
CPseudoAppUi::~CPseudoAppUi()
	{
	delete iAppView;
	}

/**
 *	Handles the Menu events
 *	@param aCommand - command to be passed based on the menu item
 *						selected by the user
 *
 */
void CPseudoAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
			{
			Exit();
			break;
			}
		default:
			{
			User::Leave(KErrAbort);
			break;
			}
		}
	}

/**
 *	Handles system events
 *	@param aEvent - event that has to be handled by the function
 *
 */
void CPseudoAppUi::HandleSystemEventL(const TWsEvent& aEvent)
	{
	switch (*(TApaSystemEvent*)(aEvent.EventData()))
		{
		case EApaSystemEventBroughtToForeground:
			RProcess::Rendezvous(KErrNone);
			break;
         default:
			break;
		}
	}

void CPseudoAppUi::HandleScreenDeviceChangedL()
	{
	//The function called after a screen rotation event occurs
	//Only rotation of one screen supported
	iAppView->RotateL();
	}


