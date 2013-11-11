// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cmocksysession.h"
#include "rmocksy.h"
#include "cmocksyengine.h"


/**
Panic the client.
*/
void PanicClient(const RMessagePtr2& aMessage, TMockSYPanic aPanic)
	{
	_LIT(KPanic,"MockSY");
	aMessage.Panic(KPanic,aPanic);
	}

/**
Factory function.
*/
CSession2* CMockSYSession::NewL(CMockSYEngine& aEngine)
	{
	CMockSYSession* self=new(ELeave) CMockSYSession(aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Constructor
*/
CMockSYSession::CMockSYSession(CMockSYEngine& aEngine)
	:iEngine(aEngine)
	{
	}
	
/**
2nd phase construction 
*/
void CMockSYSession::ConstructL()
	{
	iEngine.AddListenerL(*this);
	}

/**
Destructor
*/
CMockSYSession::~CMockSYSession()
	{
	iEngine.RemoveListener(*this);
	iEngine.SessionClosed();
	if (!iNotifyDoneMsg.IsNull())
		{
		iNotifyDoneMsg.Complete(KErrCancel);
		}
	}

/**
Engine listener
*/
void CMockSYSession::Notify(TNotificationType aNotification)
	{
	switch (aNotification)
		{
		case EHandlingTerminated:
		case EFailure:
			{
			if (!iNotifyDoneMsg.IsNull())
				{
				iNotifyDoneMsg.Complete(aNotification==EHandlingTerminated?KErrNone:KErrCorrupt);
				}
			}
			break;
		default:
			; // don't care about other notifications
		}
	}
		
/**
Handles the servicing of a client request that has been passed
to the server.
*/
void CMockSYSession::ServiceL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case RMockSY::KExpect:
			{
			HBufC8* data = HBufC8::NewLC(aMessage.GetDesLengthL(3));
			TPtr8 dataPtr = data->Des();
			aMessage.ReadL(3,dataPtr);
			CleanupStack::Pop(data);
			iEngine.QueueEpectedCallL(aMessage.Int0(),aMessage.Int1(),aMessage.Int2(),data);
			aMessage.Complete(KErrNone);	
			}
		break;
		
		case RMockSY::KComplete:
			{
			HBufC8* data = HBufC8::NewLC(aMessage.GetDesLengthL(3));
			TPtr8 dataPtr = data->Des();
			aMessage.ReadL(3,dataPtr);
			CleanupStack::Pop(data);
			iEngine.QueueReturnCallL(aMessage.Int0(),aMessage.Int1(), aMessage.Int2(),data);
			aMessage.Complete(KErrNone);	
			}
		break;
		
		case RMockSY::KNotifyTerminated:
			{			
			if (!iNotifyDoneMsg.IsNull())
				{
				PanicClient(aMessage,EPanicAlreadyReceiving);				
				}
			else
				{
				iNotifyDoneMsg = aMessage;
				}
			}
		break;
		
		case RMockSY::KGetNextLogLine:
			{
			HBufC* line = iEngine.GetNextLogLine();
			TInt ret;
			if (line!=NULL)
				{
				ret=aMessage.Write(0,line->Des());
				delete line;
				}
			else
				{
				ret=aMessage.Write(0,KNullDesC);
				}
			aMessage.Complete(ret);	
			}
		break;
	
		case RMockSY::KGetStatus:
			{
			TBool retVal;
			TPckg<TBool> retValPckg(retVal);
			retVal = iEngine.HasWaitingEvents();
			aMessage.Write(0,retValPckg);
			retVal = iEngine.HasPendingEvents();
			aMessage.Write(1,retValPckg);
			retVal = iEngine.HasFailure();
			aMessage.Write(2,retValPckg);
			aMessage.Complete(KErrNone);	
			}
		break;
        case RMockSY::KPause:
            {
            aMessage.Complete(iEngine.PauseCompletion());
            break;
            }
        case RMockSY::KResume:
            {
            aMessage.Complete(iEngine.ResumeCompletion());
            break;
            }
		
		default:
			PanicClient(aMessage,EPanicIllegalFunction);				
		}
	}
