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
// Contains definition for classes that implement the battery charging state
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSimBatteryCharger_H__
#define __CSimBatteryCharger_H__

#include <et_phone.h>
#include "csimtimer.h"
#include <etelmm.h>

class CSimPhone;
class CTestConfigSection;

/**
 * Implements the RMobilePhone based functions that constitute the Battery charger
 * functionality provided by the SIM TSY.
 */
class CSimBatteryCharger : public CBase, MTimerCallBack
	{
public:
	static CSimBatteryCharger* NewL(CSimPhone* aPhone);
	CSimBatteryCharger(CSimPhone* aPhone);
	~CSimBatteryCharger();
	TInt GetBatteryCaps(TTsyReqHandle aReqHandle,TDes8* aCaps);
	TInt GetBatteryInfo(TTsyReqHandle aReqHandle, TDes8* aInfo);
	TInt NotifyBatteryInfoChange(TTsyReqHandle aReqHandle, TDes8* aInfo);
	void NotifyBatteryInfoCancel();
	const CTestConfigSection* CfgFile();

	inline RMobilePhone::TMobilePhoneBatteryStatus Status() const {return iCurrentStatus;}

private:
	void ConstructL();
	void TimerCallBack(TInt aId);

private:
	CSimPhone* iPhone;		// < Pointer to the parent phone class.
	CSimTimer* iTimer;		// < Pointer to the owned timer class.
	
	struct TBatteryChargerInfo
		{
		TInt iDuration;											//< Duration of Battery Charger state
		RMobilePhone::TMobilePhoneBatteryStatus	iStatus;		//< Status of the Battery Charger
		TUint iChargeLevel;										//< The Battery level
		TInt iError;											//< Request complete with this error code
		};

	CArrayFixFlat<TBatteryChargerInfo>* iBatteryChargerInfo;		//< Array containing the Battery Charger changes for this test
	TInt iBatteryChargerIndex;								//< Current index from iBatteryChargerInfo array
	TInt iCurrentBatteryCharger;							//< Current value of the Battery Charger 
	RMobilePhone::TMobilePhoneBatteryStatus iCurrentStatus;	//< Current status of the battery charger
	TInt iCurrentErr;		//< error code to return to client

	TBool iBatteryChargerNotificationPending;				//< Battery Charger notification request outstanding
	TTsyReqHandle iBatteryChargerNotificationReqHandle;		//< Battery Charger notification outstanding request handle
	RMobilePhone::TMobilePhoneBatteryInfoV1* iBatteryChargerNofificationValue;				//< Battery Charger value pointer
	};

#endif
