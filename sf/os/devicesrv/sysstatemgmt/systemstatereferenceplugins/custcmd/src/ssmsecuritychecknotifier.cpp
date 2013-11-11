// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CStrtSecurityNote class.
//




/**
 @file
 @internalComponent
 @released
*/

#include "ssmsecuritychecknotifier.h"
#include "ssmdebug.h"
#include "ssmuiproviderdll.h"
#include "ssmrefcustomcmdcommon.h"
#include <ssm/ssmstateawaresession.h>
#include <e32def.h>
#include <startupdomaindefs.h>
#include <etelmm.h>
#include <ctsydomainpskeys.h>

static TBool NoteTypeToEvent(
    TInt& aEventCode, const TStrtSecurityNoteType aNoteType )
    {
    TBool result( ETrue );
    switch( aNoteType )
        {
        case ESecCodePIN1:
            {
            aEventCode = RMobilePhone::EPin1Required;
            break;
            }
        case ESecCodePIN2:
            {
            aEventCode = RMobilePhone::EPin2Required;
            break;
            }
        case ESecCodeUPIN:
            {
            aEventCode = RMobilePhone::EUniversalPinRequired;
            break;
            }
        case ESecCodePUK1:
            {
            aEventCode = RMobilePhone::EPuk1Required;
            break;
            }
        case ESecCodePUK2:
            {
            aEventCode = RMobilePhone::EPuk2Required;
            break;
            }
        case ESecCodeUPUK:
            {
            aEventCode = RMobilePhone::EUniversalPukRequired;
            break;
            }
        case ESecCodePasswd:
            {
            aEventCode = RMobilePhone::EPhonePasswordRequired;
            break;
            }
        case ESecCodeAppPIN1:
            {
            aEventCode = RMobilePhone::EUSIMAppPinRequired;
            break;
            }
        case ESecCodeAppPIN2:
            {
            aEventCode = RMobilePhone::ESecondUSIMAppPinRequired;
            break;
            }
        case ESecInfoSimLockRestrOn:
            {
            aEventCode = RMobilePhone::EICCTerminated;
            break;
            }
        case ESecInfoSimInvalid:
            {
            //aEventCode = RMobilePhone::ESIMInvalid;
            result = EFalse;
            break;
            }
        default:
            {
            result = EFalse;
            break;
            }
        }
    return result;
    }

