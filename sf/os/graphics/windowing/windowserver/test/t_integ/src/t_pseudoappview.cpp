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

#include "t_pseudoappeng.h"
#include "t_pseudoappview.h"

/**
 *	Private constructor, does nothing
 *
 */
CPseudoAppView::CPseudoAppView()
	{
	}

/**
 *	Standard 2 phase construction, constructs a View object
 *	@return pointer to the  CPseudoAppView
 *	@param aRect - rectangle coordinate defining control's extent
 */
 CPseudoAppView* CPseudoAppView::NewL(const TRect& aRect)
	{
	CPseudoAppView* self = new(ELeave) CPseudoAppView();
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 *	Destructor
 */
CPseudoAppView::~CPseudoAppView()
	{
	delete iPseudoText;
	iPseudoText = NULL;
	delete iEng;
	iEng = NULL;
	}

/**
 *	constructL method
 *	@param aRect - rectangle coordinate defining control's extent
 *
 */
void CPseudoAppView::ConstructL(const TRect& aRect)
    {
	CreateWindowL();
	SetRect(aRect);

	Window().SetBackgroundColor(KRgbBlack);
	iEng = CTPseudoAppEng::NewL(iEikonEnv->WsSession(), *(CCoeEnv::Static()->ScreenDevice()), Window());

	ActivateL();
	}

/**
 *
 *	Draws the text on the screen
 *
 */
void CPseudoAppView::Draw(const TRect& /*aRect*/) const
	{
	if(iEng->Drawing())
		{
        return;
		}

	TRAPD(err,iEng->StartDrawingL());
	if(err)
		{
		RDebug::Print(_L("StartDrawingL() trapped with error %d\n"), err);
		User::Exit(0);
		}
	}

void CPseudoAppView::RotateL()
	{
	iEng->RotateL();
	}
