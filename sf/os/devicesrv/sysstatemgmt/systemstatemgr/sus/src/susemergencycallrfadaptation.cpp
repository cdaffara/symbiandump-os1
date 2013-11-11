// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// susemergencycallrfadaptationref.cpp
// 
//

#include <e32debug.h>
#include <ssm/ssmadaptation.h>
#include "susemergencycallrfadaptation.h"
#include "suspanic.h"


//Count to reserve space for one Activate Rf call + one Deactive Rf message in Queue
const TInt KReserveCount = 2;
/**
 * Function used to cleanup the CAdaptationMessage object which is pushed on to the cleanup stack incase of Leave.
 * CAdaptationMessage message will be reset to hold default values, if it is using the precreated message pointer.
 * or else message will be deleted.
 */
static void DoCleanUp(TAny* aAdaptationMessage)
    {
    CEmergencyAdaptationMessage* storeAdaptationMessage = static_cast <CEmergencyAdaptationMessage*>(aAdaptationMessage);
    storeAdaptationMessage->Complete(KErrNone);
    if(storeAdaptationMessage->IsMessageReserved())
        {
        storeAdaptationMessage->UnsetMessageStatus(EMsgInUse);
        }
    else
        {
        delete storeAdaptationMessage;
        }
    }

/**
@publishedPartner
*/
void CEmergencyCallRfAdaptation::SubmitOrQueueL(const RMessage2 &aMessage)
	{
    CEmergencyAdaptationMessage *storeAdaptationMessage = NULL;
    TRAPD(err, storeAdaptationMessage = CEmergencyAdaptationMessage::NewL(aMessage));    
    
    //Allow only priority client to perform emergency call.
    if (KErrNoMemory == err && aMessage.Session() == iPriorityClientSession)
        {
        DEBUGPRINT1A("CEmergencyAdaptationMessage will be created using reserved pointers");
        if ( iReserveMsgCount == 0 )
           {
           DEBUGPRINT1A("There is no reserved message to perform emergency call");
           User::Leave(err);
           }
        //Traverse through the reserved message array to look for free message.
        for ( TInt index = 0 ; index < KReserveCount ; ++index )
            {
            //Use the reserve message, if it is not already used.
            if (!(iAdaptationReservedMessageArray[index]->IsMessageInuse()))
                {
                iAdaptationReservedMessageArray[index]->SetMessage(aMessage);
                iAdaptationReservedMessageArray[index]->SetMessageStatus(EMsgInUse);
                storeAdaptationMessage = iAdaptationReservedMessageArray[index];
                --iReserveMsgCount;
                break;
                }
            }
        }
    else
        {
        User::LeaveIfError(err);
        }
	if(!IsActive())
		{
		Submit(storeAdaptationMessage);
		}
	else 
		{
		//Push the CEmergencyAdaptationMessage pointer on to the cleanup stack and reset/delete the pointer
		//depending on using precreated pointer/new pointer.
		CleanupStack::PushL(TCleanupItem(DoCleanUp, storeAdaptationMessage ));
		DEBUGPRINT2A("CEmergencyCallRfAdaptationRequests queueing request with function id: %d", aMessage.Function());
		//Reserve slot only in non OOM condition
		if(!(storeAdaptationMessage->IsMessageReserved()))
		    {
		    //Priority clients are allowed to queue only two(KReserveCount) emergengency call request in the OOM condition.
		    //So always request to reserve only iPendingRequestsQueue.Count()+ iReserveMsgCount + 1
		    //slots.
		    //Memory will be only reserved if (iPendingRequestsQueue.Count()+ iReserveMsgCount + 1) > 
		    //already reserved memory
            TRAP(err, iPendingEmergencyRequestsQueue.ReserveL(iPendingEmergencyRequestsQueue.Count() + iReserveMsgCount + 1 )); 
		    }
		if(KErrNone == err || (KErrNoMemory == err && aMessage.Session() == iPriorityClientSession))
		    {	 
		    iPendingEmergencyRequestsQueue.AppendL(storeAdaptationMessage);
		    }
		else
		    {
		    User::Leave(err);
		    }
		CleanupStack::Pop(storeAdaptationMessage);
		}	
	}

