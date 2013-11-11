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
#include <eikmenup.h>
#include <t_app1.rsg>
#include "t_app1view.h"
#include "t_app1eng.h"

/**
 *	Private constructor, does nothing	
 *
 */  
CTApp1View::CTApp1View()
	{
	}
	
/**
 *	Standard 2 phase construction, constructs a View object
 *	@return pointer to the  CTApp1View
 *	@param aRect - rectangle coordinate defining control's extent
 */  
 CTApp1View* CTApp1View::NewL(const TRect& aRect)
	{
	CTApp1View* self = new(ELeave) CTApp1View();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}
	
/**
 *	
 *	Destructor
 */  
CTApp1View::~CTApp1View()
	{
	delete iTApp1Text;
	iTApp1Text = NULL;
	delete iEng;
	iEng = NULL;
	}
	
/**
 *	constructL method
 *	@param aRect - rectangle coordinate defining control's extent
 *	
 */  
void CTApp1View::ConstructL(const TRect& aRect)
    {
	iTApp1Text = iEikonEnv->AllocReadResourceL(R_EXAMPLE_TEXT_TITLE);
	
	CreateWindowL();
	iEng = CTApp1Eng::NewL(iEikonEnv->WsSession(), 
                      *(CCoeEnv::Static()->ScreenDevice()), 
					  Window());
	SetRect(aRect);
	ActivateL();
	}
	
/**
 *	
 *	Draws the text on the screen
 *	
 */  
void CTApp1View::Draw(const TRect& /*aRect*/) const
	{
	if(iEng->Drawing())
		{
        return;
		}
	
	iEng->StartDrawing();
	}
