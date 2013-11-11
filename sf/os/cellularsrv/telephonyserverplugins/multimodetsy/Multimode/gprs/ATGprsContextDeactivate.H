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

#ifndef __ATGPRSCONTEXTDEACTIVATE_H
#define __ATGPRSCONTEXTDEACTIVATE_H
#include "ATBASE.H"

class CPhoneGlobals;

class CATGprsContextDeactivate : public CATCommands
/**
Sends the AT command to deactivate a context to the network.
CATGprsContextDeactivate inherits from the CATCommands class. 
This class transmits the following commands: ATH, +++ and ATH.
This will release the ppp chain and return to AT command mode.
@internalComponent
*/
	{
public:
	static CATGprsContextDeactivate * NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	
	~CATGprsContextDeactivate ();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* /*aDummy*/);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
protected:
	CATGprsContextDeactivate (CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource);
private:
	void ValidateHangUpExpectStringL();
private:
	CCommChatString* iNoCarrierExpectString;				//< Pointer to response string.
	enum													
		{
		EATNotInProgress,									
		EDTRDropped,
		EWaitForDTRRaiseSettle,
		EATEscapeSeqWaitForWriteComplete,
		EATEscapeSeqCompleted,
		EATHangupWaitForWriteComplete,
		EATHangupReadCompleted,
		EHangUpCancelling
		} iState;											//< Enum for class states.

	};



#endif