void CEmergencyCallRfAdaptation::Submit(CEmergencyAdaptationMessage*& aMessage)
	{
    __ASSERT_ALWAYS((iEmergencyCallRfAdaptation != NULL), User::Panic(KPanicSsmSus, EEmergencyCallRfAdaptationNullPtrError1));
	DEBUGPRINT2A("CEmergencyCallRfAdaptationRequests immediate submission of request with function id: %d", aMessage->Function());
	iCurrentMessage = aMessage;
	switch(aMessage->Function())
		{
		case EActivateRfForEmergencyCall :
			{
			iEmergencyCallRfAdaptation->ActivateRfForEmergencyCall(iStatus);
			break;	
			}
		case EDeactivateRfForEmergencyCall :
			{
			iEmergencyCallRfAdaptation->DeactivateRfForEmergencyCall(iStatus);
			break;	
			}
		default :
			{
			break;
			}
		}
	SetActive();
	}
/**
CEmergencyCallRfAdaptation implements EmergencyCallRf Adaptation related functionality as part of 
CSsmAdaptationServer.CSsmAdaptationServer loads EmergencyCallRf Adaptation plugin and creates
CEmergencyCallRfAdaptation using the NewL. From then the loaded EmergencyCallRf Adaptation plugin
will be owned by CEmergencyCallRfAdaptation.

@internalComponent
*/

CEmergencyCallRfAdaptation* CEmergencyCallRfAdaptation::NewL(MEmergencyCallRfAdaptation* aAdaptation)
	{
	CEmergencyCallRfAdaptation* self = new(ELeave) CEmergencyCallRfAdaptation();
	CleanupStack::PushL(self);
	self->ConstructL(aAdaptation);
	CleanupStack::Pop(self);
	return self;
	}

void CEmergencyCallRfAdaptation::ConstructL(MEmergencyCallRfAdaptation* aAdaptation)
    {
    ReserveMemoryL();
    //Taking the ownership of emergencyCallRfAdaptationPlugin after all the leaving function have passed.
    //If some function leaves before taking ownership of emergencyCallRfAdaptationPlugin, it will be released twice, which causes system to panic.
    iEmergencyCallRfAdaptation = aAdaptation;
    }

/**
 * Function to reserve memory to make emergency call during OOM condition
 */
void CEmergencyCallRfAdaptation :: ReserveMemoryL()
    {
    //Reserve space for one Activate Rf call + one Deactive Rf message in Queue.
    iPendingEmergencyRequestsQueue.ReserveL(KReserveCount);
    
    //Pre-create an array to hold CEmergencyAdaptationMessage pointer, this is of size CEmergencyAdaptationMessage.
    RMessage2 message;    
    for( TInt index = 0 ; index < KReserveCount ; ++index )
        {
        CEmergencyAdaptationMessage* adaptationMessage = NULL;
        adaptationMessage = CEmergencyAdaptationMessage::NewL(message, EMsgReserved);
        CleanupStack::PushL(adaptationMessage);
        iAdaptationReservedMessageArray.AppendL(adaptationMessage);
        CleanupStack::Pop(adaptationMessage);
        }
    }

CEmergencyCallRfAdaptation::~CEmergencyCallRfAdaptation()
	{
    NotifyAndRemoveAll();
	Cancel();
	iPendingEmergencyRequestsQueue.Close();	
	iAdaptationReservedMessageArray.ResetAndDestroy();	 
	Release();
	}

void CEmergencyCallRfAdaptation::NotifyAndRemoveAll()
    {
    TInt count = iPendingEmergencyRequestsQueue.Count();
    
    for(TInt index =0; index < count; ++index)
        {
        iPendingEmergencyRequestsQueue[index]->Complete(KErrServerTerminated);
        if (!(iPendingEmergencyRequestsQueue[index]->IsMessageReserved()))
            {
            delete iPendingEmergencyRequestsQueue[index];
            }
        iPendingEmergencyRequestsQueue[index] = NULL;
        }
    iPendingEmergencyRequestsQueue.Reset();
    }

CEmergencyCallRfAdaptation::CEmergencyCallRfAdaptation() : CActive(EPriorityStandard)
,iReserveMsgCount(KReserveCount)
	{
	CActiveScheduler::Add(this);
	}

