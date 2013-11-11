// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "watcherbaseTraces.h"
#endif

#undef TCOM_ARG
#ifdef OST_TRACE_COMPILER_IN_USE
#define TCOM_ARG(x) x
#else
#define TCOM_ARG(x)
#endif

#include "watcherbase.h"

// System includes
 #include <commsdattypesv1_1.h>
 #include <commsdat_partner.h>

 #include <sacls.h>

 using namespace CommsDat;

// User includes
#include "phoneonoff.h"

// Constants
const TInt KOneSecond = 1000000;



//
// ------> Global exports
//


//
// ------> CWatcherBase (source)
//

EXPORT_C CWatcherBase::CWatcherBase(TInt aPriority)
:	CActive(aPriority)
	{
	CActiveScheduler::Add(this);

#ifdef WATCHER_TESTING    
    //-- this section of code is used by TE_TelWatchers(Unit) test 
	TTime now;
	now.UniversalTime();
	TheSeed = now.Int64();

    //-- define properties for test purposes
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_CTOR_1, "CTelWatcherBase : defining properties for testing");
   
    //-- For debugging purposes only, used by TE_TelWatchers(Unit).

    //- this property change (to any value) informs that CTelPhoneWatcher has re-read modem table from commdb in
    //- CTelPhoneWatcher::DoRetrieveTSYNameL(). 
    RProperty::Define(KUidSystemCategory, KUidTestProp_ModemTableRefreshed.iUid, RProperty::EInt);  
  
    //- this property changes in CTelPhoneWatcher::HandleModemChangedL()
    //- when the commsdb modem record has changed
    RProperty::Define(KUidSystemCategory, KUidTestProp_ModemRecordChanged.iUid, RProperty::EInt);
    
    //-- this property is used in CIndicatorWatcher::HandleIndicatorUpdateL()
    //-- to simulate call state change by t_watchers test
    RProperty::Define(KUidSystemCategory, KUidTestProp_CallStateChange.iUid, RProperty::EInt);

    //-- this property is used to disable and reset phone watchers
    RProperty::Define(KUidSystemCategory, KUidTestProp_WatchersDisable.iUid, RProperty::EInt);    
#endif
	}

EXPORT_C CWatcherBase::~CWatcherBase()
	{
	Cancel();
	iTimer.Close();
	iPhonePowerProperty.Close();
	}

EXPORT_C void CWatcherBase::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_CONSTRUCTL_1, "WatcherBase : Creating timer");
	User::LeaveIfError(iTimer.CreateLocal());

	User::LeaveIfError(iPhonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));    
	
	// This starts the whole ball rolling
	RequestNextState();
	}

//
//
//

EXPORT_C void CWatcherBase::SuspendFor(TInt aTimeInSeconds)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_SUSPENDFOR_1, "WatcherBase : Pausing after error");
	TTimeIntervalMicroSeconds32 timeToSuspendFor = aTimeInSeconds * KOneSecond;
	iTimer.After(iStatus, timeToSuspendFor);
	State() = EBaseStateSuspending;
	SetActive();
	}

EXPORT_C void CWatcherBase::SetDisabled(const TDesC& TCOM_ARG(aLogEntry), TInt TCOM_ARG(aError))
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_SETDISABLED_1, "Log Entry \"%S\" error %d", aLogEntry, aError);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_SETDISABLED_2, "WatcherBase : Watcher is now disabled");
	State() = EBaseStateDisabled;
	}

EXPORT_C void CWatcherBase::RequestNextState()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_REQUESTNEXTSTATE_1, "WatcherBase : Requesting State Change");

	if	(State() != EBaseStateDisabled)
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	}

