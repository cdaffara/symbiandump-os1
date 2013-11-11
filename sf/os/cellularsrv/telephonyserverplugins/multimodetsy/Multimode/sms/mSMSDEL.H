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

#ifndef __MSMSDEL_H__
#define __MSMSDEL_H__

#include "smsbase.H"


class CMobileSmsStore;
class CATSmsStorageDelete : public CATSmsCommands
	{
public:
	static CATSmsStorageDelete* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	CATSmsStorageDelete(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATSmsStorageDelete();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	void Complete(TInt aError);
	virtual void Complete(TInt aError, TEventSource aSource);
	
private:
	TInt ValidateExpectStringsWithCmsError();
	void ConstructL();
	void StartDeleteSequence();
private:
	enum {
		EATNotInProgress,
		EATWaitForSendingDeleteComplete,
		EATWaitForOKResponseComplete,
		EATWaitForSendingPrefMemComplete,
		EATWaitForPrefMemResponse
		} iState;

	TBuf8<KGenericBufferSize> iTxBuffer;
	TInt iDeleteIndex; 
	TBool iSmsDeleted;
	TBool iCancelled;

	CMobileSmsStore* iSmsStore;
	};

#endif