void CEmergencyCallRfAdaptation::Release()
	{
	if(iEmergencyCallRfAdaptation != NULL)
	    {
	    iEmergencyCallRfAdaptation->Release();
	    }
	}

void CEmergencyCallRfAdaptation::DoActivateRfForEmergencyCallL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);	
	}

void CEmergencyCallRfAdaptation::DoDeactivateRfForEmergencyCallL(const RMessage2& aMessage)
	{
	SubmitOrQueueL(aMessage);	
	}

void CEmergencyCallRfAdaptation::DoEmergencyCallRfAdaptationCancelL(const RMessage2& aMessage)
	{
    __ASSERT_ALWAYS((iEmergencyCallRfAdaptation != NULL), User::Panic(KPanicSsmSus, EEmergencyCallRfAdaptationNullPtrError2));
    
	if(iCurrentMessage != NULL)	
		{
		if(aMessage.Session() == iCurrentMessage->Session())
			{
			DEBUGPRINT1A("CEmergencyCallRfAdaptationRequests cancelling current request as requested");
			iEmergencyCallRfAdaptation->Cancel();
			}
		CEmergencyAdaptationMessage *messageToBeDeleted;
        for(TInt index = 0; index < iPendingEmergencyRequestsQueue.Count(); ++index )
            {
            if(aMessage.Session() == iPendingEmergencyRequestsQueue[index]->Session())
                {
                messageToBeDeleted = iPendingEmergencyRequestsQueue[index];
                DEBUGPRINT2A("RSsmAdaptationRequestQueue(aMessage,ReservedHeap)called to cancel the request with function id: %d", messageToBeDeleted->Function());
                iPendingEmergencyRequestsQueue.Remove(index);
                messageToBeDeleted->Complete(KErrCancel);
                //Reset the AdaptationMessage if it is created using reserved pointer or delete the pointer 
                if(messageToBeDeleted->IsMessageReserved())
                    {
                    messageToBeDeleted->UnsetMessageStatus(EMsgInUse);
                    ++iReserveMsgCount;
                    }
                else
                    {
                    delete messageToBeDeleted;
                    }
                --index;
                }
            }
		
		aMessage.Complete(KErrNone);
		}
	else
		{
		DEBUGPRINT1A("CEmergencyCallRfAdaptationRequests nothing to cancel, but cancel requested");
		aMessage.Complete(KErrNone);				
		}		
	}


void CEmergencyCallRfAdaptation::RunL()
	{
    __ASSERT_DEBUG((iCurrentMessage != NULL), User::Panic(KPanicSsmSus, EEmergencyAdaptationMessageNullPtrError));
	DEBUGPRINT2A("CEmergencyCallRfAdaptationRequests processed the request with funtion id: %d", iCurrentMessage->Function());
	iCurrentMessage->Complete(iStatus.Int());
	DeleteAdaptationMessage();
	if( (iPendingEmergencyRequestsQueue.Count()) > 0 )
		{
        CEmergencyAdaptationMessage *messageCopy = NULL;
        Dequeue(messageCopy);				
		Submit(messageCopy);
		} 
	}

TInt CEmergencyCallRfAdaptation::RunError( TInt aError )
	{
	
	if(iCurrentMessage != NULL)	
		{
        iCurrentMessage->Complete(aError);
		DeleteAdaptationMessage();
		}
	
	while( (iPendingEmergencyRequestsQueue.Count() > 0 ))
        {
        Dequeue(iCurrentMessage);
        iCurrentMessage->Complete(aError);
        DeleteAdaptationMessage();
        }
	
	return KErrNone;
		
	}

void CEmergencyCallRfAdaptation::DoCancel()
	{
	if(iCurrentMessage != NULL)	
		{
		iCurrentMessage->Complete(KErrCancel);
		DeleteAdaptationMessage();
		}
		
	while( (iPendingEmergencyRequestsQueue.Count() > 0 ))
		{
        Dequeue(iCurrentMessage);
        iCurrentMessage->Complete(KErrCancel);
        DeleteAdaptationMessage();
		}
	}

/**
 * Sets the Priority Client Session
 */
