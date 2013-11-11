// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CStrtDevLockCheck class.
//




/**
 @file
 @internalComponent
 @released
*/

#include "ssmsecuritychecknotifier.h"
#include "cmddevicesecuritycheck.h"
#include "ssmuiproviderdll.h"
#include "ssmrefcustomcmdcommon.h"
#include "ssmdebug.h"
#include "starterdomaincrkeys.h"
#include "startupdomainpskeys.h"

#include <centralrepository.h>


CCustomCmdDeviceSecurityCheck* CCustomCmdDeviceSecurityCheck::NewL()
	{
	CCustomCmdDeviceSecurityCheck* self = new (ELeave) CCustomCmdDeviceSecurityCheck();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCustomCmdDeviceSecurityCheck::ConstructL()
	{
    TInt errorCode = iServer.Connect();

	if(KErrNone != errorCode)
		{
		DEBUGPRINT2A("Failed to connect to TelServer with error %d", errorCode);
		User::Leave(errorCode);
		}

    iTsyModuleName = CSsmUiSpecific::GetTsyModuleNameL();
    errorCode = iServer.LoadPhoneModule(*iTsyModuleName);

    if (KErrNone != errorCode && KErrAlreadyExists != errorCode)
        {
        // KErrAlreadyExists may be returned if some other has already loaded
        // the TSY module.
        DEBUGPRINT2A("Failed to load phone module with error %d", errorCode);
        User::Leave(errorCode);
        }

    HBufC* tsyPhoneName = CSsmUiSpecific::PhoneTsyNameL();
    errorCode = iPhone.Open(iServer, *tsyPhoneName);
    delete tsyPhoneName;

    if(KErrNone != errorCode)
    	{
    	DEBUGPRINT2A("Failed to open phone subsession with error %d", errorCode);
    	User::Leave(errorCode);	
    	}

	iStartupRepository = CRepository::NewL(KCRUidStartup);

	//Add active object to active scheduler
	CActiveScheduler::Add(this);
	}

CCustomCmdDeviceSecurityCheck::CCustomCmdDeviceSecurityCheck()
								: CActive(EPriorityStandard),
								iState(EGetLockPhoneDevice),
								iLockInfoPckg(iLockInfo)
	{
	}

CCustomCmdDeviceSecurityCheck::~CCustomCmdDeviceSecurityCheck()
	{
	ExecuteCancel();
    delete iSsmSecurityCheckNotifier;

    iPhone.Close();
    if (iServer.Handle())
        {
        iServer.UnloadPhoneModule(*iTsyModuleName);
        iServer.Close();
        }
    delete iTsyModuleName;
	delete iStartupRepository;
	}

TInt CCustomCmdDeviceSecurityCheck::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	return KErrNone;
	}

void CCustomCmdDeviceSecurityCheck::Close()
	{
	}

void CCustomCmdDeviceSecurityCheck::Release()
	{
	delete this;
	}

void CCustomCmdDeviceSecurityCheck::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Set the user request to pending
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	
	TInt errorCode = KErrNone;
#ifdef __WINS__
#ifdef TEST_CUSTCMD_MACRO
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
#else
    // The device lock status can not be read in the emulator.
    // Just pretend the lock is not enabled.
	errorCode = RProperty::Set(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, EStarterSecurityPhaseSimOk);
    CompleteClientRequest(errorCode);
#endif //TEST_CUSTCMD_MACRO

#else

	TInt val(EStartupDevLockNotSucess);

	errorCode = iStartupRepository->Get(KStartupDevLockStatus, val);
	DEBUGPRINT3A("Getting KStartupDevLockStatus : %d completed with %d", val, errorCode);

	TInt startupReason = 0;
	if (EStartupDevLockNotSucess != val)
		{
		errorCode = iStartupRepository->Set(KStartupDevLockStatus, EStartupDevLockNotSucess);
		DEBUGPRINT2A("Setting KStartupDevLockStatus to EStartupDevLockNotSucess completed with %d", errorCode);
		}

	errorCode = RProperty::Get(CSsmUiSpecific::StartupPSUid(), KPSStartupReason, startupReason);
	DEBUGPRINT3A("Getting KPSStartupReason : %d completed with : %d", startupReason, errorCode);
	
	if (CSsmUiSpecific::IsNormalBoot() || CSsmUiSpecific::IsSimChangedReset() || (EStartupDevLockNotSucess ==  val && EUnknownReset == startupReason))
        {
        iPhone.GetLockInfo( iStatus, RMobilePhone::ELockPhoneDevice, iLockInfoPckg );
        SetActive();
        }
    else
        {
		errorCode = RProperty::Set(
        		CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, EStarterSecurityPhaseSecOk );
        CompleteClientRequest( errorCode );
        }
#endif //__WINS__
	}

void CCustomCmdDeviceSecurityCheck::ExecuteCancel()
	{
	//Call cancel method of CActive	
	Cancel();
	}

void CCustomCmdDeviceSecurityCheck::RunL()
	{
    switch (iState)
        {
        case EGetLockPhoneDevice:
            LockPhoneDeviceRespReceivedL();
            break;
        case EGetLockPhoneToIcc:
            LockPhoneToIccRespReceivedL();
            break;
        case EQuerySecCode:
            SecCodeQueryRespReceivedL();
            break;
        default:
        	__ASSERT_ALWAYS((iState <= EDeviceSecurityCheckNone) || (iState >= EDeviceSecurityCheckMaxState),
        					PanicNow(KPanicCustomCmdDeviceSecurityCheck, EInvalidDeviceSecurityState));
            break;
        }
	}

