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
//

/**
 @file
 @internalAll
*/

#ifndef __SMSPMODM_H__
#define __SMSPMODM_H__

#include <e32base.h>
#include <etel.h>
#include <e32property.h>
#include <sacls.h>
#include "smspcomm.h"
#include "smsuact.h"
#include "smspcdb.h"

/**
 *  @internalComponent
 */
const TInt KMaxNumberOfInitiaisationAttempts=3;

class TSmsSettings;


class CSmsPDURead;
class RPhone;
class RMobileSmsMessaging::TMobileSmsCapsV1;


/**
 *  class CSmsPhoneIntialization
 *  
 *  Initializes iGsmPhone (an RPhone) then checks the TSY caps and starts CSmsPDURead
 *  @internalComponent
 */
class CSmsPhoneInitialization : public CSmsuActiveBase
	{
public:
	CSmsPhoneInitialization(RMobileSmsMessaging& aSmsMessaging, RMobilePhone& aGsmPhone, CSmsPDURead& aSmsPDURead,
	                        RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps, TInt aPriority, CSmspSetBearer* aInitialisationSetBearer);
	~CSmsPhoneInitialization();

	void Start();
	inline TInt Initialized() const {return iInitialized;}
	inline TBool NetworkInfoAvailable() const {return iNetworkInfoAvailable;}
	inline const RMobilePhone::TMobilePhoneNetworkInfoV1& NetworkInfo()const {return iNetworkInfo;}

private:
	void DoRunL();
	void DoCancel();
	void Complete(TInt aStatus);

	void AfterPhoneInitialize(TInt aStatus);
	enum TSmsPhoneInitializationState
		{
		ESmsPhoneInitializationIdle,
		ESmsPhoneInitializationPhoneInit,
		ESmsPhoneInitializationSettingMoBearer,
		ESmsPhoneInitializationGettingRegistrationStatus,
		ESmsPhoneInitializationWaitingForCurrentNetwork,
		ESmsPhoneInitializationGettingCurrentNetworkInfo,
		ESmsPhoneInitializationCompleted
		};

private:
	TSmsPhoneInitializationState iState;
	RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;
	RMobileSmsMessaging& iSmsMessaging;
	RMobilePhone& iGsmPhone;
	CSmsPDURead& iSmsPDURead;
	RMobileSmsMessaging::TMobileSmsCapsV1& iMobileSmsCaps;
	TInt iInitialized;	//< Indicates whether iGsmPhone has been initialized and iMobileSmsCaps supports GsmSms
	TBool iNetworkInfoAvailable ;	//< Indicate whether network info is available
	TInt iNumberOfAttempts;
	TInt iMaxNumberOfAttempts;
	RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iNetworkInfoPckg;
	CSmspSetBearer* iInitialisationSetBearer;
	};


/**
 *  @internalComponent
 */
class CSmsModemNotification : public CSmsuActiveBase
	{
public:
	static CSmsModemNotification* NewL(MSmsComm& aSmsComm);
	~CSmsModemNotification();

	void Start();		// Start should only be called once for the lifetime of an object
	RPhone::TModemDetection ModemState();
private:
	CSmsModemNotification(MSmsComm& aSmsComm);
	void ConstructL();
	void Complete(TInt aStatus);
	void DoRunL();
	void DoCancel();

private:
	enum TSmsModemNotificationState
		{
		ESmsModemNotificationIdle,
		ESmsModemNotificationWaitingForNotification
		};

private:
	TSmsModemNotificationState iState;
	MSmsComm& iSmsComm;
	RProperty iPhonePowerProperty;
	};

#endif // !defined __SMSPMODM_H__
