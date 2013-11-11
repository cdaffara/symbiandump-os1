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

#ifndef __T_PSEUDO_APP_VIEW_H__
#define __T_PSEUDO_APP_VIEW_H__

#include <coecntrl.h>

#include "t_pseudoappeng.h"

/**
 *	CPseudoAppView for creating a window and to draw the text
 *
 */
class CPseudoAppView : public CCoeControl
    {
public:
	// creates a CPseudoAppView object
	static CPseudoAppView* NewL(const TRect& aRect);
	~CPseudoAppView();
	void RotateL();

private:
	CPseudoAppView();
	void ConstructL(const TRect& aRect);
	//Draws the text on the screen
	void Draw(const TRect& /*aRect*/) const;

private:
	//contains the text needs to be drawn
	HBufC*  iPseudoText;
	CTPseudoAppEng* iEng;
    };

#endif //__T_PSEUDO_APP_VIEW_H__
