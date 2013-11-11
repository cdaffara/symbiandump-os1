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

#ifndef __TE_NETWORK_TESTSTEP_BASE__
#define __TE_NETWORK_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>

_LIT(KMmtsyName,"MM");
_LIT(KPhoneName, "GsmPhone1");

class CNetworkTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds) ;
	void validateNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aNetwork, 
				RMobilePhone::TMobilePhoneRegistrationStatus& aStatus);
	void PrintRegistration(RMobilePhone::TMobilePhoneRegistrationStatus&  aStatus);
	void PrintNetworkStatus(RMobilePhone::TMobilePhoneNetworkInfoV1&  aNetworkInfo, 
				RMobilePhone::TMobilePhoneLocationAreaV1& aArea) ;
	void PrintNetworkMode(RMobilePhone::TMobilePhoneNetworkMode& aNetworkMode) ;
	void PrintSelectionSettings(RMobilePhone::TMobilePhoneNetworkSelectionV1& 
				aSetting) ;

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	TUint32 iCaps;
	TBool iOperatorInfoSupported;
	TRequestStatus iStatus;

private:
	TProcessPriority  iOrgProcessPriority;
	TThreadPriority  iOrgThreadPriority;
	};

#endif // __TE_NETWORK_TESTSTEP_BASE__
