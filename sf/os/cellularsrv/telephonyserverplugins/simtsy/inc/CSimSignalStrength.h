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
// Contains definitions for classes that implement signal strength functionality.
// 
//

/**
 @file
 @internalAll
*/

#ifndef CSIMSIGNALSTRENGTH_H__
#define CSIMSIGNALSTRENGTH_H__

#include <et_phone.h>
#include "csimtimer.h"
#include "CSimPubSubChange.h"

class CSimPhone;
class CTestConfigSection;

/**
 * Implements the RMobilePhone based functions that constitute the signal strength
 * functionality provided by the SIM TSY.
 */
class CSimSignalStrength : public CBase, MTimerCallBack, MPubSubChangeCallback
	{
public:
	static CSimSignalStrength* NewL(CSimPhone* aPhone);
	CSimSignalStrength(CSimPhone* aPhone);
	~CSimSignalStrength();
	TInt GetSignalCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	TInt GetSignalStrength(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	TInt NotifySignalStrengthChange(TTsyReqHandle aReqHandle,TDes8* aPckg1, TDes8* aPckg2);
	void NotifySignalStrengthChangeCancel();
	const CTestConfigSection* CfgFile();
	void PubSubCallback(TInt aNewVal);

private:
	void ConstructL();
	void TimerCallBack(TInt aId);

private:
	CSimPhone* iPhone;		// < Pointer to the parent phone class.
	CSimTimer* iTimer;		// < Pointer to the owned timer class.
	struct TSignalStrengthInfo
		{
		TInt iDuration;		// < Duration of signal strength level
		TInt32 iLevel;		// < Signal strength level that will be maintained through period iDuration
		TInt8 iBar;			// < Signal strength bar level that will be maintained through period iDuration
		TInt iErr;			//< The error code to return to the client when completing the request.  default is KErrNone
		};
	CArrayFixFlat<TSignalStrengthInfo>* iSignalStrengthInfo;	// < Array containing all the signal strength changes for this test.
	TInt iSSIndex;			// < Current index from the iSignalStrengthInfo arrays
	TInt32 iCurrentLevel;	// < Current signal strength level
	TInt8 iCurrentBar;		// < Current signal strength bar level
	TInt iCurrentError;		//< Current error to complete request with

	TBool iSSChangeNotificationPending;				// < Signal Strength notification request outstanding
	TTsyReqHandle iSSChangeNotificationReqHandle;	// < Signal Strength notification outstanding request handle
	TInt32* iSSChangeNotificationSSLevel;			// < Signal Strength notification level pointer
	TInt8* iSSChangeNotificationSSBarLevel;			// < Signal Strength notification bar pointer
	TInt iNotifyCapability;							// < Signal Strength notification capability
	CSimPubSubChange* iSignalStrengthChange;
	CSimPubSubChange* iPSSignalStrengthChange;
	};

#endif
