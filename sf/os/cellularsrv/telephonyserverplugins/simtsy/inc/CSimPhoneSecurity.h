// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SIMPHONESECURITY_H_
#define __SIMPHONESECURITY_H_

/**
 * @file
 *
 * Contains the class definitions required for the Phone Security.
 * @internalAll
 */

#include <etelmm.h>
#include <et_phone.h>
#include <mpbutil.h>
#include "csimtimer.h"
#include "CSimPhone.h"

// CDMA Mode - Timer Values   
//const TInt KNamTimeout=1;

/**
 * The main Phone Security class which simulates security features.
 */
class CSimPhone;
class CTestConfigSection;
class CTestConfigItem;
class CSimPhoneSecurity : public CSubSessionExtBase, public MTimerCallBack
	{
public:
	static CSimPhoneSecurity* NewL(CSimPhone* aPhone);
	CSimPhoneSecurity(CSimPhone* aPhone);
	~CSimPhoneSecurity();

	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);
// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);

// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual void TimerCallBack(TInt aId);	
	const CTestConfigSection* CfgFile();
	//Called by other classes
	TBool IsICCLocked();
	TBool IsPIN2Locked();
	void LockICC();
	void UnLockICC();
	void LockPIN2();
	void UnLockPIN2();
	TBool IsHiddenEnabled();
	void HideEntries();
	void ShowHidden();

	void LockSpc();
	void UnLockSpc();

protected:
	void ConstructL();
public:

	enum TSecurityEvent
		{
		EEventPin1Required,
		EEventPin1Verified,
		EEventPin2Required,
		EEventPin2Verified,
		EEventPuk1Required,
		EEventPuk1Verified,
		EEventPuk2Required,
		EEventPuk2Verified,
		EEventHiddenKeyRequired,
		EEventHiddenKeyVerified,
		EEventUSim1Required,
		EEventUSim1Verified,
		EEventUSim2Required,
		EEventUSim2Verified,
		EEventUniversalRequired,
		EEventUniversalVerified,
		};

	class TSecurityCodes
		{
		public:
			TName iPin1;
			TName iPin2;
			TName iPuk1;
			TName iPuk2;
			TName iPhonePassword;
			TName iSPC;
			TName iPhBkHiddenKey;
			TName iUSimAppPin;
			TName iSecondUSimAppPin;
			TName iUniversalPin;
		};

	class TSecurityCodeRemainigAttempts
		{
		public:
			TInt iPin1;
			TInt iPin2;
			TInt iPuk1;
			TInt iPuk2;
		};

	class TSecurityStatus
		{
		public:
			RMobilePhone::TMobilePhoneLockStatus iPin1;
			RMobilePhone::TMobilePhoneLockStatus iPin2;
			RMobilePhone::TMobilePhoneLockStatus iPuk1;
			RMobilePhone::TMobilePhoneLockStatus iPuk2;
			RMobilePhone::TMobilePhoneLockStatus iPhonePassword;
			RMobilePhone::TMobilePhoneLockStatus iSPC;
			RMobilePhone::TMobilePhoneLockStatus iPhBkHiddenKey;
			RMobilePhone::TMobilePhoneLockStatus iUSimAppPin;
			RMobilePhone::TMobilePhoneLockStatus iSecondUSimAppPin;
			RMobilePhone::TMobilePhoneLockStatus iUniversalPin;
		};
	
	class TSecuritySetting
		{
		public:
			RMobilePhone::TMobilePhoneLockSetting iPin1;
			RMobilePhone::TMobilePhoneLockSetting iPin2;
			RMobilePhone::TMobilePhoneLockSetting iPuk1;
			RMobilePhone::TMobilePhoneLockSetting iPuk2;
			RMobilePhone::TMobilePhoneLockSetting iPhonePassword;
			RMobilePhone::TMobilePhoneLockSetting iSPC;
			RMobilePhone::TMobilePhoneLockSetting iPhBkHiddenKey;
			RMobilePhone::TMobilePhoneLockSetting iUSimAppPin;
			RMobilePhone::TMobilePhoneLockSetting iSecondUSimAppPin;
			RMobilePhone::TMobilePhoneLockSetting iUniversalPin;
		};

		class TICCStatusEntry
			{
			public:
				TInt iDuration;				// < Duration of ICC state
				RMobilePhone::TMobilePhoneSecurityEvent iICCStatus;		// < State of ICC	
			};

		class TSecNotifyData
			{	
			public:
				TBool iNotifyPending;  //indicates if a notification is pending
				TTsyReqHandle iNotifyHandle; //the TSY notification handle
				TAny* iNotifyData;  //Holds the data for the notification pending
				TAny* iNotifyDataXtra;  //Holds the data for the notification pending
			};
			
		class TICCSettingEntry
			{
			public:
				TInt iDuration;				// < Duration of ICC setting
				RMobilePhone::TMobilePhoneLock iLock;	//lock change applies to.
				RMobilePhone::TMobilePhoneLockInfoV1 iLockInfo;
			};


