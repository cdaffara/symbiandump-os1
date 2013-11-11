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
 @file RequestorClassesStep.h
 @internalTechnology
*/
#if (!defined __REQUESTORCLASSES_STEP_H__)
#define __REQUESTORCLASSES_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsapivariant2suitestepbase.h"

class CRequestorClassesStep : public CTe_LbsApiVariant2SuiteStepBase
	{
public:
	CRequestorClassesStep();
	~CRequestorClassesStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
	void StartL (TInt aIndex);
	// Requestor Classes
	void RRequestorStack_StreamingL();
	void RRequestorStack_Streaming2L();
	void CRequestor_ConstructionL();
	void CRequestor_SetL();
	void CRequestor_Construction2L();
	void CRequestor_StreamingL();
	void CRequestor_Streaming2L();
	};

_LIT(KRequestorClassesStep,"RequestorClassesStep");

#endif
