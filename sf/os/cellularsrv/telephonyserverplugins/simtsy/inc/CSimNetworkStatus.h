// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definitions for classes that implement network orientated functionality.
// 
//

/**
 @file
 @internalAll
*/

#ifndef CSIMNETWORKSTATUS_H__
#define CSIMNETWORKSTATUS_H__

#include <etelmm.h>
#include <et_phone.h>
#include "csimtimer.h"
#include "CSimPubSubChange.h"

class CSimPhone;
class CTestConfigSection;

/**
 * Implements the RMobilePhone based functions that constitute the network orientated
 * functionality provided by the SIM TSY.
 */
class CSimNetworkStatus : public CBase, MTimerCallBack, MPubSubChangeCallback
	{
public:
	static CSimNetworkStatus* NewL(CSimPhone* aPhone);
	CSimNetworkStatus(CSimPhone* aPhone);
	~CSimNetworkStatus();
	TInt GetNetworkCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt GetHomeNetwork(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt GetCurrentNetwork(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt GetCurrentNetworkNoLocation(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt GetNetworkRegistrationStatus(TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt NotifyNetworkRegistrationStatusChange(TTsyReqHandle aReqHandle,TDes8* aPckg);
	void NotifyNetworkRegistrationStatusChangeCancel();
	TInt NotifyCurrentNetworkChange(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes8* aPckg2);
	void NotifyCurrentNetworkChangeCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyCurrentNetworkNoLocationChange(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	void NotifyCurrentNetworkNoLocationChangeCancel(const TTsyReqHandle aTsyReqHandle);	
	
	TInt GetNITZInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt NotifyNITZInfoChange(TTsyReqHandle aReqHandle,TDes8* aPckg);
	void NotifyNITZInfoChangeCancel();
	TInt GetCurrentServiceProvider(TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt GetCurrentNetworkName(TTsyReqHandle aReqHandle,TDes8* aNetworkName, TDes8* aLocInfo);

	TInt GetCellInfo(TTsyReqHandle aReqHandle,TDes8* aPckg);
	TInt NotifyCellInfoChange(TTsyReqHandle aReqHandle,TDes8* aPckg);
	void NotifyCellInfoChangeCancel(TTsyReqHandle aTsyReqHandle);
	
	const CTestConfigSection* CfgFile();
	RMobilePhone::TMobilePhoneNetworkStatus NetWorkStatus();
	inline RMobilePhone::TMobilePhoneRegistrationStatus RegistrationStatus() { return static_cast<RMobilePhone::TMobilePhoneRegistrationStatus>(iCurrentRegStatus);};

private:
	void ConstructL();
	void TimerCallBack(TInt aId);
	void TimerCallBackRegStat();
	void PubSubCallback(TInt aRegStat);
	void TimerCallBackNitz();
	void TimerCallBackCellInfo();
	void TimerCallBackRadioModeL();
	void PopulateNetworkInfoV1(TInt aIndex,RMobilePhone::TMobilePhoneNetworkInfoV1& aNetInfo);
	void PopulateEtel3rdPartyNetworkInfoV1(TInt aIndex,RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1& aNetInfo);
	void PopulateNitzNetworkNames(RMobilePhone::TMobilePhoneNITZ& aNitz);

private:
	CSimPhone* iPhone;			// < Pointer to the parent phone class.
	CSimTimer* iRegStatTimer;	// < Pointer to the owned Registration Status timer class.
	CSimPubSubChange* iRegStatChange;	// < Object for checking registration status change variable
	CSimPubSubChange* iPSRegStatChange;	// < Object for checking registration status change variable using new P&S uids
	CSimTimer* iNitzTimer;		// < Pointer to the owned NITZ timer class.
	CSimTimer* iRadioModeTimer;	// < Pointer to the owned RadioMode timer class.
	CSimTimer* iCellInfoTimer;  // < Pointer to the owned cell info timer class.
	
	struct TRegStatus
		{
		TInt iDuration;												// < Duration of Registration Status
		RMobilePhone::TMobilePhoneRegistrationStatus iRegStatus;	// < Registration Status that will be maintained through period iDuration
		TInt iNetworkInfoIndex;										// < Network index (into iNetworkInfo) that will be maintained through period iDuration (optional)
		};

	CArrayFixFlat<TRegStatus>* iRegStatusInfo;						// < List of network registration changes.
	TInt iRegStatIndex;					// < Current index from the iRegStatInfo array
	TInt iCurrentRegStatus;				// < Current Registration Status.
	CArrayFixFlat<RMobilePhone::TMobilePhoneNetworkInfoV2>*  iNetworkInfo;	// < List of network information.
	CArrayFixFlat<RMobilePhone::TMobilePhoneNetworkInfoV8>*  iNetworkInfoV8;	// < List of network information for V8 class.
	CArrayFixFlat<RMobilePhone::TMobilePhoneLocationAreaV1>* iLocationArea;	// < List of location area.
	TInt iCurrentNetworkIndex;			// < Current index from the iNetworkInfo array
	struct TNitzInfoAndDuration
		{
		TInt iDuration;							// < Duration when this NITZ information is valid.
		RMobilePhone::TMobilePhoneNITZ iNitz;	// < NITZ information valid for above period.
		};
	CArrayFixFlat<TNitzInfoAndDuration>* iNitzInfo;	// < List of NITZ information.
	TInt iNitzIndex;								// < Current index from the iNitzInfo array
	RMobilePhone::TMobilePhoneNITZ iCurrentNitzInfo;

	TBool iRegStatChangeNotificationPending;			// < Registration Status notification request outstanding
	TTsyReqHandle iRegStatChangeNotificationReqHandle;	// < Registration Status notification outstanding request handle
	RMobilePhone::TMobilePhoneRegistrationStatus* iRegStatChangeNotificationRegStatus;	// < Registration Status notification pointer

	TBool iCurrNetChangeV1NotificationPending;			// < Current network notification request outstanding for V1 paramater class
	TTsyReqHandle iCurrNetChangeV1NotificationReqHandle;	// < Current network notification outstanding request handle  for V1 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV1* iCurrNetChangeV1NotificationNetInfo;	// < Current network notification data pointer  for V1 paramater class

	TBool iCurrNetChangeV2NotificationPending;			// < Current network notification request outstanding for V2 paramater class
	TTsyReqHandle iCurrNetChangeV2NotificationReqHandle;	// < Current network notification outstanding request handle  for V2 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV2* iCurrNetChangeV2NotificationNetInfo;	// < Current network notification data pointer  for V2 paramater class
	
	TBool iCurrNetChangeV5NotificationPending;			// < Current network notification request outstanding for V5 paramater class
	TTsyReqHandle iCurrNetChangeV5NotificationReqHandle;	// < Current network notification outstanding request handle  for V5 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV5* iCurrNetChangeV5NotificationNetInfo;	// < Current network notification data pointer  for V5 paramater class
	
	TBool iCurrNetChangeV8NotificationPending;			// < Current network notification request outstanding for V8 paramater class
	TTsyReqHandle iCurrNetChangeV8NotificationReqHandle;	// < Current network notification outstanding request handle  for V8 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV8* iCurrNetChangeV8NotificationNetInfo;	// < Current network notification data pointer  for V8 paramater class
	
	TBool iCurrNetChangeNoLocV1NotificationPending;			// < Current network notification request outstanding for V1 paramater class
	TTsyReqHandle iCurrNetChangeNoLocV1NotificationReqHandle;	// < Current network notification outstanding request handle  for V1 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV1* iCurrNetChangeNoLocV1NotificationNetInfo;	// < Current network notification data pointer  for V1 paramater class

	TBool iCurrNetChangeNoLocV2NotificationPending;			// < Current network notification request outstanding for V2 paramater class
	TTsyReqHandle iCurrNetChangeNoLocV2NotificationReqHandle;	// < Current network notification outstanding request handle  for V2 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV2* iCurrNetChangeNoLocV2NotificationNetInfo;	// < Current network notification data pointer  for V2 paramater class
	
	TBool iCurrNetChangeNoLocV5NotificationPending;			// < Current network notification request outstanding for V5 paramater class
	TTsyReqHandle iCurrNetChangeNoLocV5NotificationReqHandle;	// < Current network notification outstanding request handle  for V5 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV5* iCurrNetChangeNoLocV5NotificationNetInfo;	// < Current network notification data pointer  for V5 paramater class
	
	TBool iCurrNetChangeNoLocV8NotificationPending;			// < Current network notification request outstanding for V8 paramater class
	TTsyReqHandle iCurrNetChangeNoLocV8NotificationReqHandle;	// < Current network notification outstanding request handle  for V8 paramater class
	RMobilePhone::TMobilePhoneNetworkInfoV8* iCurrNetChangeNoLocV8NotificationNetInfo;	// < Current network notification data pointer  for V8 paramater class
	
	TBool iCurrNetChangeEtelIsvNotificationPending;			// < Current network notification request outstanding for V2 paramater class
	TTsyReqHandle iCurrNetChangeEtelIsvNotificationReqHandle;	// < Current network notification outstanding request handle  for V2 paramater class
	RMobilePhone::TEtel3rdPartyMobileNetworkInfoV1* iCurrNetChangeEtelIsvNotificationNetInfo;	// < Current network notification data pointer  for V2 paramater class
	
	TBool iCurrentNetworkChangeLocationAreaNotificationPending;			// < Current network notification request outstanding for location area class
	RMobilePhone::TMobilePhoneLocationAreaV1* iCurrentNetworkChangeLocationArea; // < Current network notification data pointer  for location area class
	
	TBool iNitzChangeNotificationPending;			// < NITZ notification request outstanding
	TTsyReqHandle iNitzChangeNotificationReqHandle;	// < NITZ notification outstanding request handle
	RMobilePhone::TMobilePhoneNITZ* iNitzChangeNotificationInfo;	// < NITZ notification data pointer
	RMobilePhone::TMobilePhoneServiceProviderNameV2 iServiceProvider;	// < Service provider details
	struct TRadioMode
		{
		TInt iDuration;	// < Duration of Registration Status
		TBool iRadioOn;	// < Radio (RF) On or Off 
		};
	TRadioMode iRadioMode;
	CArrayFixFlat<TRadioMode>* iRadioModeInfo;
	TInt iRadioModeIndex;								// < Current index from the radioMode array

	struct TCellInfoAndDuration
		{
		TInt iDuration;							    // < Duration when this cell information is valid.
		RMobilePhone::TMobilePhoneCellInfoV9 iInfo;	// < cell information valid for above period.
		};
	CArrayFixFlat<TCellInfoAndDuration>* iCellInfo;	// < List of Cell information.
	TInt iCellInfoIndex;	    					// < Current index from the iNitzInfo array
	RMobilePhone::TMobilePhoneCellInfoV9 iCurrentCellInfo;
	TBool iCellInfoChangeNotificationPending;			// < Cell info notification request outstanding
	TTsyReqHandle iCellInfoChangeNotificationReqHandle;	// < Cell info notification outstanding request handle
	RMobilePhone::TMobilePhoneCellInfoV9* iCellInfoChangeNotificationInfo;	// < cell Info notification data pointer

private:
	void SetNetworkMode();

	CSimTimer* iRoamStatTimer;	// < Pointer to the owned Roaming Status timer class.
	void FindRoamStatSettings();

	class TRoamStatClient
		{
		public:
			TTsyReqHandle iReqHandle;
			TUint8* iRoamingStatus;
		};

	TInt iRoamStatGetTimeout;
	CArrayFixFlat<TRoamStatClient>* iRoamStatGetClients;
	CArrayFixFlat<TRoamStatClient>* iRoamStatNotifyClients;
	};

#endif
