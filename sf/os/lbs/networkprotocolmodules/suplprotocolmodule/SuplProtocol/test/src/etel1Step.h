/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file etel1Step.h
 @internalTechnology
*/
#if (!defined __ETEL1_STEP_H__)
#define __ETEL1_STEP_H__


#include <test/testexecutestepbase.h>
#include "te_suplprotocolsuitestepbase.h"
#include "suplgatewayinterface.h"

class CEtel1Step : public CTe_suplprotocolSuiteStepBase, public MLbsCallbackTimerObserver
	{
public:
	CEtel1Step();
	~CEtel1Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	// MTestTimerObserver methods
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);
		
private:
	CSuplGatewayInterface* iModule;
	TRequestStatus iStatus;
	CActiveSchedulerWait* iSchedulerWait;
	CLbsCallbackTimer* iTimer;
	};

_LIT(KEtel1Step,"etel1Step");

#endif //__ETEL1_STEP_H__