public:

	TInt GetSecurityCaps(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);

	TInt NotifySecurityCapsChange(const TTsyReqHandle aTsyReqHandle, TUint32* aCaps);
	TInt NotifySecurityCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetLockInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock* aLock, TDes8* aLockInfo);
	TInt GetLockInfoCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifyLockInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock* aLock, TDes8* aLockInfo);
	TInt NotifyLockInfoChangeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt SetLockSetting(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneLock* aLock, RMobilePhone::TMobilePhoneLockSetting* aSetting);
	TInt SetLockSettingCancel(const TTsyReqHandle aTsyReqHandle);

	TInt GetSecurityCodeInfo(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode,TDes8* aSecurityCodeInfo);
	TInt GetSecurityCodeInfoCancel(const TTsyReqHandle aReqHandle);

	TInt NotifySecurityCodeInfoChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aSecurityCode,TDes8* aSecurityCodeInfo);
	TInt NotifySecurityCodeInfoChangeCancel(const TTsyReqHandle aReqHandle);

	TInt ChangeSecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aType,const TDes8* aChange);
	TInt ChangeSecurityCodeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt NotifySecurityEvent(const TTsyReqHandle aTsyReqHandle, TDes8* aEvent);
	TInt NotifySecurityEventCancel(const TTsyReqHandle aTsyReqHandle);

	TInt VerifySecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aType, const RMobilePhone::TCodeAndUnblockCode* aCode);
	TInt VerifySecurityCodeCancel(const TTsyReqHandle aTsyReqHandle);

	TInt AbortSecurityCode(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneSecurityCode* aType);

	TInt UnlockNam(const TTsyReqHandle aReqHandle, const RMobilePhone::TMobilePassword* aSpc);
	TInt UnlockNamCancel(const TTsyReqHandle aTsyReqHandle);
	void TimerCallBackUnlockNam();

	TInt LockNamCancel(const TTsyReqHandle aTsyReqHandle);
	void TimerCallBackLockNam();

	void SecurityEvent(RMobilePhone::TMobilePhoneSecurityEvent aEvent);
	
	class CIccTimer : public CBase
		{
	public:
		CIccTimer();
		~CIccTimer();
		static CIccTimer* NewL();
	public:
		TBool iTimerStarted;
		TInt iIndex;
		CSimTimer* iTimer;
		};

private:
	TInt UpdateLockInfo(RMobilePhone::TMobilePhoneLock aLock, const RMobilePhone::TMobilePhoneLockInfoV1& aLockInfo);

	void Pin1Failure();
	void Pin1Success();
	void Pin2Failure();
	void Pin2Success();
	void Puk1Failure();
	void Puk1Success();
	void Puk2Failure();
	void Puk2Success();

	void CompleteNotifyLockInfoChange(RMobilePhone::TMobilePhoneLock aLock, const RMobilePhone::TMobilePhoneLockInfoV1& aLockInfo);
	void CompleteNotifySecurityCodeInfoChange(RMobilePhone::TMobilePhoneSecurityCode aSecurityCode, const RMobilePhone::TMobilePhoneSecurityCodeInfoV5& aSecurityCodeInfo);

	CSimPhone* iPhone;
	
	TSecurityCodes iSecCodes;
	TSecurityCodeRemainigAttempts iSecurityCodeRemainigAttempts;
	TSecurityCodeRemainigAttempts iSecurityCodeDefaultRemainigAttempts;
	CArrayFixFlat<TICCStatusEntry>* iICCStatusProfile;		// <Array containing the indicators changes for this test
	TSecNotifyData iNotifyLock;
	TSecNotifyData iNotifySecurityCodeInfo;
	TNotifyData iNotifyEvent;
	TSecurityStatus iSecStatus;
	TSecuritySetting iSecSetting;
	CArrayFixFlat<TICCSettingEntry>*  iICCSettingProfile;
	
	CIccTimer* iNotifySettingsTimer;
	CIccTimer* iNotifySecurityEventsTimer;
	
	CSimTimer* iUnlockNamTimer;
	TTsyReqHandle iUnlockNamHandle;
	CSimTimer* iLockNamTimer;
	TTsyReqHandle iLockNamHandle;
	};

#endif
