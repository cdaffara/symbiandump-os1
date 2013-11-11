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
// ETel3rdParty phone functionality part.
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __PHONEFUNCTSTEPS_H__
#define __PHONEFUNCTSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_etelisvcapsstepbase.h"

//
class CGetPhoneIdTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetPhoneIdTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TPhoneIdV1 iPhoneId;
	CTelephony::TPhoneIdV1Pckg iPhoneIdPckg;
	};

_LIT(KCapsTestGetPhoneIdStr,"GetPhoneIdTestStep");


//
class CGetSubscriberIdTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetSubscriberIdTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TSubscriberIdV1 iSubscriberId;
	CTelephony::TSubscriberIdV1Pckg iSubscriberIdPckg;
	};

_LIT(KCapsTestGetSubscriberIdStr,"GetSubscriberIdTestStep");


//
class CGetFlightModeTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetFlightModeTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TFlightModeV1 iFlightId;
	CTelephony::TFlightModeV1Pckg iFlightIdPckg;
	};

_LIT(KCapsTestGetFlightModeStr,"GetFlightModeTestStep");

//
class CGetIndicatorTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetIndicatorTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TIndicatorV1 iIndicator;
	CTelephony::TIndicatorV1Pckg iIndicatorPckg;
	};

_LIT(KCapsTestGetIndicatorStr,"GetIndicatorTestStep");

//
class CGetBatteryInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetBatteryInfoTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TBatteryInfoV1 iBatteryInfo;
	CTelephony::TBatteryInfoV1Pckg iBatteryInfoPckg;
	};

_LIT(KCapsTestGetBatteryInfoStr,"GetBatteryInfoTestStep");


//
class CGetSignalStrengthTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetSignalStrengthTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TSignalStrengthV1 iSignalStrength;
	CTelephony::TSignalStrengthV1Pckg iSignalStrengthPckg;
	};

_LIT(KCapsTestGetSignalStrengthStr,"GetSignalStrengthTestStep");

//
class CGetLockInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CGetLockInfoTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TIccLock iLock;
	CTelephony::TIccLockInfoV1 iLockInfo;
	CTelephony::TIccLockInfoV1Pckg iLockInfoPckg;
	};

_LIT(KCapsTestGetLockInfoStr,"GetLockInfoTestStep");

//
class CSendDTMFTonesTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CSendDTMFTonesTestStep();


private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();
	};

_LIT(KCapsTestSendDTMFTonesStr,"SendDTMFTonesTestStep");
//

#endif // __PHONEFUNCTSTEPS_H__
