// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _ATGPRSATTACH_H
#define _ATGPRSATTACH_H
#include "ATBASE.H"



class CATGprsAttach : public CATCommands
/**
Sends the AT command to connect to the GPRS network.
CATGprsAttach inherits from the CATCommands class. 
This class transmits the AT+CGATT=1 Command to connect to the GPRS network.
@internalComponent
*/
	{
public:
	static CATGprsAttach* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGprsAttach();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

private:
	CATGprsAttach(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	void Complete(TInt aError,TEventSource aSource);
	
private:
	enum 
		{
		EATNotInProgress,
		ESendAttachCommand,
		EAttachReadComplete
		} iState;								//< Enum for class states.
	};
#endif
