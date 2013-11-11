// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __TE_SIMSMS_TESTSTEP_BASE__
#define __TE_SIMSMS_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include <exterror.h> 
#include "SimTsy.h"

#include "Te_SimSmsConstants.h"

typedef TBuf8<200> TSmsPdu;
typedef TBuf<RMobilePhone::KMaxMobileTelNumberSize> TTelNumberBuf;

_LIT(KSimtsyName,"SIM");

class CSimSmsTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;
	TVerdict  SendSms() ;
	void PrintMessageStoreCaps(TUint32 aCaps) ;

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RMobileSmsMessaging iSms;
	RMobileSmsMessaging::TMobileSmsCapsV1 iSmsCaps ;
	TTelNumberBuf iSCANumber;
	TTelNumberBuf iTelNumber;

private:
	CActiveScheduler* iTestScheduler;
	};

#endif // __TE_SIMSMS_TESTSTEP_BASE__
