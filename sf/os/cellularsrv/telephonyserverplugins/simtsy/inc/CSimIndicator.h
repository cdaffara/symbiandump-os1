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
// Contains definition for classes that implement the indicator flags functionality
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMINDICATOR_H__
#define __CSIMINDICATOR_H__

#include <et_phone.h>
#include "csimtimer.h"
#include <etelmm.h>
#include "CSimBatteryCharger.h"
#include "CSimNetworkStatus.h"

class CSimPhone;
class CTestConfigSection;

/**
 * Implements the RMobilePhone based functions that constitute the Indicator
 * functionality provided by the SIM TSY.
 */
class CSimIndicator : public CBase, MTimerCallBack
	{
public:
	static CSimIndicator* NewL(CSimPhone* aPhone);
	CSimIndicator(CSimPhone* aPhone);
	~CSimIndicator();
	TInt GetIndicatorCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt GetIndicator(TTsyReqHandle aReqHandle, TDes8* aPckg1);
	TInt NotifyIndicatorChange(TTsyReqHandle aReqHandle, TDes8* aPckg1);
	void NotifyIndicatorChangeCancel();
	const CTestConfigSection* CfgFile();
	void CheckNotification();

private:
	void ConstructL();
	TUint32 IndicatorState();
	void TimerCallBack(TInt aId);

private:
	CSimPhone* iPhone;		// < Pointer to the parent phone class.
	CSimTimer* iTimer;		// < Pointer to the owned timer class.
	CSimBatteryCharger* iCharger; // < Pointer to the owned battery Charger
	CSimNetworkStatus* iNetwork; //< pointer to the network status object
	TInt iIndicatorCheckPeriod;
	
	struct TPhoneIndicatorsInfo
		{
		TInt iNotificationNumber;	//< notification number (Nth notification)
		TInt iError;		//< Error to return to client on this nth notification
		};

	CArrayFixFlat<TPhoneIndicatorsInfo>* iIndicatorsInfo;		// <Array containing the indicators changes for this test
	TUint32 iCurrentIndicator;							// <Current value of the indicator
	TInt iNotificationNumber;

	TBool iIndicatorsChangeNotificationPending;				// <Indicators notification request outstanding
	TTsyReqHandle iIndicatorsChangeNotificationReqHandle;		// <Indicators notification outstanding request handle
	TUint32* iIndicatorsChangeNofificationValue;				// <Indicators value pointer
	};

#endif
