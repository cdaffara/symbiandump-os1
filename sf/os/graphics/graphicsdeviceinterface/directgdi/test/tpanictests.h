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

#ifndef TPANICTESTS_H_
#define TPANICTESTS_H_

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

/**
This class contains all the panic tests. It reads the test case number from the ini file. Even when a panic is 
encountered, it does not quit the test framework. It carries on with other test cases.
*/
class CTPanicTests : public CTDirectGdiStepBase
	{
public:
	CTPanicTests();
	~CTPanicTests();
	
private:
	void TestContextNotActivatedL();	
	void TestFontNotSetL();
	void TestInvalidTargetL();
	void TestImageInvalidTargetHandleL();
	void TestImageTargetActivatedTwiceL();
	void TestImageSourceActivatedTwiceL();
	void TestDrawableSourceActivatedTwiceL();
	void TestBrushPatternNotSetL();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiPanicTestsStep,"TDirectGDIPanicTests");
_LIT(KCaseNumber, "CaseNumber");

#endif /*TPANICTESTS_H_*/
