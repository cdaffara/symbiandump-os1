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
* TelephonyActNetwork Active Object class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/


#include <etel.h>
#include <etelmm.h>
#include "Etel3rdParty.h"

class CTelephony;
class CTelephonyFunctions;	// forward reference
class CAsyncRequestBaseAct;

class CGetNetworkRegistrationStatusAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetNetworkRegistrationStatusAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetNetworkRegistrationStatusAct();
	void GetNetworkRegistrationStatus(TDes8& aId);
	static void GetRegistrationStatus(const RMobilePhone::TMobilePhoneRegistrationStatus aMMRegStatus, CTelephony::TRegistrationStatus& aTelRegStatus);
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetNetworkRegistrationStatusAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TNetworkRegistrationV1* iISVNetworkRegistration;
	RMobilePhone::TMobilePhoneRegistrationStatus iMMNetworkRegistration;
	};

/**
This is an active object responsible for interacting with Multimode
ETel.  CGetCurrentNetworkNameAct is used to request current network
information.
*/
class CGetCurrentNetworkInfoAct : public CAsyncRequestBaseAct
	{
	
public:		
	static CGetCurrentNetworkInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetCurrentNetworkInfoAct();
	void GetCurrentNetworkInfo(TDes8& aPckg);

	static void MapNetworkInfo(CTelephony::TEtelISVType* aISVNetworkInfo, RMobilePhone::TMobilePhoneNetworkInfoV2* aMobileNetworkInfo, RMobilePhone::TMobilePhoneLocationAreaV1* aMMArea);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetCurrentNetworkInfoAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TEtelISVType* iISVNetworkInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 iMMArea;
	TInt iPostedIPC;
	RMobilePhone::TMobilePhoneNetworkInfoV2 iMMNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg iMMNetworkInfoPckg;
	};

/**
This is an active object responsible for interacting with Multimode
ETel.  CGetCurrentNetworkNameAct is used to request current network
name.
*/
class CGetCurrentNetworkNameAct : public CAsyncRequestBaseAct
	{
	
public:		
	static CGetCurrentNetworkNameAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetCurrentNetworkNameAct();
	void GetCurrentNetworkName(TDes8& aPckg);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetCurrentNetworkNameAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TEtelISVType* iISVCurrentNetworkName;
	RMobilePhone::TMobilePhoneNetworkNameV3  iMMNetworkNameV3;
	RMobilePhone::TMobilePhoneNetworkNameV3Pckg iMMNetworkNameV3Pckg;
	RMobilePhone::TMobilePhoneOPlmnV3 iLocInfo;
	RMobilePhone::TMobilePhoneOPlmnV3Pckg iLocInfoPckg;
	};
	
class CGetOperatorNameAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CGetOperatorNameAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CGetOperatorNameAct();
	void GetOperatorName(TDes8& aId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CGetOperatorNameAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TOperatorNameV1* iISVOperatorName;
	RMobilePhone::TMobilePhoneServiceProviderNameV2 iMMServiceProviderName;
	RMobilePhone::TMobilePhoneServiceProviderNameV2Pckg iMMServiceProviderNamePckg;
	};

class CNotifyNetworkRegistrationStatusAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyNetworkRegistrationStatusAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifyNetworkRegistrationStatusAct();
	void NotifyNetworkRegistrationStatus(TDes8& aId);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CNotifyNetworkRegistrationStatusAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TNetworkRegistrationV1* iISVNetworkRegistration;
	RMobilePhone::TMobilePhoneRegistrationStatus iMMNetworkRegistration;
	};

/**
This is an active object responsible for interacting with Multimode
ETel.  Its use is for posting notification requests on current network
information changes.
*/
class CNotifyCurrentNetworkInfoAct : public CAsyncRequestBaseAct
	{
	
public:		
	static CNotifyCurrentNetworkInfoAct* NewL(CTelephonyFunctions* aTelephonyFunctions);
	~CNotifyCurrentNetworkInfoAct();
	void NotifyCurrentNetworkInfo(TDes8& aPckg);
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CNotifyCurrentNetworkInfoAct(CTelephonyFunctions* aTelephonyFunctions);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TEtelISVType* iISVNetworkInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 iMMArea;
	TInt iPostedIPC;
	RMobilePhone::TMobilePhoneNetworkInfoV2 iMMNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV2Pckg iMMNetworkInfoPckg;
	};
