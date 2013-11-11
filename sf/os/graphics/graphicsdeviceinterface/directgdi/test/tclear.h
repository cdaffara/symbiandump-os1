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

#ifndef __TCLEAR_H__
#define __TCLEAR_H__

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

/**
Class for testing clearing.
*/
class CTClear : public CTDirectGdiStepBase
	{
public:
	CTClear();
	~CTClear();
	
private:
	// Main test methods
 	void TestClear();
 	void TestClearRect();
 	void TestClearWithClipping();
 	void TestClearDrawModes();
 	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	
	};		
	
_LIT(KTDirectGdiClearStep,"TDirectGDIClear");

#endif
