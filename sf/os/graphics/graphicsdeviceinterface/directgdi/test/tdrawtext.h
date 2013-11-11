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
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDRAWTEXT_H__
#define __TDRAWTEXT_H__

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

class CTDrawText : public CTDirectGdiStepBase
	{
public:

	CTDrawText();
	~CTDrawText();
	
private:
	void TestBasicFunctionality();
	void TestTextPositioning();
	void TestTextBox();
	void TestTextPositioningVertical(TBool aUp);
	void TestTextBoxVertical();
	void TestPenBrushL();
	void TestAlign();
	void TestJustification();
	void TestShadowUnderStrike();
	void TestInvalidParamsL();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	// from CTDirectGdiStepBase
	void RunTestsL();

private:
	CFont* iFont;
	};		
	
_LIT(KTDirectGdiDrawTextStep,"TDirectGDIDrawText");

#endif
