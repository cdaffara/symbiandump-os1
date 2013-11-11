/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* TelephonyActPhone Active Object class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include <etel.h>
#include <etelmm.h>
#include "Etel3rdParty.h"

class CTelephony;
class CTelephonyFunctions;	
class CAsyncRequestBaseAct;

class CGetPhoneIdAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetPhoneIdAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetPhoneIdAct();
	void GetPhoneId(TDes8& aId);
			
protected:
	void Complete();//from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode);//from CActive
	void DoCancel();//from CActive
	
private:	
	CGetPhoneIdAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TPhoneIdV1* iISVphoneId;
	RMobilePhone::TMobilePhoneIdentityV1 iMMphoneId;		
	};

class CGetSubscriberIdAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetSubscriberIdAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetSubscriberIdAct();
	void GetSubscriberId(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CGetSubscriberIdAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TSubscriberIdV1* iISVSubscriberIdV1;
	RMobilePhone:: TMobilePhoneSubscriberId iMMSubscriberId;		
	};
	
class CGetIndicatorAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetIndicatorAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetIndicatorAct();
	void GetIndicator(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CGetIndicatorAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TIndicatorV1* iISVIndicator;
	TUint32 iIndicator;
	TUint32 iIndicatorCaps;		
	};

class CGetBatteryInfoAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetBatteryInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetBatteryInfoAct();
	void GetBatteryInfo(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CGetBatteryInfoAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TBatteryInfoV1* iISVBatteryInfo;
	RMobilePhone::TMobilePhoneBatteryInfoV1 iMMBatteryInfo;
	};

class CGetSignalStrengthAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetSignalStrengthAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetSignalStrengthAct();
	void GetSignalStrength(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CGetSignalStrengthAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TSignalStrengthV1* iISVSignalStrength;
	TInt32 iSignalStrength;
	TInt8 iBar;
	};

class CGetIccLockInfoAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetIccLockInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetIccLockInfoAct();
	void GetIccLockInfo(CTelephony::TIccLock aLock, TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CGetIccLockInfoAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TIccLockInfoV1* iISVLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1 iMMLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg iMMLockInfo1Pckg;
	};
	
class CSendDTMFTonesAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CSendDTMFTonesAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CSendDTMFTonesAct();
	void SendDTMFTones(const TDesC& aTones);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive

	
private:	
	CSendDTMFTonesAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	};

class CFlightModeChangeAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CFlightModeChangeAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CFlightModeChangeAct();
	void FlightModeChangeL(TDes8& aDes);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CFlightModeChangeAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TFlightModeV1* iFlightMode;
	};

class CNotifyIndicatorAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyIndicatorAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifyIndicatorAct();
	void NotifyIndicator(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CNotifyIndicatorAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TIndicatorV1* iISVIndicator;
	TUint32 iIndicator;
	TUint32 iIndicatorCaps;		
	};

class CNotifyBatteryInfoAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyBatteryInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifyBatteryInfoAct();
	void NotifyBatteryInfo(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CNotifyBatteryInfoAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TBatteryInfoV1* iISVBatteryInfo;
	RMobilePhone::TMobilePhoneBatteryInfoV1 iMMBatteryInfo;
	};

class CNotifySignalStrengthAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifySignalStrengthAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifySignalStrengthAct();
	void NotifySignalStrength(TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CNotifySignalStrengthAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TSignalStrengthV1* iISVSignalStrength;
	TInt32 iSignalStrength;
	TInt8 iBar;
	};


class CNotifyIccLockInfoAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyIccLockInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifyIccLockInfoAct();
	void NotifyIccLockInfo(CTelephony::TIccLock aLock, TDes8& aId);
			
protected:	
	void Complete(); //from CAsyncRequestBaseAct
	TInt RunError(TInt aLeaveCode); //from CActive
	void DoCancel(); //from CActive
	
private:	
	CNotifyIccLockInfoAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	//Pointer to the client data where notify Pin1 lock info will be stored
	CTelephony::TIccLockInfoV1* iISVPin1LockInfo;
	//Pointer to the client data where notify Pin2 lock info will be stored
	CTelephony::TIccLockInfoV1* iISVPin2LockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1 iMMLockInfo;
	RMobilePhone::TMobilePhoneLockInfoV1Pckg iMMLockInfoV1Pckg;
	RMobilePhone::TMobilePhoneLock iMMLock;
	};
