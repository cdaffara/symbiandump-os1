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

#ifndef _ATGPRSDETACH_H
#define _ATGPRSDETACH_H
#include "ATBASE.H"



class CPhoneGlobals;


class CATGprsDetach : public CATCommands
/**
Sends the AT command to detach from the network.
CATGprsDetach inherits from the CATCommands class. 
This class transmits the following commands: AT+CGATT=0
@internalComponent
*/
	{
public:
	static CATGprsDetach* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	
	~CATGprsDetach ();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
private:
	CATGprsDetach (CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource);
private:
	enum 
		{
		EATNotInProgress,
		ESendDetachCommand,
		EDetachReadComplete
		} iState;								//< Enum for class states.

	};



#endif
