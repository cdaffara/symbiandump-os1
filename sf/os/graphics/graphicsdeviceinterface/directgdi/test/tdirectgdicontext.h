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
 @internalComponent 
 @test
*/

#ifndef TDIRECTGDICONTEXT_H
#define TDIRECTGDICONTEXT_H


#include "tdirectgdi_test_step_base.h"

_LIT(KSamePixelType, "_SamePixelType");
_LIT(KDifferentPixelType, "_DiffPixelType");
_LIT(KTarget1, "_Tgt1");
_LIT(KTarget2, "_Tgt2");
_LIT(KContext1, "_Ctx1");
_LIT(KContext2, "_Ctx2");

/**
This class contains all the test cases related to DirectGdi Context
*/
class CTDirectGdiContext : public CTDirectGdiStepBase
	{
public:
	CTDirectGdiContext();
	~CTDirectGdiContext();
	
private:
	void OneContextOneTarget();
	void OneContextTwoTargets();
	void TwoContextsOneTarget();
	void TwoContextsTwoTargets_WithoutSharingL();
	void TwoContextsTwoTargets_WithSharingL();
	void OneContextOneTarget_TwiceActivateL();
	void ExternalizeInternalizeTestL();
	void TestCopySettings();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	// from CTDirectGdiStepBase
	void RunTestsL();

private:	
	TInt iCaseNum;
	CFont* iFont;
	};		
	
_LIT(KTDirectGdiContextStep,"TDirectGDIContext");

#endif /*TDIRECTGDICONTEXT_H*/
