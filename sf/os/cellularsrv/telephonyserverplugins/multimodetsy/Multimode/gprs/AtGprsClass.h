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

#ifndef _ATGPRSCLASS_H
#define _ATGPRSCLASS_H
#include "ATBASE.H"

/**@file
   @internalAll */

class CATGprsClass : public CATCommands
/**
Sends the  AT command to set the class.
CATGprsAttach inherits from the CATCommands class. 
This class transmits the AT+CGCLASS=%S Command to set the class.
@internalComponent
*/
	{
public:
	static CATGprsClass* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);

	~CATGprsClass();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
protected:
	CATGprsClass(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	void Complete(TInt aError,TEventSource aSource);
	
private:
	TInt MakeupCGCLASS(RPacketService::TMSClass aClass);

	RPacketService::TMSClass* iClass;				//< Pointer to clients MSClass structure.
	enum
		{
		EATNotInProgress,
		ESendClassCommand,
		EClassReadComplete
		}iState;							//< Enum for class states.
	};
#endif


