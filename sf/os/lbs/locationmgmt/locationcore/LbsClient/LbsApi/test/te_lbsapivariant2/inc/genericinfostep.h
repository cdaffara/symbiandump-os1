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
 @file GenericInfoStep.h
 @internalTechnology
*/
#if (!defined __GENERICINFO_STEP_H__)
#define __GENERICINFO_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsapivariant2suitestepbase.h"

class CGenericInfoStep : public CTe_LbsApiVariant2SuiteStepBase
	{
public:
	CGenericInfoStep();
	~CGenericInfoStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void StartL (TInt aIndex);
	// Generic Info
	void HPositionGenericInfo_ConstructionL();
	void HPositionGenericInfo_Construction2L();
	void HPositionGenericInfo_RequestingL();
	void HPositionGenericInfo_Requesting2L();
	void HPositionGenericInfo_GetL();
	void HPositionGenericInfo_GoodL();
	void HPositionGenericInfo_MismatchedL();
	void HPositionGenericInfo_HBufCL();
	void HPositionGenericInfo_BufferL();
	void HPositionGenericInfo_FieldsL();
	void HPositionGenericInfo_OverflowL();
	void HPositionGenericInfo_SetL();
	};

_LIT(KGenericInfoStep,"GenericInfoStep");

#endif
