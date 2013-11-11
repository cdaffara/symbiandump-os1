/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file ModuleClassesStep.h
 @internalTechnology
*/
#if (!defined __MODULECLASSES_STEP_H__)
#define __MODULECLASSES_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsapivariant2suitestepbase.h"

class CModuleClassesStep : public CTe_LbsApiVariant2SuiteStepBase
	{
public:
	CModuleClassesStep();
	~CModuleClassesStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void StartL(TInt aIndex);
	// Module Classes
	void TPositionModuleInfo_ConstructionL();
	void TPositionModuleInfo_GettersL();
	void TPositionModuleInfo_InvalidL();
	void TPositionQuality_ConstructionL();
	void TPositionQuality_SettersL();
	void TPositionQuality_InvalidL();
	void TPositionQuality_ComparisonL();
	void ModuleClasses_ConstructionL();
	void ModuleClasses_SettersL();
	void TPositionUpdateOptions_InvalidL();
	};

_LIT(KModuleClassesStep,"ModuleClassesStep");

#endif
