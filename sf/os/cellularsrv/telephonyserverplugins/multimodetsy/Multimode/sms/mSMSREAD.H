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

/**
 @file
 @internalAll 
*/

#ifndef __MSMSREAD_H__
#define __MSMSREAD_H__

#include "smsbase.H"
#include "mSMSSTOR.H"
#include "et_phone_util.h"

typedef TBuf8<400> TSpecialPdu;

class CATSmsMessagingRead : public CATSmsCommands
	{
public:
	static CATSmsMessagingRead* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,
									 CPhoneGlobals* aGsmStatus);

	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void Stop(TTsyReqHandle aTsyReqHandle);
	void StopReadSms(TTsyReqHandle aTsyReqHandle);
	void Complete(TInt aError);
	void Complete(TTsyReqHandle aReqHandle,TInt aError);
	virtual void Complete(TInt aError, TEventSource aSource);
	RMobileSmsStore::TMobileSmsStoreStatus GsmMsgStatusToStoreStatus(TInt aMsgStat);

private:
	CATSmsMessagingRead(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,
						CPhoneGlobals* aMmStatus);
	void CMGRResponseL();
	void StartReadSequence();
	void PopulateScaFieldsAndRemove(TPtrC8& aPdu, RMobilePhone::TMobileAddress& aGsmServiceCentre);

private:
	enum {
		EATNotInProgress,
		EATWaitForSendingCMGRRequestComplete,
		EATWaitForCMGRResponseComplete,
		EATWaitForSendingPrefMemComplete,
		EATWaitForPrefMemResponse
	} iState;

	TBuf8<KGenericBufferSize> iTxBuffer; 
	RMobileSmsStore::TMobileGsmSmsEntryV1* iMsgEntry;	
	TSpecialPdu iPdu;
	TInt iMsgLength;
	TInt iMsgLocation;			
	RMobilePhone::TMobileAddress iGsmServiceCentre;
	TStorageType iShortStoreName;
	CMobileSmsStore* iSmsStore;
	TBool iCancelled;
	};

#endif




