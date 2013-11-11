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
// This queries the ME (Mobile Equipment) signal quality, using a standard (ETSI GSM 07.07 April 1997) AT command : AT+CSQ
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MMSIGNAL_H__
#define __MMSIGNAL_H__

#include "ATBASE.H"

class CATGetSignal : public CATCommands
{
public:
	static CATGetSignal* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	CATGetSignal(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGetSignal();
	void ConstructL();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	TInt8 NumberOfSignalBars(TInt32 aSignalStrength);

	struct TTsySignalInfo
		{
		TInt32* iSignalStrength;
		TInt8* iBar;
		};

private:
	void Complete(TInt aError, TEventSource aSource);
	void ParseSignalResponseL();
	
	enum {
		EATNotInProgress,
		EATSignalWaitForWriteComplete,
		EATSignalReadComplete
		} iState;

	// Values of <rssi> parameter in AT+CSQ response
	enum {
		KSignalStrengthMinus113dBm = 0,
		KSignalStrengthMinus101dBm = 6,
		KSignalStrengthMinus89dBm = 12,
		KSignalStrengthMinus77dBm = 18,
		KSignalStrengthMinus53dBm = 30,
		KSignalStrengthMinus51dBm = 31
		};

	TTsySignalInfo iSignalInfo;
};
#endif
