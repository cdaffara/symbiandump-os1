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
 @file NoServerStep.h
 @internalTechnology
*/
#if (!defined __NOSERVER_STEP_H__)
#define __NOSERVER_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsapivariant2suitestepbase.h"

class CNoServerStep : public CTe_LbsApiVariant2SuiteStepBase
	{
public:
	CNoServerStep();
	~CNoServerStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
private:
	void StartL(TInt aIndex);
	void CheckThereIsNoServerL();
	// No Server
	void RPositionServer_NoConnectionL();
	void RPositionServer_NoL();
	void RPositioner_NoL();
	};

_LIT(KNoServerStep,"NoServerStep");

#endif
