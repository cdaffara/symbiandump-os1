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
* Declaration of CStrtSecurityCheckTask class
*
*/




/**
 @file
 @internalComponent
 @released
*/

#ifndef __CMDSIMSECURITYCHECK_H__
#define __CMDSIMSECURITYCHECK_H__

#include <ssm/ssmcustomcommand.h>
#include "ssmpanic.h"
#include "ssmsecuritychecknotifier.h"
#include "ssmuiproviderdll.h"
#include <ssm/ssmsuscli.h>
#include "securitynoteobserver.h"

#include "strtsecuritynotetype.h"
#include "strtsecuritystatus.h"
#include "startupadaptationcommands.h"
#include "starterprivatecrkeys.h"

#include <e32std.h>
#include <e32base.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/ssmadaptationcli.h>

typedef StartupAdaptation::TSecurityStateInfo TSecurityStateInfo;
typedef StartupAdaptation::TSecurityState TSecurityState;

NONSHARABLE_CLASS(CSsmSimStatusPSObserver) : public CActive
    {
public:
	static CSsmSimStatusPSObserver* NewL(TUid aCategory, TUint aKey);
    ~CSsmSimStatusPSObserver();

    void StartObserving(TRequestStatus& aStatus);
    TInt GetValue(TInt& aValue);
	
protected:
	// from CActive
	void DoCancel();
    void RunL();

private:
	CSsmSimStatusPSObserver(TUid aCategory, TUint aKey);
    void Complete(const TInt aErrorCode);
    void ConstructL();

private:
    RProperty iProperty;
    TUid iCategory;
    TUint iKey;
    TRequestStatus* iClientStatus;
    };

NONSHARABLE_CLASS (CCustomCmdSimSecurityCheck) : public CActive , public MSsmCustomCommand
	{
public:
	static CCustomCmdSimSecurityCheck* NewL();

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
	CCustomCmdSimSecurityCheck();
	~CCustomCmdSimSecurityCheck();

  	void StartStateChange(const TSecurityState aState, const TStrtSecurityNoteType aNoteType = ESecNoteNone);
	void StateChangeL( const TInt aResultCode );
	void CompleteClientRequest(TInt aReason);
	void SecurityCheckL();
	void DoSecurityCheck();

	void HandleSIMPresent();
	void HandleSIMInvalid();
    void HandleSIMRejected();
    void HandleSIMBlocked();
    void HandlePINRequired();
    void HandleSIMLessOfflineSupported();
    void HandleSIMLock();
    void HandlePINAsked();
    void HandlePUKAsked();

    void SetSecurityCheckOK();
    void StartSecurityNoteObserverL();

	//Sim related
    void SimRemoved();
    void SimNotSupported();
    void SimReadable();
    void SimUsable();
	void DoSimRemoved();
	void DoSimNotSupported();
	void DoSimReadable();
	void DoSimUsable();
    void DoObserveSimStatusPS();

	void DoExtendedSimOperations();

    TBool IsSimSupported();

    //Security Status
    void SetSecurityStatus(const TStrtSecurityStatus& aSecurityStatus);
    TStrtSecurityStatus SecurityStatus() const;

private:
	enum TSimSecuritySubState
		{
		ESecuritySubStateNone = 0,
		ESecuritySubStateSimRemoved,
		ESecuritySubStateSimNotSupported,
		ESecuritySubStateSimReadable,
		ESecuritySubStateSimUsable,
		ESecuritySubStateSimStatusPSObserver
		};

private:
    TSecurityState iState;
	RSsmMiscAdaptation iSsmMiscAdaptation;
    TSecurityStateInfo iSecurityStateInfo;
	TStrtSecurityNoteType iNoteType;
	TBool iStateChangePending;
	CSsmSecurityCheckNotifier* iSsmSecurityCheckNotifier;
	TUid iStartupPSUid;
	TPckg<TSecurityStateInfo>* iSecurityStateInfoResult;
	CSsmUiSpecific* iSsmUiSpecific;
	RSsmStateManager iSsmStateManager;
	RSsmSusCli iSsmSusCli;
	CSecurityNoteObserver* iSecurityNoteObserver;
	TSimSecuritySubState iSubState;
	TRequestStatus* iExecuteRequest;
	CSsmSimStatusPSObserver* iSimStatusPSObserver;
#ifdef TEST_CUSTCMD_MACRO
	friend class CCustomCmdTestSimSecurityCheck;
#endif

	};
#endif // __CMDSIMSECURITYCHECK_H__
