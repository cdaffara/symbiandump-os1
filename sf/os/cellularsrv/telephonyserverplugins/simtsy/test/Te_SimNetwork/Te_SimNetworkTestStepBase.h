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

#ifndef __TE_SIMNETWORK_TESTSTEP_BASE__
#define __TE_SIMNETWORK_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <mmretrieve.h>
#include <mmlist.h>
#include "SimTsy.h"

_LIT(KSimtsyName,"SIM");

class CSimNetworkTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void validateNetworkInfoL(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetwork, 
		RMobilePhone::TMobilePhoneRegistrationStatus& aStatus);
	void PrintRegistration(RMobilePhone::TMobilePhoneRegistrationStatus&  aStatus);
	void PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV1&  aNetworkInfo,
		RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea = ETrue) ;
			void PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV2&  aNetworkInfo,
		RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea = ETrue) ;
			void PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV5&  aNetworkInfo,
		RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea = ETrue) ;
	void PrintNetworkStatusISV(RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1&  aNetworkInfo,
		RMobilePhone::TMobilePhoneLocationAreaV1& aArea) ;
	void PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV8&  aNetworkInfo,
		RMobilePhone::TMobilePhoneLocationAreaV1& aArea, TBool aPrintArea = ETrue) ;	

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	TUint32 iCaps;
	TBool iOperatorInfoSupported;
	TRequestStatus iStatus ;

private:
	CActiveScheduler* iTestScheduler;
	TProcessPriority  iOrgProcessPriority;
	TThreadPriority  iOrgThreadPriority;
	};

#endif // __TE_SIMNETWORK_TESTSTEP_BASE__
