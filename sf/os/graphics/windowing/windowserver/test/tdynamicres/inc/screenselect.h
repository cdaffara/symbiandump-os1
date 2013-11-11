/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* @file
* 
*
*/



#ifndef __SCREEN_SELECT__
#define __SCREEN_SELECT__

#include <test/tefunit.h>

/**
Test step to set a static screen number selection, which can be used to run the
same test code on an alternate screen by simply adding this step and repeating
the test.

Test steps which support this must use the static CSelectScreen::Number()
function to get the currently selected screen number, and pass this to the
Construct() call for a CWsScreenDevice.
*/

class CScreenSelect : public CTestStep
	{
public:
	CScreenSelect(TInt aScreenNumber, const TDesC& aStepName);
	~CScreenSelect();

	static TInt Number();

	virtual TVerdict doTestStepL();

private:
	TInt iScreenNumber;
	};

#endif // __SCREEN_SELECT__

