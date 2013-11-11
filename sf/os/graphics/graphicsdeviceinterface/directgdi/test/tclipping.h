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

#ifndef TCLIPPING_H_
#define TCLIPPING_H_

#include "tdirectgdi_test_step_base.h"
#include <bitdev.h>

class CTClipping : public CTDirectGdiStepBase
	{
public:
	CTClipping();
	~CTClipping();
	
private:
 	void TestClippingBasicL();
 	void TestClippingErrors();
 	void TestClippingWithOriginL();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiClippingStep,"TDirectGDIClipping");

#endif /*TCLIPPING_H_*/