EXPORT_C void CWatcherBase::WaitForPhoneToPowerUpL()
//
//	The phone is currently turned off. We must wait for it to be turned back on
//	again before the watchers can continue.
//
	{
	TInt val;	
	
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_WAITFORPHONETOPOWERUPL_1, "WatcherBase : Waiting for phone to be turned on");
	__ASSERT_DEBUG(!IsActive(), WatcherBasePanic(EUnexpectedActiveState));
	Cancel();

	//-- Subscribe to phone power state change property and wait until it changes
	iPhonePowerProperty.Subscribe(iStatus);

	// Update our state
	State() = EBaseStateWaitingForPhoneToPowerUp;

	// Set us ready to go again
	SetActive();    

	User::LeaveIfError(iPhonePowerProperty.Get(val));

	if (val != ESAPhoneOff)
		{//-- phone is already ON, complete request so that we go to RunL without waiting
		iPhonePowerProperty.Cancel();        
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_WAITFORPHONETOPOWERUPL_2, "CTelWatcherBase::WaitForPhoneToPowerUpL ??? phone is already turned ON");
		}
	}

//
//
//

EXPORT_C void CWatcherBase::RunL()
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_1, "WatcherBase : RunL(%d)", iStatus.Int());

	switch(State())
		{
	case EBaseStateConnectingToPropertyNotifier:		
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_2, "WatcherBase : Attaching to Property");

		// Virtual function call back, for any subclasses that need to implement
		// any special stuff.
		HandleConnectionToChangeNotifierEstablishedL();

		// Move to next state
		State() = EBaseStatePassive;
		RequestNextState();
		break;

	case EBaseStateWaitingForPhoneToPowerUp:
		// We were waiting for the phone to become available again. We now must restart
		// this watcher from scratch.
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_3, "WatcherBase : Phone available again. Restarting watcher framework");
		
        //--  phone power state has changed (it must be turned ON)
        //--  retrieve its state and check.
        TInt val;
	    
        User::LeaveIfError(iPhonePowerProperty.Get(val));

        if (val == ESAPhoneOn)
        {   //-- everything OK, the phone has been turned ON, restart this watcher from scratch.
		    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_4, "CTelWatcherBase : Phone has been turned ON. Restarting watcher framework");
            State() = EBaseStateConnectingToPropertyNotifier;
            RequestNextState();
        }
        else
        {   //-- strange situation, we were waiting for phone On and it now Off, try to wait again
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_5, "CTelWatcherBase : ??? Phone has been turned OFF. Continue waiting...");
            WaitForPhoneToPowerUpL();
        } 		
		break;

	case EBaseStateSuspending:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_6, "WatcherBase : Completed suspension. Resuming passive state");
		
		State() = EBaseStatePassive; // Fall through

	case EBaseStatePassive: // In passive mode, so just call framework function
		HandleStateEventL(iStatus.Int());
		break;
	
	default:
	case EBaseStateDisabled:
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNL_7, "WatcherBase : RunL called in Disabled state. Ooops");
		__ASSERT_DEBUG(0, WatcherBasePanic(EUnexpectedState));
		}
	}

EXPORT_C void CWatcherBase::DoCancel()
	{
	switch(State())
		{	
	case EBaseStateSuspending:
		// Cancel outstanding timer
		iTimer.Cancel();
		break;

	case EBaseStateWaitingForPhoneToPowerUp:
		// Cancel outstanding asynchronous request
		iPhonePowerProperty.Cancel();
		break;

	default:
	case EBaseStatePassive:
	case EBaseStateConnectingToPropertyNotifier:
	case EBaseStateDisabled:
		break;
		}

	// Let other sub classes cancel their requests
	HandleCancel();
	}
 
EXPORT_C TInt CWatcherBase::RunError(TInt TCOM_ARG(aError))
//
//	Called when RunL (or a sub-function there-of) leaves.
//
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNERROR_1, "WatcherBase : RunError called with error of %d", aError);

	// Should never be called from outside the framework
	__ASSERT_DEBUG(!IsActive(), WatcherBasePanic(EUnexpectedActiveState));
	Cancel();

	// Reset all resources in this, and the derrived class so that
	// we can start up again (from scratch) (virtual function).
	if	(State() == EBaseStatePassive)
		Reset();

	// Close our resources
	iPhonePowerProperty.Close();

	// NOTE: we don't close the timer. This was opened in ConstructL so
	// we can't close that here.

	// Have we already tried this too many times?
	if	(ErrorCountIncrement() >= KErrorRetryCount)
		{
		// We've tried to resume 3 times already and we've still not managed. Give up
		// now until a reboot.
		SetDisabled(_L("WatcherBase : RunError called too many times in succession"), KErrNone);
		}
	else
		{
		// Put us in the start up state again
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CWATCHERBASE_RUNERROR_2, "WatcherBase : Phone available again. Restarting watcher framework");
		State() = EBaseStateConnectingToPropertyNotifier;
		RequestNextState();
		}

	return KErrNone;
	}

