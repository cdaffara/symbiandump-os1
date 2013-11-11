// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @released
*/

#include "ssmdebug.h"
#include "ssmuiproviderdll.h"

#include "cmdpublishsimownedandchanged.h"

#include <centralrepository.h> 
#include <e32property.h>
#include <ssm/startupdomainpskeys.h>

/**
* Used for storing the IMSI code of current SIM card. This is needed for
* determining if SIM card has changed from the previous boot.
* Key value is an descriptor containing the IMSI code.
*/
const TUint32 KSSMStoredImsi = 0x00000001;
const TUid KCRUidSSMVals = {0x2001d2aa};

CCustomCmdPublishSimOwnedAndChanged* CCustomCmdPublishSimOwnedAndChanged::NewL()
	{
	CCustomCmdPublishSimOwnedAndChanged* self = new (ELeave) CCustomCmdPublishSimOwnedAndChanged();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCustomCmdPublishSimOwnedAndChanged::CCustomCmdPublishSimOwnedAndChanged()
									: CActive(EPriorityStandard),
									iState(EPublishSimOwnedAndChangedIdle)
	{
	}

void CCustomCmdPublishSimOwnedAndChanged::ConstructL()
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

	//Add active object to active scheduler
	CActiveScheduler::Add(this);
	}

CCustomCmdPublishSimOwnedAndChanged::~CCustomCmdPublishSimOwnedAndChanged()
	{
	ExecuteCancel();
	
    iPhone.Close();
    if (iServer.Handle())
        {
        iServer.UnloadPhoneModule(*iTsyModuleName);
        iServer.Close();
        }
    delete iTsyModuleName;
	}

TInt CCustomCmdPublishSimOwnedAndChanged::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	//Connect to RSsmSimAdaptation
	return iSsmSimAdaptation.Connect();
 	}

void CCustomCmdPublishSimOwnedAndChanged::Close()
	{
	//Close RSsmSimRfAdaptation
	iSsmSimAdaptation.Close();
	}

void CCustomCmdPublishSimOwnedAndChanged::Release()
	{
	delete this;
	}

void CCustomCmdPublishSimOwnedAndChanged::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Set the user request to pending
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;
	
	// Request SIM owned status
	iSsmSimAdaptation.GetSimOwned(iBooleanPckg, iStatus);
	SetActive();

	iState = EPublishSimOwned;
	}

void CCustomCmdPublishSimOwnedAndChanged::ExecuteCancel()
	{
	Cancel();
	}

void CCustomCmdPublishSimOwnedAndChanged::RunL()
	{
	DEBUGPRINT1A("CCustomCmdPublishSimOwnedAndChanged::RunL");
    switch (iState)
        {
        case EPublishSimOwned:
        	SimOwned();
            break;
        case EPublishSimChanged:
        	SimChanged();
            break;
        default:
        	DEBUGPRINT2A("CSsmSimStatusObserver unknown internal state: %d", iState);
        	CompleteClientRequest(KErrArgument);
            break;
        }
	}

void CCustomCmdPublishSimOwnedAndChanged::SimOwned()
	{
	// Default to SIM not being owned
	TBool simOwned = EFalse;
	if(KErrNone != iStatus.Int())
		{
		DEBUGPRINT2A("CSsmSimStatusObserver error getting SIM owned: %d", iStatus.Int());
		// Go with default of SIM not being owned.
		simOwned = EFalse;
		}
	else
		{
		simOwned = iBooleanPckg();
		}
	
	// Publish SIM owned status
	TUid simStatusUid = CSsmUiSpecific::StartupPSUid(); 
	TInt err = RProperty::Set(simStatusUid, KPSSimOwned, simOwned ? ESimOwned : ESimNotOwned);
	if(KErrNone != err)
		{
		DEBUGPRINT2A("CSsmSimStatusObserver error publishing SIM owned: %d", err);
		CompleteClientRequest(err);
		return;
		}
	
	//Request for SIM changed status
    TUint32 caps = 0;
    TInt errorCode = 0;
#ifdef TEST_CLAYER_MACRO
    errorCode = KErrNone;
#else
    errorCode = iPhone.GetIdentityCaps(caps);
#endif
    if (errorCode == KErrNone )
        {
        if (caps | RMobilePhone::KCapsGetSubscriberId)
            {
            iPhone.GetSubscriberId(iStatus, iSubscriberId);
            SetActive();
        	iState = EPublishSimChanged;
            }
        else
            {
            CompleteClientRequest(KErrNotSupported);
            }
        }
    else
        {
        CompleteClientRequest(errorCode);
        }
	}

void CCustomCmdPublishSimOwnedAndChanged::SimChanged()
	{
	// Default to SIM being changed
	TBool simChanged = ETrue;
	if(KErrNone == iStatus.Int())
		{
		simChanged = (CompareToLastStoredImsi(iSubscriberId) != 0);
		}
	else
		{
		// Couldn't get status so go with default of SIM having changed.
		DEBUGPRINT2A("CSsmSimStatusObserver error getting SIM changed: %d", iStatus.Int());
		}

	// Publish SIM changed status
	TUid simStatusUid = CSsmUiSpecific::StartupPSUid(); 
	TInt err = RProperty::Set(simStatusUid, KPSSimChanged, simChanged ? ESimChanged : ESimNotChanged);
	if(KErrNone != err)
		{
		DEBUGPRINT2A("CSsmSimStatusObserver error publishing SIM changed: %d", err);
		CompleteClientRequest(err);
		return;		
		}

    CompleteClientRequest(iStatus.Int());
	}

void CCustomCmdPublishSimOwnedAndChanged::DoCancel()
	{
	iSsmSimAdaptation.GetCancel();
    iPhone.CancelAsyncRequest(EMobilePhoneGetSubscriberIdCancel);
    CompleteClientRequest(KErrCancel);
	}

TInt CCustomCmdPublishSimOwnedAndChanged::RunError(TInt aError)
	{
	//Call cancel method of CActive	
	Cancel();
	CompleteClientRequest(aError);
	return KErrNone;
	}

void CCustomCmdPublishSimOwnedAndChanged::ReadAndUpdateStoredImsiL(
    RMobilePhone::TMobilePhoneSubscriberId& aReadValue,
    const RMobilePhone::TMobilePhoneSubscriberId aUpdateValue)
    {
    CRepository* repository = CRepository::NewLC(KCRUidSSMVals);
    User::LeaveIfError(repository->Get(KSSMStoredImsi, aReadValue));
    User::LeaveIfError(repository->Set(KSSMStoredImsi, aUpdateValue));
    CleanupStack::PopAndDestroy(repository);
    }

TInt CCustomCmdPublishSimOwnedAndChanged::CompareToLastStoredImsi(
    const RMobilePhone::TMobilePhoneSubscriberId aNewValue)
    {
    RMobilePhone::TMobilePhoneSubscriberId oldValue;
    TRAPD(errorCode, ReadAndUpdateStoredImsiL(oldValue,aNewValue));
    if(KErrNone != errorCode)
    	{
    	DEBUGPRINT2A("Failed to read the stored IMSI code", errorCode);
    	return errorCode;
    	}
    
    DEBUGPRINT3A("Current IMSI: '%S', stored IMSI: '%S'", &aNewValue, &oldValue);
    return aNewValue.Compare(oldValue);
    }

void CCustomCmdPublishSimOwnedAndChanged::CompleteClientRequest(TInt aReason)
	{
	//Complete client request with reason code
	if (iExecuteRequest)
        {
        User::RequestComplete(iExecuteRequest, aReason);
        }
	}
