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
#include <e32math.h>
#include <eikenv.h>
#include "t_dsaappui.h"
#include "t_dsaappview.h"
#include "t_inidata.h"
#include "t_wservconsts.h" 

/**
 *	constructL method that creates the AppView object
 *	
 */  
void CDsaAppUi::ConstructL()
    {
    BaseConstructL();
	CWsScreenDevice * screen = CCoeEnv::Static()->ScreenDevice();
	TSize screenSize=screen->SizeInPixels();
	TInt posX = 0;
	READ_INT(KDsaPositionX, KWServDsaAppConfigFile, posX);
	CalculateAbsL(posX, screenSize.iWidth);
	
	TInt posY = 0;
	READ_INT(KDsaPositionY, KWServDsaAppConfigFile, posY);
	CalculateAbsL(posY, screenSize.iHeight);
	
	TInt width = 0;
	READ_INT(KDsaWidth, KWServDsaAppConfigFile, width);
	CalculateAbsL(width, screenSize.iWidth);
	
	TInt height = 0;
	READ_INT(KDsaHeight, KWServDsaAppConfigFile, height);
	CalculateAbsL(height, screenSize.iHeight);
	
    TRect myRect(TPoint(posX,posY), TPoint(width+posX,height+posY));
	iAppView = CDsaAppView::NewL(myRect);
	}


void CDsaAppUi::CalculateAbsL(TInt& aValue, const TInt& aFactor)
{
	TReal tempVar = static_cast<TReal>(aValue)*static_cast<TReal>(aFactor);
	tempVar = tempVar/100; // as percentage
	TInt32 tempVal = 0;
	User::LeaveIfError(Math::Int(tempVal, tempVar));
	aValue = tempVal;
}
	
/**
 *	Destructor 
 *	
 */  
CDsaAppUi::~CDsaAppUi()
	{
	delete iAppView;
	}
	
/**
 *	Handles the Menu events
 *	@param aCommand - command to be passed based on the menu item
 *						selected by the user
 *
 */  
void CDsaAppUi::HandleCommandL(TInt aCommand)
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
void CDsaAppUi::HandleSystemEventL(const TWsEvent& aEvent)
	{
	switch (*(TApaSystemEvent*)(aEvent.EventData()))
		{
		case EApaSystemEventBroughtToForeground:
			{
			RProcess::Rendezvous(KErrNone);
			break;
			}
        default:
        	{
			User::Leave(KErrAbort);
			break;
        	}
		}	
	}
