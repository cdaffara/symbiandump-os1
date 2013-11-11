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

#ifndef __SIMNTRAS_H__
#define __SIMNTRAS_H__

/**
 * @file
 * Header for NTRAS access class.
 * @internalAll
 */

#include "CSimSerComm.h"
#include "csimtimer.h"
#include "CSimPhone.h"

_LIT(KDefaultCsyName,"ECUART.CSY");			//< Default CSY
_LIT(KDefaultPortName,"COMM::0");			//< Default port name
_LIT(KDefaultSetting,"DEF");				//< Default commSettings (context independent)

const TBps KDefaultCommPortRate=EBps115200;	//< Default data rate
const TUint KDefaultHandshake=20;			//< Default handshaking

const TInt KReadBufferSize=10;		// < Read buffer size - used for storing the results from a single read request.
const TInt KInputBufferSize=100;	// < Input buffer size - used for accumulating the received data from the NTRas server.

class MNTRasCallBack;

/**
 * This class attempts an NTRas connection given a set of port and configuration settings.
 */
class CSimNtRas : public CBase, MComm, MTimerCallBack
	{
public:
	static CSimNtRas* NewL(CSimPhone* aPhone);
	~CSimNtRas();
	TInt Connect(const TDesC& aCsyName, const TDesC& aPort,
				 const TCommConfigV01& aConfig, MNTRasCallBack* aCallback);
	void Cancel();
	void Terminate();

private:
	CSimNtRas(CSimPhone* aPhone);
	void ConstructL();
	void TimerCallBack(TInt aId);
	void CommReadComplete(TInt aStatus);
	void CommWriteComplete(TInt aStatus);
	void AttemptConnect();

private:
	CSimPhone* iPhone;
	MComm* iComm;						// < Pointer to Comm Port read/write class
	CSimTimer* iTimer;					// < Pointer to timer class
	MNTRasCallBack* iCallback;			// < Pointer to callback class to be used when connection attempt has completed.
	TBuf8<KReadBufferSize> iReadBuf;	// < Read buffer - used to store the results from a single read request.
	TBuf8<KInputBufferSize> iInputBuf;	// < Input buffer - used to accumulate the received data from the NTRas server.
	TBool iPortOpen;					// < Flag indicating the open status of the port.
	TInt iAttemptCnt;					// < Transmission retry counter
	};

/**
 * This mixin class contains the pure virtual function that is used when the NTRas
 * connection attempt completes.
 */
class MNTRasCallBack
	{
public:
	virtual void NTRasCallBack(TInt aStatus)=0;
	};

#endif
