/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of CStrtDevLockCheck class.
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CMDDEVICESECURITYCHECK_H__
#define __CMDDEVICESECURITYCHECK_H__

#include <ssm/ssmcustomcommand.h>
#include "ssmsecuritychecknotifier.h"
#include <etel.h> 
#include <etelmm.h>

class CRepository;

NONSHARABLE_CLASS (CCustomCmdDeviceSecurityCheck) : public CActive , public MSsmCustomCommand
	{
public:
	static CCustomCmdDeviceSecurityCheck* NewL();

	// from MSsmcustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();

protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

protected:
	void ConstructL();
	void Cleanup(TInt aError);

private:
	CCustomCmdDeviceSecurityCheck();
    ~CCustomCmdDeviceSecurityCheck();

private:
    //Response to LockPhoneDevice lock status query has been received.
    void LockPhoneDeviceRespReceivedL();

    //Response to LockPhoneToIcc lock status query has been received.
    void LockPhoneToIccRespReceivedL();

    //Response to security code request from user has been received.
    void SecCodeQueryRespReceivedL();

    //Query the security code from the user.
    void QuerySecCodeL();

    void CompleteClientRequest(TInt aReason);

private:
    //Internal state
    enum TDeviceSecurityCheckState
        {
        EDeviceSecurityCheckNone = 0,
        EGetLockPhoneDevice,
        EGetLockPhoneToIcc,
        EQuerySecCode,
        EDeviceSecurityCheckMaxState
        };

private:
	TDeviceSecurityCheckState iState;
	RTelServer iServer;
    RMobilePhone iPhone;
    RMobilePhone::TMobilePhoneLockInfoV1 iLockInfo;
    RMobilePhone::TMobilePhoneLockInfoV1Pckg iLockInfoPckg;
    HBufC* iTsyModuleName;
	TRequestStatus* iExecuteRequest;
	CSsmSecurityCheckNotifier* iSsmSecurityCheckNotifier;
	CRepository* iStartupRepository;

#ifdef TEST_CUSTCMD_MACRO
	friend class CCustomCmdTestDeviceSecurityCheck;
#endif
	};
#endif // __CMDDEVICESECURITYCHECK_H__
