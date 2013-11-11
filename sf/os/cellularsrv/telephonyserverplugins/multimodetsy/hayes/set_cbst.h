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

#ifndef __SET_CBST_H__
#define __SET_CBST_H__

#include "ATBASE.H"		// For CATCommands

class CATSetCBST : public CATCommands
/**
@internalComponent
*/
	{
public:
	static CATSetCBST* NewL(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);

private:		// Private methods
	CATSetCBST(CATIO* aIo, CTelObject* aTelObject,CATInit* aInit,CPhoneGlobals* aPhoneGlobals);

	// Virtual from CATBase
	virtual void EventSignal(TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	virtual void Complete(TInt aError,TEventSource aSource);

	// Virtual from CATCommands
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aNextCmdToExecute);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

private:	// Private data
	enum {
		EATNotInProgress,
		EWaitForWriteComplete,
		EWaitForReadComplete
		} iState;
	
	CATCommands* iNextCmdToExecute;		//< This class does not own this object
	};

#endif
