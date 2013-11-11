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

#ifndef __T_APP1_VIEW_H__
#define __T_APP1_VIEW_H__

class CTApp1Eng;

/**
 *	CTApp1AppView for creating a window and to draw the text
 *	
 */  
class CTApp1View : public CCoeControl
    {
public:
	// creates a CTApp1AppView object
	static CTApp1View* NewL(const TRect& aRect);
	~CTApp1View();
private:
	CTApp1View();
	void ConstructL(const TRect& aRect);
	//Draws the text on the screen	           
	void Draw(const TRect& /*aRect*/) const;

private:
	//contains the text needs to be drawn
	HBufC*  iTApp1Text;
	
	CTApp1Eng * iEng;
    };

#endif // __T_APP1_VIEW_H__
