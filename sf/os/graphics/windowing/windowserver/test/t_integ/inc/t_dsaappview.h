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

#ifndef __T_DSA_APP_VIEW_H__
#define __T_DSA_APP_VIEW_H__

#include <coecntrl.h>

class CTDsaAppEng;

/**
 *	CDsaAppView for creating a window and to draw the text
 *	
 */  
class CDsaAppView : public CCoeControl
    {
public:
	// creates a CDsaAppView object
	static CDsaAppView* NewL(const TRect& aRect);
	~CDsaAppView();
private:
	CDsaAppView();
	void ConstructL(const TRect& aRect);
	//Draws the text on the screen	           
	void Draw(const TRect& /*aRect*/) const;

private:
	//contains the text needs to be drawn
	HBufC*  iDsaText;
	CTDsaAppEng* iEng;
    };

#endif // __T_DSA_APP_VIEW_H__
