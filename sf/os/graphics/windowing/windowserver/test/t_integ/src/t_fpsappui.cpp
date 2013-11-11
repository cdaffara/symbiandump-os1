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

#include <eikenv.h>
#include "t_fpsappeng.h"
#include "t_fpsappview.h"
#include "t_fpsapp.h"
#include "t_fpsappui.h"

//
// CFpsAppUi
//

/**
 *	constructL method that creates the AppView object
 *
 */
void CFpsAppUi::ConstructL()
    {
	RDebug::Print(_L("Create App Framework\n"));
    BaseConstructL();
	iAppView = CFpsAppView::NewL(ClientRect());
	}

/**
 *	Destructor
 *
 */
CFpsAppUi::~CFpsAppUi()
	{
	delete iAppView;
	}

/**
 *	Handles the Menu events
 *	@param aCommand - command to be passed based on the menu item
 *						selected by the user
 *
 */
void CFpsAppUi::HandleCommandL(TInt aCommand)
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




