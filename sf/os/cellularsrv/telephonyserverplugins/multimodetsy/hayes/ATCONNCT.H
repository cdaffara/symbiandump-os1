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

#ifndef __ATCONNCT_H__
#define __ATCONNCT_H__

#include "ATCALL.H"

class CATConnectData : public CATDataCallConnectCommands
/**
Connect command
@internalTechnology
*/
	{
public:
	static CATConnectData* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	~CATConnectData();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
protected:
	CATConnectData(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
private:
	TInt AddConnectExpectStrings();
	void RemoveConnectExpectStrings();
	TInt ValidateConnectExpectString();
private:
	CCommChatString* iNoDialToneExpectString;
	enum {
		EATNotInProgress,
		EATInitialising,
		EATSendConnectCommand,
		EATConnectWaitForWriteComplete,
		EATConnectReadCompleted,
		EATSpeedReadComplete,
		EATCancellingWaitForWriteComplete,
		EATCancellingReadCompleted
		} iState;
	TBuf8<KCommsDbSvrMaxFieldLength> iNoDialToneString;
	};


#endif
