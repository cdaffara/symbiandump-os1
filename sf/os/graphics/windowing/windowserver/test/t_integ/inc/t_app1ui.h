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

#ifndef __T_APP1_UI_H__
#define __T_APP1_UI_H__

#include <eikappui.h>
   
/**
 *	CTApp1AppUi handles the system events and menu events
 *	
 */  
class CTApp1Ui : public CEikAppUi
    {
public:
    void ConstructL();
	~CTApp1Ui();

private:
    // Inherirted from class CEikAppUi for handling menu events
	void HandleCommandL(TInt aCommand);
	
	// From CCoeAppUi to handle system events
	void HandleSystemEventL(const TWsEvent& aEvent);
	
	void CalculateAbsL(TInt& aValue, const TInt& aFactor) const;

private:
	CCoeControl* iAppView;
	};

#endif // __T_APP1_UI_H__

