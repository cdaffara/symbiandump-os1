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
#include "t_dsaappview.h"
#include "t_dsaappeng.h"

/**
 *	Private constructor, does nothing	
 *
 */  
CDsaAppView::CDsaAppView()
	{
	}
	
/**
 *	Standard 2 phase construction, constructs a View object
 *	@return pointer to the  CDsaAppView
 *	@param aRect - rectangle coordinate defining control's extent
 */  
 CDsaAppView* CDsaAppView::NewL(const TRect& aRect)
	{
	CDsaAppView* self = new(ELeave) CDsaAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}
	
/**
 *	
 *	Destructor
 */  
CDsaAppView::~CDsaAppView()
	{
	delete iDsaText;
	iDsaText = NULL;
	delete iEng;
	iEng = NULL;
	}
	
/**
 *	constructL method
 *	@param aRect - rectangle coordinate defining control's extent
 *	
 */  
void CDsaAppView::ConstructL(const TRect& aRect)
    {
	CreateWindowL();
	SetRect(aRect);
	
	// Set up our engine
	iEng = CTDsaAppEng::NewL(iEikonEnv->WsSession(), 
                             *(CCoeEnv::Static()->ScreenDevice()), 
					         Window());
					           
	ActivateL();
	}
	
/**
 *	
 *	Draws the text on the screen
 *	
 */  
 
void CDsaAppView::Draw(const TRect& /*aRect*/) const
	{
	RDebug::Print(_L("dsa draw"));
	
	if(iEng->Drawing())
		{
        return;
		}
	
	iEng->StartDrawing();
	}
