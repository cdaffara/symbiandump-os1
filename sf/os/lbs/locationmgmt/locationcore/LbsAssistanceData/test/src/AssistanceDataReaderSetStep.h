/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file AssistanceDataReaderSetStep.h
*/
#if (!defined ASSISTDATAREADERSET_STEP_H)
#define ASSISTDATAREADERSET_STEP_H
#include <test/testexecutestepbase.h>
#include "Te_LbsAssistanceDataSuiteStepBase.h"
#include <lbs/lbsassistancedatareaderset.h>

class CAssistanceDataReaderSetStep : public CTe_LbsAssistanceDataSuiteStepBase
	{
public:
	CAssistanceDataReaderSetStep(CTe_LbsAssistanceDataSuite& aOwningServer);
	~CAssistanceDataReaderSetStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KAssistanceDataReaderSetStep,"AssistanceDataReaderSetStep");

#endif
