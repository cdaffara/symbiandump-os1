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

#ifndef __MSMSMEM_H__
#define __MSMSMEM_H__

#include "smsbase.H"
#include "mSMSSTOR.H"

const TInt KStorageArraySize=5;

struct TMemType
	{
	TBuf8<12> iStoreName;
	TUint32	  iMems;
	};

enum
	{
	KMemTypeReadAndDeleted=0x00000001,
	KMemTypeWriteAndSend=0x00000002,
	KMemTypeBroadcast=0x00000004
	};

class CATSmsMemoryStorage : public CATSmsCommands
	{
public:
	static CATSmsMemoryStorage* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aGsmStatus);
	CATSmsMemoryStorage(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aGsmStatus);
	~CATSmsMemoryStorage();
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void StartGetInfo(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void Stop(TTsyReqHandle aTsyReqHandle);
	void Complete(TInt aError);
	virtual void Complete(TInt aError, TEventSource aSource);
	TInt GetNumberOfMessageStores();
	TInt WhichPreferredMem(TInt aIndex);
	void EnumerateCPMSResponseL();
	void CopyDataFromCATInit(CATInit* aInit);
	void GetPrefMemL();
private:
	void ConstructL();
	void CPMSResponseL();
	TInt SMSStoreCaps(TDes& aName);
private:
	enum {
		EATNotInProgress,
		EATWaitForSettingMem,
		EATWaitForSettingMemComplete,
		EATWaitForSendingCPMSRequestComplete,
		EATWaitForCPMSResponseComplete
		} iState;

	TStorageType iRequestedMem;
	TInt iClientPreferredMem;
	TBuf8<KGenericBufferSize> iTxBuffer;

	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg*	iInfoPckg;

	TInt iStoreCounter;
	TMemType iPeekPrevStorage[KStorageArraySize];
	CMobileSmsStore* iSmsStore;
	TBool iCancelled;
	};

#endif
