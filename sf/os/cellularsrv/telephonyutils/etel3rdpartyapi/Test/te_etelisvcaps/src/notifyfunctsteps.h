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
// ETel3rdParty notifiers. 
// 
//

/**
 @file 
 @internalTechnology
*/
#ifndef __NOTIFYFUNCTSTEPS_H__
#define __NOTIFYFUNCTSTEPS_H__

#include <test/testexecutestepbase.h>
#include "te_etelisvcapsstepbase.h"

//

class CNotifyFlightModeTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyFlightModeTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TFlightModeV1 iFlightMode;
	CTelephony::TFlightModeV1Pckg iFlightModePckg;
	};

_LIT(KCapsTestFlightModeChangeStr, "NotifyFlightModeChange"); 

//

class CNotifyIndicatorTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyIndicatorTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TIndicatorV1 iIndicator;
	CTelephony::TIndicatorV1Pckg iIndicatorPckg;
	};

_LIT(KCapsTestIndicatorChangeStr, "NotifyIndicatorChange"); 


//
class CNotifyBatteryInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyBatteryInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TBatteryInfoV1 iBatteryInfo;
	CTelephony::TBatteryInfoV1Pckg iBatteryInfoPckg;
	};

_LIT(KCapsTestBatteryInfoChange, "NotifyBatteryInfoChange");			

//

class CNotifySignalStrengthTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifySignalStrengthTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TSignalStrengthV1 iSignalStrength;
	CTelephony::TSignalStrengthV1Pckg iSignalStrengthPckg;
	};
	
_LIT(KCapsTestSignalStrengthChange, "NotifySignalStrengthChange");		

//
class CNotifyPin1LockInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyPin1LockInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TIccLockInfoV1 iLockInfo;
	CTelephony::TIccLockInfoV1Pckg iLockInfoPckg;
	};
_LIT(KCapsTestPin1LockInfoChange, "NotifyPin1LockInfoChange"); 			

//

class CNotifyPin2LockInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyPin2LockInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TIccLockInfoV1 iLockInfo;
	CTelephony::TIccLockInfoV1Pckg iLockInfoPckg;
	};
_LIT(KCapsTestPin2LockInfoChange, "NotifyPin2LockInfoChange");		

//

class CNotifyVoiceLineStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyVoiceLineStatusTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallStatusV1 iCallStatus;
	CTelephony::TCallStatusV1Pckg iCallStatusPckg;
	};
_LIT(KCapsTestVoiceLineStatusChange, "NotifyVoiceLineStatusChange"); 		

//

class CNotifyOwnedCall1StatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyOwnedCall1StatusTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallStatusV1 iCallStatus;
	CTelephony::TCallStatusV1Pckg iCallStatusPckg;
	};
_LIT(KCapsTestOwnedCall1StatusChange, "NotifyOwnedCall1StatusChange"); 		

//

class CNotifyOwnedCall2StatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyOwnedCall2StatusTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TCallStatusV1 iCallStatus;
	CTelephony::TCallStatusV1Pckg iCallStatusPckg;
	};
_LIT(KCapsTestOwnedCall2StatusChange, "NotifyOwnedCall2StatusChange"); 		

//

class CNotifyOwnedCall1RemotePartyInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyOwnedCall1RemotePartyInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TRemotePartyInfoV1 iRemoteInfo;
	CTelephony::TRemotePartyInfoV1Pckg iRemoteInfoPckg;
	};
_LIT(KCapsTestOwnedCall1RemotePartyInfoChange, "NotifyOwnedCall1RemotePartyInfoChange");

//

class CNotifyOwnedCall2RemotePartyInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyOwnedCall2RemotePartyInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TRemotePartyInfoV1 iRemoteInfo;
	CTelephony::TRemotePartyInfoV1Pckg iRemoteInfoPckg;
	};
	
_LIT(KCapsTestOwnedCall2RemotePartyInfoChange, "NotifyOwnedCall2RemotePartyInfoChange");
//

class CNotifyNetworkRegistrationStatusTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyNetworkRegistrationStatusTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TNetworkRegistrationV1 iRegistrationInfo;
	CTelephony::TNetworkRegistrationV1Pckg iRegistrationInfoPckg;
	};
	
	
_LIT(KCapsTestNetworkRegistrationStatusChange, "NotifyNetworkRegistrationStatusChange");
//

class CNotifyCurrentNetworkInfoTestStep : public CTe_CapsTestSuiteStepBase
	{
public:
	CNotifyCurrentNetworkInfoTestStep();
	
private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus);
	virtual TInt CancelRequestL();

private:
	CTelephony::TNetworkInfoV2 iNetworkInfo;
	CTelephony::TNetworkInfoV2Pckg iNetworkInfoPckg;
	};
	
_LIT(KCapsTestCurrentNetworkInfoChange, "NotifyCurrentNetworkInfoChange");
//


#endif // __NOTIFYFUNCTSTEPS_H__
