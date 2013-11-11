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

#ifndef __ATGPRSNTWKREGSTATUSCHANGE_H
#define __ATGPRSNTWKREGSTATUSCHANGE_H

#include "ATBASE.H"		// for CATBase



class CATGprsNtwkRegStatusChange : public CATBase
/**
This state machine deals with the unsolicited GPRS network registration status change events.
This state machine adds the expect string '+CGREG:*' and then waits for an event.
@internalComponent
*/
	{
public:		// public methods
	static CATGprsNtwkRegStatusChange* NewL(CATIO* aIo, CTelObject* aTelObject, CPhoneGlobals* aGlobals);
	~CATGprsNtwkRegStatusChange();

	void Enable();
	void Disable();

private:		// private methods
	CATGprsNtwkRegStatusChange(CATIO* aIo, CTelObject* aTelObject, CPhoneGlobals* aGlobals);

	// Utility methods
	void ParseResponseL(RPacketService::TRegistrationStatus& aRegStatus);

	// Pure virtuals from CATBase
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void EventSignal(TEventSource aSource);
	};

#endif