CSsmSecurityCheckNotifier* CSsmSecurityCheckNotifier::NewL()
	{
	CSsmSecurityCheckNotifier* self = new (ELeave) CSsmSecurityCheckNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
CSsmSecurityCheckNotifier* CSsmSecurityCheckNotifier::NewL(TStrtSecurityNoteType aNoteType)
	{
	CSsmSecurityCheckNotifier* self = new (ELeave) CSsmSecurityCheckNotifier(aNoteType);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CSsmSecurityCheckNotifier::CSsmSecurityCheckNotifier(TStrtSecurityNoteType aNoteType)
	: CActive( EPriorityStandard),
	iNoteType (aNoteType),
	iCodeOk(EFalse)
	{
	}

CSsmSecurityCheckNotifier::CSsmSecurityCheckNotifier()
	: CActive( EPriorityStandard),
	iCodeOk(EFalse)
	{
	}

/**
@panic EInvalidUid if the UID is invalid
*/
void CSsmSecurityCheckNotifier::ConstructL()
	{
	//Initialise the state to idle
	iCmdState = EIdle;

	//Connect to notifier
	User::LeaveIfError(iSecurityPinNotifier.Connect());

	iSecurityPinNotifierUid = CSsmUiSpecific::SecurityPinNotifierUid();
    __ASSERT_ALWAYS(0 != iSecurityPinNotifierUid.iUid, User::Panic(KPanicSecurityCheckNotifier, EInvalidUid));

	//Attach to the emergency call property
	User::LeaveIfError(iSsmEmergencyCallProperty.Attach(CSsmUiSpecific::EmergencyCallPropertyCategory(), CSsmUiSpecific::EmergencyCallPropertyKey(), EOwnerThread));

	//Add this active object to active scheduler
	CActiveScheduler::Add(this);
	}

CSsmSecurityCheckNotifier::~CSsmSecurityCheckNotifier()
	{
	//Cancel any asynchronous request before destroying
	Cancel();

	//Close the notifier
	iSecurityPinNotifier.Close();

	//Close the emergency call property
	iSsmEmergencyCallProperty.Close();
	}

void CSsmSecurityCheckNotifier::ShowNoteL(TStrtSecurityNoteType aNoteType, TRequestStatus& aRequest)
	{
	iNoteType = aNoteType;
	ShowNoteL(aRequest);
	}

void CSsmSecurityCheckNotifier::ShowNoteL(TRequestStatus& aRequest)
	{
	//Set the user request to KRequestPending
	aRequest = KRequestPending;
	iExecuteRequest = &aRequest;

	iCodeOk = EFalse;

	//Check whether notifier is cancellable by the user or not
	iIsDlgCancellable = IsDlgCancellableL();

	StartNotifier();
	}

TBool CSsmSecurityCheckNotifier::IsDlgCancellableL()
	{
	RSsmStateAwareSession ssmStateAwareSession;

	//Use the domain id of UI applications
	TDmDomainId dmId = KSM2UiServicesDomain3;

	//Connect to state aware session
	User::LeaveIfError(ssmStateAwareSession.Connect(dmId));

	//Get the current state of the system
	TSsmState currentState = ssmStateAwareSession.State();

	//Close the state aware session
	ssmStateAwareSession.Close();

	TBool isDlgCancellable;

	//Is system in start up state
	if ( currentState.MainState() == ESsmStartup )
		{
		TInt securityPhaseVal = EStarterSecurityPhaseUninitialized;
		TInt errorCode = RProperty::Get(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, securityPhaseVal);
		DEBUGPRINT3A("Getting the KStarterSecurityPhase completed with errorcode %d and its value is %d", errorCode, securityPhaseVal);
		User::LeaveIfError(errorCode);

		//TSsmStartupSubStateExt::ESsmStateNonCritical
		iAfterStartup = ( 0x34 == currentState.SubState() && securityPhaseVal > EStarterSecurityPhaseSimNok ) ? ETrue : EFalse;
		}
	else
		{
		iAfterStartup = ETrue;
		}
	DEBUGPRINT2A("iAfterStartup is %d", iAfterStartup);

	//Notifier dialogue is not cancellable if system is in startup state and
	//requested for PUK1 or PUK2 or UPUK code
    isDlgCancellable = (iAfterStartup &&
						iNoteType != ESecCodePUK1 &&
						iNoteType != ESecCodePUK2 &&
						iNoteType != ESecCodeUPUK);
	return isDlgCancellable;
	}

void CSsmSecurityCheckNotifier::RunL()
	{
	if (iCmdState == ENotifierIsActive &&
         !iIsDlgCancellable &&
         iPinResult() == KErrCancel)
        {
        // User activated an emergency call from the security note dialog.
        // Wait for the call to finish and then show the note again.
        iCmdState = EEmergencyCallIsActive;

		//Subscribe to the emergency call swp
		iSsmEmergencyCallProperty.Subscribe(iStatus);

		DEBUGPRINT1A("Pin cancelled and ECall Started");
		SetActive();
        }
    else if (iCmdState == EEmergencyCallIsActive && iStatus.Int() != KErrCancel)
        {
        TInt value = -1; 
		const TInt errorcode = iSsmEmergencyCallProperty.Get( value );
		DEBUGPRINT3A("iSsmEmergencyCallProperty.Get() has returned value %d and errorcode %d", value , errorcode);
		if (KErrNone == errorcode)
			{
			// Compare the property value with EPSCTsyCallStateNone. This indicates the call has finished.
			if (EPSCTsyCallStateNone == value)
				{
				// Emergency call activated from the security note dialog has
				// finished. Show the note again.
				DEBUGPRINT1A("ECall Ended and restarting pin notifier");
				StartNotifier();
				}
			else
				{
				//Subscribe to the property again if the property key value is not yet set to EPSCTsyCallStateNone.
				iSsmEmergencyCallProperty.Subscribe(iStatus);
				SetActive();
				}
			}
        }
    else
        {
        //Reset the state to initial start
        iCmdState = EIdle;
        DEBUGPRINT1A("Pin notifier closed");

        //Check whether the entered code is correct or not
        if (iStatus == KErrNone && iPinResult() == KErrNone)
        	{
        	iCodeOk = ETrue;
			}

		//Complete the user request
        if (iExecuteRequest)
            {
            User::RequestComplete(iExecuteRequest, iStatus.Int());
            }
        }
	}

void CSsmSecurityCheckNotifier::StartNotifier()
	{
	TInt eventCode = 0;
	
	//Change the state to notifier active state
	iCmdState = ENotifierIsActive;

	if ( NoteTypeToEvent( eventCode, iNoteType ) )
		{
		DEBUGPRINT3A( "CSsmSecurityCheckNotifier: Security code %d handled as event %d",
					iNoteType, eventCode );
        iParams().iEvent = eventCode;
        iParams().iStartup = !iAfterStartup;

    	//Start the notifier
    	iSecurityPinNotifier.StartNotifierAndGetResponse(
    			iStatus, iSecurityPinNotifierUid, iParams, iPinResult);
		}
	else
		{
        DEBUGPRINT3A("CSsmSecurityCheckNotifier: Security code %d not handled as event %d", 
        			iNoteType, eventCode);
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
		}
	SetActive();
	}

TInt CSsmSecurityCheckNotifier::RunError(TInt aError)
	{
	//Do the cleanup in case of an error
	CleanupAndCancel(aError);
	return KErrNone;
	}

void CSsmSecurityCheckNotifier::DoCancel()
	{
	//Do the cleanup in case of cancel
	CleanupAndCancel(KErrCancel);
	}

void CSsmSecurityCheckNotifier::CleanupAndCancel(TInt aError)
	{
	//Cancel if notifier is stil active
	iSecurityPinNotifier.CancelNotifier(iSecurityPinNotifierUid);

	//Cancel the request if there is any
	iSsmEmergencyCallProperty.Cancel();

	//Complete user request with the result code given
	if(iExecuteRequest)
		{
		if (iExecuteRequest->Int() == KRequestPending)
			{
			User::RequestComplete(iExecuteRequest, aError);
			}
		}
	}

TBool CSsmSecurityCheckNotifier::IsCodeAccepted()
	{
	//Return whether code entered is correct or not
	return iCodeOk;
	}

TInt CSsmSecurityCheckNotifier::SecurityCheckResult() const
    {
    return iPinResult();
    }
