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

#ifndef TDIRECTGDITARGET_H_
#define TDIRECTGDITARGET_H_

#include "tdirectgdi_test_step_base.h"

/**
This class contains all the test cases related to DirectGdi Contexts and targets, without using the test framework,
to improve the coverage results for OOM tests.
*/
class CTDirectGdiContextTarget : public CTDirectGdiStepBase
	{
public:
	CTDirectGdiContextTarget();
	~CTDirectGdiContextTarget();
	
private:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	void TestSwitchingTargetsL();	
	void TestRSgImageMetadataIntegrityL();
	void TestCreateCloseSourceTargetInvalidParametersL();

	// Helper functions
	TBool CompareInfos(const TSgImageInfo& info1, const TSgImageInfo& info2);
	
	// from CTDirectGdiStepBase
	void RunTestsL();
	};		
	
_LIT(KTDirectGdiContextTargetStep,"TDirectGdiContextTarget");

#endif /*TDIRECTGDITARGET_H_*/
