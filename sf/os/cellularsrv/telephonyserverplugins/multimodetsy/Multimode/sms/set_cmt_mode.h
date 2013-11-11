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

#ifndef __SET_CMT_MODE_H__
#define __SET_CMT_MODE_H__

#include "smsbase.H"

class CATSetPhoneToCMTMode : public CATSmsCommands
	{
public:
	static CATSetPhoneToCMTMode* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,
									 CPhoneGlobals* aGlobals,
									 RMobileSmsMessaging::TMobileSmsReceiveMode& iCurrentRxModeVariable);
	
protected:
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource, TInt aStatus);
	void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	void Stop(TTsyReqHandle aTsyReqHandle);
	void Complete(TInt aError);
	virtual void Complete(TInt aError, TEventSource aSource);

private:
	// Constructor
	CATSetPhoneToCMTMode(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit,
	                     CPhoneGlobals* aGlobals,
  	                     RMobileSmsMessaging::TMobileSmsReceiveMode& iCurrentRxModeVariable);

	RMobileSmsMessaging::TMobileSmsReceiveMode& iClientsCurrentRxMode;
	};

#endif
