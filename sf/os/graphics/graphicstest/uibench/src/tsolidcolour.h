// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TSOLIDCOLOUR_H__
#define __TSOLIDCOLOUR_H__

#include "te_graphicsperformanceSuiteStepBase.h"

class CTSolidColour : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTSolidColour();

private:
	void DrawSolidColourL();
	
	// From CTestStep
	virtual TVerdict doTestStepL();
	};

_LIT(KTSolidColourName,"tsolidcolour");

#endif
