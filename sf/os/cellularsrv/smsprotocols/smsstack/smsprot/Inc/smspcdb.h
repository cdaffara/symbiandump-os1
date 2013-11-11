// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares the CSmsCommDBEvent class.
// 
//

/**
 @file
 @internalAll
*/

#if !defined (SMSPCDB_H__)
#define SMSPCDB_H__

#include <e32base.h>
#include <e32property.h>
#include "smspcomm.h"
#include "smsstacklog.h"
#include "smsuact.h"


class TSmsSettings;



/**
 *  Requests notification of changes in CommDB and in turn notifies an observer,
 *  the CSmsProtocol object.
 *  
 *  Currently this class only waits on changes in the current SMS bearer setting,
 *  though it should be extendable for changes in other CommDB settings in the future.
 *  
 *  The class is a simple active object derived state machine with two states
 *  defined by CSmsCommDBEvent::TSmsCommDBEventState.
 *  
 *  @internalComponent
 */
class CSmspCommDbEvent : public CSmsuActiveBase
	{
protected:
	~CSmspCommDbEvent();
	virtual void ConstructL();
	virtual void NotifyOnEvent();
	CSmspCommDbEvent(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, TInt aPriority);
	void Complete(TInt aError);

protected:
	// Private members
	RProperty iProperty; // Property describing the event we require notification of
	const TSmsSettings& iSmsSettings;
	RMobileSmsMessaging& iSmsMessaging;
	};


/**
 *  Queries CommDB for the current sms bearer and attempts to change the
 *  bearer of the phone.
 *  
 *  This class works with the classes CSmsCommDBEvent and CSmsProtocol to change
 *  the sms bearer when the corresponding setting changes in CommDB.
 *  
 *  The class is a simple active object derived state machine with these states
 *  defined by CSmspSetBearer::TSmsSetBearerState.
 *  
 *  @internalComponent
 */
class CSmspSetBearer : public CSmspCommDbEvent
	{
public:
	static CSmspSetBearer* NewL(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, TInt aPriority);
	~CSmspSetBearer();
	void Start();
	void NotifyBearerSet(TRequestStatus& aStatus);

protected:
	virtual void ConstructL();

private:
	void DoRunL();
	void DoCancel();
private:
	CSmspSetBearer(const TSmsSettings& aSmsSettings,RMobileSmsMessaging& aSmsMessaging, TInt aPriority);
	void SetSmsBearer();
	void GetSmsBearerL(RMobileSmsMessaging::TMobileSmsBearer& aBearer);
	void NotifyOnEvent();
private:
	enum TSmsSetBearerState
		{
		ESmsSetBearerStateInit, 			//< Intial State
		ESmsSetBearerNotifyOnEvent, 		//< Wait for notification from property when Bearer changes
		ESmsSetBearerStateSettingBearer 	//< Setting the bearer on the current GPRS session
		};
private:
	TSmsSetBearerState iState;				//< The current state of the state machine
	RMobileSmsMessaging::TMobileSmsBearer iBearer;	//< The currently used SMS bearer
	TBool iBearerSet;						//< Boolean tell is bearer set
	RMobileSmsMessaging::TMobileSmsBearer iPreviousBearer; //< The previously used SMS bearer
	};

/**
 *  @internalComponent
 */
const TInt KSmspReceiveModeCommDbRetryLimit = 5;
const TInt KSmspReceiveModeCommDbRetryDelay = 1000000;

/**
 *  CSmspReceiveMode reads the CommDB global setting SMS_RECEIVE_MODE, compares this to the TSY's SMS capabilities then sets the Receive Mode on the TSY.
 *  After setting the Receive Mode, CSmspReceiveMode waits for notification from CommDB for changes in global setting SMS_RECEIVE_MODE.
 *  When the global setting has changed, CSmspReceiveMode sets the Receive Mode on the TSY again.
 *  @internalComponent
 */
class CSmspReceiveMode : public CSmspCommDbEvent
	{
public:

	static CSmspReceiveMode* NewL(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, const RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps, TInt aPriority);
	virtual ~CSmspReceiveMode();
	void Start(TRequestStatus& aStatus);

protected:
	virtual void ConstructL();

private:

	CSmspReceiveMode(const TSmsSettings& aSmsSettings, RMobileSmsMessaging& aSmsMessaging, const RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps, TInt aPriority);
	void DoRunL();
	void DoCancel();

	RMobileSmsMessaging::TMobileSmsReceiveMode SelectReceiveMode(RMobileSmsMessaging::TMobileSmsReceiveMode aPreferredMode) const;
	RMobileSmsMessaging::TMobileSmsReceiveMode SelectReceiveMode() const;

	void GetCommDbReceiveModeL(RMobileSmsMessaging::TMobileSmsReceiveMode& aReceiveMode);
	void NotifyOnEvent();
	void SetReceiveMode(RMobileSmsMessaging::TMobileSmsReceiveMode aReceiveMode);

private:

	enum TSmspReceiveMode
		{
		ESmspReceiveModeInit,					//< Initial state
		ESmspReceiveModeRetryCommDb,			//< Transitory state while waiting for CommDb access
		ESmspReceiveModeNotifyOnEvent,			//< Wait for notification from property when Bearer changes
		ESmspReceiveModeSet 					//< Setting the Receive Mode on the TSY
		} iState;

	// When notified that the CommDB has been changed we must re-read SMS_RECEIVE_MODE. Possibly this can transitorily
	// fail due to power-up race condition, etc. Hence this suppoer for retrying briefly
	TInt	iCommDbRetryCount;					//< Count of retry attempts before giving up
	RTimer	iCommDbRetryTimer;
	RMobileSmsMessaging::TMobileSmsReceiveMode	iSmsReceiveModeLastSet;	//< Used in the event that CommDb can't be read (rather than simply applying default)

	const RMobileSmsMessaging::TMobileSmsCapsV1& iSmsCaps;		//< TSY SMS Capabilities
	};

#endif // !defined SMSPCDB_H__
