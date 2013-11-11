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

#ifndef __T_FPS_APP_VIEW_H__
#define __T_FPS_APP_VIEW_H__

#include <coecntrl.h>
#include "t_fpsappeng.h"

/**
 *	CFpsAppView for creating a window
 *
 */
class CFpsAppView : public CCoeControl
    {
public:
	// creates a CPseudoAppView object
	static CFpsAppView* NewL(const TRect& aRect);
	~CFpsAppView();
	void Rotate();

private:
	CFpsAppView();
	void ConstructL(const TRect& aRect);
	void Draw(const TRect& /*aRect*/) const;

private:
	CFpsAppEng* iEng;
    };

#endif //__T_FPS_APP_VIEW_H__
