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

#ifndef TDIRECTGDIDRIVER_H_
#define TDIRECTGDIDRIVER_H_

#include "tdirectgdi_test_step_base.h"

/**
This class contains all the test cases related to DirectGdi Driver
*/
class CTDirectGdiDriver : public CTDirectGdiStepBase
	{
public:
	CTDirectGdiDriver();
	~CTDirectGdiDriver();
	
private:
	void TestDriverL();
	TInt TestDriverErrorCodes();
	void TestDriverOOM();
	void TestSetError();
	void TestFlush();
	void TestGetInterfaceL();
	void TestCacheSizesL();
	
	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiDriverStep,"TDirectGDIDriver");

#endif /*TDIRECTGDIDRIVER_H_*/