void CCustomCmdDeviceSecurityCheck::LockPhoneDeviceRespReceivedL()
    {
    if (iStatus.Int() == KErrNone)
        {
        DEBUGPRINT3A("ELockPhoneDevice: status: %d, setting: %d",
                iLockInfo.iStatus, iLockInfo.iSetting);

        DEBUGPRINT3A("ELockPhoneDevice: RMobilePhone::EStatusLocked: %d, RMobilePhone::ELockSetEnabled: %d",
                RMobilePhone::EStatusLocked, RMobilePhone::ELockSetEnabled);

        //Ask for security code if status is locked and lock is enabled
        if (iLockInfo.iStatus == RMobilePhone::EStatusLocked &&
             iLockInfo.iSetting == RMobilePhone::ELockSetEnabled)
        	{
            QuerySecCodeL();
            }
        else
            {
            DEBUGPRINT2A("ELockPhoneDevice: Check ELockPhoneToICC while SimPresent = %d", CSsmUiSpecific::IsSimPresent());
            iState = EGetLockPhoneToIcc;
            iPhone.GetLockInfo(
                iStatus, RMobilePhone::ELockPhoneToICC, iLockInfoPckg );
            SetActive();
            }
        }
    else
        {
        CompleteClientRequest(iStatus.Int());
        }
    }

void CCustomCmdDeviceSecurityCheck::LockPhoneToIccRespReceivedL()
    {
    if (iStatus.Int() == KErrNone)
        {
        DEBUGPRINT4A("1ELockPhoneDevice: status: %d, setting: %d, IsSimPresent: %d",
                iLockInfo.iStatus, iLockInfo.iSetting, CSsmUiSpecific::IsSimPresent());

        DEBUGPRINT3A("1ELockPhoneDevice: RMobilePhone::EStatusLocked: %d, RMobilePhone::ELockSetEnabled: %d",
                RMobilePhone::EStatusLocked, RMobilePhone::ELockSetEnabled);

        if (iLockInfo.iSetting == RMobilePhone::ELockSetEnabled &&
           (EFalse == CSsmUiSpecific::IsSimPresent() || ( iLockInfo.iStatus == RMobilePhone::EStatusLocked)))
            {
            QuerySecCodeL();
            }
        else
            {
            CompleteClientRequest(iStatus.Int());
            }
        }
    else
        {
        CompleteClientRequest(iStatus.Int());
        }
    }

void CCustomCmdDeviceSecurityCheck::SecCodeQueryRespReceivedL()
    {
    __ASSERT_DEBUG(NULL != iSsmSecurityCheckNotifier, PanicNow(KPanicCustomCmdDeviceSecurityCheck, ECmdNullPtr));
    if (iStatus.Int() == KErrNone &&  iSsmSecurityCheckNotifier->SecurityCheckResult() == KErrNone)
        {
        CompleteClientRequest(KErrNone);
        }
    else if (iStatus.Int() != KErrNone)
        {
        // Fatal error
        CompleteClientRequest(iStatus.Int());
        }
    else
        {
        // Wrong code, try again
        QuerySecCodeL(); 
        }
    }

void CCustomCmdDeviceSecurityCheck::QuerySecCodeL()
    {
    iState = EQuerySecCode;
    TInt errorCode = KErrNone;

    if (!iSsmSecurityCheckNotifier)
        {
        TRAP(errorCode, iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecCodePasswd));
        }

    if (KErrNone == errorCode)
        {
        iSsmSecurityCheckNotifier->ShowNoteL(iStatus);
        SetActive();
        }
    else
        {
        DEBUGPRINT2A("Failed to create CSsmSecurityCheckNotifier with error %d", errorCode);
        CompleteClientRequest(errorCode);
        }
    }

void CCustomCmdDeviceSecurityCheck::DoCancel()
	{
    if (iSsmSecurityCheckNotifier)
        {
        iSsmSecurityCheckNotifier->Cancel();
        }
    iPhone.CancelAsyncRequest(EMobilePhoneGetLockInfo);
    CompleteClientRequest(KErrCancel);
	}

TInt CCustomCmdDeviceSecurityCheck::RunError(TInt aError)
	{
	//Call cancel method of CActive	
	Cancel();
	CompleteClientRequest(aError);
	return KErrNone;
	}

void CCustomCmdDeviceSecurityCheck::CompleteClientRequest(TInt aReason)
	{
	DEBUGPRINT2A("Device Security Check completed with %d", aReason);

	TInt err = iStartupRepository->Set(KStartupDevLockStatus, (aReason == KErrNone ? EStartupDevLockSucess : EStartupDevLockNotSucess));
    DEBUGPRINT2A("Setting KStartupDevLockStatus completed with error %d", err);
	//Complete client request with reason code
	if (iExecuteRequest)
        {
    	err = RProperty::Set(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, 
    			aReason == KErrNone ? EStarterSecurityPhaseSecOk : EStarterSecurityPhaseSecNok );
        User::RequestComplete(iExecuteRequest, aReason);
        }
	}
