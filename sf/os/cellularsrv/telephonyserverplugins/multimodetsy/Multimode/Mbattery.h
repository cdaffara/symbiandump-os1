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
// This queries the ME (Mobile Equipment) battery, using a standard (ETSI GSM 07.07 April 1997) AT command : AT+CBC
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MMBATTERY_H__
#define __MMBATTERY_H__

#include "ATBASE.H"
#include "etelmm.h"

class CATGetBattery : public CATCommands
{
public:
	static CATGetBattery* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	CATGetBattery(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGetBattery();
	void ConstructL();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	RMobilePhone::TMobilePhoneBatteryStatus ConvertBatteryStatus(TInt aBatteryStatus);
private:
	virtual void Complete(TInt aError,TEventSource aSource);
	void ParseBatteryResponseL();

	enum {
		EATNotInProgress,
		EATBatteryWaitForWriteComplete,
		EATBatteryReadComplete
		} iState;

	RMobilePhone::TMobilePhoneBatteryInfoV1* iBatteryInfo;
	CPhoneGlobals* iPhoneGlobals;	
};
#endif
