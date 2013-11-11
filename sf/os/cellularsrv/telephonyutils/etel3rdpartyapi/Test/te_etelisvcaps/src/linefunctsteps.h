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
// ETel3rdParty line functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __LINEFUNCTSTEPS_H__
#define __LINEFUNCTSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_etelisvcapsstepbase.h"


//
class CGetLineStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetLineStatusTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallStatusV1 iCallStatus;
	CTelephony::TCallStatusV1Pckg iCallStatusPckg;
	};

_LIT(KCapsTestGetLineStatusStr,"GetLineStatusTestStep");

//
class CGetCallInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetCallInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallInfoV1 iCallInfo;
	CTelephony::TCallInfoV1Pckg iCallInfoPckg;
	CTelephony::TRemotePartyInfoV1 iRCallInfo;
	CTelephony::TRemotePartyInfoV1Pckg iRCallInfoPckg;
	CTelephony::TCallSelectionV1 iCallSelection;
	CTelephony::TCallSelectionV1Pckg iCallSelectionPckg;
	};

_LIT(KCapsTestGetCallInfoStr,"GetCallInfoTestStep");

//

#endif // __LINEFUNCTSTEPS_H__
