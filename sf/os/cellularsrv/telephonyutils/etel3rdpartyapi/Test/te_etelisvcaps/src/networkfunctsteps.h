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
// Defintion of all test objects required to verify capability check on 
// ETel3rdParty network functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __NETWORKFUNCTSTEPS_H__
#define __NETWORKFUNCTSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_etelisvcapsstepbase.h"


//
class CGetNetworkRegistrationStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetNetworkRegistrationStatusTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TNetworkRegistrationV1 iNetworkRegistration;
	CTelephony::TNetworkRegistrationV1Pckg iNetworkRegistrationPckg;
	};

_LIT(KCapsTestGetNetworkRegistrationStr,"GetNetworkRegistrationTestStep");

//
class CGetCurrentNetworkInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCurrentNetworkInfoTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TNetworkInfoV1 iNetworkInfo;
	CTelephony::TNetworkInfoV1Pckg iNetworkInfoPckg;
	};

_LIT(KCapsTestGetNetworkInfoStr,"GetNetworkInfoTestStep");

//

class CGetCurrentNetworkNameTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCurrentNetworkNameTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TNetworkNameV1 iNetworkName;
	CTelephony::TNetworkNameV1Pckg iNetworkNamePckg;
	};

_LIT(KCapsTestGetNetworkNameStr,"GetNetworkNameTestStep");

//
class CGetOperatorNameTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetOperatorNameTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TOperatorNameV1 iOperatorName;
	CTelephony::TOperatorNameV1Pckg iOperatorNamePckg;
	};

_LIT(KCapsTestGetOperatorNameStr,"GetOperatorNameTestStep");

//
#endif // __NETWORKFUNCTSTEPS_H__
