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

#include <coecntrl.h>
#include <eikenv.h>

#include "t_fpsappeng.h"
#include "t_fpsappview.h"

/**
 *	Private constructor, does nothing
 *
 */
CFpsAppView::CFpsAppView()
	{
	}

/**
 *	Standard 2 phase construction, constructs a View object
 *	@return pointer to the  CPseudoAppView
 *	@param aRect - rectangle coordinate defining control's extent
 */
 CFpsAppView* CFpsAppView::NewL(const TRect& aRect)
	{
	CFpsAppView* self = new(ELeave) CFpsAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 *	Destructor
 */
CFpsAppView::~CFpsAppView()
	{
	delete iEng;
	}

/**
 *	constructL method
 *	@param aRect - rectangle coordinate defining control's extent
 *
 */
void CFpsAppView::ConstructL(const TRect& aRect)
    {
	CreateWindowL();
	SetRect(aRect);	
	iEng = CFpsAppEng::NewL(iEikonEnv->WsSession(), *(CCoeEnv::Static()->ScreenDevice()), Window());
	ActivateL();
	}

void CFpsAppView::Draw(const TRect& /*aRect*/) const
	{	
	iEng->StartDrawing();	
	}

