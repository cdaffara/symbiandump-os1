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
// Set to online data mode
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATO_H__
#define __ATO_H__

#include "ATCALL.H"

class CATSetToOnlineDataMode : public CATCallAlterCommands
/**
@internalComponent
*/
	{
public:
	static CATSetToOnlineDataMode* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATSetToOnlineDataMode();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
protected:
	CATSetToOnlineDataMode(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource);
private:
	void ValidateExpectStringWithConnectL();
	void RemoveATOExpectStrings();
private:
	enum {
		EATNotInProgress,
		EATOWaitForWriteComplete,
		EATOReadCompleted,
		EATCancelling
		} iState;
	CCommChatString* iConnectExpectString;
	TBuf8<KCommsDbSvrMaxFieldLength> iConnectString;
	};

#endif
