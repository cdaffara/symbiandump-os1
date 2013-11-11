// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Deal with Subscriber Id
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MIMSI_H__
#define __MIMSI_H__

#include "ATBASE.H"
#include <etelmm.h>

class CATSubscriberId : public CATCommands
	{
public:
	static CATSubscriberId* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATSubscriberId();
//
	inline TBool IsNotSupported();
	TBool CachedValue(TDes& aImsi);
private:
	CATSubscriberId(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
//
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
//
	virtual void Complete(TInt aError, TEventSource aSource);
	void ParseResponseL();
private:
	enum {
		EATNotInProgress,
		EATImsiWaitForWriteComplete,
		EATImsiReadComplete,
		EATWaitForStopState,
		EATCompleted,
		EATNotSupported
		} iState;
		
	RMobilePhone::TMobilePhoneSubscriberId* iReturnPtr;
	RMobilePhone::TMobilePhoneSubscriberId  iImsi;
	};

inline TBool CATSubscriberId::IsNotSupported()
	{return iState==EATNotSupported;}

#endif
