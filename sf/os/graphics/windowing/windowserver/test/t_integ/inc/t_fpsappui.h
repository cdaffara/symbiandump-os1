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

#ifndef __T_FPSAPPUI_H__
#define __T_FPSAPPUI_H__

#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <e32base.h>

#include "t_fpsappview.h"

class CFpsAppUi : public CEikAppUi
    {
public:
    void ConstructL();
	~CFpsAppUi();
	void Draw(const TRect& aRect);

private:
    // Inherirted from class CEikAppUi for handling menu events
	void HandleCommandL(TInt aCommand);

private:
	CFpsAppView* iAppView;
};

#endif //__T_FPSAPPUI_H__


