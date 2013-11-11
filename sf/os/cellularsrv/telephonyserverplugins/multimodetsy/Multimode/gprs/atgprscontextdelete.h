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

#ifndef __ATGPRSCONTEXTDELETE_H
#define __ATGPRSCONTEXTDELETE_H

#include "ATBASE.H"		// for CATCommands


class CATGprsContextDelete : public CATCommands
/**
Sends the AT command '+CGDCONT=x' to delete a context from the phone
@internalComponent
*/
	{
public:
	static CATGprsContextDelete* NewL(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGprsContextDelete ();

private:
	CATGprsContextDelete(CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aDummy);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource);

private:
	enum {EIdle,EWaitForWrite,EWaitForRead} iState;	//< Enum for class states.
	};


#endif
