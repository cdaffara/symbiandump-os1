/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation for the RDosEventRcv class.
*
*/


#include <DosSvrServices.h>
#include "dosclientserver.h"

// ---------------------------------------------------------
// RDosEventRcv Constructor
// ---------------------------------------------------------
//  

RDosEventRcv::RDosEventRcv()
{
	iParamContent = NULL;
	iContentPtr = NULL;
	iRegistered = EFalse;
	iQueue = EQueue;
}


// ---------------------------------------------------------
// RDosEventRcv Destructor
// ---------------------------------------------------------
//  

RDosEventRcv::~RDosEventRcv()
{
	if(iParamContent) delete iParamContent;
	if(iContentPtr) delete iContentPtr;
}

// ---------------------------------------------------------
// RDosEventRcv::Open
// ---------------------------------------------------------
//  

TInt RDosEventRcv::Open(RDosServer& aServer)
{
	return CreateSubSession(aServer,EDosCreateEventRcvSubSession, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));
}


// ---------------------------------------------------------
// RDosEventRcv::Register
// ---------------------------------------------------------
//  

TInt RDosEventRcv::Register(TInt aEvent,TInt aParamSize,TQueueType aQueue)
{
	if(iRegistered)
			return KErrAlreadyExists;
	
	if(iParamContent)
	{
	   delete iParamContent;
	}

	iParamContent = HBufC8::New(aParamSize);
	if(iParamContent)
	{
		if(iContentPtr)
		{
		   delete iContentPtr;
		}

		iContentPtr = new TPtr8(iParamContent->Des());

		if(iContentPtr)
		{
			TInt err = SendReceive(ERegisterEvent, TIpcArgs(aEvent, TIpcArgs::ENothing, KAutoComplete));

			if(!err) 
			{
				iRegistered = ETrue;
				iQueue = aQueue;
				iDosEvent = aEvent;
			}

			return err;
		}
	}
	return KErrNoMemory;
}

// ---------------------------------------------------------
// RDosEventRcv::UnRegister
// ---------------------------------------------------------
//  

void RDosEventRcv::UnRegister()
{
	if(iRegistered) 
	{
		TInt err = SendReceive(EUnRegisterEvent, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));

		if(!err)
		{

			delete iContentPtr;
			iContentPtr=NULL;
			delete iParamContent;
			iParamContent = NULL;

			iRegistered = EFalse;
		}
	}
}

// ---------------------------------------------------------
// RDosEventRcv::WaitEvent
// ---------------------------------------------------------
//  

TInt RDosEventRcv::WaitEvent(TRequestStatus& aStatus) const
{
	if(iRegistered)
	{
		SendReceive(EWaitEvent, TIpcArgs(iQueue, iContentPtr, 0), aStatus);

		return KErrNone;
	}
	return KErrAbort; //It can't wait when it's not registered.
}

// ---------------------------------------------------------
// RDosEventRcv::CancelWaitEvent
// ---------------------------------------------------------
//  

TInt RDosEventRcv::CancelWaitEvent() const
{
	if(iRegistered)
	{
		SendReceive(ECancelWaitEvent, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));

		return KErrNone;
	}
	return KErrAbort; //It can't cancel wait when it's not registered.
}
