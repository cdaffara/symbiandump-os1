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

#ifndef __WATCHERBASE_H__
#define __WATCHERBASE_H__

// System includes
#include <e32base.h>
#include <etelext.h>
#include <etelmm.h>
#include <etel.h>

// User includes
#include "phoneonoff.h"
#include "ModemChangeObserver.h"

// Test code hooks (when this file is embedded inside an EXE)
#include "WatcherTestHooks.h"

/**
Constants
@internalComponent
*/
const TInt KErrorRetryCount = 3;
const TInt KErrorRetryPausePeriod = 60; // Seconds
const TInt KWatcherBaseModemNotDetected = -2017;

const TUint KPhonePropertyKey = 0; //-- Phone properties key

#ifdef WATCHER_TESTING

//-- These UIds used for debugging purposes only by T_WATCHERS test.

//-- Changing property with this Uid (to any value) informs that CTelPhoneWatcher has re-read modem table from Commdb in
//-- CTelPhoneWatcher::DoRetrieveTSYNameL(). 
const TUid  KUidTestProp_ModemTableRefreshed    = {0x11111111};
                                                                                           
//-- Property with this Uid is changed in CTelPhoneWatcher::HandleModemChangedL()
//-- when the Commdb modem record has been changed
const TUid  KUidTestProp_ModemRecordChanged     = {0xDEADBEEF};

//-- Property with this Uid is used in CIndicatorWatcher::HandleIndicatorUpdateL()
//-- to simulate call state change by t_watchers test
const TUid  KUidTestProp_CallStateChange        = {0x12341234};

//-- Property with this Uid is used to disable and reset phone watchers
const TUid  KUidTestProp_WatchersDisable        = {0xFABBBABE};

#endif

//
// ------> CWatcherBase (header)
//
class CWatcherBase : public CActive
/**
@internalComponent
*/
	{
//
public: // DESTRUCT
//
	IMPORT_C ~CWatcherBase();

//
protected: // CONSTRUCT
//
	IMPORT_C CWatcherBase(TInt aPriority = CActive::EPriorityStandard);
	IMPORT_C virtual void ConstructL();

//
#ifdef WATCHER_TESTING
public: // NEW FUNCTIONS
#else
protected: // NEW FUNCTIONS
#endif
//
	IMPORT_C void SuspendFor(TInt aTimeInSeconds);
	IMPORT_C void SetDisabled(const TDesC& aLogEntry, TInt aError);
	IMPORT_C void RequestNextState();
	IMPORT_C void WaitForPhoneToPowerUpL();
	//
	virtual void HandleStateEventL(TInt aCompletionCode) = 0;
	virtual void HandleConnectionToChangeNotifierEstablishedL() { }
	virtual void HandleCancel() = 0;
	virtual void Reset() { }

//
protected: // INLINE MUTATORS
//

	enum TWatcherPanic
		{
		EGeneral,
		EUnexpectedState,
		EUnexpectedActiveState
		};
	void WatcherBasePanic(TWatcherPanic aPanicNumber);

	inline TInt ErrorCountIncrement() { return ++iErrorCount; }
	inline void ErrorCountReset() { iErrorCount = 0; }

//
private: // FROM CActive
//
	IMPORT_C void RunL();
	IMPORT_C void DoCancel();
	IMPORT_C TInt RunError(TInt aError);

//
private: // INTERNAL STATE
//
	enum TBaseState
	{
		EBaseStateConnectingToPropertyNotifier,
		EBaseStatePassive,
		EBaseStateSuspending,
		EBaseStateDisabled,
		EBaseStateWaitingForPhoneToPowerUp,
		//
		EBaseStateLast
	};
	inline TBaseState& State() { return iBaseState; }

//
private: // STATEMACHINE
//
	
	// Error count
	TInt iErrorCount;

	// Our current state
	TBaseState iBaseState;
	
	// Retry timer
	RTimer iTimer;
	
	RProperty iPhonePowerProperty;
	};





//
// ------> CPhoneWatcher (header)
//
class CPhoneWatcher : public CWatcherBase, public MPhoneOnOffObserver, public MModemChangeObserver
/**
@internalComponent
*/
	{
//
public: // DESTRUCT
//
	IMPORT_C ~CPhoneWatcher();

//
protected: // CONSTRUCT
//
	IMPORT_C CPhoneWatcher(TInt aPriority = CActive::EPriorityStandard);
	IMPORT_C void ConstructL();

//
protected: // NEW
//
	virtual void HandlePhoneStateEventL(TInt aCompletionCode) = 0;
	virtual void ReleasePhoneResources() { }

//
protected: // INLINE MUTATORS
//
	inline RMobilePhone& Phone() { return iPhone; }
	inline RTelServer& ETel() { return iETelServer; }

//
protected: // FROM CWatcherBase
//
	IMPORT_C void HandleStateEventL(TInt aCompletionCode);
	IMPORT_C void Reset();

//
protected: // FROM MPhoneOnOffObserver
//
	IMPORT_C void PhoneIsOff();

//
private: // FROM MModemChangeObserver
//
	IMPORT_C void HandleModemChangedL();

//
private: // STANDARD SETUP STATE-MACHINE STEPS
//
	TInt RetrieveTSYName();
	TInt ConnectToETelServer();
	TInt LoadPhoneModule();
	TInt ConnectToPhone();

//
private: // INTERNAL
//
	enum TPhoneState
	{
		EPhoneStateRetrievingTsyName = 0,
		EPhoneStateConnectingToETel,
		EPhoneStateLoadingPhoneModule,
		EPhoneStateConnectingToPhone,
		EPhoneStatePassive
	};
	inline TPhoneState& PhoneState() { return iPhoneState; }
	void DoRetrieveTSYNameL();

//
private: // STATE MACHINE MEMBER DATA
//
	TName iTSYName;
	TPhoneState iPhoneState;
	//
	RMobilePhone iPhone;
	RTelServer iETelServer;	
	
	// Waits to be informed when the phone is turned on or off.
	CPhoneOnOff* iPhoneWait;

	// Waits to be informed when the CommsDb modem changes.
	CModemChangeObserver* iModemChangeObserver;
	};


#endif
