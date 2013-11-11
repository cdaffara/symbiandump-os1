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
 @file mtlr25Step.h
 @internalTechnology
*/
#if (!defined __MTLR25_STEP_H__)
#define __MTLR25_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_suplprotocolsuitestepbase.h"
#include "suplgatewayinterface.h"
#include "suplinitgenerator.h"
#include <lbs/lbssuplpush.h>

class Cmtlr25Step : public CTe_suplprotocolSuiteStepBase, 
                   public MLbsSuplPushObserver
	{
public:
	Cmtlr25Step();
	~Cmtlr25Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// from MLbsSuplPushObserver
	void OnSuplInitComplete(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TInt aError, TInt aReserved);

private:
	CSuplGatewayInterface* iModule;
	CSuplInitGenerator* iSuplInitGenerator;
	CLbsSuplPush* iSuplPush;

	TInt iSmsSendError;
	TBufC8<1024> iReceiveBuffer;

	};

_LIT(Kmtlr25Step,"mtlr25Step");

#endif
