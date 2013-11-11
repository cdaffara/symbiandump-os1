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
#include <e32math.h>
#include "t_app1ui.h"
#include "t_app1view.h"
#include "t_wservconsts.h" 
#include "t_inidata.h"

/**
 *	constructL method that creates the AppView object
 *	
 */  
void CTApp1Ui::ConstructL()
    {
    BaseConstructL();
	CWsScreenDevice * screen = CCoeEnv::Static()->ScreenDevice();
	TSize screenSize=screen->SizeInPixels();
	
	TInt posX = 0;
	READ_INT(KTApp1PositionX, KWServTApp1ConfigFile, posX);
	CalculateAbsL(posX, screenSize.iWidth);
	
	TInt posY = 0;
	READ_INT(KTApp1PositionY, KWServTApp1ConfigFile, posY);
	CalculateAbsL(posY, screenSize.iHeight);
	
	TInt width = 0;
	READ_INT(KTApp1Width, KWServTApp1ConfigFile, width);
	CalculateAbsL(width, screenSize.iWidth);
	
	TInt height = 0;
	READ_INT(KTApp1Height, KWServTApp1ConfigFile, height);
	CalculateAbsL(height, screenSize.iHeight);
	
    TRect myRect(TPoint(posX,posY), TPoint(width+posX,height+posY));
	iAppView = CTApp1View::NewL(myRect);
	}


void CTApp1Ui::CalculateAbsL(TInt& aValue, const TInt& aFactor) const
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
CTApp1Ui::~CTApp1Ui()
	{
	delete iAppView;
	}
	
/**
 *	Handles the Menu events
 *	@param aCommand - command to be passed based on the menu item
 *						selected by the user
 *
 */  
void CTApp1Ui::HandleCommandL(TInt aCommand)
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
void CTApp1Ui::HandleSystemEventL(const TWsEvent& aEvent)
	{
	switch (*(TApaSystemEvent*)(aEvent.EventData()))
		{
		case EApaSystemEventBroughtToForeground:
			RProcess::Rendezvous(KErrNone);
			break;
         default:
			User::Leave(KErrAbort);
			break;
		}	
	}
