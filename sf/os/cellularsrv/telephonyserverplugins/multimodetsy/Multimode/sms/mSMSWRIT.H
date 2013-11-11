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
// Header file contains the CATSmsMessagingWrite class. This class is responsible for 
// implementing the AT+CMGW command. (As per ETSI Standard, GSM 07.05 version 4.5.0)
// This command basically writes a pdu message to the preferred memory. 
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MSMSWRIT_H__
#define __MSMSWRIT_H__

#include "smsbase.H"
#include "mSMSSTOR.H"


class CATSmsMessagingWrite : public CATSmsCommands
	{
public:
	static CATSmsMessagingWrite* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aMmStatus);
	CATSmsMessagingWrite(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aMmStatus);
	~CATSmsMessagingWrite();
	void Start(TTsyReqHandle aTsyReqHandle,TAny* aParams);
	void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	void Complete(TInt aError);
	virtual void Complete(TInt aError,TEventSource aSource);

private:
	void ParseResponseL();
	TInt SetMsgStatus();
	void StartPduWrite();

private:
	CCommChatString* iExpectString;
	enum {
		EATNotInProgress,
		EATWaitForSendingPduLengthComplete,
		EATReadSmsRefNumCompleted,
		EATReadPduEnterPduModeResponseCompleted,
		EATWaitForSendingPduComplete,
		EATCancellingWaitForWriteComplete,
		EATCancellingReadCompleted,
		EATWaitForSendingPrefMemComplete,
		EATWaitForPrefMemResponse
		} iState;

	TBuf8<KMaxAsciiPduLength> iTxBuffer;

	RMobileSmsStore::TMobileGsmSmsEntryV1* iSmsEntry;

	TBool iCancelled;
	CMobileSmsStore* iSmsStore;
	CPhoneGlobals::TPhoneSmsPduStd iNextAttempt;
	TBuf8<KMaxAsciiPduLength> iMsgDataAscii;
	};

#endif
