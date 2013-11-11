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

#ifndef __ATGPRSNTWKREGSTATUS_H
#define __ATGPRSNTWKREGSTATUS_H

#include "ATBASE.H"



// Forward declarations
class CATGprsNtwkRegStatusChange;

class CATGprsNtwkRegStatus : public CATCommands
/**
This state machine uses the "AT+CGREG?" command to find out the phones current network
registration status.
@internalComponent
*/
	{
public:			// Public methods
	
	// Factory function
	static CATGprsNtwkRegStatus* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals,CATGprsNtwkRegStatusChange* aATGprsNtwkRegStatusChange);

	// C++ destructor
	~CATGprsNtwkRegStatus();

private:		// Private methods

	// C++ constuctor and Symbian standard 2nd stage constructor
	CATGprsNtwkRegStatus(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals,CATGprsNtwkRegStatusChange* aATGprsNtwkRegStatusChange);
	void ConstructL();

	// Methods inherited from CATCommands
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aParams);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	void Complete(TInt aError,TEventSource aSource);
	
	// Utility methods 
	void ParseResponseL();

private:		// Private data
	CATGprsNtwkRegStatusChange* iATGprsNtwkRegStatusChange;		//< Pointer to an object owned by our client
	RPacketService::TRegistrationStatus* iRegStatus;				//< Pointer to clients structure initialised in ::Start()
	enum { EIdle=0,EWaitForWrite,EWaitForRead } iState;	//< Enum for class states.
	};

#endif


