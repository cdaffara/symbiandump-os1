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


#ifndef __ATGPRSCONTEXTACTIVATE_H
#define __ATGPRSCONTEXTACTIVATE_H
#include "ATBASE.H"



/**
Sends the AT command to attach a context to the network.
CATGprsContextActivate inherits from the CATCommands class. 
This class transmits the following commands: ATD*99***%d#. 
@internalComponent
*/
class CATGprsContextActivate : public CATCommands
{
public:
	static CATGprsContextActivate * NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGprsContextActivate();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

protected:
	CATGprsContextActivate(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	void Complete(TInt aError,TEventSource aSource);
	TInt ValidateExpectString();	
private:
	CCommChatString* iExpectString;				//< Pointer to response string.
	CCommChatString* iNoCarrierString;			//< Pointer to response string.

	TInt iCid;									//< Context id.
	enum 
		{
		EATNotInProgress,
		ESendEricssonConnectCommand,
		EConnectEricssonReadComplete,
		ESendNormalConnectCommand,
		EConnectNormalReadComplete
		} iState;									//< Enum for class states.
};



#endif