//
// Panic Function
//
void CWatcherBase::WatcherBasePanic(TWatcherPanic aPanicNumber)
	{
	_LIT(panicText,"Watcher Base");
	User::Panic(panicText,aPanicNumber);
	}



//
// ------> CPhoneWatcher (source)
//


EXPORT_C CPhoneWatcher::CPhoneWatcher(TInt aPriority)
:	CWatcherBase(aPriority)
	{
	}

EXPORT_C CPhoneWatcher::~CPhoneWatcher()
	{
	delete iModemChangeObserver;
	delete iPhoneWait;
	Phone().Close();
	ETel().Close();
	}

EXPORT_C void CPhoneWatcher::ConstructL()
	{
	CWatcherBase::ConstructL();
	}

//
//
//

EXPORT_C void CPhoneWatcher::HandleStateEventL(TInt aCompletionCode)
	{
	switch(PhoneState())
		{
	case EPhoneStateRetrievingTsyName:
		// Cconstruct observers
		if	(!iPhoneWait)
			{
			iPhoneWait = new(ELeave) CPhoneOnOff(*this);
			iPhoneWait->ConstructL();
			}
		if	(!iModemChangeObserver)
			{
			iModemChangeObserver = new(ELeave) CModemChangeObserver(*this);
			iModemChangeObserver->ConstructL();
			}

		User::LeaveIfError(RetrieveTSYName());
		PhoneState() = EPhoneStateConnectingToETel;
		RequestNextState();
		break;

	case EPhoneStateConnectingToETel:
		User::LeaveIfError(ConnectToETelServer());
		PhoneState() = EPhoneStateLoadingPhoneModule;
		RequestNextState();
		break;

	case EPhoneStateLoadingPhoneModule:
		User::LeaveIfError(LoadPhoneModule());
		PhoneState() = EPhoneStateConnectingToPhone;
		RequestNextState();
		break;

	case EPhoneStateConnectingToPhone:
		User::LeaveIfError(ConnectToPhone());
		PhoneState() = EPhoneStatePassive;
		RequestNextState();
		break;
	
	case EPhoneStatePassive:
		HandlePhoneStateEventL(aCompletionCode);
		break;

	default:
		__ASSERT_DEBUG(0, WatcherBasePanic(EUnexpectedState));
		}
	}

EXPORT_C void CPhoneWatcher::Reset()
	{
	// Ensures CModemChangeObserver object stops running
	if (iModemChangeObserver)
		{
		iModemChangeObserver->Cancel();
		}

	if	(PhoneState() == EPhoneStatePassive)
		{
		// Get children to release any resources they have
		ReleasePhoneResources();
		}
	
	// Close our connections to the phone & ETel
	Phone().Close();
	ETel().Close();

	// Reset state
	PhoneState() = EPhoneStateRetrievingTsyName;
	}

//
//
//

TInt CPhoneWatcher::RetrieveTSYName()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_RETRIEVETSYNAME_1, "PhoneWatcher : RetrieveTSYName()");
	TRAPD(error, DoRetrieveTSYNameL());
	return error;
	}

TInt CPhoneWatcher::ConnectToETelServer()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOETELSERVER_1, "PhoneWatcher : ConnectToETelServer()");
	return ETel().Connect();
	}

TInt CPhoneWatcher::LoadPhoneModule()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_LOADPHONEMODULE_1, "PhoneWatcher : LoadPhoneModule()");
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_LOADPHONEMODULE_2, "TSY Name to load is %S",iTSYName);

	return ETel().LoadPhoneModule(iTSYName);
	}

