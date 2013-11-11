/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DB_STEP_H__
#define __DB_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbslocationmonitorsuitestepbase.h"
#include <Lbs.h>

class CDBStep : public CTe_LbsLocationMonitorSuiteStepBase
	{
public:
	CDBStep();
	~CDBStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	TPosition iDummyPosition;
	TPosition iDummyPosition2;
	};

_LIT(KDBStep,"DBStep");

#endif
