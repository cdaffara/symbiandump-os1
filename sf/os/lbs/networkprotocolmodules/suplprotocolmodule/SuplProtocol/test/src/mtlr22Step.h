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
 @file mtlr22Step.h
 @internalTechnology
*/
#if (!defined __MTLR22_STEP_H__)
#define __MTLR22_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_suplprotocolsuitestepbase.h"
#include "suplgatewayinterface.h"
#include "suplinitgenerator.h"
#include <lbs/lbssuplpush.h>

class Cmtlr22Step : public CTe_suplprotocolSuiteStepBase, 
                   public MLbsSuplPushObserver
	{
public:
	Cmtlr22Step();
	~Cmtlr22Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	// from MLbsSuplPushObserver
	void OnSuplInitComplete(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TInt aError, TInt aReserved);

private:
	CSuplGatewayInterface* iModule;
//	CTe_LbsSuplSmsTriggerSender* iSmsSender;
	CSuplInitGenerator* iSuplInitGenerator;
	CLbsSuplPush* iSuplPush;

	TInt iSmsSendError;
	TBufC8<1024> iReceiveBuffer;

	};

_LIT(Kmtlr22Step,"mtlr22Step");

#endif