TInt CPhoneWatcher::ConnectToPhone()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_1, "PhoneWatcher : ConnectToPhone()");
	TInt error;

	RTelServer::TPhoneInfo phoneInfo;

	// Get the number of phones
	TInt phoneCount = 0;
	error = ETel().EnumeratePhones(phoneCount);
	if	(error < KErrNone)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_2, "PhoneWatcher : Failed to enumerate phones (%d)", error);
		return error;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_3, "PhoneWatcher : Counted %d 'phones'", phoneCount);

	// Iterate through all the phones
	for(TInt i=0; i<phoneCount; i++)
		{
		// Get the TSY name for the phone
		TName matchTsyName;
		
		error = ETel().GetTsyName(i, matchTsyName);
		if	(error < KErrNone)
			{
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_4, "PhoneWatcher : Getting TSY name failed (%d)", error);
			return error;
			}

		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_5, "PhoneWatcher : TSY for phone %d is '%S'", i, matchTsyName);


		// See if the phone belongs to the TSY
		if	(matchTsyName.CompareF(iTSYName) == 0)
			{
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_6, "PhoneWatcher : %S is a match for CommDb TSY: %S", matchTsyName, iTSYName);

			error = ETel().GetPhoneInfo(i, phoneInfo);
			if	(error < KErrNone)
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_7, "PhoneWatcher : Getting phone info failed (%d)", error);
				return error;
				}
			break;
			}
		}

	// Connect to the specified phone
	error = Phone().Open(ETel(), phoneInfo.iName);
	if	(error < KErrNone)
		{
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_8, "PhoneWatcher : Open phone %S failed (%d)", phoneInfo.iName, error);
		return error;
		}
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CPHONEWATCHER_CONNECTTOPHONE_9, "PhoneWatcher : Opened 'phone' %S", phoneInfo.iName);


	// Indicate we're connected and to move to next state.
	return error;
	}

//
//
//

void CPhoneWatcher::DoRetrieveTSYNameL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
#else
	CMDBSession* db = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(db);

	CMDBField<TUint32>* globalSettingsField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
	CleanupStack::PushL(globalSettingsField);
	globalSettingsField->SetRecordId(1);
	globalSettingsField->LoadL(*db);
	TUint32 modemId = *globalSettingsField;
	CleanupStack::PopAndDestroy(globalSettingsField);
	
	CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
	CleanupStack::PushL(tsyField);
	tsyField->SetRecordId(modemId);
	tsyField->SetMaxLengthL(KMaxTextLength);
	tsyField->LoadL(*db);
	iTSYName = *tsyField;
	CleanupStack::PopAndDestroy(tsyField);
	
	
	// Strip any file extension
	TInt pos = iTSYName.LocateReverse('.');
	if	(pos >= 0)
		iTSYName = iTSYName.Left(pos);

#ifdef WATCHER_TESTING
	{
	TTime now;
	now.UniversalTime();
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidTestProp_ModemTableRefreshed.iUid, I64LOW(now.Int64())));
	}
#endif

	CleanupStack::PopAndDestroy(); // db or commsDatabase 
	}

EXPORT_C void CPhoneWatcher::PhoneIsOff()
	{
	Cancel();
	Reset(); // Kill phone resources
#ifdef	_DEBUG
	TRAPD(err, WaitForPhoneToPowerUpL());
	__ASSERT_DEBUG(err == KErrNone, WatcherBasePanic(EGeneral));
#else
	TRAP_IGNORE(WaitForPhoneToPowerUpL());
#endif
	}

EXPORT_C void CPhoneWatcher::HandleModemChangedL()
//
//	Called when the commsdb modem record has changed. Must
//	re-read the current modem and attempt to reinitialise the watcher
//	using this new modem.
//
	{
#ifdef WATCHER_TESTING
	{
	TTime now;
	now.UniversalTime();
	// State set to ETrue/EFalse
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidTestProp_ModemRecordChanged.iUid, I64LOW(now.Int64())));
	}
#endif

	Cancel();

	// Kill phone resources
	Reset();

	// Start us going again. This will (in turn) 
	// re-read the comms database. NOTE: Our state has been set to
	// 'EPhoneStateRetrievingTsyName' by Reset()
	RequestNextState();
	}

