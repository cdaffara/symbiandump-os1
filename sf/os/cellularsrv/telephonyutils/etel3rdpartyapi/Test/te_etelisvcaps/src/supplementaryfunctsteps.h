// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of all test objects required to verify capability check on 
// ETel3rdParty supplementary functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/
 
#ifndef __SUPPLEMENTARYFUNCTSTEPS_H__
#define __SUPPLEMENTARYFUNCTSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_etelisvcapsstepbase.h"

//
class CGetCallForwardingStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCallForwardingStatusTestStep();

private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallForwardingSupplServicesV1 iCallForwarding;
	CTelephony::TCallForwardingSupplServicesV1Pckg iCallForwardingPckg;
	};

_LIT(KCapsTestGetCallForwardingStr,"GetCallForwardingTestStep");

//
class CGetCallBarringStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCallBarringStatusTestStep();

private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallBarringSupplServicesV1 iCallBarring;
	CTelephony::TCallBarringSupplServicesV1Pckg iCallBarringPckg;
	};

_LIT(KCapsTestGetCallBarringStr,"GetCallBarringTestStep");

//
class CGetCallWaitingStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCallWaitingStatusTestStep();

private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallWaitingSupplServicesV1 iCallWaiting;
	CTelephony::TCallWaitingSupplServicesV1Pckg iCallWaitingPckg;
	};

_LIT(KCapsTestGetCallWaitingStr,"GetCallWaitingTestStep");

//
class CGetIdentityServiceTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetIdentityServiceTestStep();

private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TIdentityServiceV1 iIdentityService;
	CTelephony::TIdentityServiceV1Pckg iIdentityServicePckg;
	};

_LIT(KCapsTestGetIdentityServiceStr,"GetIdentityServiceTestStep");

//
#endif // __SUPPLEMENTARYFUNCTSTEPS_H__
