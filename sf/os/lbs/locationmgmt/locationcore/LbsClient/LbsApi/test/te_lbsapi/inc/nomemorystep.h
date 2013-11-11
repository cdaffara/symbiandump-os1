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
 @file NoMemoryStep.h
 @internalTechnology
*/
#if (!defined __NOMEMORY_STEP_H__)
#define __NOMEMORY_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsapisuitestepbase.h"

class CNoMemoryStep : public CTe_LbsApiSuiteStepBase
	{
public:
	CNoMemoryStep();
	~CNoMemoryStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
	void StartL(TInt aIndex);
	void OOM_RPositionServer_Open_NoMemoryL();
	void OOM_RPositioner_OpenL();
	void OOM_RPositioner_SetRequestorL();
	void OOM_CRequestor_ConstructionL();
	void OOM_CRequestor_SetRequestorL();
	void OOM_CRequestor_StreamingL();
	void OOM_RRequestorStack_StreamingL();
	void OOM_HPositionGenericInfo_ConstructionL();
	};

_LIT(KNoMemoryStep,"NoMemoryStep");

#endif