void CEmergencyCallRfAdaptation::SetPriorityClientSession(CSsmAdaptationSession* aPriorityClientSession)
    {
    iPriorityClientSession = aPriorityClientSession;
    }

/**
 * Unsets the Priority Client session
 */
void CEmergencyCallRfAdaptation::RemovePriorityClientSession()
    {
    iPriorityClientSession = NULL;
    }

/**
 * Reset the AdaptationMessage if it is created using reserved pointer or delete the pointer
 */
void CEmergencyCallRfAdaptation::DeleteAdaptationMessage()
    {
	//Do not delete iCurrentMessage if it is owned by iAdaptationReservedMessageArray.
	if(iCurrentMessage->IsMessageReserved())
        {
        iCurrentMessage->UnsetMessageStatus(EMsgInUse);
        ++iReserveMsgCount;
        }
    else
        {
        delete iCurrentMessage;
		}
	iCurrentMessage = NULL;
    }

void CEmergencyCallRfAdaptation::Dequeue(CEmergencyAdaptationMessage *&aCurrentMessage)
    {
    aCurrentMessage = iPendingEmergencyRequestsQueue[0];
    iPendingEmergencyRequestsQueue.Remove(0);
    }

/**
 * Constructor.
 */
CEmergencyAdaptationMessage::CEmergencyAdaptationMessage(const TInt8 aMessageStatus): iAdaptationMessage(NULL), iMessageStatus(0)
    {
    iMessageStatus |= aMessageStatus ;
    }

/**
 * Destructor.
 */
CEmergencyAdaptationMessage::~CEmergencyAdaptationMessage()
    {
    if (iAdaptationMessage != NULL)
        {
        delete iAdaptationMessage;
        }
    }

CEmergencyAdaptationMessage* CEmergencyAdaptationMessage::NewL(const RMessage2& aMessage)
    {
    return CEmergencyAdaptationMessage::NewL(aMessage, EMsgStatusNULL);
    }

CEmergencyAdaptationMessage* CEmergencyAdaptationMessage::NewL(const RMessage2& aMessage, const TInt8 aMessageStatus)
    {
    CEmergencyAdaptationMessage* self = new(ELeave) CEmergencyAdaptationMessage(aMessageStatus);
    //custom cleanup is not required here as it is a normal message till this point.
    CleanupStack::PushL(self);
    self->ConstructL(aMessage);
    CleanupStack::Pop(self);
    return self;
    }

void CEmergencyAdaptationMessage::ConstructL(const RMessage2& aMessage)
    {
    iAdaptationMessage = new (ELeave)CAdaptationMessage(aMessage);     
    }

void CEmergencyAdaptationMessage::Complete(TInt aReason)
    {
    if (iAdaptationMessage != NULL)
        {
        iAdaptationMessage->Complete(aReason);
        }
    }

/**
 * Function to check whether the messages is using reserved heap or not.
 */
inline TBool CEmergencyAdaptationMessage::IsMessageReserved()  const
    {    
    return (iMessageStatus & EMsgReserved)? ETrue:EFalse;    
    }

/**
 * Function to check whether the reserved messages is in use or not.
 */
inline TBool CEmergencyAdaptationMessage::IsMessageInuse() const 
    {    
    return (iMessageStatus & EMsgInUse)? ETrue:EFalse;    
    }

/**
 * Unset the message status .
 */
inline void CEmergencyAdaptationMessage::UnsetMessageStatus(const TInt8 aMessageStatus)
    {    
    iMessageStatus &= ~aMessageStatus ;
    }

/**
 * Set the message status .
 */
inline void CEmergencyAdaptationMessage::SetMessageStatus(const TInt8 aMessageStatus)
    {    
    iMessageStatus |= aMessageStatus ;
    }

/**
 * Sets the RMessage2 .
 */
void CEmergencyAdaptationMessage::SetMessage(const RMessage2& aMessage)
    {    
    iAdaptationMessage->SetMessage(aMessage);
    }

/**
 * Returns the Function .
 */
TInt CEmergencyAdaptationMessage::Function() const
    {    
    return iAdaptationMessage->Function();
    }

/**
 * Returns the Session .
 */
CSession2 *CEmergencyAdaptationMessage::Session() const
    {
    return iAdaptationMessage->Session();
    }

